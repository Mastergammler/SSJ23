#pragma once

#include "../globals.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

void InitLogger(Logger& logger, const string& log_filePath);
void stopLogger(Logger& logger);

// template <typename... Args> void Log(const string message, Args... args);
//  The overload breaks the finding of the template function
void Logf(const string message, ...);
void Log(const string message);
