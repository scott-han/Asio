

#include "active.h"
#include <iostream>


Active::~Active() {
  Task quit_token = std::bind(&Active::doDone, this);
  send(quit_token); // tell thread to exit
  thd_.join();
}

// Add asynchronously a work-message to queue
void Active::send(Task msg_){
  mq_.push(msg_);
}



void Active::run() {
	std::cout << thread_name.c_str()<< " run on:" << std::this_thread::get_id()<<"\n";
  while (!done_) {
    // wait till job is available, then retrieve it and
    // executes the retrieved job in this thread (background)
    Task func;
    mq_.wait_and_pop(func);
    func();
  }
}

// Factory: safe construction of object before thread start
std::unique_ptr<Active> Active::createActive(const std::string& thread_name){
  std::unique_ptr<Active> aPtr(new Active(thread_name));
  aPtr->thd_ = std::thread(&Active::run, aPtr.get());
  return aPtr;
}
