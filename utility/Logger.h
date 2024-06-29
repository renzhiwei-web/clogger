#pragma once
#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <string.h>
#include <mutex>
#include <thread>
// #include <functional>
using namespace std;

namespace yazi{
namespace utility{

#define debug(format,...) Logger::instance() -> log(Logger::DEBUG,__FILE__,__LINE__,format,##__VA_ARGS__);
#define info(format,...) Logger::instance() -> log(Logger::INFO,__FILE__,__LINE__,format,##__VA_ARGS__);
#define warn(format,...) Logger::instance() -> log(Logger::WARN,__FILE__,__LINE__,format,##__VA_ARGS__);
#define error(format,...) Logger::instance() -> log(Logger::ERROR,__FILE__,__LINE__,format,##__VA_ARGS__);
#define fatal(format,...) Logger::instance() -> log(Logger::FATAL,__FILE__,__LINE__,format,##__VA_ARGS__);


class Logger
{

public:
    enum Level
    {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT
    };
    static Logger * instance();

    // 日志等级，日志文件，程序行号，输出格式,输出的内容由可变参数接收
    // 这里的可变参数是值传递
    template<typename... Args>
    void log(Level level,const char* file,int line,const std::string& format,Args... args){
        // 这里值捕获
        auto func = [=](){
            log_(level,file,line,format,args...);
        };
        thread t1(func);
        t1.detach();
    }


    // 日志等级，日志文件，程序行号，输出格式,输出的内容由可变参数接收
    // 使用可变参数模板进行重构
    template<typename... Args>
    void log_(Level level,const char* file,int line,const std::string& format,Args&&... args){
        std::unique_lock<std::mutex> lk(m_mtx);
        if (m_level > level)
        {
            return;
        }
        if (m_fout.fail())
        {
            throw logic_error("open file failed" + m_filename);
        }
        
        time_t ticks = time(NULL);
        struct tm * ptm = localtime(&ticks);
        char timestamp[32];
        memset(timestamp,0,sizeof(timestamp));
        // 将时间转为自己的格式
        strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptm);

        std::string fmt = "%s %s %s:%d";
        std::string buffer;
        formt_str(buffer,fmt,timestamp,s_level[level],file,line);
        m_fout << buffer;


        m_fout << "\t";

        if (sizeof...(args) > 0)
        {
            buffer.clear();
            formt_str(buffer,format,args...);
            m_fout << buffer;
        }
        else{
            m_fout << format;
        }
        
        m_fout << "\n";
        m_row++;
        m_fout.flush();
        if (m_row_max > 0 && m_row >= m_row_max){
            rotate();
        }
    }
    void open(const string & filename);
    void close();
    // 设置日志的等级，低于这个等级的日志将不会被显示
    void level(Level level);
    void max(int bytes);
    
private:
    Logger();
    ~Logger();
    void rotate();
    void get_row();

    template<typename... Args>
    void formt_str(std::string& content,const std::string& fmt,Args... args){
        int len_str = std::snprintf(nullptr,0,fmt.c_str(),args...);
        if (0 >= len_str)
        {
            return;
        }
        content.resize(len_str);
        std::snprintf(&content[0],++len_str,fmt.c_str(),args...);
    }

private:
    string m_filename;
    // 输出日志文件指针
    ofstream m_fout;
    // 在日志文件中记录的日志等级
    static const char * s_level[LEVEL_COUNT];
    // 单例模式，全局唯一的
    static Logger * m_instance;
    Level m_level;
    // 每个日志文件最大存放长度
    int m_row_max;
    // 当前日志文件已经存放的长度
    int m_row;
    // 多线程下
    std::mutex m_mtx;
};
}
}

#endif