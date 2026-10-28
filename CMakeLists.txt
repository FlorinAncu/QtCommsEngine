cmake_minimum_required(VERSION 3.16)

project(QtCommsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

# Source files
set(QTCOMMSENGINE_HEADERS
    include/qtcommsengine/BinaryProtocolSerializer.hpp
    include/qtcommsengine/CommChannel.hpp
    include/qtcommsengine/CommClient.hpp
    include/qtcommsengine/CommManager.hpp
    include/qtcommsengine/ConfigLoader.hpp
    include/qtcommsengine/ConnectionConfig.hpp
    include/qtcommsengine/Crc32.hpp
    include/qtcommsengine/ErrorCode.hpp
    include/qtcommsengine/ErrorHandler.hpp
    include/qtcommsengine/Logger.hpp
    include/qtcommsengine/Message.hpp
    include/qtcommsengine/MessageBuilder.hpp
    include/qtcommsengine/MessageParser.hpp
    include/qtcommsengine/MessagePriority.hpp
    include/qtcommsengine/MessageQueue.hpp
    include/qtcommsengine/MockChannel.hpp
    include/qtcommsengine/Protocol.hpp
    include/qtcommsengine/ProtocolCompatibility.hpp
    include/qtcommsengine/ProtocolValidator.hpp
    include/qtcommsengine/ProtocolVersion.hpp
    include/qtcommsengine/ResponseHandler.hpp
    include/qtcommsengine/RetryPolicy.hpp
    include/qtcommsengine/TcpChannel.hpp
    include/qtcommsengine/TcpServer.hpp
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
    src/TcpServer.cpp
)

add_library(QtCommsEngine STATIC
    ${QTCOMMSENGINE_HEADERS}
    ${QTCOMMSENGINE_SOURCES}
)

add_library(QtCommsEngine::QtCommsEngine ALIAS QtCommsEngine)

set_target_properties(QtCommsEngine PROPERTIES AUTOMOC ON)

target_link_libraries(QtCommsEngine
    PUBLIC
        Qt6::Core
        Qt6::Network
)

target_include_directories(QtCommsEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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
