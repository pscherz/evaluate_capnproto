cmake_minimum_required (VERSION 3.0)

project(capnp_test1 VERSION 0.1.0 LANGUAGES CXX)

find_package(CapnProto PATHS C:/Users/patri/Documents/Projects/Libs/lib/cmake/CapnProto)

add_executable (${PROJECT_NAME} "main.cpp" message.capnp.c++ message.capnp.h)
target_link_libraries(${PROJECT_NAME} CapnProto::capnp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(OUTPUT message.capnp.c++ message.capnp.h
	COMMAND CapnProto::capnp_tool ARGS compile -oc++:${CMAKE_CURRENT_BINARY_DIR} --src-prefix=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/message.capnp
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS message.capnp)