# cmake file

# build tool configuration

cmake_minimum_required(VERSION 3.2.0)

enable_testing()

# add project cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(CMakePackageConfigHelpers)
include(Util)
include(HandleLLVMDependency)

option(LLVMUTILS_SKIP_TESTS "Skip configuring the project tests" ON)
option(LLVMUTILS_DEBUG "Enable project custom debug output facilities" OFF)

message(STATUS "project debug: ${LLVMUTILS_DEBUG}")

get_version(SHORT VERSION PRJ_VERSION)

set(PRJ_NAME LLVMUtils)
string(TOUPPER ${PRJ_NAME} PRJ_NAME_UPPER)
string(TOLOWER ${PRJ_NAME} PRJ_NAME_LOWER)

# part of LLVM or standalone
set(PRJ_STANDALONE_BUILD FALSE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(${PRJ_NAME} C CXX)

  set(PRJ_STANDALONE_BUILD TRUE)
  set(LLVMUTILS_SKIP_TESTS OFF)
  set(LLVMUTILS_DEBUG ON)
  set(LLVMUTILS_EXPORT ${PRJ_NAME})

  message(STATUS "configuring as standalone LLVM project")
endif()

if(NOT LLVMUTILS_EXPORT)
  message(FATAL_ERROR 
    "LLVMUTILS_EXPORT must be defined when built as dependent project")
endif()

file(RELATIVE_PATH PATH_PREFIX ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT PATH_PREFIX)
  set(PATH_PREFIX ".")
endif()

# CMP0056 is required for any libc++abi dependency of the try_compile() command
# this bumps required version to 3.2
# otherwise LD_LIBRARY_DIR needs to be set
set_policies(CMP0056 NEW)


# requirements

handle_llvm_dependency(${PRJ_STANDALONE_BUILD})


# project configuration

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

message(STATUS "RTTI support: ${LLVM_ENABLE_RTTI}")

if(NOT LLVM_ENABLE_EH)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
endif()

message(STATUS "EH support: ${LLVM_ENABLE_EH}")


#

set(LIB_NAME "${PRJ_NAME}")
set(LIB_SOURCES 
  "lib/BasicBlockUtils.cpp"
  "lib/LoopUtils.cpp"
  "lib/FunctionUtils.cpp")

add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

# according to http://llvm.org/docs/CMake.html
# we do not need the below library dependencies since the plugin will be loaded
# via opt or clang which already have those libs in their dependencies

llvm_map_components_to_libnames(llvm_libs core support analysis)

if(llvm_libs)
  list(GET llvm_libs 0 single_llvm_lib)
  get_target_property(target_type ${single_llvm_lib} TYPE)
endif()

if(${target_type} STREQUAL "STATIC_LIBRARY")
  target_link_libraries(${LIB_NAME} PUBLIC ${llvm_libs})
endif()

set_target_properties(${LIB_NAME} PROPERTIES 
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CMAKE_CXX_EXTENSIONS OFF)

target_compile_definitions(${LIB_NAME} PUBLIC ${LLVM_DEFINITIONS})
target_compile_definitions(${LIB_NAME} PRIVATE VERSION_STRING=${PRJ_VERSION})

set(PRJ_DEBUG_VALUE 0)
if(LLVMUTILS_DEBUG)
  set(PRJ_DEBUG_VALUE 1)
endif()

target_include_directories(${LIB_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(${LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(${LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
target_include_directories(${LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${PATH_PREFIX}/include>)

#

get_property(TRGT_PREFIX TARGET ${LIB_NAME} PROPERTY PREFIX)
get_property(TRGT_SUFFIX TARGET ${LIB_NAME} PROPERTY SUFFIX)

if(NOT TRGT_PREFIX)
  set(TRGT_PREFIX ${CMAKE_SHARED_MODULE_PREFIX})
endif()

if(NOT TRGT_SUFFIX)
  set(TRGT_SUFFIX ${CMAKE_SHARED_MODULE_SUFFIX})
endif()

#

set(TESTEE_PREFIX ${TRGT_PREFIX})
set(TESTEE_SUFFIX ${TRGT_SUFFIX})
set(TESTEE_LIB ${LIB_NAME})

add_subdirectory(unittests)
add_subdirectory(tests)
add_subdirectory(doc)


if(PRJ_STANDALONE_BUILD)
  attach_compilation_db(TARGET ${LIB_NAME})
endif()


# installation

set(CMAKE_INSTALL_MESSAGE "LAZY")

set(PRJ_INSTALL_INCLUDE_DIR "${PATH_PREFIX}/include")
set(PRJ_INSTALL_SHARE_DIR "${PATH_PREFIX}/share")
set(PRJ_CONFIG_HEADER_FILE "Config.hpp")
set(PRJ_CMAKE_CONFIG_FILE "config.cmake")
set(PRJ_CMAKE_CONFIG_EXTRA_FILE "config-extra.cmake")

install(TARGETS ${LIB_NAME} EXPORT ${LLVMUTILS_EXPORT}
  ARCHIVE DESTINATION "${PATH_PREFIX}/lib")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PRJ_CONFIG_HEADER_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/${PRJ_CONFIG_HEADER_FILE}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/cmake/${PRJ_CMAKE_CONFIG_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME_LOWER}-${PRJ_CMAKE_CONFIG_FILE}"
  @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/cmake/${PRJ_CMAKE_CONFIG_EXTRA_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME}-${PRJ_CMAKE_CONFIG_EXTRA_FILE}"
  @ONLY)

get_version(SHORT VERSION PRJ_VERSION_SHORT)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME_LOWER}-config-version.cmake"
  VERSION ${PRJ_VERSION_SHORT} COMPATIBILITY SameMajorVersion)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" 
  DESTINATION ${PRJ_INSTALL_INCLUDE_DIR}
  PATTERN "*.in" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/" 
  DESTINATION ${PRJ_INSTALL_INCLUDE_DIR})

if(PRJ_STANDALONE_BUILD)
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/share/"
    DESTINATION ${PRJ_INSTALL_SHARE_DIR})
endif()

if(PRJ_STANDALONE_BUILD)
  install(EXPORT ${LLVMUTILS_EXPORT} DESTINATION "${PRJ_INSTALL_SHARE_DIR}/cmake")
endif()

