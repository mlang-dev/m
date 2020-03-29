#pragma once
#include <string>

using namespace std;

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
string format(const char * string_format, ...);
vector<string> split(string, char separator);
string make_unique_name(const char* root);
void dumpf(void* p);
void dumpm(void* p);
int random(int min, int max);
string get_filename(const char* fullfilename);
bool is_new_line(int ch);
string vector_to_string(vector<string>& v);
string char_to_string(char c);
string get_id_name();
void reset_id_name(string idname = "a");
