#pragma once

namespace qtcommsengine
{

    struct RetryPolicy
    {
        /// Maximum number of connection attempts after the initial failure.
        int maxRetries = 3;

        /// Delay between retry attempts, in milliseconds.
        int retryDelayMs = 100;

        /// Delay before starting reconnection, in milliseconds.
        int reconnectDelayMs = 500;
    };

}
