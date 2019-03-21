#include <iostream>

#include "GetTimeMs64.h"
#include "tinyfiledialogs.h"

using namespace std;

int main() {
  cout << "Startup!" << endl;
  try {

    char const * fileFilterPatterns[] = { "*.txt"};

    const char* filename = tinyfd_openFileDialog("Selecione seu Grafo", "./graphs/.", 1, fileFilterPatterns, nullptr, 0);
  }catch (const char* msg){
    cout << msg<<endl;
    cout << "Halting" <<endl;
    exit(1);
  }

  return 0;
}