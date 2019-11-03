#pragma once

#include <mutex>
#include <vector>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename Job>
class ConcurrentTask
{
public:
    ConcurrentTask(std::vector<Job> &tasks)
            : _tasks(std::move(tasks)
                    {}

    Job& getNextJob() {
        return _tasks.pop_back();}
private:

    std::mutex          _mutex;
    std::vector<Job>    _tasks;
};
