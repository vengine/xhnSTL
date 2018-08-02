#include "xhn_memory.hpp"

WeakNode* WeakNodeList::NewNode_NotLocked(RefObjectBase* ptr) {
    WeakNode* ret = VNEW
    WeakNode(ptr);
    ///Insert(ret);
    if (m_tail) {
        m_tail->m_next = ret;
        ret->m_prev = m_tail;
        m_tail = ret;
    }
    else {
        m_head = m_tail = ret;
    }
    return ret;
}

void WeakNodeList::DeleteNode_HalfLocked(WeakNode* node)
{
    RefSpinLock::Instance selfInst = m_lock.Lock();
    if (node == m_head) { m_head = node->m_next; }
    if (node == m_tail) { m_tail = node->m_prev; }
    if (node->m_prev) { node->m_prev->m_next = node->m_next; }
    if (node->m_next) { node->m_next->m_prev = node->m_prev; }
    if (node->m_weakPtr) {
        node->m_weakPtr->m_node = NULL;
        node->m_weakPtr->m_nodeList = NULL;
    }
    delete node;
}

void WeakNodeList::DeleteAllNodes_Locked() {
    RefSpinLock::Instance selfInst = m_lock.Lock();
    WeakNode* node = m_head;
    while (node) {
        WeakNode* next = node->m_next;
        if (node->m_weakPtr) {
            RefSpinLock::Instance inst = node->m_weakPtr->m_weakLock.Lock();
            node->m_weakPtr->m_node = NULL;
            node->m_weakPtr->m_nodeList = NULL;
        }
        delete node;
        node = next;
    }
    m_head = m_tail = NULL;
}
/**
void WeakNodeList::Insert(WeakNode* node) {
    RefSpinLock::Instance selfInst = m_lock.Lock();
    if (m_tail) {
        m_tail->m_next = node;
        node->m_prev = m_tail;
        m_tail = node;
    }
    else {
        m_head = m_tail = node;
    }
}
 **/
/**
void WeakNodeList::Erase(WeakNode* node) {
    RefSpinLock::Instance selfInst = m_lock.Lock();
    if (node == m_head) { m_head = node->m_next; }
    if (node == m_tail) { m_tail = node->m_prev; }
    if (node->m_prev) { node->m_prev->m_next = node->m_next; }
    if (node->m_next) { node->m_next->m_prev = node->m_prev; }
    if (node->m_weakPtr) {
        RefSpinLock::Instance inst = node->m_weakPtr->m_lock.Lock();
        node->m_weakPtr->m_node = NULL;
        node->m_weakPtr->m_nodeList = NULL;
    }
}
 **/
void WeakNodeList::SetWeakPtr(WeakNode* node,
                              WeakPtrBase* ptr)
{
    node->m_weakPtr = ptr;
}
