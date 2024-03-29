cmake_minimum_required(VERSION 3.13.4)

project(rpr)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=cp1252")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-stack-arg-probe -fno-stack-check -fno-stack-protector")

string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

if(WIN32)
        add_compile_definitions(WIN32)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BINARY_SUFFIX "d")
        add_compile_definitions(_DEBUG)
else()
        set(BINARY_SUFFIX "")
        add_compile_definitions(NDEBUG)
endif()

include_directories(include)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")


add_library(${PROJECT_NAME}${BINARY_SUFFIX} SHARED ${SOURCES})
add_library(static${PROJECT_NAME}${BINARY_SUFFIX} STATIC ${SOURCES})


# Remove the lib prefix from the dll name.
if(WIN32)
        set_property(TARGET ${PROJECT_NAME}${BINARY_SUFFIX} PROPERTY PREFIX "")
        target_link_options(${PROJECT_NAME}${BINARY_SUFFIX} PUBLIC "-nostartfiles" "-nodefaultlibs" "-nostdlib" "-ffreestanding" "-Xlinker" "--stack=0x0fffff,0x100000")

        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} ws2_32)
        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} rpcrt4)
else()
        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} rt)
        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} uuid)
        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} pthread)
        target_link_libraries(${PROJECT_NAME}${BINARY_SUFFIX} crypto)
endif()

target_compile_definitions(${PROJECT_NAME}${BINARY_SUFFIX} PUBLIC RT_SHARED_LIBRARY)
