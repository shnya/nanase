#define DEBUG
#include "serializer.hpp"
#include <iostream>
using namespace std;


using namespace nanase::serializer;
int main(int argc, char *argv[])
{
  try {
    Serializer ser(sizeof(int) + sizeof(char) *3);

    int c = 5;
    ser << c;
    PtrContainer<const char> b = PtrCon("abc", 3);
    ser << b;
    //ser << PtrCon("abc", 3);

    PrintBinary(ser.data(), 7);


    DeSerializer des(ser.data(), 7);
    char *ptr = new char[4];
    int a;
    des >> a;
    /*
      PtrContainer<char> b = PtrCon(ptr, 3);
      des >> b;
    */
    des >> PtrCon(ptr, 3);
    ptr[3] = '\0';

    cout << a << " " << ptr << endl;
  }catch(exception &e){
    cout << "acd" << endl;
  }

  return 0;
}
