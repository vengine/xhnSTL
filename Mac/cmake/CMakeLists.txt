cmake_minimum_required(VERSION 3.9)
PROJECT(xhnSTL)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
#MESSAGE("CMAKE_CXX_FLAGS_DEBUG="+${CMAKE_CXX_FLAGS_DEBUG})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INFO
    ../xhnSTL/Info.plist
)

set(PUB_HEADERS
    ../xhnSTL/common.h
    ../xhnSTL/etypes.h

    ../../xhnSTL/objc/apple_file_manager.h
    ../../xhnSTL/objc/Singleton.h
    ../../xhnSTL/objc/ObjCPropertys.h
    ../../xhnSTL/objc/ObjCString.h

    ../../xhnSTL/xlibc/array.h
    ../../xhnSTL/xlibc/comparison_algorithm.h
    ../../xhnSTL/xlibc/container.h
    ../../xhnSTL/xlibc/cpu.h
    ../../xhnSTL/xlibc/eassert.h
    ../../xhnSTL/xlibc/elog.h
    ../../xhnSTL/xlibc/emem_allocator.h
    ../../xhnSTL/xlibc/emem.h
    ../../xhnSTL/xlibc/esignal.h
    ../../xhnSTL/xlibc/estring.h
    ../../xhnSTL/xlibc/exception.h
    ../../xhnSTL/xlibc/hash.h
    ../../xhnSTL/xlibc/list_base.h
    ../../xhnSTL/xlibc/list.h
    ../../xhnSTL/xlibc/prime.h
    ../../xhnSTL/xlibc/rwbuffer.h
    ../../xhnSTL/xlibc/sha256.h
    ../../xhnSTL/xlibc/spin_lock.h
    ../../xhnSTL/xlibc/stack.h
    ../../xhnSTL/xlibc/static_string.h
    ../../xhnSTL/xlibc/tree.h
    ../../xhnSTL/xlibc/utility.h

    ../../xhnSTL/xstl/map.hpp
    ../../xhnSTL/xstl/rtti.hpp
    ../../xhnSTL/xstl/timer.h
    ../../xhnSTL/xstl/xhn_atomic_bool.hpp
    ../../xhnSTL/xstl/xhn_atomic_operation.hpp
    ../../xhnSTL/xstl/xhn_atomic_ptr.hpp
    ../../xhnSTL/xstl/xhn_btree.hpp
    ../../xhnSTL/xstl/xhn_cache.hpp
    ../../xhnSTL/xstl/xhn_cloned_ptr.hpp
    ../../xhnSTL/xstl/xhn_concurrent.hpp
    ../../xhnSTL/xstl/xhn_config.hpp
    ../../xhnSTL/xstl/xhn_crypto.hpp
    ../../xhnSTL/xstl/xhn_device.h
    ../../xhnSTL/xstl/xhn_dictionary.hpp
    ../../xhnSTL/xstl/xhn_duplicate_finder.h
    ../../xhnSTL/xstl/xhn_exception.hpp
    ../../xhnSTL/xstl/xhn_file_stream.hpp
    ../../xhnSTL/xstl/xhn_filesystem_node.h
    ../../xhnSTL/xstl/xhn_fixed_queue.hpp
    ../../xhnSTL/xstl/xhn_functor.hpp
    ../../xhnSTL/xstl/xhn_garbage_collector.hpp
    ../../xhnSTL/xstl/xhn_gc_array.h
    ../../xhnSTL/xstl/xhn_gc_container_base.h
    ../../xhnSTL/xstl/xhn_gc_string.h
    ../../xhnSTL/xstl/xhn_gc_values.h
    ../../xhnSTL/xstl/xhn_hash_map.hpp
    ../../xhnSTL/xstl/xhn_hash_set.hpp
    ../../xhnSTL/xstl/xhn_iterator.hpp
    ../../xhnSTL/xstl/xhn_lambda.hpp
    ../../xhnSTL/xstl/xhn_list_template.hpp
    ../../xhnSTL/xstl/xhn_list.hpp
    ../../xhnSTL/xstl/xhn_list2.h
    ../../xhnSTL/xstl/xhn_lock.hpp
    ../../xhnSTL/xstl/xhn_logger.hpp
    ../../xhnSTL/xstl/xhn_map.hpp
    ../../xhnSTL/xstl/xhn_math.hpp
    ../../xhnSTL/xstl/xhn_memory_pool.hpp
    ../../xhnSTL/xstl/xhn_memory.hpp
    ../../xhnSTL/xstl/xhn_neural_network.hpp
    ../../xhnSTL/xstl/xhn_open_hash_set.hpp
    ../../xhnSTL/xstl/xhn_operation_manager.hpp
    ../../xhnSTL/xstl/xhn_operation.hpp
    ../../xhnSTL/xstl/xhn_pair.hpp
    ../../xhnSTL/xstl/xhn_parallel.hpp
    ../../xhnSTL/xstl/xhn_path_tree.hpp
    ../../xhnSTL/xstl/xhn_platform.h
    ../../xhnSTL/xstl/xhn_rbtree.hpp
    ../../xhnSTL/xstl/xhn_realtime_thread.hpp
    ../../xhnSTL/xstl/xhn_recursive.hpp
    ../../xhnSTL/xstl/xhn_routine_pool.hpp
    ../../xhnSTL/xstl/xhn_serialization.hpp
    ../../xhnSTL/xstl/xhn_set.hpp
    ../../xhnSTL/xstl/xhn_singly_linked_list.hpp
    ../../xhnSTL/xstl/xhn_smart_ptr.hpp
    ../../xhnSTL/xstl/xhn_stacktrace.hpp
    ../../xhnSTL/xstl/xhn_state_machine.hpp
    ../../xhnSTL/xstl/xhn_static_string.hpp
    ../../xhnSTL/xstl/xhn_string_base.hpp
    ../../xhnSTL/xstl/xhn_string.hpp
    ../../xhnSTL/xstl/xhn_thread_map.hpp
    ../../xhnSTL/xstl/xhn_thread_pool.hpp
    ../../xhnSTL/xstl/xhn_thread.hpp
    ../../xhnSTL/xstl/xhn_triple_buffer.hpp
    ../../xhnSTL/xstl/xhn_tuple.hpp
    ../../xhnSTL/xstl/xhn_unique_identifier.hpp
    ../../xhnSTL/xstl/xhn_utility.hpp
    ../../xhnSTL/xstl/xhn_vector.hpp
    ../../xhnSTL/xstl/xhn_void_vector.hpp
    ../../xhnSTL/xstl/xhn_z.hpp
)

INCLUDE_DIRECTORIES(./
                    ../xhnSTL/
                    ../../xhnSTL/objc/
                    ../../xhnSTL/xstl/
                    ../../xhnSTL/xlibc/)


ADD_LIBRARY(xhnSTL SHARED
    ../../xhnSTL/xlibc/eassert.c
    ../../xhnSTL/xlibc/elog.c
    ../../xhnSTL/xlibc/emem.c
    ../../xhnSTL/xlibc/estring.c
    ../../xhnSTL/xlibc/hash.c
    ../../xhnSTL/xlibc/list.c
    ../../xhnSTL/xlibc/prime.c
    ../../xhnSTL/xlibc/rwbuffer.c
    ../../xhnSTL/xlibc/stack.c
    ../../xhnSTL/xlibc/static_string.c
    ../../xhnSTL/xlibc/tree.c
    ../../xhnSTL/xlibc/sha256.c
    ../../xhnSTL/xlibc/utility.c
    ../../xhnSTL/xlibc/cpu_x86_64.c
    ../../xhnSTL/xlibc/cpu_common.c

    ../../xhnSTL/xstl/exception.cpp
    ../../xhnSTL/xstl/rtti.cpp
    ../../xhnSTL/xstl/xhn_btree.cpp
    ../../xhnSTL/xstl/xhn_crypto.cpp
    ../../xhnSTL/xstl/xhn_concurrent.cpp
    ../../xhnSTL/xstl/xhn_duplicate_finder.cpp
    ../../xhnSTL/xstl/xhn_exception.cpp
    ../../xhnSTL/xstl/xhn_file_stream.cpp
    ../../xhnSTL/xstl/xhn_filesystem_node.cpp
    ../../xhnSTL/xstl/xhn_garbage_collector.cpp
    ../../xhnSTL/xstl/xhn_gc_array.cpp
    ../../xhnSTL/xstl/xhn_gc_container_base.cpp
    ../../xhnSTL/xstl/xhn_gc_string.cpp
    ../../xhnSTL/xstl/xhn_gc_values.cpp
    ../../xhnSTL/xstl/xhn_list2.cpp
    ../../xhnSTL/xstl/xhn_lock.cpp
    ../../xhnSTL/xstl/xhn_memory.cpp
    ../../xhnSTL/xstl/xhn_operation.cpp
    ../../xhnSTL/xstl/xhn_operation_manager.cpp
    ../../xhnSTL/xstl/xhn_parallel.cpp
    ../../xhnSTL/xstl/xhn_realtime_thread.cpp
    ../../xhnSTL/xstl/xhn_path_tree.cpp
    ../../xhnSTL/xstl/xhn_routine_pool.cpp
    ../../xhnSTL/xstl/xhn_smart_ptr.cpp
    ../../xhnSTL/xstl/xhn_state_machine.cpp
    ../../xhnSTL/xstl/xhn_static_string.cpp
    ../../xhnSTL/xstl/xhn_string.cpp
    ../../xhnSTL/xstl/xhn_thread.cpp
    ../../xhnSTL/xstl/xhn_thread_map.cpp
    ../../xhnSTL/xstl/xhn_thread_pool.cpp
    ../../xhnSTL/xstl/xhn_triple_buffer.cpp
    ../../xhnSTL/xstl/xhn_unique_identifier.cpp
    ../../xhnSTL/xstl/xhn_utility.cpp
    ../../xhnSTL/xstl/xhn_neural_network.cpp
    ../../xhnSTL/xstl/xhn_z.cpp

    ../../xhnSTL/objc/ObjCString.mm
    ../../xhnSTL/objc/ObjCPropertys.m
    ../../xhnSTL/objc/apple_file_manager.mm

    ../xhnSTL/common.h
    ../xhnSTL/etypes.h

    ../../xhnSTL/objc/apple_file_manager.h
    ../../xhnSTL/objc/Singleton.h
    ../../xhnSTL/objc/ObjCPropertys.h
    ../../xhnSTL/objc/ObjCString.h

    ../../xhnSTL/xlibc/array.h
    ../../xhnSTL/xlibc/comparison_algorithm.h
    ../../xhnSTL/xlibc/container.h
    ../../xhnSTL/xlibc/cpu.h
    ../../xhnSTL/xlibc/eassert.h
    ../../xhnSTL/xlibc/elog.h
    ../../xhnSTL/xlibc/emem_allocator.h
    ../../xhnSTL/xlibc/emem.h
    ../../xhnSTL/xlibc/esignal.h
    ../../xhnSTL/xlibc/estring.h
    ../../xhnSTL/xlibc/exception.h
    ../../xhnSTL/xlibc/hash.h
    ../../xhnSTL/xlibc/list_base.h
    ../../xhnSTL/xlibc/list.h
    ../../xhnSTL/xlibc/prime.h
    ../../xhnSTL/xlibc/rwbuffer.h
    ../../xhnSTL/xlibc/sha256.h
    ../../xhnSTL/xlibc/spin_lock.h
    ../../xhnSTL/xlibc/stack.h
    ../../xhnSTL/xlibc/static_string.h
    ../../xhnSTL/xlibc/tree.h
    ../../xhnSTL/xlibc/utility.h

    ../../xhnSTL/xstl/map.hpp
    ../../xhnSTL/xstl/rtti.hpp
    ../../xhnSTL/xstl/timer.h
    ../../xhnSTL/xstl/xhn_atomic_bool.hpp
    ../../xhnSTL/xstl/xhn_atomic_operation.hpp
    ../../xhnSTL/xstl/xhn_atomic_ptr.hpp
    ../../xhnSTL/xstl/xhn_btree.hpp
    ../../xhnSTL/xstl/xhn_cache.hpp
    ../../xhnSTL/xstl/xhn_cloned_ptr.hpp
    ../../xhnSTL/xstl/xhn_concurrent.hpp
    ../../xhnSTL/xstl/xhn_config.hpp
    ../../xhnSTL/xstl/xhn_crypto.hpp
    ../../xhnSTL/xstl/xhn_device.h
    ../../xhnSTL/xstl/xhn_dictionary.hpp
    ../../xhnSTL/xstl/xhn_duplicate_finder.h
    ../../xhnSTL/xstl/xhn_exception.hpp
    ../../xhnSTL/xstl/xhn_file_stream.hpp
    ../../xhnSTL/xstl/xhn_filesystem_node.h
    ../../xhnSTL/xstl/xhn_fixed_queue.hpp
    ../../xhnSTL/xstl/xhn_functor.hpp
    ../../xhnSTL/xstl/xhn_garbage_collector.hpp
    ../../xhnSTL/xstl/xhn_gc_array.h
    ../../xhnSTL/xstl/xhn_gc_container_base.h
    ../../xhnSTL/xstl/xhn_gc_string.h
    ../../xhnSTL/xstl/xhn_gc_values.h
    ../../xhnSTL/xstl/xhn_hash_map.hpp
    ../../xhnSTL/xstl/xhn_hash_set.hpp
    ../../xhnSTL/xstl/xhn_iterator.hpp
    ../../xhnSTL/xstl/xhn_lambda.hpp
    ../../xhnSTL/xstl/xhn_list_template.hpp
    ../../xhnSTL/xstl/xhn_list.hpp
    ../../xhnSTL/xstl/xhn_list2.h
    ../../xhnSTL/xstl/xhn_lock.hpp
    ../../xhnSTL/xstl/xhn_logger.hpp
    ../../xhnSTL/xstl/xhn_map.hpp
    ../../xhnSTL/xstl/xhn_math.hpp
    ../../xhnSTL/xstl/xhn_memory_pool.hpp
    ../../xhnSTL/xstl/xhn_memory.hpp
    ../../xhnSTL/xstl/xhn_neural_network.hpp
    ../../xhnSTL/xstl/xhn_open_hash_set.hpp
    ../../xhnSTL/xstl/xhn_operation_manager.hpp
    ../../xhnSTL/xstl/xhn_operation.hpp
    ../../xhnSTL/xstl/xhn_pair.hpp
    ../../xhnSTL/xstl/xhn_parallel.hpp
    ../../xhnSTL/xstl/xhn_path_tree.hpp
    ../../xhnSTL/xstl/xhn_platform.h
    ../../xhnSTL/xstl/xhn_rbtree.hpp
    ../../xhnSTL/xstl/xhn_realtime_thread.hpp
    ../../xhnSTL/xstl/xhn_recursive.hpp
    ../../xhnSTL/xstl/xhn_routine_pool.hpp
    ../../xhnSTL/xstl/xhn_serialization.hpp
    ../../xhnSTL/xstl/xhn_set.hpp
    ../../xhnSTL/xstl/xhn_singly_linked_list.hpp
    ../../xhnSTL/xstl/xhn_smart_ptr.hpp
    ../../xhnSTL/xstl/xhn_stacktrace.hpp
    ../../xhnSTL/xstl/xhn_state_machine.hpp
    ../../xhnSTL/xstl/xhn_static_string.hpp
    ../../xhnSTL/xstl/xhn_string_base.hpp
    ../../xhnSTL/xstl/xhn_string.hpp
    ../../xhnSTL/xstl/xhn_thread_map.hpp
    ../../xhnSTL/xstl/xhn_thread_pool.hpp
    ../../xhnSTL/xstl/xhn_thread.hpp
    ../../xhnSTL/xstl/xhn_triple_buffer.hpp
    ../../xhnSTL/xstl/xhn_tuple.hpp
    ../../xhnSTL/xstl/xhn_unique_identifier.hpp
    ../../xhnSTL/xstl/xhn_utility.hpp
    ../../xhnSTL/xstl/xhn_vector.hpp
    ../../xhnSTL/xstl/xhn_void_vector.hpp
    ../../xhnSTL/xstl/xhn_z.hpp
)

target_link_libraries(xhnSTL stdc++ "-framework CoreFoundation" "-framework Foundation" "-framework Cocoa" objc objc.A objc-trampolines)

target_compile_options(xhnSTL PUBLIC "-fobjc-arc")

set_target_properties(xhnSTL PROPERTIES
  MACOSX_BUNDLE TRUE
  FRAMEWORK TRUE
  FRAMEWORK_VERSION C
  MACOSX_FRAMEWORK_IDENTIFIER "${INFO}"
  # "current version" in semantic format in Mach-O binary file
  VERSION 1.0.0
  # "compatibility version" in semantic format in Mach-O binary file
  SOVERSION 1.0.0
  PUBLIC_HEADER "${PUB_HEADERS}"
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)
