cmake_minimum_required(VERSION 3.5.0)


project (ababisme)

set(ProjectOutDir "${CMAKE_SOURCE_DIR}/game")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/projects/cmake")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

#set(CMAKE_CXX_COMPILER "$ENV{CROSS}g++" CACHE PATH "" FORCE)
#set(CMAKE_C_COMPILER "$ENV{CROSS}gcc" CACHE PATH "" FORCE)
#set(CMAKE_SYSROOT "/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(LIBC ON CACHE BOOL "" FORCE)
add_subdirectory(libs/SDL)
add_subdirectory(libs/SDL_image)
add_subdirectory(libs/zlib)

#add_compile_options(-Wno-unused-parameter -Wno-missing-field-initializers
#  -Wno-sign-compare -Wno-parentheses -Wno-unused-variable -Wno-narrowing
#)

# add_compile_options(-g -O2 -W -Wall -Wextra)
#add_compile_options(-g)
#add_compile_options(
#  $<$<COMPILE_LANGUAGE:CXX>:-Wno-reorder>
#  $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
#)

include_directories(src)
include_directories(libs/SDL/include libs/SDL_image libs/zlib)
include_directories(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/libs/zlib)

set(SRC_ROOT "${CMAKE_SOURCE_DIR}/src")

file(GLOB SOURCES_ROOT "${SRC_ROOT}/*.cpp")
file(GLOB SOURCES_VIEWS "${SRC_ROOT}/game/*.cpp")
file(GLOB SOURCES_IO "${SRC_ROOT}/gfx/*.cpp")
file(GLOB SOURCES_VM "${SRC_ROOT}/io/*.cpp")

set(SOURCES ${SOURCES_ROOT} ${SOURCES_VIEWS} ${SOURCES_IO} ${SOURCES_VM} ${SOURCES_LUA})

add_executable(ababisme ${SOURCES})

target_link_libraries(ababisme SDL2-static SDL2main SDL2_image zlibstatic)

set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${ProjectOutDir}")
if (CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELEASE ${PROJECT_NAME}64)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_DEBUG ${PROJECT_NAME}64_DEBUG)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_MINSIZEREL ${PROJECT_NAME}64_MINSIZEREL)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}64_RELWITHDEBINFO)
else()
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELEASE ${PROJECT_NAME})
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_DEBUG ${PROJECT_NAME}_DEBUG)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_MINSIZEREL ${PROJECT_NAME}_MINSIZEREL)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}_RELWITHDEBINFO)
endif()