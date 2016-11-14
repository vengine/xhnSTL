filepath=$(cd "$(dirname "$0")"; pwd)
echo $filepath
cd $filepath
source /home/parallels/Projects/SDK/environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi
rm -rf CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
rm Makefile
rm -rf xhnSTL
cmake . -DCMAKE_TOOLCHAIN_FILE=OEToolchainConfig.cmake
mkdir xhnSTL
cp ../xhnSTL/xlibc/array.h xhnSTL/array.h
cp ../xhnSTL/xlibc/comparison_algorithm.h xhnSTL/comparison_algorithm.h
cp ../xhnSTL/xlibc/container.h xhnSTL/container.h
cp ../xhnSTL/xlibc/eassert.h xhnSTL/eassert.h
cp ../xhnSTL/xlibc/elog.h xhnSTL/elog.h
cp ../xhnSTL/xlibc/emem.h xhnSTL/emem.h
cp ../xhnSTL/xlibc/emem_allocator.h xhnSTL/emem_allocator.h
cp ../xhnSTL/xlibc/esignal.h xhnSTL/esignal.h
cp ../xhnSTL/xlibc/estring.h xhnSTL/estring.h
cp ../xhnSTL/xlibc/exception.h xhnSTL/exception.h
cp ../xhnSTL/xlibc/hash.h xhnSTL/hash.h
cp ../xhnSTL/xlibc/list.h xhnSTL/list.h
cp ../xhnSTL/xlibc/list_base.h xhnSTL/list_base.h
cp ../xhnSTL/xlibc/mem.h xhnSTL/mem.h
cp ../xhnSTL/xlibc/prime.h xhnSTL/prime.h
cp ../xhnSTL/xlibc/rwbuffer.h xhnSTL/rwbuffer.h
cp ../xhnSTL/xlibc/sha256.h xhnSTL/sha256.h
cp ../xhnSTL/xlibc/spin_lock.h xhnSTL/spin_lock.h
cp ../xhnSTL/xlibc/stack.h xhnSTL/stack.h
cp ../xhnSTL/xlibc/static_string.h xhnSTL/static_string.h
cp ../xhnSTL/xlibc/tree.h xhnSTL/tree.h
cp ../xhnSTL/xlibc/utility.h xhnSTL/utility.h

cp ../xhnSTL/xstl/map.hpp xhnSTL/map.hpp
cp ../xhnSTL/xstl/rtti.hpp xhnSTL/rtti.hpp
cp ../xhnSTL/xstl/timer.h xhnSTL/timer.h
cp ../xhnSTL/xstl/xhn_atomic_operation.hpp xhnSTL/xhn_atomic_operation.hpp
cp ../xhnSTL/xstl/xhn_btree.hpp xhnSTL/xhn_btree.hpp
cp ../xhnSTL/xstl/xhn_cloned_ptr.hpp xhnSTL/xhn_cloned_ptr.hpp
cp ../xhnSTL/xstl/xhn_config.hpp xhnSTL/xhn_config.hpp
cp ../xhnSTL/xstl/xhn_crypto.hpp xhnSTL/xhn_crypto.hpp
cp ../xhnSTL/xstl/xhn_duplicate_finder.h xhnSTL/xhn_duplicate_finder.h
cp ../xhnSTL/xstl/xhn_exception.hpp xhnSTL/xhn_exception.hpp
cp ../xhnSTL/xstl/xhn_file_stream.hpp xhnSTL/xhn_file_stream.hpp
cp ../xhnSTL/xstl/xhn_fixed_queue.hpp xhnSTL/xhn_fixed_queue.hpp
cp ../xhnSTL/xstl/xhn_functor.hpp xhnSTL/xhn_functor.hpp
cp ../xhnSTL/xstl/xhn_garbage_collector.hpp xhnSTL/xhn_garbage_collector.hpp
cp ../xhnSTL/xstl/xhn_gc_array.h xhnSTL/xhn_gc_array.h
cp ../xhnSTL/xstl/xhn_gc_container_base.h xhnSTL/xhn_gc_container_base.h
cp ../xhnSTL/xstl/xhn_gc_string.h xhnSTL/xhn_gc_string.h
cp ../xhnSTL/xstl/xhn_gc_values.h xhnSTL/xhn_gc_values.h
cp ../xhnSTL/xstl/xhn_hash_set.hpp xhnSTL/xhn_hash_set.hpp
cp ../xhnSTL/xstl/xhn_iterator.hpp xhnSTL/xhn_iterator.hpp
cp ../xhnSTL/xstl/xhn_lambda.hpp xhnSTL/xhn_lambda.hpp
cp ../xhnSTL/xstl/xhn_list.hpp xhnSTL/xhn_list.hpp
cp ../xhnSTL/xstl/xhn_list2.h xhnSTL/xhn_list2.h
cp ../xhnSTL/xstl/xhn_list_template.hpp xhnSTL/xhn_list_template.hpp
cp ../xhnSTL/xstl/xhn_lock.hpp xhnSTL/xhn_lock.hpp
cp ../xhnSTL/xstl/xhn_map.hpp xhnSTL/xhn_map.hpp
cp ../xhnSTL/xstl/xhn_memory.hpp xhnSTL/xhn_memory.hpp
cp ../xhnSTL/xstl/xhn_open_addressing_hash_table.hpp xhnSTL/xhn_open_addressing_hash_table.hpp
cp ../xhnSTL/xstl/xhn_operation.hpp xhnSTL/xhn_operation.hpp
cp ../xhnSTL/xstl/xhn_operation_manager.hpp xhnSTL/xhn_operation_manager.hpp
cp ../xhnSTL/xstl/xhn_pair.hpp xhnSTL/xhn_pair.hpp
cp ../xhnSTL/xstl/xhn_rbtree.hpp xhnSTL/xhn_rbtree.hpp
cp ../xhnSTL/xstl/xhn_recursive.hpp xhnSTL/xhn_recursive.hpp
cp ../xhnSTL/xstl/xhn_routine_pool.hpp xhnSTL/xhn_routine_pool.hpp
cp ../xhnSTL/xstl/xhn_set.hpp xhnSTL/xhn_set.hpp
cp ../xhnSTL/xstl/xhn_singly_linked_list.hpp xhnSTL/xhn_singly_linked_list.hpp
cp ../xhnSTL/xstl/xhn_smart_ptr.hpp xhnSTL/xhn_smart_ptr.hpp
cp ../xhnSTL/xstl/xhn_state_machine.hpp xhnSTL/xhn_state_machine.hpp
cp ../xhnSTL/xstl/xhn_static_string.hpp xhnSTL/xhn_static_string.hpp
cp ../xhnSTL/xstl/xhn_string.hpp xhnSTL/xhn_string.hpp
cp ../xhnSTL/xstl/xhn_string_base.hpp xhnSTL/xhn_string_base.hpp
cp ../xhnSTL/xstl/xhn_thread.hpp xhnSTL/xhn_thread.hpp
cp ../xhnSTL/xstl/xhn_thread_map.hpp xhnSTL/xhn_thread_map.hpp
cp ../xhnSTL/xstl/xhn_thread_pool.hpp xhnSTL/xhn_thread_pool.hpp
cp ../xhnSTL/xstl/xhn_tuple.hpp xhnSTL/xhn_tuple.hpp
cp ../xhnSTL/xstl/xhn_unique_identifier.hpp xhnSTL/xhn_unique_identifier.hpp
cp ../xhnSTL/xstl/xhn_utility.hpp xhnSTL/xhn_utility.hpp
cp ../xhnSTL/xstl/xhn_vector.hpp xhnSTL/xhn_vector.hpp
cp ../xhnSTL/xstl/xhn_void_vector.hpp xhnSTL/xhn_void_vector.hpp

cp common.h xhnSTL/common.h
cp etypes.h xhnSTL/etypes.h

cd ./UnitTest
./build.sh
