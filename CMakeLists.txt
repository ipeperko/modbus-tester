cmake_minimum_required(VERSION 3.10)
project(modbus_tester)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    add_compile_options(-mno-ms-bitfields)
endif(WIN32)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message("QTDIR HINT=${QTDIR}")
message("Qt5_DIR=${Qt5_DIR}")

if (QTDIR)
    set(Qt5_DIR ${QTDIR})
    set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif(QTDIR)

message("Qt5_DIR=${Qt5_DIR}")

set (LIBS PRIVATE Qt5::Widgets pthread modbus)

file (GLOB SOURCES
    src/*.cpp
    src/*.ui
    )

file (GLOB headers
    src/*.h
    )

include_directories(
    src
)

add_executable(${PROJECT_NAME} ${SOURCES} resources.qrc)
target_link_libraries(${PROJECT_NAME} ${LIBS})
