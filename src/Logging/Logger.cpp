#include "module.h"

void InitLogger(Logger& logger, const string& log_filePath)
{
    logger.log_file.open(log_filePath, ios::out | ios::app);
    logger.running = true;
    logger.log_thread = thread([&logger]() {
        while (logger.running)
        {
            vector<string> logsToWrite;
            {
                unique_lock<mutex> lock(logger.log_mutex);
                logger.log_condition.wait(lock, [&logger]() {
                    return !logger.log_buffer.empty() || !logger.running;
                });
                logsToWrite = std::move(logger.log_buffer);
                logger.log_buffer.clear();
            }
            for (const string& log : logsToWrite)
            {
                logger.log_file << log << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    });
}

template <typename... Args> string format(const char* format, Args... args)
{
    char buffer[256];
    // vsprintf_s(buffer, format, args);

    return string(buffer);
}

void Logf(const string message, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, message);
    vsprintf_s(buffer, message.c_str(), args);
    va_end(args);

    string formatted(buffer);

    Debug(formatted);
    {
        unique_lock<mutex> lock(logger.log_mutex);
        logger.log_buffer.push_back(formatted);
    }
    logger.log_condition.notify_one();
}

void Log(const string message) { Logf(message, ""); }

// template <typename... Args> void Log(const string message, Args... args)
//{
//     // va_list args;
//     // va_start(args, message);
//     Log(logger, message, args...);
//     // va_end(args);
// }

void stopLogger(Logger& logger)
{
    logger.running = false;
    logger.log_condition.notify_one();
    logger.log_thread.join();
    logger.log_file.close();
}
