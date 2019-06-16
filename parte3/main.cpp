#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "Networker.h"

using namespace std;

sem_t is_sleeping_mutex, receive_msg_thread_mutex, check_leader_thread_mutex;
bool is_sleeping = false, leader_checking = false, start_election_intention = false;
int s_election_count = 0, s_ok_count = 0, s_leader_count = 0, s_alive_count = 0, s_alive_ok_count = 0;
int r_election_count = 0, r_ok_count = 0, r_leader_count = 0, r_alive_count = 0, r_alive_ok_count = 0;
int id, leader_id;
char *port, *port2; //TODO remove these they're for testing only

bool leaderStatus(){
  return false;
}

string receiveMessage(){
  return "a";
}

int getMessageType(string msg){
  return 0;
}

int getSenderId(string msg) {
  return 0;
}

void sendMessage(int msg){
  return;
}

void startElection(){
  return;
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
      cout << "stats" << endl;
    }

    else{
      Networker::send_msg("localhost", "8080", cmd.c_str());
      cout << "invalid command" << endl;
    }

  }while(cmd != "end");

  return nullptr;
}

void *thread_msgs_code(void *arg){
  time_t start_election_time = 0;
  bool on_election = false;
  cout << "MSGS" << endl;

  Networker server("8080");
  char incoming[MSG_BUFSIZE];
  while(true){

    cout << "Waiting accept" << endl;
    server.accept_msg();
    server.get_msg(incoming);
    cout << "Received: " << incoming << endl;
    server.respond_msg("OK");
    server.close_msg();

    continue;

    string msg = receiveMessage();
    int type = getMessageType(msg);

    //TODO MUTEX SLEEP
    //these handlers are inactive if the process is sleeping
    if(!is_sleeping) {
      if (type == 0) { //received election message
        r_election_count++; //todo mutex
        int sender_id = getSenderId(msg);
        if (sender_id < id){ //this process has a higher id, sends id to election starter
          sendMessage(1);
          s_ok_count++; //todo mutex
          start_election_intention = true; //todo mutex
        }

      } else if (type == 1) { //received ok message
        r_ok_count++; //todo mutex
//        on_election == false;

      } else if (type == 3) { //received alive message
        r_alive_count++; //todo mutex
        sendMessage(4);
        s_alive_ok_count++; //todo mutex

      } else if (type == 4) { //received alive_ok message
        r_alive_ok_count++; //todo mutex
        leader_checking = false;  //todo mutex
      }
    }

    //this handler still works even if the process is sleeping
    if (type == 2) { //received leader message
      r_leader_count++; //todo mutex
      leader_id = getSenderId(msg);
    }

    //starts election if there is intent
    if(start_election_intention){
      start_election_intention = false; //todo mutex
      //todo mandar mensagem p todo mundo
      on_election = true;
      start_election_time = time(nullptr);
    }

    if(on_election && ((time(nullptr) - start_election_time) > 10)){
      //this process is the new leader
      leader_id = id; //todo mutex
      //todo avisar todo mundo
    }
  }
}

void *thread_leader_code(void *arg){
  return nullptr;
  while(true){
    sleep(10); //TODO ID MULTIPLIER
    leader_checking = true; //todo mutex
    sendMessage(3);
    s_alive_count++;  //todo mutex

    sleep(10);
    if(leader_checking){
      leader_checking = false;  //todo mutex
      start_election_intention = true; //todo mutex
    } else {
      cout << "Found Leader" << endl;
    }
  }
}

int main(int argc, char **argv) {

//  if(argc != 3){
//    printf("Missing receiver host. run \"p3 $id $maxid\"\n"
//           "$host: Receiver's IP\n");
//    return 1;
//  }
//
//  port  = argv[1];
//  port2 = argv[2];
//
//  id = atoi(argv[1]);
//  if(id < 1){
//    printf("id should be bigger than 0\n");
//    return 1;
//  }
//
//  int maxid = atoi(argv[2]);
//  if(maxid < id){
//    printf("max can't be less than id \n");
//    return 1;
//  }


//  sem_init(&is_sleeping_mutex, 0, 1);
//  sem_init(&check_leader_thread_mutex, 0, 1);
//  sem_init(&receive_msg_thread_mutex, 0, 1);
//
  pthread_t thread_ui_id, thread_msgs_id, thread_leader_id;

  pthread_create(&thread_ui_id , nullptr, thread_ui_code, nullptr);
  pthread_create(&thread_msgs_id , nullptr, thread_msgs_code, nullptr);
  pthread_create(&thread_leader_id , nullptr, thread_leader_code, nullptr);

  pthread_join(thread_ui_id, nullptr);
//
  pthread_cancel(thread_msgs_id);
  pthread_cancel(thread_leader_id);
//
//  sem_destroy(&is_sleeping_mutex);
//  sem_destroy(&receive_msg_thread_mutex);
//  sem_destroy(&check_leader_thread_mutex);

  return 0;
}