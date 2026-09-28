#include "qtcommsengine/ResponseHandler.hpp"
#include "qtcommsengine/Protocol.hpp"

namespace qtcommsengine
{

    ErrorCode ResponseHandler::handleResponse(const Message &msg)
    {
        MessageId id = static_cast<MessageId>(msg.getId());

        switch (id)
        {
        case MessageId::Error:
            return ErrorCode::ProtocolError;
        default:
            return ErrorCode::Ok;
        }
    }

}
