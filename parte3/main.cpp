#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "Networker.h"

using namespace std;

#define BASE_PORT 9000

atomic_bool is_sleeping, is_leader_checking, is_claiming;
atomic_bool aux_threads_running;

atomic_int counters_sent[(int) RequestType::size];
atomic_int counters_recv[(int) RequestType::size];
atomic_int leader_id;
int id, max_id;

void *thread_msgs_code(void *arg){

  std::string RequestTypeNames[] = {
      "ELEICAO",
      "OK",
      "LIDER",
      "VIVO",
      "VIVO_OK"
  };


  char port[6];
  sprintf(port, "%d", BASE_PORT + id);
  Networker server(port);

  char incoming[MSG_BUFSIZE];

  char out_port[6];

  while(aux_threads_running){
    server.accept_msg();
    server.get_msg(incoming);

    auto msg_type = (RequestType) incoming[0];
    auto msg_sender = (unsigned char) incoming[1];

    counters_recv[(int) msg_type]++;
    printf("Got %s from %u\n", RequestTypeNames[(int) msg_type].c_str(), msg_sender);

    // these handlers are inactive if the process is sleeping
    if(!is_sleeping) {
      if (msg_type == RequestType::ELEICAO) {
        if (msg_sender < id){ //this process has a higher id, sends id to election starter
          sprintf(out_port, "%d", BASE_PORT + msg_sender);
          Networker::send_msg(out_port, RequestType::OK, id);
          counters_sent[(int) RequestType::OK]++;
          is_claiming = true;
        }

      } else if (msg_type == RequestType::OK) {
        is_claiming = false;
      } else if (msg_type == RequestType::VIVO) {
        if(leader_id == id){
          sprintf(out_port, "%d", BASE_PORT + msg_sender);
          Networker::send_msg(out_port, RequestType::VIVO_OK, id);
          counters_sent[(int) RequestType::VIVO_OK]++;
        }
      } else if (msg_type == RequestType::VIVO_OK) {
        is_leader_checking = false;
      }
    }

    //this handler still works even if the process is sleeping
    if (msg_type == RequestType::LIDER) { //received leader message
      leader_id = msg_sender;
      is_claiming = false;
      is_leader_checking = false;
    }


    server.close_msg();
  }

  return nullptr;
}

void *thread_leader_code(void *arg){
  char port[6];

  while(aux_threads_running){
    sleep(5);
    if(is_sleeping) {
      cout << "Am sleeping"<<endl;
      continue;
    }

    if(leader_id == id) {
      cout << "Am leader"<<endl;
      continue;
    }

    if(!is_claiming){

      sprintf(port, "%d", BASE_PORT + leader_id);
      Networker::send_msg(port, RequestType::VIVO, id);
      counters_sent[(int) RequestType::LIDER]++;
      is_leader_checking = true;
      sleep(2);
      if(is_leader_checking) is_claiming = true;

    }

    if (!is_claiming){
      cout << "leader is alive" << endl;
      continue;
    } else {
      cout << "king's dead" << endl;
      sleep(1); // TODO id based timeouts

      for(int i = 1; i<= max_id; i++){
        if(i!=id){
          sprintf(port, "%d", BASE_PORT + i);
          Networker::send_msg(port, RequestType::ELEICAO, id);
        }
      }
      sleep(10);
      if(is_claiming){
        is_claiming = false;
        leader_id = id;
        cout << "Am leader"<<endl;


        for(int i = 1; i<= max_id; i++){
          if(i!=id){
            sprintf(port, "%d", BASE_PORT + i);
            Networker::send_msg(port, RequestType::LIDER, id);
          }
        }
      }

    }

  }
  return nullptr;
}

int main(int argc, char **argv) {

  // Parse Args
  if(argc != 3){
    printf("Missing arguments. run \"p3 $id $maxid\"\n");
    return 1;
  }

  id = atoi(argv[1]);
  if(id < 1){
    printf("id should be bigger than 0\n");
    return 1;
  }

  max_id = atoi(argv[2]);
  if(max_id < id){
    printf("max can't be less than id\n");
    return 1;
  }


  // Startup Globs
  is_sleeping = false;
  is_leader_checking = false;
  is_claiming = false;
  aux_threads_running = true;
  leader_id = max_id;


  // Start threads
  pthread_t thread_msgs_id, thread_leader_id;

  pthread_create(&thread_msgs_id , nullptr, thread_msgs_code, nullptr);
  pthread_create(&thread_leader_id , nullptr, thread_leader_code, nullptr);


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
      char port[6];
      sprintf(port, "%d", BASE_PORT + leader_id);
      Networker::send_msg(port, RequestType::VIVO, id);
      is_leader_checking = true;
      sleep(2);

      if (is_leader_checking){
        cout << "king's dead" << endl;
        is_claiming = true;
      } else {
        cout << "leader is alive" << endl;
      }
    }

    else if(cmd == "2"){
      //puts self to sleep
      is_sleeping = true;
      cout << "going to sleep" << endl;
    }

    else if(cmd == "3"){
      //wakes up
      is_sleeping = false;
      cout << "bom dia familia muita saude para todos mais um dia de batalha começa" << endl;
    }

    else if(cmd == "4"){
      //print stats
      // TODO Stats
      printf("Me: %u\nLeader: %u\n", id, leader_id.load());
    }

    else{
      cout << "invalid command" << endl;
    }

  }while(cmd != "end");

  aux_threads_running = false;
  pthread_join(thread_msgs_id, nullptr);
  pthread_join(thread_leader_id, nullptr);

  return 0;
}