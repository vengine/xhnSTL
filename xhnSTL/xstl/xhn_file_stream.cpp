//
//  xhn_file_stream.cpp
//  7z
//
//  Created by 徐 海宁 on 13-11-5.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#include "xhn_file_stream.hpp"
#include "xhn_set.hpp"
#include "xhn_list.hpp"
#include <wchar.h>
ImplementRTTI(xhn::folder_information, xhn::operation_data);

///==========================================================================///
///  file_block                                                              ///
///==========================================================================///

xhn::file_block::file_block()
: prev(NULL)
, next(NULL)
, buffered_size(0)
, stream(NULL)
{
    buffer.convert<xhn::void_vector::FCharProc>(1);
    buffer.resize(BLOCK_SIZE);
}
xhn::file_block::~file_block()
{
    if (stream) {
        MutexLock::Instance inst = stream->lock.Lock();
        euint64 size = stream->get_size();
        if (begin_addr >= size) {
            euint64 fill_size = begin_addr - size;
            if (fill_size > 0) {
                stream->set_pos(size);
                euint64 num_blocks = fill_size / BLOCK_SIZE;
                for (euint64 i = 0; i < num_blocks; i++) {
                    stream->write((const euint8*)buffer.get(), BLOCK_SIZE);
                }
                if (fill_size % BLOCK_SIZE) {
                    stream->write((const euint8*)buffer.get(), fill_size % BLOCK_SIZE);
                }
            }
        }
        stream->set_pos(begin_addr);
        stream->write((const euint8*)buffer.get(), buffered_size);
    }
}
void xhn::file_block::write(euint64 offs, const euint8* buf, euint64 size)
{
    memcpy(buffer[offs], buf, size);
    if (offs + size > buffered_size)
        buffered_size = offs + size;
}

/// 这里不应该读越界
/// |---------stream size-----|-------------------------|
/// |---------buffered size---|
/// |---------read size-------|--incremented size--|
void xhn::file_block::read(euint64 offs, euint8* buf, euint64 size)
{
    if (offs + size > buffered_size) {
        if (stream) {
            MutexLock::Instance inst = stream->lock.Lock();
            euint64 stream_size = stream->get_size();
            if (stream_size > begin_addr + offs) {
                stream->set_pos(begin_addr + offs);
                euint64 incremented_size = offs + size - buffered_size;
                stream->read((euint8*)buffer[buffered_size], incremented_size);
            }
        }
        buffered_size = offs + size;
    }
    memcpy(buf, buffer[offs], size);
}

void xhn::file_block::init()
{
    MutexLock::Instance inst = stream->lock.Lock();
    euint64 stream_size = stream->get_size();
    if (stream_size > begin_addr) {
        buffered_size = stream_size - begin_addr;
        if (buffered_size > BLOCK_SIZE)
            buffered_size = BLOCK_SIZE;
        stream->set_pos(begin_addr);
        stream->read((euint8*)buffer.get(), buffered_size);
    }
}

euint8* xhn::file_block::access(euint64 offs, euint64 size)
{
    if (offs + size > buffered_size) {
        buffered_size = offs + size;
    }
    return (euint8*)buffer[offs];
}

const euint8* xhn::file_block::access(euint64 offs, euint64 size) const
{
    return (const euint8*)buffer[offs];
}

///==========================================================================///
///  file_stream                                                             ///
///==========================================================================///

void xhn::file_stream::synchronize_file()
{
    set<euint64> blocks;
    file_block* node = head;
    while (node) {
        blocks.insert(node->begin_addr);
        node = node->next;
    }
    set<euint64>::iterator iter = blocks.begin();
    set<euint64>::iterator end = blocks.end();
    for (; iter != end; iter++) {
        file_block_tree.remove(*iter);
    }
}

xhn::file_block* xhn::file_stream::access(file_stream* file, euint64 pos)
{
    if (file->cache0 && pos >= file->cache0->begin_addr && pos <= file->cache0->end_addr) {
        return file->cache0;
    }
    else if (file->cache1 && pos >= file->cache1->begin_addr && pos <= file->cache1->end_addr) {
        file_block* tmp = file->cache0;
        file->cache0 = file->cache1;
        file->cache1 = tmp;
        return file->cache0;
    }
    file_block* block = NULL;
    euint64 beginAddr = pos / BLOCK_SIZE * BLOCK_SIZE;
    {
        SpinLock::Instance inst = file->spin_lock.Lock();
        block = file->file_block_tree.find(pos);
        if (!block) {
            if (file->file_block_count == MAX_BLOCKS) {
                file_block* node = file->tail;
                if (node) {
                    if (node == file->head) { file->head = node->next; }
                    if (node == file->tail) { file->tail = node->prev; }
                    if (node->prev)   { node->prev->next = node->next; }
                    if (node->next)   { node->next->prev = node->prev; }
                    file->file_block_tree.remove(node->begin_addr);
                    file->file_block_count--;
                }
                else {
                    VEngineExce(UndesiredBranch, "");
                }
            }
            block = file->file_block_tree.insert(beginAddr, BLOCK_SIZE, NULL);
            block->stream = file;
            block->tree = &file->file_block_tree;
            block->init();
            file->push_file_block(block);
            file->file_block_count++;
        }
        else {
            file->push_file_block(block);
        }
    }
    if (block != file->cache0) {
        file_block* tmp = file->cache0;
        file->cache0 = block;
        file->cache1 = tmp;
    }
    else {
        VEngineExce(UndesiredBranch, "");
    }
    return block;
}

void xhn::file_stream::push_file_block(file_block* node)
{
    if (node != head) {
        if (node == tail)   { tail = node->prev; }
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
}

///==========================================================================///
///  filer                                                                   ///
///==========================================================================///

xhn::filer::~filer()
{
    if (m_implement)
        m_implement->close();
}
euint64 xhn::filer::read(euint8* buffer, euint64 size)
{
    if (m_implement)
        return m_implement->read(buffer, size);
    else
        return 0;
}
euint64 xhn::filer::write(const euint8* buffer, euint64 size)
{
    if (m_implement)
        return m_implement->write(buffer, size);
    else
        return 0;
}
euint64 xhn::filer::get_size()
{
    if (m_implement)
        return m_implement->get_size();
    else
        return 0;
}
euint64 xhn::filer::get_pos()
{
    if (m_implement)
        return m_implement->get_pos();
    else
        return 0;
}
euint64 xhn::filer::set_pos(euint64 pos)
{
    if (m_implement)
        return m_implement->set_pos(pos);
    else
        return 0;
}
void xhn::filer::set_base_offset(euint64 offs)
{
    m_baseOffset = offs;
}
euint8& xhn::filer::operator[] (euint64 pos)
{
    pos += m_baseOffset;
    xhn::file_block* block = access(this, pos);
    return *((euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}
const euint8& xhn::filer::operator[] (euint64 pos)const
{
    pos += m_baseOffset;
    filer* file = (filer*)this;
    const xhn::file_block* block = access((filer*)file, pos);
    return *((const euint8*)block->access(pos - block->begin_addr, sizeof(euint8)));
}

///==========================================================================///
///  console_output_device                                                   ///
///==========================================================================///

bool xhn::console_output_device::open(const xhn::wstring& path)
{
    wprintf(L"##%s##\n", path.c_str());
    return true;
}
void xhn::console_output_device::close()
{
    wprintf(L"##Closed##\n");
}
euint64 xhn::console_output_device::read(euint8* buf, euint64 size)
{
    return 0;
}
euint64 xhn::console_output_device::write(const euint8* buf, euint64 size)
{
    xhn::string tmp;
    tmp.resize(size);
    memcpy(&tmp[0], buf, size);
    printf("%s\n", tmp.c_str());
    return size;
}
euint64 xhn::console_output_device::set_pos(euint64 pos)
{
    return 0;
}
euint64 xhn::console_output_device::get_pos()
{
    return 0;
}
euint64 xhn::console_output_device::get_size()
{
    return 0;
}

///==========================================================================///
///  memory_output_device                                                    ///
///==========================================================================///

xhn::memory_output_device::~memory_output_device()
{
    close();
}
bool xhn::memory_output_device::open(const xhn::wstring& path)
{
    if (!m_buffer) {
        m_buffer = (euint8*)Malloc(128);
        m_size = 0;
        m_capacity = 128;
    }
    return true;
}
void xhn::memory_output_device::close()
{
    if (m_buffer) {
        Mfree(m_buffer);
        m_buffer = NULL;
        m_size = 0;
        m_capacity = 0;
    }
}
euint64 xhn::memory_output_device::read(euint8* buf, euint64 size)
{
    if (size > m_size) {
        size = m_size;
    }
    if (size) {
        memcpy(buf, m_buffer, size);
    }
    return size;
}
euint64 xhn::memory_output_device::write(const euint8* buf, euint64 size)
{
    if (m_size + size < m_capacity) {
        memcpy(&m_buffer[m_size], buf, size);
        m_size += size;
        return size;
    }
    else {
        m_capacity = (m_capacity + size) * 2;
        euint8* tmp = (euint8*)Malloc(m_capacity);
        memcpy(tmp, m_buffer, m_size);
        memcpy(&tmp[m_size], buf, size);
        m_size += size;
        Mfree(m_buffer);
        m_buffer = tmp;
        return size;
    }
}
euint64 xhn::memory_output_device::set_pos(euint64 pos)
{
    if (pos >= m_capacity) {
        m_capacity = (pos + 1) * 2;
        euint8* tmp = (euint8*)Malloc(m_capacity);
        memcpy(tmp, m_buffer, m_size);
        Mfree(m_buffer);
        m_buffer = tmp;
        m_size = pos;
    }
    else
        m_size = pos;
    return pos;
}
euint64 xhn::memory_output_device::get_pos()
{
    return m_size;
}
euint64 xhn::memory_output_device::get_size()
{
    return m_size;
}
///==========================================================================///
///  bit_writer                                                              ///
///==========================================================================///

void xhn::bit_writer::synchronize_file()
{
    if (m_bit_count == 8) {
#if XHN_DEBUG
        printf("+%u\n", m_byte_buffer);
#endif
        m_file->write(&m_byte_buffer, 1);
        m_write_count++;
    }
    else if (m_byte_buffer > 0) {
        m_byte_buffer <<= (8 - m_bit_count);
        m_byte_buffer >>= (8 - m_bit_count);
#if XHN_DEBUG
        printf("+%u\n", m_byte_buffer);
#endif
        m_file->write(&m_byte_buffer, 1);
        m_write_count++;
    }
    m_bit_count = 0;
}

xhn::bit_writer::~bit_writer()
{
    synchronize_file();
}

void xhn::bit_writer::write_bit(bool bit)
{
    if (m_bit_count == 8) {
#if XHN_DEBUG
        printf("+%u\n", m_byte_buffer);
#endif
        m_file->write(&m_byte_buffer, 1);
        m_write_count++;
        m_byte_buffer = 0;
        m_bit_count = 0;
    }
    if (bit) {
        m_byte_buffer |= (0x01 << m_bit_count);
    }
    m_bit_count++;
}

void xhn::bit_writer::write_bits(void* bits, euint num_bits)
{
    euint num_bytes = num_bits / 8;
    euint8* bytes = (euint8*)bits;
    for (euint i = 0; i < num_bytes; i++) {
        write_byte(bytes[i]);
    }
    euint8 tmp = bytes[num_bytes];
    for (euint i = 0; i < num_bits % 8; i++) {
        write_bit(tmp & 0x01);
        tmp >>= 1;
    }
}

void xhn::bit_writer::write_byte(euint8 byte)
{
    if (m_bit_count == 8) {
#if XHN_DEBUG
        printf("+%u\n", m_byte_buffer);
#endif
        m_file->write(&m_byte_buffer, 1);
        m_byte_buffer = byte;
    }
    else if (m_bit_count > 0) {
        m_byte_buffer <<= (8 - m_bit_count);
        m_byte_buffer >>= (8 - m_bit_count);
        euint8 tmp = byte;
        tmp <<= m_bit_count;
        m_byte_buffer |= tmp;
#if XHN_DEBUG
        printf("+%u\n", m_byte_buffer);
#endif
        m_file->write(&m_byte_buffer, 1);
        m_write_count++;
        m_byte_buffer = byte;
        m_byte_buffer >>= (8 - m_bit_count);
    }
    else {
        m_byte_buffer = byte;
        m_bit_count = 8;
    }
}

///==========================================================================///
///  bit_reader                                                              ///
///==========================================================================///

bool xhn::bit_reader::read_bit(bool* output)
{
    if (m_bit_count) {
        m_bit_count--;
        *output = m_byte_buffer & 0x01;
        m_byte_buffer >>= 1;
        return true;
    }
    else {
        euint64 read_size = m_file->read(&m_byte_buffer, 1);
        if (read_size == 1) {
#if XHN_DEBUG
            printf("-%u\n", m_byte_buffer);
#endif
            *output = m_byte_buffer & 0x01;
            m_byte_buffer >>= 1;
            m_bit_count = 7;
            return true;
        }
        else
            return false;
    }
}

void xhn::bit_reader::read_bits(void* output, euint* num_bits)
{
    euint num_bytes = *num_bits / 8;
    euint remainder = *num_bits % 8;
    euint8* bytes = (euint8*)output;
    for (euint i = 0; i < num_bytes; i++) {
        if (!read_byte(&bytes[i])) {
            *num_bits = i * 8;
            return;
        }
    }
    *num_bits = num_bytes * 8;
    if (remainder) {
        bytes[num_bytes] >>= remainder;
        bytes[num_bytes] <<= remainder;
        for (euint i = 0; i < remainder; i++) {
            bool bit;
            if (read_bit(&bit)) {
                if (bit) {
                    bytes[num_bytes] |= (0x01 << i);
                }
                (*num_bits)++;
            }
            else
                return;
        }
    }
}
bool xhn::bit_reader::read_byte(euint8* output)
{
    if (m_bit_count == 8) {
        *output = m_byte_buffer;
        m_bit_count = 0;
        return true;
    }
    else if (m_bit_count > 0) {
        euint8 tmp = 0;
        euint64 read_size = m_file->read(&tmp, 1);
        if (read_size == 1) {
#if XHN_DEBUG
            printf("-%u\n", tmp);
#endif
            euint8 tmp2 = tmp;
            tmp2 <<= m_bit_count;
            m_byte_buffer <<= (8 - m_bit_count);
            m_byte_buffer >>= (8 - m_bit_count);
            m_byte_buffer |= tmp2;
            *output = m_byte_buffer;
            tmp >>= (8 - m_bit_count);
            m_byte_buffer = tmp;
            return true;
        }
        else
            return false;
    }
    else {
        euint64 read_size = m_file->read(&m_byte_buffer, 1);
        if (read_size == 1) {
#if XHN_DEBUG
            printf("-%u\n", m_byte_buffer);
#endif
            *output = m_byte_buffer;
            return true;
        }
        else
            return false;
    }
}

///==========================================================================///
///  file_manager                                                            ///
///==========================================================================///

#if defined(__APPLE__)
#include "apple_file_manager.h"
static AppleFileManager* s_AppleFileManager = NULL;

xhn::wstring xhn::file_manager::get_upper_directory(const wstring& path)
{
    wstring ret;
    wstring tmp = path;
    transform(tmp.begin(), tmp.end(), tmp.begin(), FWCharFormat());
    if (tmp.size() && tmp[tmp.size() - 1] == L'/') {
        tmp[tmp.size() - 1] = 0;
        tmp.resize(tmp.size() - 1);
    }
    ret.resize(tmp.size());
    wstring::iterator iter = tmp.begin();
    wstring::iterator end = tmp.end();
    euint count = 0;
    euint lastSlash = (euint)-1;
    bool foundSlash = false;
    for (; iter != end; iter++) {
        if (*iter == L'/') {
            if (!foundSlash) {
                foundSlash = true;
                ret[count] = *iter;
                lastSlash = count;
                count++;
            }
        }
        else {
            foundSlash = false;
            ret[count] = *iter;
            count++;
        }
    }
    if (lastSlash != (euint)-1) {
        ret[lastSlash + 1] = 0;
        ret.resize(lastSlash + 1);
    }
    else {
        ret[count] = 0;
        ret.resize(count);
    }
    return ret;
}

xhn::string xhn::file_manager::get_upper_directory(const xhn::string& path)
{
    string ret;
    string tmp = path;
    transform(tmp.begin(), tmp.end(), tmp.begin(), FCharFormat());
    if (tmp.size() && tmp[tmp.size() - 1] == '/') {
        tmp[tmp.size() - 1] = 0;
        tmp.resize(tmp.size() - 1);
    }
    ret.resize(tmp.size());
    string::iterator iter = tmp.begin();
    string::iterator end = tmp.end();
    euint count = 0;
    euint lastSlash = (euint)-1;
    bool foundSlash = false;
    for (; iter != end; iter++) {
        if (*iter == '/') {
            if (!foundSlash) {
                foundSlash = true;
                ret[count] = *iter;
                lastSlash = count;
                count++;
            }
        }
        else {
            foundSlash = false;
            ret[count] = *iter;
            count++;
        }
    }
    if (lastSlash != (euint)-1) {
        ret[lastSlash + 1] = 0;
        ret.resize(lastSlash + 1);
    }
    else {
        ret[count] = 0;
        ret.resize(count);
    }
    return ret;
}

void xhn::file_manager::force_create_file(const xhn::wstring& path)
{
    wstring upperDir = get_upper_directory(path);
    list<wstring> dirStack;
    xhn::file_manager* mgr = xhn::file_manager::get();
    bool isDir = false;
    while (!mgr->is_exist(upperDir, isDir)) {
        dirStack.push_front(upperDir);
        upperDir = get_upper_directory(upperDir);
    }
    if (dirStack.size()) {
        list<wstring>::iterator iter = dirStack.begin();
        list<wstring>::iterator end = dirStack.end();
        for (; iter != end; iter++) {
            wstring& dir = *iter;
            mgr->create_directory(dir);
        }
    }
    mgr->create_file(path);
}
euint64 xhn::file_manager::compare_two_files(const wstring& path0, const wstring& path1)
{
    file_stream_ptr file0 = open(path0);
    file_stream_ptr file1 = open(path1);
    if (!file0 || !file1)
        return INVALID_UNSIGNED_INTEGER_64;
    if (file0->get_size() != file1->get_size())
        return INVALID_UNSIGNED_INTEGER_64;
    euint64 size = file0->get_size();
    const file_stream& stream0 = *(file0.get());
    const file_stream& stream1 = *(file1.get());
    for (euint64 i = 0; i < size; i++) {
        if (stream0[i] != stream1[i])
            return i;
    }
    return 0;
}
xhn::file_manager* xhn::file_manager::get()
{
    if (!s_AppleFileManager) {
        s_AppleFileManager = VNEW AppleFileManager;
    }
    return s_AppleFileManager;
}
#endif

xhn::wstring xhn::file_manager::get_file_name(const xhn::wstring& path)
{
    wstring dir = get_upper_directory(path);
    return path.substr(dir.size(), path.size() - dir.size());
}
xhn::string xhn::file_manager::get_file_name(const string& path)
{
    string dir = get_upper_directory(path);
    return path.substr(dir.size(), path.size() - dir.size());
}

xhn::file_stream_ptr xhn::file_manager::open(file_implement_ptr implement, const xhn::wstring& path)
{
    xhn::file_stream_ptr ret;
    ret = VNEW xhn::filer(implement);
    if (implement) {
        implement->open(path);
    }
    return ret;
}