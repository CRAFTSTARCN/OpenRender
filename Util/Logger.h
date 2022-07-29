#pragma once

#include  <fstream>

class Logger {

    std::ofstream* OutStream;
    Logger(const std::string& To);

    std::string GetCurrentTimeString() const;
    
public:


    ~Logger();

    void Log(const std::string& Content, const std::string& Title = "") const;
    void Warning(const std::string& Content, const std::string& Title = "") const;
    void Error(const std::string& Content, const std::string& Title = "") const;
    static Logger& Get();
    
};
