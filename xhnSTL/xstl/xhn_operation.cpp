#include "xhn_operation.hpp"
#include "xhn_path_tree.hpp"
#include "xhn_duplicate_finder.h"
///==========================================================================///
///  collect_folder_information                                              ///
///==========================================================================///
STATIC_CONST xhn::static_string str_collect_folder_information_operation("collect_folder_information_operation");
bool xhn::collect_folder_information_operation::run()
{
    xhn::file_manager* mgr = xhn::file_manager::get();
    xhn::folder_information* info = m_folder_information->DynamicCast<xhn::folder_information>();
    mgr->get_folder_information(m_folder, *info);
    m_data = info;
    if (info->m_cancel_flag) {
        info->clear();
    }
    complete();
    return true;
}
float xhn::collect_folder_information_operation::get_progress_rate()
{
    return 0.0f;
}
void xhn::collect_folder_information_operation::send_data(operation_handle* handle)
{
    handle->receive(m_data);
}
void xhn::collect_folder_information_operation::cancel()
{
    xhn::folder_information* info = m_folder_information->DynamicCast<xhn::folder_information>();
    info->cancel();
}
const xhn::static_string xhn::collect_folder_information_operation::type() const
{
    return str_collect_folder_information_operation;
}
void xhn::collect_folder_information_operatoion_handle::process_impl(operation_data_ptr data)
{
    if (data) {
        folder_information* info = data->DynamicCast<folder_information>();
        collect_folder_information(info);
    }
}

///==========================================================================///
///  collect_folder_information                                              ///
///==========================================================================///
///==========================================================================///
///  search_duplicate_files                                                  ///
///==========================================================================///
ImplementRTTI(xhn::duplicate_files, xhn::operation_data);

class filename_collector : public xhn::walker
{
public:
    xhn::static_string m_base_folder;
    xhn::vector<xhn::pair<xhn::string, xhn::static_string>>* m_result;
public:
    filename_collector(const char* base_folder,
                       xhn::vector<xhn::pair<xhn::string, xhn::static_string>>* result)
    : m_base_folder(base_folder)
    , m_result(result)
    {}
    virtual void walk(xhn::path_node* node);
};
void filename_collector::walk(xhn::path_node* node)
{
    if (!node->is_folder) {
        xhn::pair<xhn::string, xhn::static_string> p =
        xhn::make_pair(node->path_name, m_base_folder);
        m_result->push_back(p);
    }
}
void xhn::duplicate_files::print()
{
    euint file_count = 0;
    map<euint64, vector<duplicate_node*>>::iterator iter = m_duplicate_files.begin();
    map<euint64, vector<duplicate_node*>>::iterator end = m_duplicate_files.end();
    for (; iter != end; iter++) {
        euint64 size = iter->first;
        vector<duplicate_node*>& entrances0 = iter->second;
        if (entrances0.size()) {
            vector<duplicate_node*>::iterator eiter = entrances0.begin();
            vector<duplicate_node*>::iterator eend = entrances0.end();
            for (; eiter != eend; eiter++) {
                duplicate_node* node = *eiter;
                while (node) {
                    file_count++;
                    printf("size %lld, %s\n", size, node->m_path.c_str());
                    node = node->m_next0;
                }
                printf("\n");
            }
            printf("\n");
        }
    }
#if BIT_WIDTH == 32
    printf("file_count %d\n", file_count);
#else
    printf("file_count %lld\n", file_count);
#endif
}
STATIC_CONST xhn::static_string str_search_duplicate_files_operation("search duplicate files operation");
const xhn::static_string xhn::search_duplicate_files_operation::type() const
{
    return str_search_duplicate_files_operation;
}
xhn::search_duplicate_files_operation::~search_duplicate_files_operation()
{
    if (m_cache) {
        delete m_cache;
    }
}
bool xhn::search_duplicate_files_operation::run()
{
    prepare();
    if (!m_cancel_flag)
        find();
    complete();
    return true;
}
float xhn::search_duplicate_files_operation::get_progress_rate()
{
    return (float)m_proced_size / (float)m_totel_size;
}
void xhn::search_duplicate_files_operation::send_data(operation_handle* handle)
{
    handle->receive(m_data);
}
void xhn::search_duplicate_files_operation::cancel()
{
    AtomicIncrement(&m_cancel_flag);
}
void xhn::search_duplicate_files_operation::prepare()
{
    if (m_cache) {
        delete m_cache;
    }
    m_cache = VNEW duplicate_node_cache;
    duplicate_files* search_table = VNEW duplicate_files;
    m_data = search_table;
    vector<pair<string, static_string>> paths;
    if (!m_cancel_flag) {
        SpinLock::Instance inst = m_lock.Lock();
        m_status = VNEW search_duplicate_files_status(0);
        duplicate_node::set_status(m_status.get());
    }
    if (!m_cancel_flag)
    {
        path_ptr path = m_paths->next_path();
        while (path && !m_cancel_flag) {
            paths.push_back(make_pair(path->m_path, static_string(path->m_base_folder.c_str())));
            path = m_paths->next_path();
        }
        {
            SpinLock::Instance inst = m_lock.Lock();
            m_status->m_remainder_file_count = paths.size();
        }
    }
    if (!m_cancel_flag)
    {
        m_proced_size = 0;
        file_manager* mgr = file_manager::get();
        vector<pair<string, static_string>>::iterator iter = paths.begin();
        vector<pair<string, static_string>>::iterator end = paths.end();
        for (; iter != end && !m_cancel_flag; iter++) {
            const pair<string, static_string>& path_folder_pair = *iter;
            euint64 size = mgr->file_size(path_folder_pair.first);
            euint64 num_symbols = size / sizeof(SYMBOL);
            if (size % sizeof(SYMBOL)) {
                num_symbols++;
            }
            m_totel_size += num_symbols * sizeof(SYMBOL);
            duplicate_node* node = VNEW duplicate_node(path_folder_pair.first,
                                                       path_folder_pair.second,
                                                       num_symbols * sizeof(SYMBOL));
            vector<duplicate_node*>& group = search_table->m_duplicate_files[size];
            if (group.size()) {
                node->m_next0 = group[0];
                group[0] = node;
            }
            else {
                group.push_back(node);
            }
        }
    }
}
void xhn::search_duplicate_files_operation::find()
{
    vector<duplicate_node*> entrances1;
    duplicate_files* search_table = m_data->DynamicCast<duplicate_files>();
    map<euint64, vector<duplicate_node*>>::iterator iter = search_table->m_duplicate_files.begin();
    map<euint64, vector<duplicate_node*>>::iterator end = search_table->m_duplicate_files.end();
    for (; iter != end && !m_cancel_flag; iter++) {
        euint64 size = iter->first;
        vector<duplicate_node*>& entrances0 = iter->second;
        euint64 num_symbols = size / sizeof(SYMBOL);
        for (euint64 i = 0; i < num_symbols && !m_cancel_flag; i++) {
            entrances1.clear();
            vector<duplicate_node*>::iterator eiter = entrances0.begin();
            vector<duplicate_node*>::iterator eend = entrances0.end();
            for (; eiter != eend && !m_cancel_flag; eiter++) {
                duplicate_node* node = *eiter;
                m_proced_size += node->prepare(m_cache, m_status.get(), m_lock);
                node->match(entrances1, m_status.get(), m_lock);
                m_proced_size += node->complete(m_cache);
            }
            if (entrances1.size() < entrances0.size()) {
                SpinLock::Instance inst = m_lock.Lock();
                m_status->m_current_status = search_duplicate_files_status::Idling;
            }
            entrances0 = entrances1;
            if (entrances0.empty())
                break;
        }
        if (entrances0.size() && !m_cancel_flag) {
            euint64 count = 0;
            vector<duplicate_node*>::iterator eiter = entrances0.begin();
            vector<duplicate_node*>::iterator eend = entrances0.end();
            for (; eiter != eend && !m_cancel_flag; eiter++) {
                duplicate_node* node = *eiter;
                while (node) {
                    count++;
                    node = node->m_next0;
                }
            }
            m_status->m_remainder_file_count -= count;
        }
    }
}
void xhn::search_duplicate_files_operation::print()
{
    duplicate_files* search_table = m_data->DynamicCast<duplicate_files>();
    map<euint64, vector<duplicate_node*>>::iterator iter = search_table->m_duplicate_files.begin();
    map<euint64, vector<duplicate_node*>>::iterator end = search_table->m_duplicate_files.end();
    for (; iter != end; iter++) {
        euint64 size = iter->first;
        vector<duplicate_node*>& entrances0 = iter->second;
        if (entrances0.size()) {
            vector<duplicate_node*>::iterator eiter = entrances0.begin();
            vector<duplicate_node*>::iterator eend = entrances0.end();
            for (; eiter != eend; eiter++) {
                duplicate_node* node = *eiter;
                while (node) {
                    printf("size %lld, %s\n", size, node->m_path.c_str());
                    node = node->m_next0;
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}

void xhn::search_duplicate_files_operation_handle::process_impl(operation_data_ptr data)
{
    if (data && !m_search_duplicate_files_operation->get_cancel_flag()) {
        duplicate_files* files = data->DynamicCast<duplicate_files>();
        collect_duplicate_files(files);
    }
}
///==========================================================================///
///  search_duplicate_files                                                  ///
///==========================================================================///
