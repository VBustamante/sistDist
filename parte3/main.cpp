#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "nuklear.h"

using namespace std;

void *thread_ui_code(void *arg){
  cout << "UI" << endl;


  return nullptr;
}

void *thread_msgs_code(void *arg){
  cout << "MSGS" << endl;
  return nullptr;
}

void *thread_leader_code(void *arg){

  while(true){
    cout << "Found Leader" << endl;
    sleep(10);
  }
  return nullptr;
}

int main() {
  pthread_t thread_ui_id, thread_msgs_id, thread_leader_id;
  pthread_create(&thread_ui_id , nullptr, thread_ui_code, nullptr);
  pthread_create(&thread_msgs_id , nullptr, thread_msgs_code, nullptr);
  pthread_create(&thread_leader_id , nullptr, thread_leader_code, nullptr);
  pthread_join(thread_ui_id, nullptr);
  pthread_cancel(thread_msgs_id);
  pthread_cancel(thread_leader_id);

  return 0;
}