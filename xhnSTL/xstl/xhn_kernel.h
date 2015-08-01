//
//  xhn_kernel.h
//  7zOpenCL
//
//  Created by 徐海宁 on 14-1-2.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef ___zOpenCL__xhn_kernel__
#define ___zOpenCL__xhn_kernel__

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include "xhn_library.h"
#include "xhn_kernel_memory.h"
#include <OpenCL/cl.h>
#include "xhn_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_map.hpp"
#include "xhn_void_vector.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_btree.hpp"

namespace xhn
{
    class device;
    class platform;
    class context;
    class kernel : public RefObject
    {
        friend class context;
    public:
        struct execute_event
        {
            cl_event event;
        };
    public:
        template <typename T>
        class argument_handle
        {
            friend class kernel;
        private:
            void_vector* m_buffer;
            euint m_offset;
            argument_handle(void_vector* buffer, euint offset)
            : m_buffer(buffer)
            , m_offset(offset)
            {}
            argument_handle(const argument_handle& handle)
            : m_buffer((void_vector*)handle.m_buffer)
            , m_offset(handle.m_offset)
            {}
        public:
            argument_handle()
            : m_buffer(NULL)
            , m_offset(0)
            {}
            ~argument_handle() {}
            void write(const T& from) {
                vptr buffer = m_buffer->get();
                vptr address = (vptr)((ref_ptr)buffer + m_offset);
                memcpy(address, &from, sizeof(T));
            }
            void read(T& to) {
                vptr buffer = m_buffer->get();
                vptr address = (vptr)((ref_ptr)buffer + m_offset);
                memcpy(&to, &address, sizeof(T));
            }
            inline void operator = (const argument_handle& handle) {
                m_buffer = (void_vector*)handle.m_buffer;
                m_offset = handle.m_offset;
            }
        };
        class memory_handle
        {
            friend class kernel;
        private:
            kernel_memory_ptr m_kernel_memory;
            euint m_offset;
            memory_handle(kernel_memory_ptr kernel_memory)
            : m_kernel_memory(kernel_memory)
            , m_offset(0)
            {}
            memory_handle(const memory_handle& handle)
            : m_kernel_memory(handle.m_kernel_memory)
            , m_offset(handle.m_offset)
            {}
        public:
            memory_handle()
            : m_offset(0)
            {}
            ~memory_handle() {}
            template <typename T>
            void write(const T& from) {
                m_kernel_memory->write(from, m_offset);
            }
            template <typename T>
            void read(T& to) {
                m_kernel_memory->read(to, m_offset);
            }
            void read_to_host() {
                m_kernel_memory->read_to_host();
            }
            void fill_as_image(euint8 color) {
                m_kernel_memory->fill_as_image(color);
            }
            void write_to_device(const void* data, euint size) {
                m_kernel_memory->write_to_device(data, size);
            }
            inline void operator = (const memory_handle& handle) {
                m_kernel_memory = handle.m_kernel_memory;
                m_offset = handle.m_offset;
            }
            inline void reset() {
                m_offset = 0;
            }
            inline void set_offset(euint offset) {
                m_offset = offset;
            }
            inline euint size() {
                return m_kernel_memory->size();
            }
            inline vptr get_host_memory() {
                return m_kernel_memory->get_host_memory();
            }
            inline void_vector* get_void_vector() {
                return m_kernel_memory->get_void_vector();
            }
            void* lock(void* address, euint size) {
                return m_kernel_memory->lock(address, size);
            }
            void unlock(void* address) {
                m_kernel_memory->unlock(address);
            }
        };
    protected:
        enum argument_type
        {
            value,
            memory,
        };
        struct argument_entry
        {
            argument_type type;
            string name;
        };
    protected:
        void_vector m_main_argument_buffer;
        map<string, range<euint> > m_main_argument_map;
        map<string, kernel_memory_ptr> m_host_memory_map;
        vector<argument_entry> m_arguments;
        execute_event* m_execute_event;
        context* m_context;
    private:
        cl_program m_program;
        cl_kernel m_kernel;
        euint m_global_work_offset;
        euint m_global_work_size;
        euint m_local_work_size;
        vector<library_ptr> m_dependent_librarys;
    private:
        static void callback(cl_program program , void * user_data);
    private:
        void initialize();
        double launch(execute_event* event = NULL);
    protected:
        kernel(context* context)
        : m_execute_event(NULL)
        , m_context(context)
        , m_program(NULL)
        , m_kernel(NULL)
        , m_global_work_offset(0)
        , m_global_work_size(256)
        , m_local_work_size(16)
        {
            m_main_argument_buffer.convert<void_vector::FCharProc>(1);
        }
        public:
        template <typename T>
        argument_handle<T> add_parameter(const string& name) {
            euint offset = m_main_argument_buffer.size();
            m_main_argument_buffer.resize(m_main_argument_buffer.size() + sizeof(T));
            range<euint> range;
            range.begin_addr = offset;
            range.end_addr =
            (ref_ptr)m_main_argument_buffer.get_barrier() - (ref_ptr)m_main_argument_buffer.get();
            m_main_argument_map.insert(make_pair(name, range));
            argument_entry entry = {value, name};
            m_arguments.push_back(entry);
            return argument_handle<T>(&m_main_argument_buffer, offset);
        }
        memory_handle add_memory_parameter(const string& name,
                                           euint memory_size,
                                           kernel_memory::access access);
        memory_handle add_memory_parameter(const string& name,
                                           memory_handle& handle);
        memory_handle add_image_memory_parameter(const string& name,
                                                 euint memory_size,
                                                 kernel_memory::access access);
        void add_dependent_library(library_ptr lib);
    protected:
        virtual void make_host_interface() = 0;
        virtual string make_opencl_source_code() = 0;
        virtual string get_kernel_name() = 0;
        virtual string get_preprocessor_options() = 0;
        virtual void prelaunch(euint& global_work_offset,
                               euint& global_work_size,
                               euint& local_work_size) = 0;
        virtual void postlaunch() = 0;
    public:
        virtual ~kernel();
        inline context* get_context() {
            return m_context;
        }
    };
    typedef SmartPtr<kernel> kernel_ptr;
}


#endif /* defined(___zOpenCL__xhn_kernel__) */
