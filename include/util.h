#pragma once
#include <string>

#define ENABLE_DEBUG_LOG 0
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_ENUM_STRING(STRING) #STRING,
#define FOREACH_LOGLEVEL(ENUM_ITEM)\
        ENUM_ITEM(DEBUG)\
        ENUM_ITEM(INFO)\
        ENUM_ITEM(ERROR)\

enum LogLevel{
    FOREACH_LOGLEVEL(GENERATE_ENUM)
};

static const char* LogLevelString[] = {
    FOREACH_LOGLEVEL(GENERATE_ENUM_STRING)
};

void* log(LogLevel level, const char * string_format, ...);
std::string format(const char * string_format, ...);
std::vector<std::string> split(std::string, char separator);
std::string make_unique_name(const char* root);
void dumpf(void* p);
void dumpm(void* p);
int random(int min, int max);
std::string get_filename(const char* fullfilename);
bool is_new_line(int ch);