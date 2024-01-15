#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include <cstdlib>
#include <time.h>

class Task {
public:
    Task(std::function<void()> func) : func(func) {
        //duration = rand() % 6 + 5; // Generate random duration between 5 and 10 seconds
        //id++;
    }

    void operator()() const {
        //std::string str("Executing task with duration " + std::to_string(duration) + " seconds. Id: " + std::to_string(id) + "\n");
        //std::cout << str;
        //std::this_thread::sleep_for(std::chrono::seconds(duration));
        func();
    }

    bool operator<(const Task& other) const {
        return id < other.id;
        //return duration < other.duration; // Shorter tasks have higher priority
    }
    bool operator>(const Task& other) const {
        return id > other.id;
        //return duration > other.duration; // Shorter tasks have higher priority
    }
    //int duration;
    int id = rand();
private:
    std::function<void()> func;

};

class ThreadPool {
public:
    ThreadPool(int num_threads = 4) : shutdown(false) {
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(std::bind(&ThreadPool::Worker, this));
            //threads[i].detach();
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            shutdown = true;
        }
        condition.notify_all();
        for (auto& thread : threads) {
            thread.join();
        }
    }

    void addTask(const Task& task) {
        std::unique_lock<std::mutex> lock(mutex);
        /*if (queue.size() == 15) {
            queue.pop();
        }*/
        queue.push(task);
        condition.notify_one();
    }
    std::vector<std::thread> threads;
private:

    std::priority_queue<Task, std::vector<Task>, std::less<Task>> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool shutdown;

    void Worker() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            while (!shutdown && queue.empty()) {
                condition.wait(lock);
            }
            if (shutdown) {
                return;
            }
            Task task = queue.top();
            queue.pop();
            lock.unlock();
            task();
            //std::this_thread::sleep_for(std::chrono::seconds(task.duration));
        }
    }
public:
    bool empty() {
        return queue.empty();
    }
    void lock() {
        std::unique_lock<std::mutex> lock(mutex);
    }
};
