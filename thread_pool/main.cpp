#include <iostream>

#include "ThreadPool.hpp"

int main()
{
    // ThreadPool pool = ThreadPool::instance(); // error, can't copy
    ThreadPool& pool = ThreadPool::instance();

    // print pool status
    std::cout << "pool is running: " << pool.isRunning() << std::endl;

    {
        std::vector<std::future<int>> int_futs;

        for (int i = 0; i < 10; ++i)
        {
            int_futs.emplace_back(pool.async([](int i) -> int { return i; }, i));
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