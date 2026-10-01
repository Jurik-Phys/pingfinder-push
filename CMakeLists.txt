cmake_minimum_required(VERSION 3.20)

# *** Application version info *** #
#         ${APP_VERSION}           #
#         ${APP_GIT_HASH}          #
#         ${APP_GIT_BRANCH}        #
#         ${APP_GIT_DIRTY}         #
# ******************************** #

project(pingfinder-push VERSION 0.1.2)

add_compile_definitions(
    APP_VERSION="${PROJECT_VERSION}"
)

execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git status --porcelain
    OUTPUT_VARIABLE GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(GIT_DIRTY "")
if(GIT_STATUS)
    set(GIT_DIRTY "-dirty")
endif()

add_compile_definitions(
    APP_GIT_HASH="${GIT_HASH}"
    APP_GIT_BRANCH="${GIT_BRANCH}"
    APP_GIT_DIRTY="${GIT_DIRTY}"
)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCE_EXE main.cpp cmd.cpp)

add_subdirectory(src)

add_executable(pingfinder-push ${SOURCE_EXE} ${RESOURCES})

target_link_libraries(pingfinder-push PRIVATE Qt6::Core Qt6::Network src-lib)
