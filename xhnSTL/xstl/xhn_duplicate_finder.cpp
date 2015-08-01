//
//  xhn_duplicate_finder.cpp
//  x_duplicate_cleaner
//
//  Created by 徐海宁 on 14-1-24.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_duplicate_finder.h"
#include "timer.h"

namespace xhn
{
    search_duplicate_files_status* search_duplicate_files_status::clone()
    {
        search_duplicate_files_status* ret = VNEW search_duplicate_files_status(m_remainder_file_count);
        ret->m_processing_file = m_processing_file;
        ret->m_current_status = m_current_status;
        return ret;
    }
    
    duplicate_node_cache::~duplicate_node_cache()
    {
        clear();
    }
    void duplicate_node_cache::open(duplicate_node* node)
    {
        if (node->m_file) {
            if (node != m_head) {
                if (node == m_tail) {
                    m_tail = node->m_prev;
                }
                node->m_prev->m_next = node->m_next;
                if (node->m_next) {
                    node->m_next->m_prev = node->m_prev;
                }
                node->m_prev = nullptr;
                node->m_next = m_head;
                m_head->m_prev = node;
                m_head = node;
            }
        }
        else {
            if (m_node_count > 128) {
                duplicate_node* removed = m_tail;
                m_tail = m_tail->m_prev;
                ///fclose(removed->m_file);
                ///removed->m_file = nullptr;
                removed->m_file.close();
                removed->m_prev = nullptr;
                removed->m_next = nullptr;
            }
            else {
                m_node_count++;
            }
    
            node->m_file.open(node->m_path);
            node->m_prev = nullptr;
            node->m_next = m_head;
            if (m_head) {
                m_head->m_prev = node;
            }
            m_head = node;
            if (!m_tail) {
                m_tail = node;
            }
        }
    }
    void duplicate_node_cache::remove(duplicate_node* node)
    {
        if (node->m_prev || node->m_next) {
            if (node == m_head) {
                m_head = node->m_next;
            }
            if (node == m_tail) {
                m_tail = node->m_prev;
            }
            if (node->m_prev) {
                node->m_prev->m_next = node->m_next;
            }
            if (node->m_next) {
                node->m_next->m_prev = node->m_prev;
            }

            node->m_prev = nullptr;
            node->m_next = nullptr;
            ///fclose(node->m_file);
            ///node->m_file = nullptr;
            node->m_file.close();
            
            m_node_count--;
        }
        else {
            if (node == m_head) {
                m_head = m_tail = nullptr;
                
                node->m_prev = nullptr;
                node->m_next = nullptr;
                ///fclose(node->m_file);
                ///node->m_file = nullptr;
                node->m_file.close();
                
                m_node_count--;
            }
        }
    }
    void duplicate_node_cache::clear()
    {
        while (m_head) {
            duplicate_node* node = m_head;
            remove(node);
            delete node;
        }
    }
    void duplicate_node_cache::test()
    {
        file_manager* mgr = file_manager::get();
        {
            wstring homeDir = mgr->get_home_dir();
            wstring filename0 = homeDir + L"/file0.txt";
            wstring filename1 = homeDir + L"/file1.txt";
            wstring filename2 = homeDir + L"/file2.txt";
            wstring filename3 = homeDir + L"/file3.txt";
            wstring filename4 = homeDir + L"/file4.txt";
            wstring filename5 = homeDir + L"/file5.txt";
            
            file_stream_ptr file0 = mgr->create_and_open(filename0);
            file_stream_ptr file1 = mgr->create_and_open(filename1);
            file_stream_ptr file2 = mgr->create_and_open(filename2);
            file_stream_ptr file3 = mgr->create_and_open(filename3);
            file_stream_ptr file4 = mgr->create_and_open(filename4);
            file_stream_ptr file5 = mgr->create_and_open(filename5);
            const char* str0 = "abcDefghijklmnopqrstuvwxyz";
            const char* str1 = "aBcdefghijklmnopqrstuvwxyz";
            const char* str2 = "aBcdefghijklmnopqrstuvwxyz";
            const char* str3 = "abCdefghijklmnopqrstuvwxyz";
            const char* str4 = "abcdefghijklmnopqrstuvwxyz";
            const char* str5 = "abcdefghijklmnopqrsTuvwxyz";
            file0->write((euint8*)str0, strlen(str0));
            file1->write((euint8*)str1, strlen(str1));
            file2->write((euint8*)str2, strlen(str2));
            file3->write((euint8*)str3, strlen(str3));
            file4->write((euint8*)str4, strlen(str4));
            file5->write((euint8*)str5, strlen(str5));
        }
        {
            wstring homeDir = mgr->get_home_dir();
            Utf8 utf8HomeDir(homeDir.c_str());
            
            string filename0 = ((string)utf8HomeDir) + "/file0.txt";
            string filename1 = ((string)utf8HomeDir) + "/file1.txt";
            string filename2 = ((string)utf8HomeDir) + "/file2.txt";
            string filename3 = ((string)utf8HomeDir) + "/file3.txt";
            string filename4 = ((string)utf8HomeDir) + "/file4.txt";
            string filename5 = ((string)utf8HomeDir) + "/file5.txt";
            
            duplicate_node* node0 = VNEW duplicate_node(filename0, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node1 = VNEW duplicate_node(filename1, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node2 = VNEW duplicate_node(filename2, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node3 = VNEW duplicate_node(filename3, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node4 = VNEW duplicate_node(filename4, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node5 = VNEW duplicate_node(filename5, ((string)utf8HomeDir).c_str(), 0);
            
            duplicate_node_cache* cache = VNEW duplicate_node_cache;
            cache->open(node0);
            cache->open(node1);
            cache->open(node0);
            cache->open(node1);
            cache->open(node2);
            cache->open(node3);
            cache->open(node4);
            cache->open(node5);
            cache->remove(node0);
            cache->remove(node1);
            cache->remove(node2);
            cache->remove(node3);
            cache->remove(node4);
            cache->remove(node5);
            printf("here\n");
        }
    }
    search_duplicate_files_status* duplicate_node::s_status = nullptr;
    void duplicate_node::set_status(search_duplicate_files_status* status)
    {
        s_status = status;
    }
    duplicate_node::~duplicate_node()
    {
        m_file.close();
        m_prev = nullptr;
        m_next = nullptr;
        if (s_status) {
            s_status->m_remainder_file_count--;
        }
    }
    euint64 duplicate_node::prepare(duplicate_node_cache* cache,
                                    search_duplicate_files_status* status,
                                    SpinLock& lock)
    {
        euint64 proced_size = 0;
        duplicate_node* iter = this;
        while (iter) {
            iter->m_next1 = nullptr;
            iter->m_symbol = 0;
            iter->m_is_matched = false;
            cache->open(iter);

            {
                SpinLock::Instance inst = lock.Lock();
                if (status) {
                    status->m_current_status = search_duplicate_files_status::Reading;
                    status->m_processing_file = iter->m_path.c_str();
                }
            }
            
            ///fseek(iter->m_file.get(), iter->m_offset, SEEK_SET);
            iter->m_file.Seek(iter->m_offset);
            ///fread(&iter->m_symbol, 1, sizeof(SYMBOL), iter->m_file.get());
            iter->m_file.Read(&iter->m_symbol, sizeof(SYMBOL));

            proced_size += sizeof(SYMBOL);
            m_remainder -= sizeof(SYMBOL);
            iter->m_offset += sizeof(SYMBOL);
            iter = iter->m_next0;
        }
        {
            {
                SpinLock::Instance inst = lock.Lock();
                if (status) {
                    status->m_current_status = search_duplicate_files_status::Idling;
                    status->m_processing_file = nullptr;
                }
            }
        }
        return proced_size;
    }
    void duplicate_node::match(vector<duplicate_node*>& entrances,
                               search_duplicate_files_status* status,
                               SpinLock& lock)
    {
        duplicate_node* iter = this;
        while (iter) {
            bool first_matched = true;
            duplicate_node* current_node = iter;
            duplicate_node* last_node = iter;
            euint current_symbol = iter->m_symbol;
            while (current_node) {
                {
                    SpinLock::Instance inst = lock.Lock();
                    if (status) {
                        status->m_current_status = search_duplicate_files_status::Matching;
                        status->m_processing_file = current_node->m_path.c_str();
                    }
                }
                if (current_node->m_symbol == current_symbol && current_node != last_node) {
                    last_node->m_next1 = current_node;
                    if (first_matched) {
                        entrances.push_back(last_node);
                        first_matched = false;
                    }
                    last_node->m_is_matched = true;
                    current_node->m_is_matched = true;
                    last_node = current_node;
                }
                current_node = current_node->m_next0;
            }
            iter = iter->m_next0;
            while (iter && iter->m_is_matched) {
                iter = iter->m_next0;
            }
        }
        {
            SpinLock::Instance inst = lock.Lock();
            if (status) {
                status->m_current_status = search_duplicate_files_status::Idling;
                status->m_processing_file = nullptr;
            }
        }
    }
    euint64 duplicate_node::complete(duplicate_node_cache* cache)
    {
        euint64 skipped_size = 0;
        duplicate_node* iter = this;
        while (iter) {
            duplicate_node* next = iter->m_next0;
            if (!iter->m_is_matched) {
                skipped_size += iter->m_remainder;
                cache->remove(iter);
                delete iter;
            }
            else {
                iter->m_next0 = iter->m_next1;
            }
            iter = next;
        }
        return skipped_size;
    }
    void duplicate_node::print()
    {
        duplicate_node* iter = this;
        while (iter) {
            duplicate_node* next = iter->m_next0;
            printf("node %llx next %llx, %lld\n", (ref_ptr)iter, (ref_ptr)iter->m_next0, iter->m_symbol);
            iter = next;
        }
    }
    void duplicate_node::test()
    {
        file_manager* mgr = file_manager::get();
        vector<duplicate_node*> entrances;
        
        {
            wstring homeDir = mgr->get_home_dir();
            wstring filename0 = homeDir + L"/file0.txt";
            wstring filename1 = homeDir + L"/file1.txt";
            wstring filename2 = homeDir + L"/file2.txt";
            wstring filename3 = homeDir + L"/file3.txt";
            wstring filename4 = homeDir + L"/file4.txt";
            wstring filename5 = homeDir + L"/file5.txt";
            
            file_stream_ptr file0 = mgr->create_and_open(filename0);
            file_stream_ptr file1 = mgr->create_and_open(filename1);
            file_stream_ptr file2 = mgr->create_and_open(filename2);
            file_stream_ptr file3 = mgr->create_and_open(filename3);
            file_stream_ptr file4 = mgr->create_and_open(filename4);
            file_stream_ptr file5 = mgr->create_and_open(filename5);
            const char* str0 = "abcDefghijklmnopqrstuvwxyz";
            const char* str1 = "aBcdefghijklmnopqrstuvwxyz";
            const char* str2 = "aBcdefghijklmnopqrstuvwxyz";
            const char* str3 = "abCdefghijklmnopqrstuvwxyz";
            const char* str4 = "abcdefghijklmnopqrstuvwxyz";
            const char* str5 = "abcdefghijklmnopqrsTuvwxyz";
            file0->write((euint8*)str0, strlen(str0));
            file1->write((euint8*)str1, strlen(str1));
            file2->write((euint8*)str2, strlen(str2));
            file3->write((euint8*)str3, strlen(str3));
            file4->write((euint8*)str4, strlen(str4));
            file5->write((euint8*)str5, strlen(str5));
        }
        {
            search_duplicate_files_status* status = VNEW search_duplicate_files_status(0);
            SpinLock lock;
            
            duplicate_node_cache* cache = VNEW duplicate_node_cache;
            wstring homeDir = mgr->get_home_dir();
            Utf8 utf8HomeDir(homeDir.c_str());
            
            string filename0 = ((string)utf8HomeDir) + "/file0.txt";
            string filename1 = ((string)utf8HomeDir) + "/file1.txt";
            string filename2 = ((string)utf8HomeDir) + "/file2.txt";
            string filename3 = ((string)utf8HomeDir) + "/file3.txt";
            string filename4 = ((string)utf8HomeDir) + "/file4.txt";
            string filename5 = ((string)utf8HomeDir) + "/file5.txt";
            
            duplicate_node* node0 = VNEW duplicate_node(filename0, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node1 = VNEW duplicate_node(filename1, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node2 = VNEW duplicate_node(filename2, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node3 = VNEW duplicate_node(filename3, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node4 = VNEW duplicate_node(filename4, ((string)utf8HomeDir).c_str(), 0);
            duplicate_node* node5 = VNEW duplicate_node(filename5, ((string)utf8HomeDir).c_str(), 0);
            node0->m_next0 = node1;
            node1->m_next0 = node2;
            node2->m_next0 = node3;
            node3->m_next0 = node4;
            node4->m_next0 = node5;
            node5->m_next0 = nullptr;
            node0->prepare(cache, status, lock);
            node0->match(entrances, status, lock);
            node0->complete(cache);

            vector<duplicate_node*>::iterator iter = entrances.begin();
            vector<duplicate_node*>::iterator end = entrances.end();
            for (; iter != end; iter++) {
                duplicate_node* node = *iter;
                node->print();
                printf("\n");
            }
            printf("here\n");
        }
    }
}