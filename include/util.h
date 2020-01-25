#pragma once
#include <string>

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_ENUM_STRING(STRING) #STRING,
#define FOREACH_LOGLEVEL(LOGLEVEL)\
        LOGLEVEL(DEBUG)\
        LOGLEVEL(INFO)\
        LOGLEVEL(ERROR)\

enum LogLevel{
    FOREACH_LOGLEVEL(GENERATE_ENUM)
};

static const char* LogLevelString[] = {
    FOREACH_LOGLEVEL(GENERATE_ENUM_STRING)
};

void* log(LogLevel level, const char * string_format, ...);

std::string make_unique_name(const char* root);
void dump(void* p);
