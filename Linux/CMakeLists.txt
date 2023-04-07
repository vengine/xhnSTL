cmake_minimum_required (VERSION 2.6)
PROJECT(xhnSTL C CXX)
# Initialize CXXFLAGS.
if (SANITIZE)
  SET(CMAKE_CXX_FLAGS "-Wall -std=c++17 -g -gdwarf-4 -fsanitize=address")
else()
  SET(CMAKE_CXX_FLAGS "-Wall -std=c++17 -g -gdwarf-4")
endif()
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

INCLUDE_DIRECTORIES(private
                    ../xhnSTL/xstl/
                    ../xhnSTL/linux/
                    ../xhnSTL/xlibc/)

add_definitions(-DLINUX -DBIT_WIDTH=64)
ADD_LIBRARY(xhnSTL SHARED
    ../xhnSTL/xlibc/eassert.c
    ../xhnSTL/xlibc/elog.c
    ../xhnSTL/xlibc/emem.c
    ../xhnSTL/xlibc/estring.c
    ../xhnSTL/xlibc/hash.c
    ../xhnSTL/xlibc/list.c
    ../xhnSTL/xlibc/prime.c
    ../xhnSTL/xlibc/rwbuffer.c
    ../xhnSTL/xlibc/stack.c
    ../xhnSTL/xlibc/static_string.c
    ../xhnSTL/xlibc/tree.c
    ../xhnSTL/xlibc/sha256.c
    ../xhnSTL/xlibc/utility.c
    ../xhnSTL/xlibc/cpu_x86_64.c
    ../xhnSTL/xlibc/cpu_common.c

    ../xhnSTL/xstl/exception.cpp
    ../xhnSTL/xstl/rtti.cpp
    ../xhnSTL/xstl/xhn_btree.cpp
    ../xhnSTL/xstl/xhn_crypto.cpp
    ../xhnSTL/xstl/xhn_concurrent.cpp
    ../xhnSTL/xstl/xhn_duplicate_finder.cpp
    ../xhnSTL/xstl/xhn_exception.cpp
    ../xhnSTL/xstl/xhn_file_stream.cpp
    ../xhnSTL/xstl/xhn_filesystem_node.cpp
    ../xhnSTL/xstl/xhn_garbage_collector.cpp
    ../xhnSTL/xstl/xhn_gc_array.cpp
    ../xhnSTL/xstl/xhn_gc_container_base.cpp
    ../xhnSTL/xstl/xhn_gc_string.cpp
    ../xhnSTL/xstl/xhn_gc_values.cpp
    ../xhnSTL/xstl/xhn_list2.cpp
    ../xhnSTL/xstl/xhn_lock.cpp
    ../xhnSTL/xstl/xhn_memory.cpp
    ../xhnSTL/xstl/xhn_operation.cpp
    ../xhnSTL/xstl/xhn_operation_manager.cpp
    ../xhnSTL/xstl/xhn_parallel.cpp
    ../xhnSTL/xstl/xhn_realtime_thread.cpp
    #../xhnSTL/xstl/xhn_path_tree.cpp
    ../xhnSTL/xstl/xhn_routine_pool.cpp
    ../xhnSTL/xstl/xhn_smart_ptr.cpp
    ../xhnSTL/xstl/xhn_state_machine.cpp
    ../xhnSTL/xstl/xhn_static_string.cpp
    ../xhnSTL/xstl/xhn_string.cpp
    ../xhnSTL/xstl/xhn_thread.cpp
    ../xhnSTL/xstl/xhn_thread_map.cpp
    ../xhnSTL/xstl/xhn_thread_pool.cpp
    ../xhnSTL/xstl/xhn_triple_buffer.cpp
    ../xhnSTL/xstl/xhn_unique_identifier.cpp
    ../xhnSTL/xstl/xhn_utility.cpp
    ../xhnSTL/xstl/xhn_neural_network.cpp
    ../xhnSTL/xstl/xhn_z.cpp

    ../xhnSTL/linux/LinuxString.cpp
    ../xhnSTL/linux/linux_file_manager.cpp
)

target_link_libraries(xhnSTL m ${CMAKE_THREAD_LIBS_INIT} uuid)

add_custom_command(TARGET xhnSTL
  PRE_BUILD 
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/copy_headers.sh
  COMMENT "This command will be executed before build target xhnSTL"
)
