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
    template <typename T, U16 StackSize = 0x40>
    class ListBinaryTree
    {
    public:
        RT_DECLARE_TYPE(T)

    public:
        class Node
        {
        private:
            friend class ListBinaryTree;

            Node*     _left{nullptr};
            Node*     _right{nullptr};
            ValueType _data{};

        public:
            Node() = default;

            explicit Node(ConstValueType v) :
                _data(v)
            {
            }

            ~Node()
            {
                destruct();
            }

            Node* left()
            {
                return _left;
            }

            Node* right()
            {
                return _right;
            }

            ReferenceType data()
            {
                return _data;
            }

        private:
            void destruct()
            {
                delete _left;
                _left = nullptr;

                delete _right;
                _right = nullptr;
            }
        };

        RT_DECLARE_NAMED_TYPE(Node)

        using Iterator         = SimpleArray<T>;
        using ArrayPointerType = typename Iterator::PointerType;

    private:
        NodePointerType            _root{nullptr};
        size_t                     _size{0};
        Iterator                   _array;
        FixedStackGuard<StackSize> _guard;

        ListBinaryTree(const ListBinaryTree& rhs) = default;

    public:
        ListBinaryTree() = default;

        ~ListBinaryTree()
        {
            clear();
        }

        void clear()
        {
            delete _root;
            _root = nullptr;
            _array.clear();
        }

        void insert(ConstReferenceType val)
        {
            if (_root == nullptr)
            {
                _root = new Node(val);
                _size++;
            }
            else
            {
                _guard.reset();
                insertRecursive(_root, val);
                if (_guard.isValid())
                    _size++;
            }
        }

        bool find(ConstReferenceType val) const
        {
            if (_root == nullptr)
                return false;
            return findNonRecursive(val);
        }

        bool findNonRecursive(ConstReferenceType val) const
        {
            if (_root == nullptr)
                return false;

            NodePointerType node = _root;
            while (node != nullptr)
            {
                if (node->_data == val)
                    return true;
                if (node->_data < val)
                    node = node->_right;
                else
                    node = node->_left;
            }
            return false;
        }

        bool findNonRecursive(ReferenceType out, ConstReferenceType val) const
        {
            if (_root == nullptr)
                return false;

            NodePointerType node = _root;
            while (node != nullptr)
            {
                if (node->_data == val)
                {
                    out = node->_data;
                    return true;
                }
                if (node->_data < val)
                    node = node->_right;
                else
                    node = node->_left;
            }
            return false;
        }

        void erase(ConstReferenceType val)
        {
            if (_root == nullptr)
                return;

            _guard.reset();
            _root = eraseRecursive(_root, val);
        }

        ArrayPointerType begin()
        {
            if (_array.size() != _size)
                makeArray(false);

            if (!_array.empty())
                return _array.begin();

            return nullptr;
        }

        ArrayPointerType end()
        {
            if (_array.size() != _size)
                makeArray(false);

            if (!_array.empty())
                return _array.end();
            return nullptr;
        }

        void makeArray(const bool descending = false)
        {
            _guard.reset();
            _array.resizeFast(0);
            populate(_root, descending);
        }

        NodePointerType minimum(NodePointerType node = nullptr)
        {
            _guard.reset();
            return minRecursive(node ? node : _root);
        }

        NodePointerType maximum(NodePointerType node = nullptr)
        {
            _guard.reset();
            return maxRecursive(node ? node : _root);
        }

        NodePointerType root()
        {
            return _root;
        }

        NodePointerType left()
        {
            return _root ? _root->_left : nullptr;
        }

        NodePointerType right()
        {
            return _root ? _root->_right : nullptr;
        }

        size_t size() const
        {
            return _size;
        }

    private:
        NodePointerType minRecursive(NodePointerType node)
        {
            if (_guard.test())
                return node;
            if (node && node->_left)
                return minRecursive(node->_left);
            return node;
        }

        NodePointerType maxRecursive(NodePointerType node)
        {
            if (_guard.test())
                return node;

            if (node && node->_right)
                return maxRecursive(node->_right);
            return node;
        }

        bool insertRecursive(NodePointerType node, ConstReferenceType val)
        {
            if (_guard.test())
                return false;

            if (val < node->_data)
            {
                if (node->_left)
                    return insertRecursive(node->_left, val);

                node->_left = new Node(val);
                return true;
            }

            if (node->_right)
                return insertRecursive(node->_right, val);

            node->_right = new Node(val);
            return true;
        }

        NodePointerType findRecursive(NodePointerType node, ConstReferenceType val) const
        {
            if (!node || _guard.test())
                return nullptr;

            if (node->_data == val)
                return node;

            if (node->_data > val)
                return findRecursive(node->_left, val);

            return findRecursive(node->_right, val);
        }

        NodePointerType eraseRecursive(NodePointerType node, ConstReferenceType val)
        {
            if (!node || _guard.test())
                return nullptr;

            if (node->_data > val)
                node->_left = eraseRecursive(node->_left, val);
            else if (node->_data < val)
                node->_right = eraseRecursive(node->_right, val);
            else
                return detach(node);
            return node;
        }

        NodePointerType detach(NodePointerType node)
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
                NodePointerType local = node->_left == nullptr ? node->_right : node->_left;
                node->_left = node->_right = nullptr;

                --_size;
                delete node;
                node = local;
            }
            else
            {
                _guard.reset();
                NodePointerType cur = minRecursive(node->_right);

                _guard.reset();
                node->_data  = cur->_data;
                node->_right = eraseRecursive(node->_right, cur->_data);
            }
            return node;
        }

        void populate(NodePointerType node, bool descending)
        {
            if (!node)
                return;

            populate(descending ? node->_right : node->_left, descending);
            _array.push_back(node->_data);
            populate(descending ? node->_left : node->_right, descending);
        }
    };

}  // namespace Rt2
