# Multi-thread

## Include

### thread_pool

It's a simple job system rather than thread pool by locks which may loss preformance.

### job_system

To learn Lock-Free Programming firstly.

Here is the plan.

- [✔] Free lock deques
- [✔] Steal jobs
- [✖] **Three deque cache**
- [✖] Distribute jobs

Well, it like a factory, main thread creates jobs -> job poll distributes jobs -> deques in job pool stores jobs -> deques of threads get jobs -> threads do jobs.

Over all, I think the most important theroy in Lock-Free Programming is avoiding collision(like deque or cache).
