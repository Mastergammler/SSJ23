#include "../imports.h"
#include "../utils.h"

using namespace std;

struct Logger
{
    ofstream logFile;
    vector<string> logBuffer;
    mutex logMutex;
    condition_variable logCondition;
    thread logThread;

    bool running;
};

void InitLogger(Logger& logger, const string& logFilePath)
{
    logger.logFile.open(logFilePath, ios::out | ios::app);
    logger.running = true;
    logger.logThread = thread([&logger]() {
        while (logger.running)
        {
            vector<string> logsToWrite;
            {
                unique_lock<mutex> lock(logger.logMutex);
                logger.logCondition.wait(lock, [&logger]() {
                    return !logger.logBuffer.empty() || !logger.running;
                });
                logsToWrite = std::move(logger.logBuffer);
                logger.logBuffer.clear();
            }
            for (const string& log : logsToWrite)
            {
                logger.logFile << log << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    });
}

string format(const char* format, va_list args)
{
    char buffer[256];
    vsprintf_s(buffer, format, args);

    return string(buffer);
}

void Log(Logger& logger, const string& message, ...)
{
    // va_list args;
    // va_start(args,message);
    // string formattedMsg = format(message, args);
    Debug(message);
    {
        unique_lock<mutex> lock(logger.logMutex);
        logger.logBuffer.push_back(message);
    }
    logger.logCondition.notify_one();
}

void stopLogger(Logger& logger)
{
    logger.running = false;
    logger.logCondition.notify_one();
    logger.logThread.join();
    logger.logFile.close();
}
