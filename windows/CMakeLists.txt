cmake_minimum_required(VERSION 3.9)
project(jwm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JWM_ARCH)
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(JWM_ARCH "arm64")
    else()
        set(JWM_ARCH "x64")
    endif()
endif()

# Find jdk location to use jni includes
set(JAVA_HOME $ENV{JAVA_HOME})
if (JAVA_HOME)
    message(STATUS "Java home found by JAVA_HOME env variable: ${JAVA_HOME}")
else()
    message(FATAL_ERROR "Java home not found! Set JAVA_HOME env variable to provide correct path")
endif()

file(GLOB SOURCES_CXX ${CMAKE_CURRENT_LIST_DIR}/../shared/cc/*.cc ${CMAKE_CURRENT_LIST_DIR}/cc/*.cc)
file(GLOB SOURCES_CXX_IMPL ${CMAKE_CURRENT_LIST_DIR}/../shared/cc/impl/*.cc)
add_library(jwm SHARED ${SOURCES_CXX} ${SOURCES_CXX_IMPL})

target_include_directories(jwm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../shared/cc)
target_include_directories(jwm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cc)
target_include_directories(jwm PRIVATE ${JAVA_HOME}/include ${JAVA_HOME}/include/win32)

target_link_libraries(jwm PRIVATE "shcore")
target_link_libraries(jwm PRIVATE "gdi32")
target_link_libraries(jwm PRIVATE "opengl32")
target_link_libraries(jwm PRIVATE "user32")
set_target_properties(jwm PROPERTIES OUTPUT_NAME "jwm_${JWM_ARCH}")