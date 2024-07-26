#pragma once

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "Uncopyable.hpp"

using wlock = std::unique_lock<std::shared_mutex>;
using rlock = std::shared_lock<std::shared_mutex>;

#define thread_num 2

#ifdef _WIN32
    #define COMPILER_BARRIER _ReadWriteBarrier()

#elif __linux__
    #define COMPILER_BARRIER asm volatile("" ::: "memory")

#endif
