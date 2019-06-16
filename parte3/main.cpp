#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "Networker.h"

using namespace std;

#define BASE_PORT 9000

atomic_bool is_sleeping, leader_checking, start_election_intention;
atomic_bool aux_threads_running;

atomic_int counters_sent[(int) RequestType::size];
atomic_int counters_recv[(int) RequestType::size];
atomic_int leader_id;
int id, max_id;


bool leaderStatus(){
  return false;
}

int getSenderId(string msg) {
  return 0;
}

void sendMessage(int msg){
}

void *thread_msgs_code(void *arg){
  time_t start_election_time = 0;
  bool on_election = false;

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

  while(aux_threads_running){
    server.accept_msg();
    server.get_msg(incoming);

    auto msg_type = (RequestType) incoming[0];
    auto msg_sender = (unsigned char) incoming[1];

    counters_recv[(int) msg_type]++;
    printf("Got %s from %u\n", RequestTypeNames[(int) msg_type].c_str(), msg_sender);

    server.close_msg();
    continue;
    // these handlers are inactive if the process is sleeping
    if(!is_sleeping) {
      if (msg_type == RequestType::ELEICAO) {
        if (msg_sender < id){ //this process has a higher id, sends id to election starter
          sendMessage(1);
          counters_sent[(int) RequestType::OK]++;

          start_election_intention = true;
        }

      } else if (msg_type == RequestType::OK) {

      } else if (msg_type == RequestType::VIVO) {
        sendMessage(4);
        counters_sent[(int) RequestType::VIVO_OK]++;

      } else if (msg_type == RequestType::VIVO_OK) {
        leader_checking = false;
      }
    }

    //this handler still works even if the process is sleeping
    if (msg_type == RequestType::LIDER) { //received leader message
//      leader_id = getSenderId(incoming);
    }



    //starts election if there is intent
//    if(start_election_intention){
//      start_election_intention = false;
//      //todo mandar mensagem p todo mundo
//      on_election = true;
//      start_election_time = time(nullptr);
//    }
//
//    if(on_election && ((time(nullptr) - start_election_time) > 10)){
//      //this process is the new leader
//      leader_id = id;
//      //todo avisar todo mundo
//    }

    server.close_msg();
  }

  return nullptr;
}

void *thread_leader_code(void *arg){

  return nullptr;
  while(aux_threads_running){
    leader_checking = true;
    sendMessage(3);
    counters_sent[(int) RequestType::LIDER]++;

    sleep(10);
    if(leader_checking){
      leader_checking = false;  //todo mutex
      start_election_intention = true;
    } else {
      cout << "Found Leader" << endl;
    }
  }
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
  aux_threads_running = true;


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
      if (leaderStatus()){
        cout << "leader is alive" << endl;
      } else {
        cout << "king's dead" << endl;
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
      cout << "stats" << endl;
    }

    else{
      int a = 0;
      int b = 0;
      cin >> a;
      cin >> b;

      char msg[MSG_BUFSIZE];
      msg[0] = (unsigned char) a;
      msg[1] = (unsigned char) b;

      printf("Sending %u to %u\n", a, b);

      char port[6];
      sprintf(port, "%d", BASE_PORT + b);

      Networker::send_msg(port, msg);

//      cout << "invalid command" << endl;
    }

  }while(cmd != "end");

  aux_threads_running = false;
  pthread_join(thread_msgs_id, nullptr);
  pthread_join(thread_leader_id, nullptr);

  return 0;
}