cmake_minimum_required(VERSION 3.16)

project(QtCommsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 location (important when using vcpkg toolchain)
set(Qt6_DIR "C:/Qt/6.11.1/msvc2022_64/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core Network Test)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(QTCOMMSENGINE_HEADERS
    include/qtcommsengine/BinaryProtocolSerializer.h
    include/qtcommsengine/CommChannel.h
    include/qtcommsengine/CommClient.h
    include/qtcommsengine/CommManager.h
    include/qtcommsengine/ConfigLoader.h
    include/qtcommsengine/ConnectionConfig.h
    include/qtcommsengine/Crc32.h
    include/qtcommsengine/ErrorCode.h
    include/qtcommsengine/ErrorHandler.h
    include/qtcommsengine/Logger.h
    include/qtcommsengine/Message.h
    include/qtcommsengine/MessageBuilder.h
    include/qtcommsengine/MessageParser.h
    include/qtcommsengine/MessagePriority.h
    include/qtcommsengine/MessageQueue.h
    include/qtcommsengine/MockChannel.h
    include/qtcommsengine/Protocol.h
    include/qtcommsengine/ProtocolCompatibility.h
    include/qtcommsengine/ProtocolValidator.h
    include/qtcommsengine/ProtocolVersion.h
    include/qtcommsengine/ResponseHandler.h
    include/qtcommsengine/RetryPolicy.h
    include/qtcommsengine/TcpChannel.h
)

set(QTCOMMSENGINE_SOURCES
    src/BinaryProtocolSerializer.cpp
    src/CommChannel.cpp
    src/CommClient.cpp
    src/CommManager.cpp
    src/ConfigLoader.cpp
    src/Crc32.cpp
    src/ErrorHandler.cpp
    src/Logger.cpp
    src/Message.cpp
    src/MessageBuilder.cpp
    src/MessageParser.cpp
    src/MessageQueue.cpp
    src/MockChannel.cpp
    src/ProtocolCompatibility.cpp
    src/ProtocolValidator.cpp
    src/ResponseHandler.cpp
    src/TcpChannel.cpp
)

add_library(QtCommsEngine STATIC
    ${QTCOMMSENGINE_HEADERS}
    ${QTCOMMSENGINE_SOURCES}
)

set_target_properties(QtCommsEngine PROPERTIES AUTOMOC ON)

target_link_libraries(QtCommsEngine
    Qt6::Core
    Qt6::Network
)

target_include_directories(QtCommsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qtcommsengine
)

# Warnings
if(MSVC)
    target_compile_options(QtCommsEngine PRIVATE /W4)
else()
    target_compile_options(QtCommsEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Build options
option(QTCOMMSENGINE_BUILD_TESTS "Build QtCommsEngine unit tests" ON)
option(QTCOMMSENGINE_BUILD_EXAMPLES "Build QtCommsEngine examples" ON)

if(QTCOMMSENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(QTCOMMSENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
