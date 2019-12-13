// source: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
// setting the k'th bit
#include "bits.h"
#include <iostream>
void set_bit(int* arr, int k) {
   arr[k/32] |= 1 << (k%32);
}

void clear_bit(int* arr, int k) {
   arr[k/32] &= ~(1 << (k%32));  
}

bool test_bit(int* arr, int k) {
  return (arr[k/32] & (1 << (k%32) ));  
}

 
// int main() {
//   int A[10];

//   for (int i = 0; i < 10; i ++) {
//     A[i] = 0;
//   }

//   std::cout<<test_bit(A, 10)<<std::endl;
//   set_bit(A, 10);
//   std::cout<<test_bit(A, 10)<<std::endl;
//   clear_bit(A, 10);
//   std::cout<<test_bit(A, 10)<<std::endl;
// }
