include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)
set(CATCH2_ENABLE_MAIN OFF CACHE BOOL "" FORCE)

add_executable(QtCommsEngineTests
    main.cpp
    MessageQueueTests.cpp
    ProtocolValidatorTests.cpp
    TrySendOnceTests.cpp
    ReconnectionTests.cpp
)

target_link_libraries(QtCommsEngineTests PRIVATE QtCommsEngine Catch2::Catch2)

if(WIN32)
    add_custom_command(TARGET QtCommsEngineTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:QtCommsEngine> $<TARGET_FILE_DIR:QtCommsEngineTests>
    )
endif()

if(WIN32)
    add_custom_command(TARGET QtCommsEngineTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core> $<TARGET_FILE_DIR:QtCommsEngineTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Network> $<TARGET_FILE_DIR:QtCommsEngineTests>
    )
endif()

include(Catch)
catch_discover_tests(QtCommsEngineTests)