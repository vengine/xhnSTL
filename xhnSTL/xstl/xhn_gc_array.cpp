#include "xhn_gc_array.h"

xhn::GCArray::GCArray()
{
    m_memory = Alloc(sizeof(GCObject*) * 8);
    m_capacity = 8;
    m_size = 0;
    gc->Attach(gcNode, m_memory->gcNode);
}
void xhn::GCArray::GrowUp(euint newCapacity)
{
    GCMemory* tmp = Alloc(sizeof(GCObject*) * newCapacity);
    GCObject** dst = (GCObject**)tmp->Get();
    GCObject** src = (GCObject**)m_memory->Get();
    for (euint i = 0; i < m_capacity; i++) {
        dst[i] = src[i];
    }
    m_capacity = newCapacity;
    gc->Detach(gcNode, m_memory->gcNode);
    gc->Attach(gcNode, tmp->gcNode);
    m_memory = tmp;
}
void xhn::GCArray::AddObject(GCObject* object)
{
    if (m_size == m_capacity) {
        GrowUp(m_capacity * 2);
    }
    GCObject** array = (GCObject**)m_memory->Get();
    array[m_size] = object;
    gc->Attach(gcNode, object->gcNode);
    m_size++;
}
euint xhn::GCArray::Size() const
{
    return m_size;
}
GCObject* xhn::GCArray::GetObject(euint index)
{
    if (index < m_size) {
        return ((GCObject**)m_memory->Get())[index];
    }
    else {
        return NULL;
    }
}
void xhn::GCArray::RemoveObject(euint index)
{
    if (index < m_size) {
        GCObject** array = (GCObject**)m_memory->Get();
        gc->Detach(gcNode, array[index]->gcNode);
        euint numMoved = m_size - index - 1;
        euint to = index;
        euint from = to + 1;
        for (euint i = 0; i < numMoved; i++, to++, from++) {
            array[to] = array[from];
        }
        m_size--;
    }
}
void xhn::GCArray::InsertObject(euint index, GCObject* object)
{
    if (index < m_size) {
        if (m_size == m_capacity) {
            GrowUp(m_capacity * 2);
        }
        GCObject** array = (GCObject**)m_memory->Get();
        euint numMoved = m_size - index;
        euint to = m_size;
        euint from = to - 1;
        for (euint i = 0; i < numMoved; i++, to--, from--) {
            array[to] = array[from];
        }
        array[index] = object;
        gc->Attach(gcNode, object->gcNode);
        m_size++;
    }
    else {
        AddObject(object);
    }
}