#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

sem_t is_sleeping_mutex, election_count_mutex, ok_count_mutex, leader_count_mutex, alive_count_mutex, alive_ok_count_mutex;
bool is_sleeping = false;
int election_count = 0, ok_count = 0, leader_count = 0, alive_count = 0, alive_ok_count = 0;

bool leaderStatus(){
  return false;
}

void *thread_ui_code(void *arg){
  string cmd;

  cout << "1 - check if leader is alive" << endl;
  cout << "2 - go to sleep" << endl;
  cout << "3 - wake up" << endl;
  cout << "4 - stats" << endl;
  cout << "end - exit" << endl;

  do {
    cout << "enter command" << endl;
    cin >> cmd;

    if(cmd == "end") break;
    else if(cmd == "1"){
      //check if leader is alive
      if (leaderStatus()){
        cout << "leader is alive" << endl;
      } else {
        cout << "king's dead" << endl;
      }
    }

    else if(cmd == "2"){
      //puts self to sleep
      sem_wait(&is_sleeping_mutex);
      is_sleeping = true;
      sem_post(&is_sleeping_mutex);
      cout << "going to sleep" << endl;
    }

    else if(cmd == "3"){
      //wakes up
      sem_wait(&is_sleeping_mutex);
      is_sleeping = false;
      sem_post(&is_sleeping_mutex);
      cout << "bom dia familia muita saude para todos mais um dia de batalha começa" << endl;
    }

    else if(cmd == "4"){
      //print stats
      sem_wait(&)

    }

    else{
      cout << "invalid command" << endl;
    }

  }while(cmd != "end");

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
  sem_init(&is_sleeping_mutex, 0, 1);
  sem_init(&election_count_mutex, 0, 1);
  sem_init(&ok_count_mutex, 0, 1);
  sem_init(&leader_count_mutex, 0, 1);
  sem_init(&alive_count_mutex, 0, 1);
  sem_init(&alive_ok_count_mutex, 0, 1);

  pthread_t thread_ui_id, thread_msgs_id, thread_leader_id;

  pthread_create(&thread_ui_id , nullptr, thread_ui_code, nullptr);
  pthread_create(&thread_msgs_id , nullptr, thread_msgs_code, nullptr);
  pthread_create(&thread_leader_id , nullptr, thread_leader_code, nullptr);

  pthread_join(thread_ui_id, nullptr);
  pthread_join(thread_msgs_id, nullptr);

  pthread_cancel(thread_msgs_id);
  pthread_cancel(thread_leader_id);

  sem_destroy(&is_sleeping_mutex);
  sem_destroy(&election_count_mutex);
  sem_destroy(&ok_count_mutex);
  sem_destroy(&leader_count_mutex);
  sem_destroy(&alive_count_mutex);
  sem_destroy(&alive_ok_count_mutex);

  return 0;
}