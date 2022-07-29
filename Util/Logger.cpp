#include "Logger.h"

#include <chrono>

#define SEP "---------------------------------\n"
#define CAUSE_PREFIX "Cause: "
#define LOG_PREFIX "Level: LOG\n"
#define DEBUG_PREFIX "Level: DEBUG\n"
#define SEVER_PREFIX "Level: SEVER!\n"
#define TIME_PREFIX "Time: "

Logger::Logger(const std::string& To) {
    OutStream = new std::ofstream();
    OutStream->open(To.c_str(), std::ios::out);
}

std::string Logger::GetCurrentTimeString() const {
    auto Now = std::chrono::system_clock::now();
    std::string Result;
    Result.resize(26,0);

    time_t CtimeNow = std::chrono::system_clock::to_time_t(Now);
    ctime_s(&Result[0], 26, &CtimeNow);
    return Result;
}

Logger::~Logger() {
    OutStream->close();
    delete OutStream;
}

void Logger::Log(const std::string& Content, const std::string& Title) const {
    
    (*OutStream)<<LOG_PREFIX<<TIME_PREFIX<<GetCurrentTimeString().c_str()<<"\n";
    (*OutStream)<<CAUSE_PREFIX<<Title<<"\n";
    (*OutStream)<<Content<<std::endl;
    (*OutStream)<<SEP;
    (*OutStream)<<std::endl;
}

void Logger::Warning(const std::string& Content, const std::string& Title) const {
    (*OutStream)<<DEBUG_PREFIX<<TIME_PREFIX<<GetCurrentTimeString().c_str()<<"\n";
    (*OutStream)<<CAUSE_PREFIX<<Title<<"\n";
    (*OutStream)<<Content<<std::endl;
    (*OutStream)<<SEP;
    (*OutStream)<<std::endl;
}

void Logger::Error(const std::string& Content, const std::string& Title) const {
    (*OutStream)<<LOG_PREFIX<<TIME_PREFIX<<GetCurrentTimeString().c_str()<<"\n";
    (*OutStream)<<SEVER_PREFIX<<Title<<"\n";
    (*OutStream)<<Content<<std::endl;
    (*OutStream)<<SEP;
    (*OutStream)<<std::endl;
}

Logger& Logger::Get() {
    static Logger SingleTon("Log.txt");
    return SingleTon;
}
