
#include <thread>
#include <vector>
using namespace std;

#include "Logger.h"
using namespace yazi::utility;

Logger* loginfo = Logger::instance();

void printlog(){
    debug("Hello world");
    debug("name is %s, age is %d","jack",18);
    info("info message");
    error("error message");
    warn("warn message");
}

int main(){
    loginfo->open("test.log");
    Logger::instance() -> level(Logger::DEBUG);
    Logger::instance() -> max(90);


    // debug("Hello world");
    // debug("name is %s, age is %d","jack",18);

    vector<thread> vec;


    for (size_t i = 0; i < 10; i++)
    {
        
        vec.emplace_back(thread(printlog));
    }
    for (size_t i = 0; i < 10; i++)
    {
        if (vec[i].joinable())
        {
            vec[i].join();
        }
    }
    
    
    return 0;
}