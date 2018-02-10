#include "patches.hpp"

int main(int argc, char *argv[]) {

  using namespace std;


  if (argc != 3)
    exit(1);

  ifstream file1;
  ifstream file2;

  file1.open(argv[1], ios::in);
  file2.open(argv[2], ios::in);

  Document f1(file1);
  Document f2(file2);

  file1.close();
  file2.close();

  Patch p = f1 - f2;

  cout << p;
  cout << "------" << endl;
  cout << !p;

  return 0;
}
