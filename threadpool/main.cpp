#include "ThreadPool.h"
#include <iostream>

void taskFunc(void* arg) {
    int nNum = *(int*)arg;
    delete (int*)arg;
    std::cout << "thread: " << std::this_thread::get_id() << ", number=" << nNum << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
    // 设置线程池最小5个线程，最大10个线程
    ThreadPool::ThreadPool pool(5, 20);
    int i;
    // 往任务队列中添加100个任务
    for (i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int* pNum = new int(i + 100);
        pool.addTask(taskFunc, (void*)pNum);
    }
	// for (; i < 200; ++i) {
	// 	std::this_thread::sleep_for(std::chrono::seconds(1));
	// 	int* pNum = new int(i + 100);
	// 	pool.addTask(taskFunc, (void*)pNum);
	// }
    std::this_thread::sleep_for(std::chrono::seconds(12));
    std::cout << pool.getCreatedThreadNums() << ' ' << pool.getExitedThreadNums() << std::endl;
    std::cout << "exiting" << std::endl;
    pool.shutdownPool();
    std::cout << pool.getCreatedThreadNums() << ' ' << pool.getExitedThreadNums() << std::endl;
    return 0;
}
