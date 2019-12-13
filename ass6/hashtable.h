#include <vector>
#include "prime.h"
template<typename T> class HashEntry {
 public:
  T val;
 HashEntry(T item) : val(item) {}
};

template<typename T> class HashMap {

  int SIZE;
  std::vector<HashEntry<T>*> table;
 public:
  HashMap(int s) {
    SIZE = find_special_prime(s);
   
    table = std::vector<HashEntry<T>*>(SIZE);
  }

  bool put(T item) {
    // make sure that key is never 0
    int key = item == 0 ? 1 : item;
    int location = key % SIZE;
    int num_collisions = 1;
    while (table[location] != NULL) {
      // table is full
      assert (num_collisions <= SIZE);
      // item already exists
      if (table[location]->val == item) {
	return false;
      }
      num_collisions++;
      location = num_collisions * key % SIZE;
    }
    table[location] = new HashEntry<T>(item);
    return true;
  }

  bool remove(T item) {
    // make sure that key is never 0
    int key = item == 0 ? 1 : item;
    int location = key % SIZE;
    int num_collisions = 1;
    while (table[location] != NULL) {
      // item exists
      if (table[location]->val == item) {
        table[location] = NULL;
	return true;
      }
      num_collisions++;
      location = num_collisions * key % SIZE;
    }
    return false;
  }

  bool get(T item) const {
    // make sure that key is never 0
    int key = item == 0 ? 1 : item;
    int location = key % SIZE;
    int num_collisions = 1;
    while (table[location] != NULL) {
      // item exists
      if (table[location]->val == item) {
	return true;
      }
      num_collisions++;
      location = num_collisions * key % SIZE;
    }
    return false;
  }
};


/* int main() { */
/*   HashMap<int>* map = new HashMap<int>(5); */
/*   map->put(10); */
/*   map->put(1); */
/*   map->put(13); */
/*   map->put(11); */
/*   map->put(12); */
/*   map->remove(12); */
/*   map->put(12); */
/*   map->remove(2100); */
/*   std::cout<<map->get(12); */
/*   std::cout<<map->get(19); */
  
/* } */
