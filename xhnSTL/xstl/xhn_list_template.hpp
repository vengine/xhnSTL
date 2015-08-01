/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LIST_TEMPLATE_H
#define XHN_LIST_TEMPLATE_H
#include "common.h"
#include "etypes.h"
#include "emem.hpp"
namespace xhn
{
    template <typename LIST_NODE>
    class list_base : public RefObject
    {
    protected:
        LIST_NODE* head;
        LIST_NODE* tail;
    public:
        list_base()
        : head(NULL)
        , tail(NULL)
        {}
        void ring_push_back(LIST_NODE* node)
        {
            if (node == tail)
                return;
            if (node == head)   { head = node->next; }
            if (node->prev)     { node->prev->next = node->next; }
            if (node->next)     { node->next->prev = node->prev; }
            node->next = NULL;
            node->prev = tail;
            if (tail)
                tail->next = node;
            tail = node;
            if (!head)
                head = node;
        }
        void ring_push_front(LIST_NODE* node)
        {
            if (node == head)
                return;
            if (node == tail)   { tail = node->next; }
            if (node->prev)     { node->prev->next = node->next; }
            if (node->next)     { node->next->prev = node->prev; }
            node->next = head;
            node->prev = NULL;
            if (head)
                head->prev = node;
            head = node;
            if (!tail)
                tail = node;
        }
        void erase_node(LIST_NODE* node)
        {
            if (node == head) { head = node->next; }
            if (node == tail) { tail = node->prev; }
            if (node->prev)   { node->prev->next = node->next; }
            if (node->next)   { node->next->prev = node->prev; }
            node->prev = NULL;
            node->next = NULL;
        }
    };
}
#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
