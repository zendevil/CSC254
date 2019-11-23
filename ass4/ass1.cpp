#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "ass1.h"
using namespace std;
vector<string> tree(int n) {
  if (n == 1) {
    vector<string> to_return;
    to_return.push_back("(.)");
    return to_return;
  }
  vector<string> strings;
  vector<string> prev_strings = tree(n - 1);
  for (int i = 0; i < prev_strings.size(); i++) {
    strings.push_back("(" + prev_strings[i] + ".)");
    strings.push_back("(." + prev_strings[i] + ")");
  }
  return strings;
}

int main() {
  cout << "Enter the number of nodes: ";
  int n;
  cin >> n;
  vector<string> result = tree(n);
  for(int i; i < result.size(); i++) {
    cout << result[i] << endl;
  }
  return 0;
}
