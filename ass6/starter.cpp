/*
  Starter code for assignment 6, CSC 2/454, Fall 2019

  Provides skeleton of code for a simple hierarchy of set
  abstractions.

  Everything but /main/ should be moved to a .h file, which should
  then be #included from here.
*/
#include "starter.h"
/* My Tests. 
   Follow along the print statements in the console with the following code.
   One great thing about my range_sets is that they not only check whether an element is contained but also whether a range is contained.*/
void carray_simple_set_test() {
  carray_simple_set<int> ss_i(0, 200);
  ss_i += 10;
  ss_i += 30;
  ss_i += 199; // throws out of bounds
  ss_i += 0; // throws out of bounds
  cout << "10 is " << (ss_i.contains(10) ? "" : "not ") << "in ss_i\n";
  cout << "5 is " << (ss_i.contains(5) ? "" : "not ") << "in ss_i\n";
  cout << "30 is " << (ss_i.contains(30) ? "" : "not ") << "in ss_i\n";
}

void carray_range_set_test() {
  
  carray_range_set<int> rs_i(0, 100);
  
  rs_i += 5;
  
  try {
    rs_i += 101;
  } catch (out_of_bounds e) {
    std::cout<<"This will cause out of bounds because we are adding 101 to a set with range [0, 100)"<<std::endl;
  }
  
  rs_i += 5;
  std::cout<<"here1"<<std::endl;
  std::cout << "5 is " << (rs_i.contains(5) ? "" : "not ") << "in rs_i\n";

  std::cout << "6 is " << (rs_i.contains(6) ? "" : "not ") << "in rs_i\n";
  range<int> r3(0, true, 50, false);
  std::cout<<"Adding the range to the set"<<std::endl;
  rs_i += r3;

  range<int> r4(0, true, 200, false);
  std::cout<<"Adding an out of bounds range to the set"<<std::endl;
  try {
    rs_i += r4;
    } catch (out_of_bounds e) {
    std::cout<<"out of bounds"<<std::endl;
  } 
  std::cout<<"Testing whether an element is contained in the range set"<<std::endl;
  
  std::cout <<"20 is " << (rs_i.contains(20) ? "" : "not ") << "in rs_i\n";
  std::cout <<"50 is " << (rs_i.contains(50) ? "" : "not ") << "in rs_i\n";
  std::cout <<"49 is " << (rs_i.contains(49) ? "" : "not ") << "in rs_i\n";
  range<int> r6(0, true, 200, true);
  std::cout<<"Adding the range"<<std::endl;
  try {
    rs_i += r6;
  } catch(out_of_bounds e) {
    std::cout<<"caught"<<std::endl;
  }
  
  range<int> r5(13, true, 19, true);
  
  std::cout<<"Removing the range "<<" from the range set that also splits the underlying sets into two"<<std::endl;
  std::cout <<"16 is " << (rs_i.contains(16) ? "" : "not ") << "in rs_i\n"<<std::endl;
  rs_i -= r5;
  std::cout<<"Now checking whether the removed range is contained in the set"<<std::endl;
  std::cout <<"16 is " << (rs_i.contains(16) ? "" : "not ") << "in rs_i\n"<<std::endl;
}

void hashed_simple_set_test() {
  
  // Hashed Simple Set
  hashed_simple_set<double> hs_d(1);
  hs_d += 10;
  std::cout << "10 is " << (hs_d.contains(10) ? "" : "not ") << "in hs_d\n";
  std::cout<<"We will catch an overflow exception"<<std::endl;
  try {
  hs_d += 5;
  } catch (overflow e) {
    std::cout<<"overflow exception caught"<<std::endl;
  }
  hs_d -= 5;
  std::cout << "5 is " << (hs_d.contains(5) ? "" : "not ") << "in hs_d\n";
}

void hashed_range_set_test() {
  // Hashed Range Set
  hashed_range_set<int> hr_s(50);
  range<int> r7(20, true, 30, true);
  
  try {
    hr_s += r7;
  } catch (out_of_bounds e) {
    std::cout<<"This will throw an exception because \"apple\" is out of the range "<<std::endl;
  }
  std::cout << "5 is " << (hr_s.contains(5) ? "" : "not ") << "in hr_s\n";
  std::cout << "25 is " << (hr_s.contains(25) ? "" : "not ") << "in hr_s\n";
  
}

void bin_search_simple_set_test() {
  // Binary Search Simple Set
  
  bin_search_simple_set<int> bss_i(1);
  bss_i += 30;
  std::cout << "30 is " << (bss_i.contains(30) ? "" : "not ") << "in bss_i\n";
  std::cout << "15 is " << (bss_i.contains(15) ? "" : "not ") << "in bss_i\n";
  bss_i -= 30;
  std::cout << "30 is " << (bss_i.contains(30) ? "" : "not ") << "in bss_i\n";
}


void char_pointer(char* cp, string string) {
  cp = (char*)malloc(string.size() + 1);
  string.copy(cp, string.size() + 1);
  cp[string.size()] = '\0';
}
// // Binary Search Range Set
void bin_search_range_set_test() {
  bin_search_range_set<int> bsr_i(10);
  range<int> r1(10, true, 30, false);
  bsr_i += r1;
  range<int> r2(15, true, 40, false);
  bsr_i += r2;
  
  range<int> r3(16, true, 18, false);
  std::cout <<"r3 is " << (bsr_i.contains(r3) ? "" : "not ") << "in bsr_i\n";
  
  bsr_i -= r3;
  
  std::cout <<"r3 is " << (bsr_i.contains(r3) ? "" : "not ") << "in bsr_i\n";
  
  bsr_i -= 10;
  
  char* l;
  char_pointer(l, "aaa");
  char* h;
  char_pointer(h, "zzz");
  bin_search_range_set<char*, char_pointer_comp<char*> > bsr_cp(10);
  
  char* l_ins;
  char_pointer(l_ins, "boink");
  
  char* r_ins;
  char_pointer(r_ins, "run");
  
  // try {
  //   const range<char*, char_pointer_comp<char*> > r9(l_ins, true, r_ins, true);
  // std::cout<<"so far so good"<<std::endl;
  // std::cout<<"Adding the range [boink, run] to the set"<<std::endl;
  // bsr_cp += r9;
  // } catch(empty_range e) {
  //   std::cout<<"why empty range?"<<std::endl;
  // }
  
  // std::cout<<"Testing whether a range is contained in the range set"<<std::endl;
  // char* l_test;
  // char_pointer(l_test, "cat");
  // char* r_test;
  // char_pointer(r_test, "dog");
  // const range<char*, char_pointer_comp<char*> > r10(l_test, true, r_test, true);
  // std::cout <<"[cat, dog] is " << (bsr_cp.contains(r10) ? "" : "not ") << "in bsr_cp\n"<<std::endl;

  // char* l_rem;
  // char_pointer(l_rem, "dog");
  // char* r_rem;
  // char_pointer(r_rem, "tree");
  // range<char*, char_pointer_comp<char*> > r11(l_rem, true, r_rem, true);
  // std::cout<<"Removing the range [dog, tree] from the range set that also splits the underlying sets into two"<<std::endl;
  // bsr_cp -= r11;
  // std::cout<<"Now checking whether the removed range is contained in the set"<<std::endl;
  // std::cout <<"[cat, dog] is " << (bsr_cp.contains(r11) ? "" : "not ") << "in bsr_cp\n"<<std::endl;

}
/* Tests end */  
  



int main() {

  // My tests
  std::cout<<"carray_simple_set_test"<<std::endl;
  carray_simple_set_test();
  std::cout<<"carray_range_set_test"<<std::endl;
  carray_range_set_test();
  std::cout<<"hashed_simple_set_test"<<std::endl;
  hashed_simple_set_test();
  std::cout<<"carray_range_set_test"<<std::endl;
  hashed_range_set_test();
  std::cout<<"bin_search_simple_set_test"<<std::endl;
  bin_search_simple_set_test();
  std::cout<<"bin_search_range_set_test"<<std::endl;
  bin_search_range_set_test();
  // Some miscellaneous code to get you started on testing your sets.
  // The following should work:

  std_simple_set<int> R;
  R += 3;
  cout << "3 is " << (R.contains(3) ? "" : "not ") << "in R\n";
  cout << "5 is " << (R.contains(5) ? "" : "not ") << "in R\n";

  simple_set<double>* S = new std_simple_set<double>();
  *S += 3.14;
  cout << "pi is " << (S->contains(3.14) ? "" : "not ") << "in S\n";
  cout << "e is " << (S->contains(2.718) ? "" : "not ") << "in S\n";

  std_simple_set<string> U;
  U += "hello";
  cout << "\"hello\" is " << (U.contains("hello") ? "" : "not ") << "in U\n";
  cout << "\"foo\" is " << (U.contains("foo") ? "" : "not ") << "in U\n";

  range<string> r1("a", true, "f", true);
  cout << "\"b\" is " << (r1.contains("b") ? "" : "not ") << "in r1\n";
  cout << "\"aaa\" is " << (r1.contains("aaa") ? "" : "not ") << "in r1\n";
  cout << "\"faa\" is " << (r1.contains("faa") ? "" : "not ") << "in r1\n";

  range<const char*, lexico_less> r2("a", true, "f", true);
  cout << "\"b\" is " << (r2.contains("b") ? "" : "not ") << "in r2\n";
  cout << "\"aaa\" is " << (r2.contains("aaa") ? "" : "not ") << "in r2\n";
  cout << "\"faa\" is " << (r2.contains("faa") ? "" : "not ") << "in r2\n";

  // The following will not work correctly yet:

  range_set<int>* X = new std_range_set<int>();
  *X += range<int>(5, true, 8, false);
  if (X->contains(4)) cout << "4 is in X\n";
  if (X->contains(5)) cout << "5 is in X\n";      // should print
  if (X->contains(6)) cout << "6 is in X\n";      // should print
  if (X->contains(7)) cout << "7 is in X\n";      // should print
  if (X->contains(8)) cout << "8 is in X\n";
  if (X->contains(9)) cout << "9 is in X\n";
  *X -= range<int>(6, true, 10, false);
  if (X->contains(4)) cout << "4 is now in X\n";
  if (X->contains(5)) cout << "5 is now in X\n";      // should print
  if (X->contains(6)) cout << "6 is now in X\n";
  if (X->contains(7)) cout << "7 is now in X\n";
  if (X->contains(8)) cout << "8 is now in X\n";
  if (X->contains(9)) cout << "9 is now in X\n";

}
