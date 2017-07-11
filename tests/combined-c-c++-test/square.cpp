#include <iostream>
extern "C" {
  #include "square.h"
}
int main(){
  std::cout << square( 23 );
  return 0;
}
