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

#include "StackGuard.h"
#include "Utils/Array.h"
#include "Utils/Definitions.h"

namespace Rt2
{
    template <typename Key,
              typename Value,
              U16 StackSize    = 0x40,
              int ArrayOptions = AOP_SIMPLE_TYPE>
    class PairedBinaryTree
    {
    public:
        struct Pair
        {
            Key   key;
            Value value;

            bool operator<(const Pair& rhs) const { return key < rhs.key; }
            bool operator>(const Pair& rhs) const { return key > rhs.key; }
            bool operator==(const Pair& rhs) const { return key == rhs.key; }
        };
        using Guard = FixedStackGuard<StackSize>;

    public:
        class Node
        {
        private:
            friend class PairedBinaryTree;

            Node* _left{nullptr};
            Node* _right{nullptr};

            Pair _data{};

            ~Node() { destruct(); }

            Node() = default;

            explicit Node(const Pair& v) :
                _data(v)
            {
            }

            void destruct()
            {
                delete _left;
                _left = nullptr;

                delete _right;
                _right = nullptr;
            }

        public:
            Node* left() { return _left; }

            Node* right() { return _right; }

            Pair& pair() { return _data; }

            Value& value() { return _data.value; }

            Key& key() { return _data.key; }
        };

        using Keys    = Array<Key, ArrayOptions>;
        using Values  = Array<Value, ArrayOptions>;
        using KvPairs = Array<Pair, ArrayOptions>;

        using ValuePointerType = typename Values::PointerType;
        using KeyPointerType   = typename Keys::PointerType;
        using KvpPointerType   = typename KvPairs::PointerType;

        using InsertionPoint = std::function<void(Node* parent, Node* node)>;
        using ValueCallback  = std::function<int(const Value&)>;

    private:
        Node*  _root{nullptr};
        size_t _size{0};
        Values _array;
        Guard  _guard;

    public:
        PairedBinaryTree()                            = default;
        PairedBinaryTree(const PairedBinaryTree& rhs) = delete;

        ~PairedBinaryTree()
        {
            clear();
        }

        void clear()
        {
            delete _root;
            _root = nullptr;
            if (ArrayOptions & AOP_SIMPLE_TYPE)
                _array.resizeFast(0);
            else
                _array.clear();
        }

        void insert(const Key& key, const Value& val, const InsertionPoint& event = nullptr)
        {
            if (_root == nullptr)
            {
                _root = new Node({key, val});
                _size++;
            }
            else
            {
                _guard.reset();
                insertRecursive(_root, {key, val}, event);
                if (_guard.isValid())
                    _size++;
            }
        }

        bool contains(const Key& key) const
        {
            if (_root == nullptr) return false;
            return findNode(key) != nullptr;
        }

        const Value& get(const Key& key, const Value& error = {}) const
        {
            Node* node = findNode(key);
            return node == nullptr ? error : node->value();
        }

        Node* findNode(const Key& key) const
        {
            if (_root == nullptr) return nullptr;
            Node* node = _root;
            while (node != nullptr)
            {
                if (node->_data.key == key) return node;

                if (node->_data.key < key)
                    node = node->_right;
                else
                    node = node->_left;
            }
            return nullptr;
        }

        void erase(const Key& key)
        {
            if (_root == nullptr) return;
            _guard.reset();
            _root = eraseRecursive(_root, key);
        }

        void keys(Keys& dest, const bool descending = false)
        {
            decompose<Key>(dest, &Node::key, descending);
        }

        void values(Values& dest, const bool descending = false)
        {
            decompose<Value>(dest, &Node::value, descending);
        }

        void values(const ValueCallback& dest, const bool descending = false)
        {
            decompose<Value>(dest, &Node::value, descending);
        }

        void keyValuePairs(KvPairs& dest, const bool descending = false)
        {
            decompose<Pair>(dest, &Node::pair, descending);
        }

        ValuePointerType begin()
        {
            if (_array.size() != _size)
                decompose<Value>(_array, &Node::value);
            if (!_array.empty()) return _array.begin();
            return nullptr;
        }

        ValuePointerType end()
        {
            if (_array.size() != _size)
                decompose<Value>(_array, &Node::value);
            if (!_array.empty()) return _array.end();
            return nullptr;
        }

        Node* minimum(Node* node = nullptr)
        {
            _guard.reset();
            return minRecursive(node ? node : _root);
        }

        Node* maximum(Node* node = nullptr)
        {
            _guard.reset();
            return maxRecursive(node ? node : _root);
        }

        Node* root() { return _root; }

        Node* left() { return _root ? _root->_left : nullptr; }

        Node* right() { return _root ? _root->_right : nullptr; }

        size_t size() const { return _size; }

        bool empty() const { return _size < 1; }

    private:
        template <typename U>
        void decompose(
            Array<U, ArrayOptions>& dest,
            U& (Node::*accessor)(),
            const bool descending = false)
        {
            _guard.reset();
            if (ArrayOptions & AOP_SIMPLE_TYPE)
                dest.resizeFast(0);
            else
                dest.clear();
            if (!accessor) return;
            populate<U>(dest, _root, descending, accessor);
        }

        template <typename U>
        void populate(Array<U, ArrayOptions>& dest, Node* node, bool descending, U& (Node::*accessor)())
        {
            if (!node) return;
            populate<U>(dest, descending ? node->_right : node->_left, descending, accessor);
            dest.push_back((node->*accessor)());
            populate<U>(dest, descending ? node->_left : node->_right, descending, accessor);
        }

        template <typename U>
        void decompose(
            const std::function<int(const U&)>& callback, U& (Node::*accessor)(), const bool descending = false)
        {
            _guard.reset();
            if (!accessor) return;
            populate<U>(callback, _root, descending, accessor);
        }

        template <typename U>
        void populate(const std::function<int(const U&)>& callback, Node* node, bool descending, U& (Node::*accessor)())
        {
            if (!node) return;
            populate<U>(callback, descending ? node->_right : node->_left, descending, accessor);
            if (callback((node->*accessor)()))
                return;
            populate<U>(callback, descending ? node->_left : node->_right, descending, accessor);
        }

        Node* minRecursive(Node* node)
        {
            if (_guard.test())
                return node;
            if (node && node->_left)
                return minRecursive(node->_left);
            return node;
        }

        Node* maxRecursive(Node* node)
        {
            if (_guard.test())
                return node;

            if (node && node->_right)
                return maxRecursive(node->_right);
            return node;
        }

        bool insertRecursive(Node* node, const Pair& val, const InsertionPoint& event)
        {
            if (_guard.test())
                return false;

            if (val < node->_data)
            {
                if (node->_left)
                    return insertRecursive(node->_left, val, event);

                node->_left = new Node(val);
                if (event)
                    event(node, node->_left);
                return true;
            }

            if (node->_right)
                return insertRecursive(node->_right, val, event);

            node->_right = new Node(val);

            if (event)
                event(node, node->_right);
            return true;
        }

        Node* findRecursive(Node* node, const Pair& val) const
        {
            if (!node || _guard.test())
                return nullptr;

            if (node->_data == val)
                return node;

            if (node->_data > val)
                return findRecursive(node->_left, val);

            return findRecursive(node->_right, val);
        }

        Node* eraseRecursive(Node* node, const Key& val)
        {
            if (!node || _guard.test())
                return nullptr;

            if (node->_data.key > val)
                node->_left = eraseRecursive(node->_left, val);
            else if (node->_data.key < val)
                node->_right = eraseRecursive(node->_right, val);
            else
                return detach(node);
            return node;
        }

        Node* detach(Node* node)
        {
            RT_ASSERT(node)

            if (node->_left == nullptr && node->_right == nullptr)
            {
                --_size;
                delete node;
                node = nullptr;
            }
            else if (node->_left == nullptr || node->_right == nullptr)
            {
                Node* local = node->_left == nullptr ? node->_right : node->_left;
                node->_left = node->_right = nullptr;

                --_size;
                delete node;
                node = local;
            }
            else
            {
                _guard.reset();
                Node* cur = minRecursive(node->_right);

                _guard.reset();
                node->_data  = cur->_data;
                node->_right = eraseRecursive(node->_right, cur->_data.key);
            }
            return node;
        }
    };

}  // namespace Rt2
