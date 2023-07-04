/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#pragma once
#include "Utils/Char.h"
#include "Utils/HashMap.h"
#include "Utils/ScratchString.h"
#include "Utils/Stack.h"
#include "Utils/StackGuard.h"
#include "Utils/StreamConverters/Hex.h"
#include "Utils/StringBuilder.h"
#include "Utils/TextStreamWriter.h"

#ifndef JSON_STACK_DEPTH
    #define JSON_STACK_DEPTH 16
#endif

#ifndef JSON_HASH_RESERVE
    #define JSON_HASH_RESERVE 64
#endif

#ifndef JSON_ARRAY_RESERVE
    #define JSON_ARRAY_RESERVE 8
#endif

namespace Rt2::Json
{

    namespace Internal
    {
        // limits the maximum number of nested elements
        using StackCheck = FixedStackGuard<JSON_STACK_DEPTH>;
        namespace Dom
        {
            class Value;
            class Document;
            class BoolValue;
            class RealValue;
            class IntegerValue;
            class StringValue;
            class PointerValue;
            class ObjectValue;
            class ArrayValue;
        }  // namespace Dom
        class Parser;

        namespace Lex
        {
            class Scanner;
            class Token;

            enum Type
            {
                JT_UNDEFINED,
                JT_COLON,
                JT_COMMA,
                JT_L_BRACE,
                JT_L_BRACKET,
                JT_R_BRACE,
                JT_R_BRACKET,
                JT_NUMBER,
                JT_INTEGER,
                JT_BOOL,
                JT_STRING,
                JT_NULL,
                JT_EOF
            };

            class Token
            {
            private:
                ScratchString _value{16};
                Type          _type{JT_UNDEFINED};

            public:
                Token()  = default;
                ~Token() = default;

                void push_back(const char value)
                {
                    RT_GUARD_CHECK_VOID(isPrintableAscii(value))
                    _value.push(value);
                }

                void push_back(const String& value)
                {
                    RT_GUARD_CHECK_VOID(!value.empty())
                    // only allow for 32 - 127, \n\r\t
                    String nv;
                    Su::filterAscii(nv, value);
                    _value.push(nv);
                }

                void clear()
                {
                    _type = JT_NULL;
                    _value.clear();
                }

                const String& value() const;

                const Type& type() const;

                void setType(const Type& type);
            };

            namespace Keyword
            {
                struct LookupTable
                {
                    const char* c;
                    int         n;
                };

                struct Escape
                {
                    const char c;
                    const char v;
                };

                constexpr LookupTable Chars[] = {
                    {"ntf", 3},
                    {"ura", 3},
                    { "lu", 2},
                    {"les", 3},
                    {  "e", 1},
                };

                constexpr Escape Sequences[] = {
                    { 'b', '\b'},
                    { 'f', '\f'},
                    { 'n', '\n'},
                    { 'r', '\r'},
                    { 't', '\t'},
                    {'\\', '\\'},
                    { '/',  '/'},
                    { '"',  '"'},
                };

                union ComputedLookup
                {
                    constexpr ComputedLookup() = default;
                    constexpr ComputedLookup(const char a, const char b, const char c, const char d, const char e)
                    {
                        code[0] = a;
                        code[1] = b;
                        code[2] = c;
                        code[3] = d;
                        code[4] = e;
                        code[5] = 0xFF;
                        code[6] = 0xFF;
                        code[7] = 0xFF;
                    }
                    uint8_t  code[8]{0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
                    uint64_t index;
                };

                constexpr ComputedLookup Null  = {'n', 'u', 'l', 'l', 0};
                constexpr ComputedLookup True  = {'t', 'r', 'u', 'e', 0};
                constexpr ComputedLookup False = {'f', 'a', 'l', 's', 'e'};

                static bool contains(const int ch, const char* code, const int n)
                {
                    bool res = false;
                    for (int i = 0; i < n && !res; ++i)
                        res = code[i] == ch;
                    return res;
                }

                static char check(IStream* stream)
                {
                    RT_GUARD_CHECK_RET(stream, 0)

                    int u = 0;

                    ComputedLookup tested;
                    for (const auto& [c, n] : Chars)
                    {
                        if (contains(stream->peek(), c, n))
                        {
                            tested.code[u++] = (char)stream->get();
                            if (tested.index == Null.index)
                                return '*';
                            if (tested.index == True.index)
                                return '+';
                            if (tested.index == False.index)
                                return '-';
                        }
                        else
                            break;
                    }
                    for (int i = u - 1; i >= 0; i--)
                        stream->putback((char)tested.code[i]);
                    return 0;
                }

            }  // namespace Keyword

            class Scanner
            {
            private:
                IStream* _stream{nullptr};

            public:
                explicit Scanner(IStream* stream) :
                    _stream(stream)
                {
                }

                ~Scanner() = default;

                void scan(Token& tok)
                {
                    tok.clear();
                    RT_GUARD_CHECK_VOID(_stream)
                    do
                    {
                        switch (const int ch = _stream->get())
                        {
                        case -1:
                            tok.setType(JT_EOF);
                            return;
                        case ' ':  // exclusion cases
                        case '\t':
                        case '\n':
                        case '\r':
                            break;
                        case '[':
                            tok.setType(JT_L_BRACE);
                            return;
                        case ']':
                            tok.setType(JT_R_BRACE);
                            return;
                        case ',':
                            tok.setType(JT_COMMA);
                            return;
                        case ':':
                            tok.setType(JT_COLON);
                            return;
                        case '{':
                            tok.setType(JT_L_BRACKET);
                            return;
                        case '}':
                            tok.setType(JT_R_BRACKET);
                            return;
                        case '-':  // decimal cases
                        case Digits09:
                            _stream->putback((char)ch);
                            scanDigit(tok);
                            return;
                        case 'n':
                        case 't':
                        case 'f':
                            _stream->putback((char)ch);
                            scanKeyword(tok);
                            return;
                        case '\"':
                            _stream->putback((char)ch);
                            scanString(tok);
                            return;
                        default:
                            throw Exception("unknown character parsed (0x", Hex((uint8_t)ch), ")");
                        }
                    } while (!_stream->eof());
                }

                bool isOpen() const;

            private:
                bool check(int& cur)
                {
                    if (cur == '\\')
                    {
                        int nch = _stream->peek();
                        for (const auto& v : Keyword::Sequences)
                        {
                            if (nch == v.c)
                            {
                                (void)_stream->get();
                                cur = v.v;
                                return true;
                            }
                        }
                    }
                    return cur > 0 && cur != 0x22;
                }

                void scanString(Token& tok)
                {
                    RT_GUARD_CHECK_VOID(_stream)

                    int ch = _stream->get();
                    if (ch != 0x22)
                        throw Exception("missing '\"' character");

                    for (ch = _stream->get(); check(ch); ch = _stream->get())
                        tok.push_back((char)ch);
                    if (ch <= 0)
                        throw Exception("premature end of file");
                    tok.setType(JT_STRING);
                }

                void scanDigit(Token& tok)
                {
                    RT_GUARD_CHECK_VOID(_stream)
                    bool hasDot = false;

                    int ch;
                    for (ch = _stream->get(); ch > 0 && isReal(ch); ch = _stream->get())
                    {
                        if (ch == '.') hasDot = true;
                        if (ch == 'e') hasDot = true;
                        if (ch == 'E') hasDot = true;
                        tok.push_back((char)ch);
                    }

                    if (ch != 0)
                    {
                        tok.setType(hasDot ? JT_NUMBER
                                           : JT_INTEGER);
                        _stream->putback((char)ch);
                    }
                }

                void scanKeyword(Token& tok) const
                {
                    switch (Keyword::check(_stream))
                    {
                    case '*':
                        tok.setType(JT_NULL);
                        tok.push_back("null");
                        break;
                    case '+':
                        tok.setType(JT_BOOL);
                        tok.push_back("true");
                        break;
                    case '-':
                        tok.setType(JT_BOOL);
                        tok.push_back("false");
                        break;
                    default:
                        throw Exception("unknown keyword code");
                    }
                }
            };

            inline const String& Token::value() const
            {
                return _value.value();
            }

            inline const Type& Token::type() const
            {
                return _type;
            }

            inline void Token::setType(const Type& type)
            {
                _type = type;
            }

            inline bool Scanner::isOpen() const
            {
                return _stream != nullptr && !_stream->eof();
            }

        }  // namespace Lex

        namespace Dom
        {
            class ArrayValue;
            class ObjectValue;

            enum Class
            {
                CT_UNDEFINED,
                CT_ARRAY,
                CT_BOOLEAN,
                CT_REAL,
                CT_INTEGER,
                CT_OBJECT,
                CT_STRING,
                CT_POINTER
            };

            template <typename T>
            bool isTypeOf(const Class& code)
            {
                return code == T::id;
            }

            class Value
            {
            protected:
                const Class _type;
                String      _value;

            protected:
                explicit Value(const Class& type) :
                    _type(type) {}

                Value() :
                    _type{CT_UNDEFINED} {}

                virtual void notifyStringChanged() {}
                virtual void notifyValueChanged() {}

            public:
                virtual ~Value() = default;

                virtual Value* clone() = 0;

                template <typename T>
                T* cast()
                {
                    return isTypeOf(T::id) ? static_cast<T*>(this) : nullptr;
                }

                template <typename T>
                const T* cast() const
                {
                    return isTypeOf(T::id) ? static_cast<const T*>(this) : nullptr;
                }

                template <typename T>
                T* assert_cast()
                {
                    if (isTypeOf(T::id))
                        return static_cast<T*>(this);
                    throw Exception("bad cast");
                }

                const Class& type() const
                {
                    return _type;
                }

                const String& value() const
                {
                    return _value;
                }

                bool isString() const
                {
                    return _type == CT_STRING;
                }

                bool isInteger() const
                {
                    return _type == CT_INTEGER;
                }

                bool isReal() const
                {
                    return _type == CT_REAL;
                }

                bool isBoolean() const
                {
                    return _type == CT_BOOLEAN;
                }

                bool isPointer() const
                {
                    return _type == CT_POINTER;
                }

                bool isObject() const
                {
                    return _type == CT_OBJECT;
                }

                bool isArray() const
                {
                    return _type == CT_ARRAY;
                }

                bool isTypeOf(const Class& code) const
                {
                    return this->type() == code;
                }

                ArrayValue* array()
                {
                    return cast<ArrayValue>();
                }

                ObjectValue* object()
                {
                    return cast<ObjectValue>();
                }

                void setValue(const String& v)
                {
                    _value.assign(v.c_str(), v.size());
                    notifyStringChanged();
                }

                int64_t integer(const int64_t defaultValue = -1) const
                {
                    if (isInteger()) return Char::toInt64(_value, defaultValue);
                    if (isBoolean()) return boolean() ? 1 : 0;
                    if (isReal()) return (int64_t)real();
                    return defaultValue;
                }

                double real(const double defaultValue = 0.0) const
                {
                    if (isReal()) return Char::toDouble(_value);
                    if (isBoolean()) return boolean() ? 1.0 : 0.0;
                    if (isInteger()) return (double)integer();
                    return defaultValue;
                }

                bool boolean(const bool defaultValue = false) const
                {
                    if (isBoolean()) return Char::toBool(_value);
                    if (isInteger()) return Char::toInt64(_value) != 0;
                    if (isReal()) return Char::toDouble(_value) != 0.0;
                    return defaultValue;
                }

                String toString() const
                {
                    StringBuilder sb;
                    toString(sb);
                    return sb.toString();
                }

                virtual void toString(String& dest) const
                {
                    StringBuilder sb;
                    toString(sb);
                    sb.toString(dest);
                }

                virtual void toString(StringBuilder& dest) const = 0;
            };

            class BoolValue final : public Value
            {
            public:
                static constexpr Class id = CT_BOOLEAN;

            private:
                bool _bool;

                void notifyStringChanged() override
                {
                    _bool = Char::toBool(_value);
                }

                void notifyValueChanged() override
                {
                    Char::toString(_value, _bool);
                }

            public:
                BoolValue() :
                    Value(id),
                    _bool(false)
                {
                }

                explicit BoolValue(const bool val) :
                    Value(id),
                    _bool(val)
                {
                    notifyValueChanged();
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write(_bool ? "true" : "false");
                }

            protected:
                Value* clone() override
                {
                    return new BoolValue(_bool);
                }
            };

            class RealValue final : public Value
            {
            public:
                static constexpr Class id = CT_REAL;

            private:
                double _double;

                void notifyStringChanged() override
                {
                    _double = Char::toDouble(_value);
                }

                void notifyValueChanged() override
                {
                    Char::toString(_value, _double, true);
                }

            public:
                RealValue() :
                    Value(id),
                    _double(0.0)
                {
                }

                explicit RealValue(const double& v) :
                    Value(id),
                    _double(v)
                {
                    notifyValueChanged();
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write(_double);
                }

            protected:
                Value* clone() override
                {
                    return new RealValue(_double);
                }
            };

            class IntegerValue final : public Value
            {
            public:
                static constexpr Class id = CT_INTEGER;

            private:
                union Integer
                {
                    Integer() = default;

                    explicit Integer(const uint64_t& v) { u64 = v; }

                    int8_t   i8[8];
                    int16_t  i16[4];
                    int32_t  i32[2];
                    int64_t  i64;
                    uint8_t  u8[8];
                    uint16_t u16[4];
                    uint32_t u32[2];
                    uint64_t u64{0};
                };

                Integer _integer{};

                void notifyStringChanged() override
                {
                    _integer.i64 = Char::toInt64(_value);
                }

                void notifyValueChanged() override
                {
                    Char::toString(_value, _integer.i64);
                }

            public:
                IntegerValue() :
                    Value(id)
                {
                }

                explicit IntegerValue(const uint64_t& val) :
                    Value(id),
                    _integer(val)
                {
                    notifyValueChanged();
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write(_integer.i64);
                }

            protected:
                Value* clone() override
                {
                    return new IntegerValue(_integer.u64);
                }
            };

            class StringValue final : public Value
            {
            public:
                static constexpr Class id = CT_STRING;

            public:
                StringValue() :
                    Value(id)
                {
                }

                explicit StringValue(const String& str) :
                    Value(id)
                {
                    setValue(str);
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write('"');
                    dest.write(_value);
                    dest.write('"');
                }

            protected:
                Value* clone() override
                {
                    return new StringValue(_value);
                }
            };

            class PointerValue final : public Value
            {
            public:
                static constexpr Class id = CT_POINTER;

            private:
                Handle _address;

                void notifyStringChanged() override
                {
                    if (_value == "null")
                        _address = nullptr;
                    else
                        _address = (Handle)Char::toUint64(_value);
                }

                void notifyValueChanged() override
                {
                    if (!_address)
                        _value.assign("null");
                    else
                        Char::toString(_value, _address);
                }

            public:
                PointerValue() :
                    Value(id),
                    _address(nullptr)
                {
                    notifyValueChanged();
                }

                explicit PointerValue(const Handle vp) :
                    Value(id),
                    _address((Handle)vp)
                {
                    notifyValueChanged();
                }

                void toString(StringBuilder& dest) const override
                {
                    if (!_address)
                        dest.write("null");
                    else
                        dest.write((uint64_t)_address);
                }

            protected:
                Value* clone() override
                {
                    return new PointerValue(_address);
                }
            };

            class ObjectValue final : public Value
            {
            public:
                static constexpr Class id = CT_OBJECT;
                using Dictionary          = HashTable<String, Value*>;
                using PointerType         = Dictionary::PointerType;

            private:
                Dictionary _dictionary{JSON_HASH_RESERVE};

                Value* at(const size_t& pos)
                {
                    if (pos < _dictionary.size())
                        return _dictionary.at(pos);
                    return nullptr;
                }

                Value* at(const String& key)
                {
                    if (const size_t pos = _dictionary.find(key);
                        pos != Npos) return at(pos);
                    return nullptr;
                }

            public:
                ObjectValue() :
                    Value(id)
                {
                }

                explicit ObjectValue(const ObjectValue* rhs) :
                    Value(id)
                {
                    copy(rhs);
                }

                ~ObjectValue() override
                {
                    for (const auto& el : _dictionary)
                        delete el.second;
                    _dictionary.clear();
                }

                Value* clone() override
                {
                    const auto v = new ObjectValue();
                    v->copy(this);
                    return v;
                }

                bool contains(const String& key) const
                {
                    return _dictionary.find(key) != Npos;
                }

                Value* get(const String& key)
                {
                    return at(key);
                }

                void insert(const String& key, Value* value)
                {
                    RT_GUARD_CHECK_VOID(value)

                    if (const size_t pos = _dictionary.find(key); pos == Npos)
                        _dictionary.insert(key, value);
                    else
                    {
                        delete _dictionary.at(pos);
                        _dictionary.remove(key);
                        _dictionary.insert(key, value);
                    }
                }

                void insert(const String& key, const int8_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const int16_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const int32_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const int64_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const uint8_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const uint16_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const uint32_t& value) { insert(key, (uint64_t)value); }
                void insert(const String& key, const float& value) { insert(key, (double)value); }

                void insert(const String& key, const uint64_t& value)
                {
                    insert(key, new IntegerValue(value));
                }

                void insert(const String& key, const double& value)
                {
                    insert(key, new RealValue(value));
                }

                void insert(const String& key, const bool& value)
                {
                    insert(key, new BoolValue(value));
                }

                void insert(const String& key, const Handle value)
                {
                    insert(key, new PointerValue(value));
                }

                void insert(const String& key, const String& value)
                {
                    insert(key, new StringValue(value));
                }

                void insert(const String& key, const char* value)
                {
                    insert(key, new StringValue(value));
                }

                void string(String& dest, const String& key, const String& def = "")
                {
                    if (const auto v = at(key))
                        dest.assign(v->value());
                    else
                        dest.assign(def);
                }

                String string(const String& key, const String& def = "")
                {
                    String copy;
                    string(copy, key, def);
                    return copy;
                }

                void integer(int64_t& dest, const String& key, const int64_t& def = -1)
                {
                    if (const auto v = at(key))
                        dest = v->integer(def);
                    else
                        dest = def;
                }

                int64_t integer(const String& key, const int64_t& def = -1)
                {
                    int64_t v;
                    integer(v, key, def);
                    return v;
                }

                bool boolean(const String& key, const bool def = false)
                {
                    if (const auto v = at(key))
                        return v->boolean(def);
                    return def;
                }

                double real(const String& key, const double& def = 0.0)
                {
                    if (const auto v = at(key))
                        return v->real(def);
                    return def;
                }

                void toString(String& dest) const override
                {
                    Value::toString(dest);
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write('{');
                    bool first = true;
                    for (const auto& it : _dictionary)
                    {
                        if (!first)
                            dest.write(',');
                        else
                            first = false;
                        dest.write('"');
                        dest.write(it.first);
                        dest.write('"');
                        dest.write(':');
                        dest.write(it.second->toString());
                    }
                    dest.write('}');
                }

                PointerType begin() const
                {
                    return _dictionary.begin();
                }

                PointerType end() const
                {
                    return _dictionary.end();
                }

                bool empty() const
                {
                    return _dictionary.empty();
                }

            private:
                void copy(const ObjectValue* rhs)
                {
                    RT_GUARD_CHECK_VOID(rhs)

                    for (const auto& entry : rhs->_dictionary)
                    {
                        if (entry.second)
                            insert(entry.first, entry.second->clone());
                    }
                }
            };

            class ArrayValue final : public Value
            {
            public:
                static constexpr Class id = CT_ARRAY;

                using TypeArray   = SimpleArray<Value*>;
                using PointerType = TypeArray::PointerType;
                using SizeType    = TypeArray::SizeType;

            private:
                TypeArray _array;

            public:
                ArrayValue() :
                    Value(id)
                {
                }

                explicit ArrayValue(const ArrayValue* rhs) :
                    Value(id)
                {
                    copy(rhs);
                }

                ~ArrayValue() override
                {
                    for (const auto& it : _array)
                        delete it;
                }

                PointerType begin()
                {
                    return _array.begin();
                }

                PointerType end()
                {
                    return _array.end();
                }

                void push_back(Value* value)
                {
                    RT_GUARD_CHECK_VOID(value)
                    _array.push_back(value);
                }

                void push_back(const int16_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const int32_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const int64_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const uint16_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const uint32_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const uint64_t& value)
                {
                    push_back(new IntegerValue(value));
                }

                void push_back(const bool value)
                {
                    push_back(new BoolValue(value));
                }

                void push_back(const double& value)
                {
                    push_back(new RealValue(value));
                }

                void push_back(const float& value)
                {
                    push_back(new RealValue((double)value));
                }

                void push_back(const String& value)
                {
                    push_back(new StringValue(value));
                }

                void push_back(const char* value)
                {
                    push_back(new StringValue(value));
                }

                void push_back(const Handle value)
                {
                    push_back(new PointerValue(value));
                }

                SizeType size() const
                {
                    return _array.size();
                }

                Value* at(const SizeType& i)
                {
                    return i < _array.size() ? _array.at(i) : nullptr;
                }

                template <typename T>
                T* at(const SizeType& i)
                {
                    if (const auto v = at(i))
                        return v->cast<T>();
                    return nullptr;
                }

                void toString(String& dest) const override
                {
                    Value::toString(dest);
                }

                void toString(StringBuilder& dest) const override
                {
                    dest.write('[');
                    bool tag = false;
                    for (const auto type : _array)
                    {
                        if (tag) dest.write(',');
                        type->toString(dest);
                        tag = true;
                    }
                    dest.write(']');
                }

                Value* clone() override
                {
                    const auto v = new ArrayValue();
                    v->copy(this);
                    return v;
                }

            private:
                void copy(const ArrayValue* rhs)
                {
                    RT_GUARD_CHECK_VOID(rhs)
                    for (const auto& value : rhs->_array)
                    {
                        if (value) push_back(value->clone());
                    }
                }
            };
        }  // namespace Dom

        class Visitor
        {
        public:
            virtual ~Visitor() = default;

        protected:
            friend class Parser;

            template <typename... Args>
            void error(const Lex::Token& last, Args&&... args)
            {
                OutputStringStream oss;
                Ts::print(oss, std::forward<Args>(args)...);
                parseError(oss.str(), last);
            }

            virtual Dom::Value* detachRoot() { return nullptr; }

            virtual void parseError(const String& message, const Lex::Token& last) {}

            virtual void objectStarted() {}

            virtual void objectParsed() {}

            virtual void objectFinished() {}

            virtual void keyValueParsed(const String&    key,
                                        const Lex::Type& valueType,
                                        const String&    value) {}

            virtual void arrayStarted() {}

            virtual void arrayFinished() {}

            virtual void arrayParsed() {}

            virtual void stringParsed(const String& value) {}

            virtual void integerParsed(const String& value) {}

            virtual void doubleParsed(const String& value) {}

            virtual void booleanParsed(const String& value) {}

            virtual void pointerParsed(const String& value) {}
        };

        Visitor* defaultVisitor();

        class Parser
        {
        private:
            Dom::Value* _root{nullptr};
            bool        _rootDetached{false};
            Visitor*    _visitor{nullptr};
            bool        _owns{false};
            StackCheck  _guard;

        private:
            void parseObject(Lex::Scanner& scn, Lex::Token& tok)
            {
                RT_GUARD_CHECK_VOID(_visitor)

                if (_guard.test())
                {
                    _visitor->error(tok, "stack limit (", _guard.depth(), ") exceeded");
                    return;
                }
                _visitor->objectStarted();
                Lex::Token t1, t2;

                while (tok.type() != Lex::JT_R_BRACKET)
                {
                    scn.scan(t1);

                    if (t1.type() == Lex::JT_R_BRACKET)
                        break;  // empty

                    if (t1.type() != Lex::JT_STRING)
                    {
                        _visitor->error(t1, "expected string");
                        return;
                    }

                    scn.scan(t2);
                    if (t2.type() != Lex::JT_COLON)
                    {
                        _visitor->error(t2, "expected colon");
                        return;
                    }
                    scn.scan(t2);

                    Lex::Type type = t2.type();
                    switch (type)
                    {
                    case Lex::JT_L_BRACE:
                        parseArray(scn, t2);
                        break;
                    case Lex::JT_L_BRACKET:
                        parseObject(scn, t2);
                        break;
                    case Lex::JT_STRING:
                    case Lex::JT_NULL:
                    case Lex::JT_BOOL:
                    case Lex::JT_NUMBER:
                    case Lex::JT_INTEGER:
                        break;
                    default:
                    case Lex::JT_UNDEFINED:
                    case Lex::JT_COLON:
                    case Lex::JT_COMMA:
                    case Lex::JT_R_BRACE:
                    case Lex::JT_R_BRACKET:
                        _visitor->error(t2, "undefined token in this context");
                        return;
                    case Lex::JT_EOF:
                        _visitor->error(t2, "unexpected end of file");
                        return;
                    }

                    _visitor->keyValueParsed(t1.value(), type, t2.value());

                    scn.scan(tok);
                    if (tok.type() >= Lex::JT_NULL)
                    {
                        _visitor->error(tok, "unexpected end of file");
                        return;
                    }
                }

                _visitor->objectFinished();
                _guard.release();
            }

            void parseArray(Lex::Scanner& scn, Lex::Token& tok)
            {
                RT_GUARD_CHECK_VOID(_visitor)

                if (_guard.test())
                {
                    _visitor->error(tok, "stack limit (", _guard.depth(), ") exceeded");
                    return;
                }

                _visitor->arrayStarted();
                Lex::Token t1;

                while (tok.type() != Lex::JT_R_BRACE)
                {
                    scn.scan(t1);

                    if (t1.type() == Lex::JT_R_BRACE)
                        break;  // empty

                    switch (t1.type())
                    {
                    case Lex::JT_L_BRACE:
                        parseArray(scn, t1);
                        _visitor->arrayParsed();
                        break;
                    case Lex::JT_L_BRACKET:
                        parseObject(scn, t1);
                        _visitor->objectParsed();
                        break;
                    case Lex::JT_STRING:
                        _visitor->stringParsed(t1.value());
                        break;
                    case Lex::JT_NULL:
                        _visitor->pointerParsed(t1.value());
                        break;
                    case Lex::JT_BOOL:
                        _visitor->booleanParsed(t1.value());
                        break;
                    case Lex::JT_NUMBER:
                        _visitor->doubleParsed(t1.value());
                        break;
                    case Lex::JT_INTEGER:
                        _visitor->integerParsed(t1.value());
                        break;
                    case Lex::JT_UNDEFINED:
                    case Lex::JT_COLON:
                    case Lex::JT_COMMA:
                    case Lex::JT_R_BRACE:
                    case Lex::JT_R_BRACKET:
                        _visitor->error(t1, "undefined token in this context");
                        return;
                    default: 
                    case Lex::JT_EOF:
                        _visitor->error({}, "unexpected end of file");
                        return;
                    }

                    scn.scan(tok);
                    if (tok.type() >= Lex::JT_NULL)
                    {
                        _visitor->error(tok, "unexpected end of file");
                        return;
                    }
                }
                _visitor->arrayFinished();
                _guard.release();
            }

            void parseCommon(Lex::Scanner& scanner)
            {
                RT_GUARD_CHECK_VOID(_visitor)
                Lex::Token t0;
                scanner.scan(t0);

                Dom::Value* root;
                if (t0.type() == Lex::JT_L_BRACKET)
                {
                    parseObject(scanner, t0);
                    root = _visitor->detachRoot();
                }
                else if (t0.type() == Lex::JT_L_BRACE)
                {
                    parseArray(scanner, t0);
                    root = _visitor->detachRoot();
                }
                else
                    root = nullptr;

                _root         = root;
                _rootDetached = false;
            }

        public:
            explicit Parser(Visitor* visitor = nullptr) :
                _visitor(visitor ? visitor : defaultVisitor()),
                _owns(visitor == nullptr)
            {
            }

            ~Parser()
            {
                if (!_rootDetached) delete _root;
                _root = nullptr;

                if (_owns) delete _visitor;
                _visitor = nullptr;
            }

            Dom::Value* detachRoot()
            {
                if (_root)
                {
                    Dom::Value* r = _root;
                    _root         = nullptr;
                    _rootDetached = true;
                    return r;
                }
                return nullptr;
            }

            Dom::Value* root() const
            {
                return _root;
            }

            void parse(IStream& stream)
            {
                try
                {
                    _guard.reset();
                    Lex::Scanner scn(&stream);
                    parseCommon(scn);
                }
                catch (Exception& exception)
                {
                    if (_visitor)
                        _visitor->error({}, exception.what());

                    delete _root;
                    _root = nullptr;
                }
                catch (...)
                {
                    delete _root;
                    _root = nullptr;
                }
            }
        };

        namespace Dom
        {
            class Document final : public Visitor
            {
            public:
                typedef Stack<ObjectValue*> ObjectStack;
                typedef Stack<ArrayValue*>  ArrayStack;

            private:
                ObjectStack _obj{JSON_ARRAY_RESERVE};
                ArrayStack  _arr{JSON_ARRAY_RESERVE};
                ObjectStack _fObj{JSON_ARRAY_RESERVE};
                ArrayStack  _fArr{JSON_ARRAY_RESERVE};

            public:
                Document() = default;

                ~Document() override
                {
                    clear();
                }

            private:
                Value* detachRoot() override
                {
                    Value* root = nullptr;
                    if (!_fObj.empty())
                    {
                        root = _fObj.top();
                        _fObj.pop();
                    }
                    else if (!_fArr.empty())
                    {
                        root = _fArr.top();
                        _fArr.pop();
                    }
                    return root;
                }

                void clear()
                {
                    while (!_arr.empty())
                    {
                        delete _arr.top();
                        _arr.pop();
                    }

                    while (!_obj.empty())
                    {
                        delete _obj.top();
                        _obj.pop();
                    }

                    while (!_fObj.empty())
                    {
                        delete _fObj.top();
                        _fObj.pop();
                    }

                    while (!_fArr.empty())
                    {
                        delete _fArr.top();
                        _fArr.pop();
                    }
                }

                void parseError(const String& str, const Lex::Token& last) override
                {
                    Console::println("parse error: ", str);
                    Console::println("   : [", last.value().c_str(), ',', last.type(), ']');
                    clear();
                }

                void objectStarted() override
                {
                    _obj.reserve(_obj.size() + 1);
                    _obj.push(new ObjectValue());
                }

                void objectFinished() override
                {
                    if (!_obj.empty())
                    {
                        _fObj.reserve(_fObj.size() + 1);
                        _fObj.push(_obj.top());
                        _obj.pop();
                    }
                }

                void objectParsed() override
                {
                    if (!_arr.empty() && !_fObj.empty())
                    {
                        _arr.top()->push_back(_fObj.top());
                        _fObj.pop();
                    }
                }

                void arrayStarted() override
                {
                    _arr.push(new ArrayValue());
                }

                void arrayFinished() override
                {
                    if (!_arr.empty())
                    {
                        _fArr.push(_arr.top());
                        _arr.pop();
                    }
                }

                void arrayParsed() override
                {
                    if (!_arr.empty() && !_fArr.empty())
                    {
                        _arr.top()->push_back(_fArr.top());
                        _fArr.pop();
                    }
                }

                void keyValueParsed(const String&    key,
                                    const Lex::Type& valueType,
                                    const String&    value) override
                {
                    if (_obj.empty())
                    {
                        Console::println("no object on the parse stack");
                        return;
                    }

                    ObjectValue* top = _obj.top();

                    Value* obj = nullptr;
                    switch (valueType)
                    {
                    case Lex::JT_L_BRACE:
                        if (!_fArr.empty())
                        {
                            obj = _fArr.top();
                            _fArr.pop();
                        }
                        break;
                    case Lex::JT_L_BRACKET:
                        if (!_fObj.empty())
                        {
                            obj = _fObj.top();
                            _fObj.pop();
                        }
                        break;
                    case Lex::JT_STRING:
                        obj = new StringValue();
                        break;
                    case Lex::JT_NULL:
                        obj = new PointerValue();
                        break;
                    case Lex::JT_BOOL:
                        obj = new BoolValue();
                        break;
                    case Lex::JT_NUMBER:
                        obj = new RealValue();
                        break;
                    case Lex::JT_INTEGER:
                        obj = new IntegerValue();
                        break;
                    case Lex::JT_UNDEFINED:
                    case Lex::JT_COLON:
                    case Lex::JT_COMMA:
                    case Lex::JT_R_BRACE:
                    case Lex::JT_R_BRACKET:
                        break;
                    case Lex::JT_EOF:
                        Console::println("unexpected end of file.");
                        return;
                    default:
                        break;
                    }

                    if (obj != nullptr)
                    {
                        obj->setValue(value);
                        top->insert(key, obj);
                    }
                }

                void handleArrayType(Value* obj, const String& value)
                {
                    if (obj != nullptr)
                    {
                        obj->setValue(value);
                        auto* arrayObject = _arr.top();
                        arrayObject->push_back(obj);
                    }
                }

                void stringParsed(const String& value) override
                {
                    RT_GUARD_CHECK_VOID(!_arr.empty())
                    handleArrayType(new StringValue(), value);
                }

                void integerParsed(const String& value) override
                {
                    RT_GUARD_CHECK_VOID(!_arr.empty())
                    handleArrayType(new IntegerValue(), value);
                }

                void doubleParsed(const String& value) override
                {
                    RT_GUARD_CHECK_VOID(!_arr.empty())
                    handleArrayType(new RealValue(), value);
                }

                void booleanParsed(const String& value) override
                {
                    RT_GUARD_CHECK_VOID(!_arr.empty())
                    handleArrayType(new BoolValue(), value);
                }

                void pointerParsed(const String& value) override
                {
                    RT_GUARD_CHECK_VOID(!_arr.empty())
                    handleArrayType(new PointerValue(), value);
                }
            };

        }  // namespace Dom

        inline Visitor* defaultVisitor()
        {
            return new Dom::Document();
        }

    }  // namespace Internal

    namespace Print
    {
        class Printer
        {
        private:
            class Private
            {
            private:
                StringBuilder _buffer;
                int           _depth;
                int           _indent{4};

            public:
                Private() :
                    _depth(0)
                {
                    _buffer.setStrategy(ALLOC_N_BYTE_BLOCK, 128);
                }

                void setIndent(const int spc)
                {
                    _indent = spc;
                }

                void writeSpace()
                {
                    for (int i = 0; i < _depth; ++i)
                    {
                        for (int j = 0; j < _indent; ++j)
                            _buffer.write(' ');
                    }
                }

                void writeObject(const Internal::Dom::ObjectValue* root)
                {
                    RT_GUARD_CHECK_VOID(root)
                    _buffer.write('{');
                    _buffer.write('\n');
                    _depth++;

                    bool first = true;
                    for (const auto& it : *root)
                    {
                        if (!first)
                        {
                            _buffer.write(',');
                            _buffer.write('\n');
                        }
                        else
                            first = false;

                        writeSpace();
                        _buffer.write('"');
                        _buffer.write(it.first);
                        _buffer.write('"');
                        _buffer.write(':');
                        _buffer.write(' ');

                        if (it.second->isArray())
                        {
                            // handle array
                            writeArray(it.second->array());
                        }
                        else if (it.second->isObject())
                        {
                            // handle object
                            ++_depth;
                            writeObject(it.second->object());
                            --_depth;
                        }
                        else
                            it.second->toString(_buffer);
                    }
                    _buffer.write('\n');
                    if (_depth-- > 0)
                        writeSpace();
                    _buffer.write('}');
                }

                void writeArray(Internal::Dom::ArrayValue* array)
                {
                    _buffer.write('[');

                    for (U32 i = 0; i < array->size(); ++i)
                    {
                        Internal::Dom::Value* idx = array->at(i);

                        const bool split = idx->isObject() || idx->isArray();
                        if (i > 0)
                        {
                            _buffer.write(',');
                            if (split || i % 20 == 19)
                            {
                                _buffer.write('\n');
                                writeSpace();
                            }
                        }
                        if (idx->isObject())
                            writeObject(idx->object());
                        else if (idx->isArray())
                            writeArray(idx->array());
                        else
                            idx->toString(_buffer);
                    }
                    _buffer.write(']');
                }

                void write(Internal::Dom::Value* obj)
                {
                    if (obj)
                    {
                        if (obj->isObject())
                            writeObject(obj->object());
                        else if (obj->isArray())
                            writeArray(obj->array());
                    }
                }

                void getBuffer(String& dest) const
                {
                    _buffer.toString(dest);
                }
            };

            Private* _private;

        public:
            Printer()
            {
                _private = new Private();
            }

            ~Printer()
            {
                delete _private;
            }

            void setIndent(const int sp) const
            {
                RT_GUARD_CHECK_VOID(_private)
                _private->setIndent(sp);
            }

            void toStream(OStream& dest, Internal::Dom::Value* obj) const
            {
                RT_GUARD_CHECK_VOID(_private && obj)
                _private->write(obj);
                String buffer;
                _private->getBuffer(buffer);
                dest.write(buffer.c_str(), (std::streamsize)buffer.size());
            }

            void toString(String& dest, Internal::Dom::Value* obj) const
            {
                RT_GUARD_CHECK_VOID(_private && obj)
                _private->write(obj);
                _private->getBuffer(dest);
            }

            static void formatted(Internal::Dom::Value* val,
                                  bool&                 cache,
                                  String&               buffer,
                                  const int             space = 4)
            {
                RT_GUARD_CHECK_VOID(val)
                if (cache)
                {
                    cache = false;
                    const Printer p;
                    p.setIndent(space);
                    p.toString(buffer, val);
                }
            }

            static void compact(const Internal::Dom::Value* val,
                                bool&                       cache,
                                String&                     buffer)
            {
                RT_GUARD_CHECK_VOID(val)
                if (cache)
                {
                    cache = false;
                    val->toString(buffer);
                }
            }
        };

    }  // namespace Print

    class Dictionary;
    class MixedArray;

    class Value
    {
    protected:
        Internal::Dom::Value* _val{nullptr};

    public:
        Value() = default;

        explicit Value(Internal::Dom::Value* val) :
            _val(val)
        {
        }

        bool isObject() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isObject();
        }

        Dictionary object() const;

        bool isArray() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isArray();
        }

        MixedArray array() const;

        bool isString() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isString();
        }

        String string() const
        {
            RT_GUARD_CHECK_RET(_val, {})
            return _val->value();
        }

        bool isBoolean() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isBoolean();
        }

        bool boolean() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->boolean();
        }

        bool isReal() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isReal();
        }

        double real() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->real();
        }

        bool isInteger() const
        {
            RT_GUARD_CHECK_RET(_val, false)
            return _val->isInteger();
        }

        int integer() const
        {
            RT_GUARD_CHECK_RET(_val, 0)
            return (int)Clamp<int64_t>(_val->integer(), INT32_MIN, INT32_MAX);
        }
    };

    template <typename T>
    class ApiType
    {
    protected:
        using ValueType = T;
        using SelfType  = ApiType<T>;

        ValueType*     _value{nullptr};
        mutable String _serialization;
        mutable bool   _mark{true};

        void clear()
        {
            delete _value;
            _value = nullptr;
        }

        static ValueType* clone(const ValueType* oth)
        {
            return new ValueType(oth);
        }

        static ValueType* create()
        {
            return new ValueType();
        }

    public:
        ApiType() :
            _value(create())
        {
        }

        ApiType(const SelfType& v) :
            _value(clone(v._value))
        {
        }

        explicit ApiType(ValueType* v, const bool own = false) :
            _value(own ? v : clone(v))
        {
            // Here the object is always owned.
            // But the own=true case, allows the object to be
            // allocated outside of this constructor.
        }

        ~ApiType()
        {
            clear();
        }

        SelfType& operator=(const SelfType& rhs)
        {
            if (this != &rhs)
            {
                if (_value != rhs._value)
                {
                    clear();
                    _value = clone(rhs._value);
                }
            }
            return *this;
        }

        const String& compact() const
        {
            Print::Printer::compact(_value, _mark, _serialization);
            return _serialization;
        }

        const String& formatted(const int space = 4) const
        {
            Print::Printer::formatted(_value, _mark, _serialization, space);
            return _serialization;
        }


        ValueType* value() const
        {
            return _value;
        }
    };

    class Dictionary : public ApiType<Internal::Dom::ObjectValue>
    {
    public:
        using BaseType = ApiType<Internal::Dom::ObjectValue>;

    public:
        Dictionary() :
            BaseType()
        {
        }

        explicit Dictionary(Internal::Dom::ObjectValue* v, const bool own = false) :
            BaseType(v, own)
        {
        }

        Dictionary& operator=(const Dictionary& rhs)
        {
            BaseType::operator=(rhs);
            return *this;
        }

        template <typename T>
        void insert(const String& key, const T& val) const
        {
            RT_GUARD_CHECK_VOID(_value)
            _mark = true;
            _value->insert(key, val);
        }

        template <typename T>
        void insert(const String& key, const std::initializer_list<T>& val) const
        {
            RT_GUARD_CHECK_VOID(_value)
            _mark = true;

            const auto array = new Internal::Dom::ArrayValue();
            for (const auto v : val)
                array->push_back(v);
            _value->insert(key, array);
        }



        void insert(const String& key, const Dictionary& dict) const
        {
            RT_GUARD_CHECK_VOID(_value && dict._value)
            _mark = true;
            _value->insert(key, dict._value->clone());
        }

        void insert(const String& key, const MixedArray& arr) const;

        int integer(const String& key, const int& def = -1) const
        {
            RT_GUARD_CHECK_RET(_value, def)
            return (int)_value->integer(key, (int64_t)def);
        }

        String string(const String& key, const String& def = {}) const
        {
            RT_GUARD_CHECK_RET(_value, def)
            return _value->string(key, def);
        }

        void insertCode(const String& key, const String& code) const
        {
            RT_GUARD_CHECK_VOID(_value)

            Internal::Parser  p;
            InputStringStream is(code);
            p.parse(is);

            if (p.root())
            {
                _mark = true;
                _value->insert(key, p.root()->clone());
            }
        }

        bool contains(const String& key) const
        {
            RT_GUARD_CHECK_RET(_value, false)
            return _value->contains(key);
        }

        bool empty() const
        {
            RT_GUARD_CHECK_RET(_value, true)
            return _value->empty();
        }

        Value get(const String& key) const
        {
            RT_GUARD_CHECK_RET(_value, {})
            return Value{_value->get(key)};
        }
    };

    class MixedArray final : public ApiType<Internal::Dom::ArrayValue>
    {
    public:
        using BaseType = ApiType<Internal::Dom::ArrayValue>;
        using SizeType = Internal::Dom::ArrayValue::SizeType;

    public:
        MixedArray() :
            BaseType()
        {
        }

        explicit MixedArray(Internal::Dom::ArrayValue* v, const bool own = false) :
            BaseType(v, own)
        {
        }

        MixedArray& operator=(const MixedArray& rhs)
        {
            BaseType::operator=(rhs);
            return *this;
        }

        template <typename T>
        void push(const T& val) const
        {
            RT_GUARD_CHECK_VOID(_value)
            _mark = true;
            _value->push_back(val);
        }

        void push(const Dictionary& val) const
        {
            RT_GUARD_CHECK_VOID(_value && val.value())
            _mark = true;
            _value->push_back(val.value()->clone());
        }


        void pushCode(const String& code) const
        {
            RT_GUARD_CHECK_VOID(_value)
            Internal::Parser  p;
            InputStringStream is(code);
            p.parse(is);

            if (p.root())
            {
                _mark = true;
                _value->push_back(p.root()->clone());
            }
        }

        bool empty() const
        {
            RT_GUARD_CHECK_RET(_value, true)
            return _value->size() < 1;
        }

        SizeType size() const
        {
            RT_GUARD_CHECK_RET(_value, 0)
            return _value->size();
        }

        int sizeI() const
        {
            return (int)Clamp<uint64_t>(size(), 0, 0x7FFFFFFF);
        }

        Value at(const SizeType& idx) const
        {
            RT_GUARD_CHECK_RET(_value && idx < size(), {})
            return Value{_value->at(idx)};
        }
    };

    class Document final
    {
    private:
        Dictionary _dictionary;
        MixedArray _array;

        void attach(Internal::Dom::Value* value)
        {
            RT_GUARD_CHECK_VOID(value)
            if (value->isObject())
                _dictionary = Dictionary(value->cast<Internal::Dom::ObjectValue>(), true);
            else if (value->isArray())
                _array = MixedArray(value->cast<Internal::Dom::ArrayValue>(), true);
        }

    public:
        ~Document() = default;

        Dictionary& dictionary();

        const Dictionary& dictionary() const;

        MixedArray& array();

        const MixedArray& array() const;

        void load(IStream& stream)
        {
            Internal::Parser p;
            p.parse(stream);
            attach(p.detachRoot());
        }

        void load(const String& text)
        {
            InputStringStream iss(text);
            Internal::Parser  p;
            p.parse(iss);
            attach(p.detachRoot());
        }
    };

    inline Dictionary Value::object() const
    {
        RT_GUARD_CHECK_RET(_val, {})
        return Dictionary(_val->cast<Internal::Dom::ObjectValue>());
    }

    inline MixedArray Value::array() const
    {
        RT_GUARD_CHECK_RET(_val, {})
        return MixedArray(_val->cast<Internal::Dom::ArrayValue>());
    }

    inline void Dictionary::insert(const String& key, const MixedArray& arr) const
    {
        RT_GUARD_CHECK_VOID(_value && arr.value())
        _mark = true;
        _value->insert(key, arr.value()->clone());
    }

    inline Dictionary& Document::dictionary()
    {
        return _dictionary;
    }

    inline const Dictionary& Document::dictionary() const
    {
        return _dictionary;
    }

    inline MixedArray& Document::array()
    {
        return _array;
    }

    inline const MixedArray& Document::array() const
    {
        return _array;
    }

}  // namespace Rt2::Json