#include <iostream>

#include "JobDequeManager.hpp"
#include "ThreadPool.hpp"

int main()
{
    JobDequeManager& jobDequeManager = JobDequeManager::instance();
    ThreadPool&      pool            = ThreadPool::instance(thread_num);

    // print pool status
    std::cout << "pool is running: " << pool.isRunning() << std::endl;

    {
        std::vector<std::future<int>> int_futs;

        for (int i = 0; i < 10; ++i)
        {
            int_futs.emplace_back(jobDequeManager.async([](int i) -> int { return i; }, i));
        }

        bool flag = true;
        for (int i = 0; i != 10; ++i)
        {
            if (int_futs[i].get() != i)
                flag = false;
        }

        if (flag)
            std::cout << "test successful" << std::endl;
        else
            std::cout << "test failed" << std::endl;
    }

    return 0;
}