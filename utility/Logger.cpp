#include "Logger.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdexcept>
#include <stdarg.h>
#include <fstream>
using namespace yazi::utility;
using namespace std;
const char * Logger::s_level[LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger * Logger::m_instance = NULL;

Logger::Logger() : m_level(DEBUG),m_row_max(0),m_row(0){

}

Logger::~Logger(){
    if (log_file == LogFile::LOGFILE) {
        m_fout.close();
    }
}

Logger * Logger::instance(){
    if (m_instance == NULL){
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::set_logfile(const string & filename){
    m_filename = filename;
    log_file = LogFile::LOGFILE;
    get_row();
    // 以追加的方式打开文件
    m_fout.open(m_filename,ios::app);
    if (m_fout.fail()){
        throw logic_error("open file failed" + m_filename);
    }
    redirect();
}

void Logger::get_row(){
    ifstream fin(m_filename);
    if (fin.fail())
    {
        m_row = 0;
        return;
    }
    
    std::string line;
    while (std::getline(fin, line)) {
        m_row++;
    }
    fin.close();
}
void Logger::redirect(){
    if (log_file != LogFile::LOGFILE) {
        std::cerr << "redircet log file failed when in COUT mode\n";
        throw;
    }
    std::streambuf* old = std::cout.rdbuf();
    
    std::cout.rdbuf(m_fout.rdbuf());
    
}

void Logger::open(){
    m_fout.open(m_filename,ios::app);
    if (m_fout.fail()){
        throw logic_error("open file failed" + m_filename);
    }
    m_row = 0;
    redirect();
}

void Logger::close(){
    m_fout.close();
}

// void Logger::log(Level level,const char * file,int line,const char * format,...){
//     if (m_level > level){
//         return;
//     }
//     if (m_fout.fail()){
//         throw logic_error("open file failed" + m_filename);
//     }
//     // 返回的是时间戳
//     time_t ticks = time(NULL);
//     // 将时间戳转为时间的结构
//     struct tm * ptm = localtime(&ticks);
//     char timestamp[32];
//     memset(timestamp,0,sizeof(timestamp));
//     // 将时间转为自己的格式
//     strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptm);
//     // cout << timestamp << endl;
//     // 存放日志的前缀格式
//     const char * fmt = "%s %s %s:%d";
//     // 因为不知道需要存储的字符数是多少，所以使用下面方法获取存储的字符数
//     int size = snprintf(NULL,0,fmt,timestamp,s_level[level],file,line);
//     if (size > 0){
//         char * buffer = new char[size + 1];
//         // memset(buffer,0,size + 1);
//         snprintf(buffer,size + 1,fmt,timestamp,s_level[level],file,line);
//         buffer[size] = '\0';
//         // cout << buffer << endl;
//         m_fout << buffer;
//         m_len += size;
//         delete [] buffer;
//     }
//     m_fout << "\t";
//     // 输出日志的内容
//     // 若字符串中存在可变参数，要求字符串的长度，需要使用以下方法
//     // 存放可变参数
//     va_list arg_ptr;
//     // 初始化参数
//     va_start(arg_ptr,format);
//     size = vsnprintf(NULL,0,format,arg_ptr);
//     va_end(arg_ptr);
//     if (size > 0)
//     {
//         char * content = new char[size + 1];
//         va_start(arg_ptr,format);
//         vsnprintf(content,size + 1,format,arg_ptr);
//         va_end(arg_ptr);
//         // cout << content << endl;
//         m_fout << content;
//         m_len += size;
//         delete [] content;
//     }
//     m_fout << "\n";
//     m_fout.flush();

//     if (m_len >= m_max && m_max > 0){
//         rotate();
//     }
// }

void Logger::level(Level level){
    m_level = level;
}

void Logger::max(int row){
    if (log_file != LogFile::LOGFILE) {
        std::cerr << "can not set m_row_max value when in LogFile::COUT mode\n";
        throw;
    }
    m_row_max = row;
}

void Logger::rotate(){
    close();
    // 返回的是时间戳
    time_t ticks = time(NULL);
    // 将时间戳转为时间的结构
    struct tm * ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp,0,sizeof(timestamp));
    // 将时间转为自己的格式
    strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptm);
    string filename = m_filename + timestamp;
    if (rename(m_filename.c_str(),filename.c_str()) != 0){
        throw logic_error("rename log file failed:" + string(strerror(errno)));
    }
    open();
}