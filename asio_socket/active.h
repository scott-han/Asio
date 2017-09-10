
#pragma once

#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "shared_queue.h"


using Task =  std::function<void()>;

class Active {
private:
  Active(const Active&) = delete;
  Active& operator=(const Active&) = delete;

  Active(std::string _thread_name) :done_(false),thread_name(_thread_name) {}                            // Construction ONLY through factory createActive();

  void doDone(){done_ = true;}
  void run();
  shared_queue<Task> mq_;
  std::thread thd_;
  bool done_;  // finished flag to be set through msg queue by ~Active
  std::string thread_name;
public:
  virtual ~Active();
  void send(Task msg_);
  static std::unique_ptr<Active> createActive(const std::string& thread_name); // Factory: safe construction & thread start
};




