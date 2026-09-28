#include "qtcommsengine/ErrorHandler.hpp"
#include "qtcommsengine/Logger.hpp"

namespace qtcommsengine
{

    void ErrorHandler::handle(ErrorCode code)
    {
        switch (code)
        {
        case ErrorCode::Ok:
            Logger::info("Ok");
            break;
        case ErrorCode::ChannelError:
            Logger::error("Channel error");
            break;
        case ErrorCode::Timeout:
            Logger::warn("Timeout");
            break;
        case ErrorCode::InvalidMessage:
            Logger::error("Invalid message");
            break;
        case ErrorCode::UnknownMessageId:
            Logger::warn("Unknown message id");
            break;
        case ErrorCode::ProtocolError:
            Logger::error("Protocol error");
            break;
        }
    }

}
