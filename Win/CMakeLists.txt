cmake_minimum_required (VERSION 3.15)
PROJECT(xhnSTL LANGUAGES C CXX)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
#MESSAGE("CMAKE_CXX_FLAGS_DEBUG="+${CMAKE_CXX_FLAGS_DEBUG})

#message("CMAKE_GENERATOR = "${CMAKE_GENERATOR})

SET(VSPrefixPos)
string(FIND ${CMAKE_GENERATOR} "Visual Studio" VSPrefixPos REVERSE)
message("VSPrefixPos = "${VSPrefixPos})

if (VSPrefixPos STREQUAL "0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4819 /wd4099")
endif ()

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../pthread/lib/x64)

INCLUDE_DIRECTORIES(./
                    ./xhnSTL/
                    ../pthread/include/
                   ../xhnSTL/win/
                    ../xhnSTL/xstl/
                    ../xhnSTL/xlibc/
                    ../xhnSTL/objc/)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WIN64 -DHAVE_STRUCT_TIMESPEC -DDEBUG=1 -DXHN_DLL_EXPORT)
ELSE()
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WIN64 -DHAVE_STRUCT_TIMESPEC -DXHN_DLL_EXPORT)
ENDIF()

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

   ../xhnSTL/win/WinString.cpp
   ../xhnSTL/win/win_file_manager.cpp
   ../xhnSTL/win/swift_parser.cpp
)

#set_target_properties(xhnSTL PROPERTIES ENABLE_EXPORTS 1)
#set_target_properties(xhnSTL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
set_property(TARGET xhnSTL PROPERTY CXX_STANDARD 14)

add_custom_command(TARGET xhnSTL
  PRE_BUILD 
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/copy_headers.sh
  COMMENT "This command will be executed before build target xhnSTL"
)

target_link_libraries(xhnSTL 
${CMAKE_CURRENT_SOURCE_DIR}/../pthread/lib/x64/pthreadVC2.lib 
uuid)
