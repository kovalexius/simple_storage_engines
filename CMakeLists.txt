cmake_minimum_required(VERSION 2.8)

project(storageEngine)

find_package(Boost REQUIRED)

message(STATUS "${Boost_LIBRARY_DIRS}${Boost_LIBRARIES}")

include_directories(src/
					CatchTestInclude/single_include/
					${Boost_INCLUDE_DIRS}
					tests/commonTestTools/
					)

set(LOWLEVEL_STORAGE_SRC
			    src/stream_storage.h
				src/disk_storage.h
				src/disk_storage.cpp
				src/hash_storage.h
				src/hash_storage.cpp
				src/multikey_storage.h
				src/multikey_storage.cpp
				src/storage_types.h
    )

set(TEST_TOOLS
				tests/commonTestTools/testStorageTools.cpp
				tests/commonTestTools/testStorageTools.h)

add_executable( testHighLevelStorage
				tests/testStorage/main.cpp
				src/storage.h
                ${LOWLEVEL_STORAGE_SRC}
				)

target_link_libraries( testHighLevelStorage
						"${Boost_LIBRARY_DIRS}${Boost_LIBRARIES}"
						)
				
add_executable(
				testDiskStorage
				tests/testDiskStorage/main.cpp
				${LOWLEVEL_STORAGE_SRC}
				CatchTestInclude/single_include/catch.hpp
				${TEST_TOOLS}
				)
				
add_executable(
				perfomanceDiskStorage
				tests/perfomanceDiskStorage/main.cpp
				${LOWLEVEL_STORAGE_SRC}
				${TEST_TOOLS}
				)