cmake_minimum_required(VERSION 3.13.4)

project(tests)

set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=cp1252")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-stack-arg-probe -fno-stack-check -fno-stack-protector")

string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

if(WIN32)
        add_compile_definitions(WIN32)
        add_compile_definitions(_CONSOLE)
        add_link_options("-nostartfiles" "-nodefaultlibs" "-nostdlib" "-ffreestanding" "-Xlinker" "--stack=0x100000,0x100000")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BINARY_SUFFIX "d")
        add_compile_definitions(_DEBUG)
else()
        set(BINARY_SUFFIX "")
        add_compile_definitions(NDEBUG)
        add_link_options("-s")
endif()

include_directories(include)

include_directories(${PARENT_DIR}/rt15_portable_runtime/rpr/include)

link_directories(${PARENT_DIR}/rt15_portable_runtime/rpr/lib)

link_libraries(rpr${BINARY_SUFFIX})

if(WIN32)
        link_libraries(ws2_32)
        link_libraries(rpcrt4)
        link_libraries(shell32)
        link_libraries(user32)
        link_libraries(kernel32)
else()
        link_libraries(uuid)
        link_libraries(pthread)
        link_libraries(crypto)
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(${PROJECT_NAME}${BINARY_SUFFIX} ${SOURCES})
