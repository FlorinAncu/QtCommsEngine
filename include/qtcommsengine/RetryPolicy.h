#pragma once

namespace qtcommsengine
{

    struct RetryPolicy
    {
        int maxRetries = 3;
        int retryDelayMs = 100;
        int reconnectDelayMs = 500;
    };

}
