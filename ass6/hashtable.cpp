
int main() {
  HashMap<int>* map = new HashMap<int>(5);
  map->put(10);
  map->put(1);
  map->put(13);
  map->put(11);
  map->put(12);
  map->remove(12);
  map->put(12);
  map->remove(2100);
  std::cout<<map->get(12);
  std::cout<<map->get(19);
  
}

