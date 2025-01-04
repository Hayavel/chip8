cmake_minimum_required(VERSION 3.31.0)

project(Chip8 VERSION 0.0.1)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libraries/include)
set(LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libraries/lib)

include_directories(${INCLUDE_PATH})

set(USING_FILES src/main.cpp
                src/chip8.cpp)

set(RESOURCE res/chip8.rc)

if (CMAKE_BUILD_TYPE STREQUAL "")
    add_executable(${PROJECT_NAME} ${USING_FILES} ${RESOURCE})
    
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # set(CMAKE_<LANG>_FLAGS_RELEASE_INIT "-O2 -DNDEBUG") FOR assert
    add_executable(${PROJECT_NAME} WIN32 ${USING_FILES}) # WIN32
    
else()
    add_executable(${PROJECT_NAME} ${USING_FILES})
endif()

target_link_libraries(${PROJECT_NAME}
                      ${LIBRARY_PATH}/raylib.dll)



if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message("You're on Mac OS !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message("You're on Linux !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message("You're on Windows !")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_definitions(-DCOMPILE_WITH_SOME_PREPROCESSOR_DIRECTIVE)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})