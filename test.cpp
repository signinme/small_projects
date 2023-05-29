#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <shared_mutex>
 
std::shared_timed_mutex g_sMutex;
 
void writeTime(const std::string funName ,int time) {
    for (size_t i = 1; i < time; i++) {
        std::string outPut = funName + ":" + std::to_string(time);
        std::cout << outPut << std::endl;
        Sleep(1000);
    }
}
 
void shared_1(int seconds) {
    std::shared_lock<std::shared_mutex> theLock(g_sMutex);
    writeTime("shared_1" , seconds);
}
 
void shared_2(int seconds) {
    std::shared_lock<std::shared_mutex> theLock(g_sMutex);
    writeTime("shared_2", seconds);
}
 
 
 
void unique_1(int seconds) {
    std::unique_lock<std::shared_mutex> lck(g_sMutex);
    writeTime("unique_1", seconds);
}
 
void unique_2(int seconds) {
    std::unique_lock<std::shared_mutex> lck(g_sMutex);
    writeTime("unique_2", seconds);
}
 
void nomralSuo(int seconds) {
    std::lock_guard<std::shared_mutex> lck(g_sMutex);
    writeTime("nomralSuo", seconds);
}
 
int main()
{
    std::vector<std::thread> vecThread;
    vecThread.push_back(std::thread(nomralSuo, 10));
    vecThread.push_back(std::thread(shared_1, 10));
    vecThread.push_back(std::thread(shared_2, 10));
    vecThread.push_back(std::thread(unique_1, 10));
    vecThread.push_back(std::thread(unique_2, 10));
    for (auto & oneThread : vecThread)
    {
        oneThread.join();
    }
}