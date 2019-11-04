cmake_minimum_required (VERSION 2.6)
PROJECT(xhnSTL C CXX)

IF (NOT CMAKE_BUILD_TYPE)
# SET(CMAKE_BUILD_TYPE "Debug")
 SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
#MESSAGE("CMAKE_CXX_FLAGS_DEBUG="+${CMAKE_CXX_FLAGS_DEBUG})

#statically linking VC++ isn't supported for WindowsPhone/WindowsStore
IF (CMAKE_SYSTEM_NAME STREQUAL WindowsPhone OR CMAKE_SYSTEM_NAME STREQUAL WindowsStore)
	OPTION(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)
ELSE ()
	OPTION(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" OFF)
ENDIF (CMAKE_SYSTEM_NAME STREQUAL WindowsPhone OR CMAKE_SYSTEM_NAME STREQUAL WindowsStore)

IF(MSVC)
	IF (NOT USE_MSVC_INCREMENTAL_LINKING)
		#MESSAGE("MSVC_INCREMENTAL_DEFAULT"+${MSVC_INCREMENTAL_DEFAULT})
		SET( MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")

		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO ${replacementFlags}" )
		MESSAGE("CMAKE_EXE_LINKER_FLAGS_DEBUG=${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags2 ${CMAKE_EXE_LINKER_FLAGS})

		SET(CMAKE_EXE_LINKER_FLAGS ${replacementFlag2})
		STRING(REPLACE "INCREMENTAL:YES" "" replacementFlags3 "${CMAKE_EXTRA_LINK_FLAGS}")

		SET(CMAKE_EXTRA_LINK_FLAGS ${replacementFlag3})


		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags3 "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${replacementFlags3})
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO ${replacementFlags3}" )

	ENDIF (NOT USE_MSVC_INCREMENTAL_LINKING)

	IF (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		#We statically link to reduce dependancies
		FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
			IF(${flag_var} MATCHES "/MD")
				STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			ENDIF(${flag_var} MATCHES "/MD")
			IF(${flag_var} MATCHES "/MDd")
				STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
			ENDIF(${flag_var} MATCHES "/MDd")
		ENDFOREACH(flag_var)
	ENDIF (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)

	IF (CMAKE_CL_64)
	  ADD_DEFINITIONS(-D_WIN64)
	ELSE()
	  OPTION(USE_MSVC_SSE "Use MSVC /arch:sse option"	ON)
	  option(USE_MSVC_SSE2 "Compile your program with SSE2 instructions" ON)

	  IF (USE_MSVC_SSE)
		  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE")
	  ENDIF()
	  IF (USE_MSVC_SSE2)
		  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
	  ENDIF()

	ENDIF()

	option(USE_MSVC_AVX "Compile your program with AVX instructions"  OFF)

	IF(USE_MSVC_AVX)
		add_definitions(/arch:AVX)
	ENDIF()

	OPTION(USE_MSVC_FAST_FLOATINGPOINT "Use MSVC /fp:fast option"	ON)
	IF (USE_MSVC_FAST_FLOATINGPOINT)
		  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
  ENDIF()

	OPTION(USE_MSVC_STRING_POOLING "Use MSVC /GF string pooling option"	ON)
	IF (USE_MSVC_STRING_POOLING)
		SET(CMAKE_C_FLAGS "/GF ${CMAKE_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "/GF ${CMAKE_CXX_FLAGS}")
	ENDIF()

	OPTION(USE_MSVC_FUNCTION_LEVEL_LINKING "Use MSVC /Gy function level linking option"	ON)
	IF(USE_MSVC_FUNCTION_LEVEL_LINKING)
		SET(CMAKE_C_FLAGS "/Gy ${CMAKE_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "/Gy ${CMAKE_CXX_FLAGS}")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF")
	ENDIF(USE_MSVC_FUNCTION_LEVEL_LINKING)

	OPTION(USE_MSVC_EXEPTIONS "Use MSVC C++ exceptions option"	OFF)



	OPTION(USE_MSVC_COMDAT_FOLDING "Use MSVC /OPT:ICF COMDAT folding option"	ON)

	IF(USE_MSVC_COMDAT_FOLDING)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:ICF")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:ICF")
	ENDIF()

	OPTION(USE_MSVC_DISABLE_RTTI "Use MSVC /GR- disabled RTTI flags option"	ON)
	IF(USE_MSVC_DISABLE_RTTI)
	  STRING(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable RTTI
		SET(CMAKE_C_FLAGS "/GR- ${CMAKE_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "/GR- ${CMAKE_CXX_FLAGS}")
	ENDIF(USE_MSVC_DISABLE_RTTI)

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4267")
ENDIF(MSVC)
# Initialize CXXFLAGS.
#set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
#set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../pthread/lib/x64)

INCLUDE_DIRECTORIES(./
                    ./xhnSTL/
                    ../pthread/include/
                   ../xhnSTL/win/
                    ../xhnSTL/xstl/
                    ../xhnSTL/xlibc/)

add_definitions(-D_WIN64 -DHAVE_STRUCT_TIMESPEC)
ADD_LIBRARY(xhnSTL STATIC
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
)
#set_target_properties(xhnSTL PROPERTIES ENABLE_EXPORTS 1)
#set_target_properties(xhnSTL PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)


add_custom_command(TARGET xhnSTL
  PRE_BUILD 
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/copy_headers.sh
  COMMENT "This command will be executed before build target xhnSTL"
)



target_link_libraries(xhnSTL pthreadVC2 uuid)
