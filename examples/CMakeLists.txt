cmake_minimum_required(VERSION 3.16)

add_executable(QtCommsEngineExample
    main.cpp
)

set_target_properties(QtCommsEngineExample PROPERTIES AUTOMOC ON)

target_link_libraries(QtCommsEngineExample PRIVATE
    Qt6::Core
    Qt6::Network
    QtCommsEngine
)
