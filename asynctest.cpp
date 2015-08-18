#include <memory>
#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include "Queue.h"

using namespace std;

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
//_declspec(thread) int a = 0;
class Worker{
public:
    Worker(){
        std::thread work([this](){
            while (running_){
                std::function<void()> task;
                queue_.pop(task);
                task();
            }
            cout << " end thread" << std::this_thread::get_id() << endl;
        });
        thread_ = std::move(work);
    }
    ~Worker(){
        running_ = false;
        queue_.push([](){});
        thread_.join();
    }
    void execute(std::function<void()> task){
        queue_.push(task);
    }
private:
    Queue<std::function<void()>> queue_;
    std::thread thread_;
    bool running_ = true;
};

class ExecutorService{
public:
    ExecutorService( int size){
        this->size_ = size;
        for (int i = 0; i < size; i++){
            workers_.push_back(std::unique_ptr<Worker>(new Worker));
        }
    }

    ~ExecutorService(){
        workers_.clear();
        cout << "exit" << endl;
    }
    void execute(std::function<void()> task){
        workers_[(sequence_++) % size_]->execute(task);
    }
private:
    std::vector<std::unique_ptr<Worker>> workers_;
    int size_ = 0;
    int sequence_ = 0;
};



int main(int argc, char* argv[]){
    {
        ExecutorService executorService(4);
        for (int i = 0; i < 16; i++){
            executorService.execute(
                [](){cout << std::this_thread::get_id() << " executed." << endl; }
            );
        }
        std::chrono::seconds dura(5);
        std::this_thread::sleep_for(dura);
    }
}
