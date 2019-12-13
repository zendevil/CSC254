
#include <set>
#include <iostream>
#include <string.h>
#include <type_traits>
#include "bits.h"
#include "hashtable.h"
#define MIN_INT -100000
using std::set;
using std::cout;
using std::string;

// Naive comparator.
// Provides a default for any type that has an operator<
// and an operator==.
//
template<typename T>
class comp {
public:
  bool precedes(const T& a, const T& b) const {
    
    return a < b;
  }
  bool equals(const T& a, const T& b) const {
    return a == b;
  }
};

// char pointer comparator
template<typename T>
class char_pointer_comp {
  
public:
  bool precedes(const T& a, const T& b) const {
    std::cout<<"preceded func"<<std::endl;
    int i = 0;
    while (a[i] != '\0' || b[i] != '\0') {
      
      if (a[i] < b[i]) return true;
      i++;
    }
    return false;
  }
  bool equals(const T& a, const T& b) const {
    
    int i = 0;
    while (a[i] != '\0' || b[i] != '\0') {
      std::cout<<"comparing"<<a[i]<<" and "<<b[i]<<std::endl;
      if (a[i] != b[i]) return false;
      i++;
    }
    return true;
  }
};


// Abstract base class from which all sets are derived.
//
template<typename T, typename C = comp<T> >
class simple_set {
public:
  virtual ~simple_set<T, C>() { }
  // destructor should be virtual so that we call the right
  // version when saying, e.g.,
  // simple_set* S = new derived_set(args);
  //  ...
  // delete S;
  /* simple_set(int a, int b) { */
  /* 	       std::cout<<"Simple set constructor"<<std::endl; */
  /* } */
  virtual simple_set<T, C>& operator+=(const T item) = 0;
  // inserts item into set
  // returns a ref so you can say, e.g.
  // S += a += b += c;
  virtual simple_set<T, C>& operator-=(const T item) = 0;
  // removes item from set, if it was there (otherwise does nothing)
  virtual bool contains(const T& item) const = 0;
  // indicates whether item is in set
};

//---------------------------------------------------------------

// Example of a set that implements the simple_set interface.
// Borrows the balanced tree implementation of the standard template
// library.  Note that you are NOT to use any standard library
// collections in your code (though you may use strings and streams).
//
template<typename T>
class std_simple_set : public virtual simple_set<T>, protected set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
public:
  virtual ~std_simple_set<T>() { }  // will invoke std::~set<T>()
  virtual std_simple_set<T>& operator+=(const T item) {
    set<T>::insert(item);
    return *this;
  }
  virtual std_simple_set<T>& operator-=(const T item) {
    (void) set<T>::erase(item);
    return *this;
  }
  virtual bool contains(const T& item) const {
    return (set<T>::find(item) != set<T>::end());
  }
};

//---------------------------------------------------------------

// Characteristic array implementation of set.
// Requires instantiation with guaranteed low and one-more-than-high
// bounds on elements that can be placed in the set.  Should compile
// and run correctly for any element class T that can be cast to int.
// Throws out_of_bounds exception when appropriate.
//
class out_of_bounds { };    // exception
template<typename T>
class carray_simple_set : public virtual simple_set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
  // You'll need some data members here.
  T L;
  T H;
  int* array;
public:

  virtual T get_low() {
    return L;
  }

  virtual T get_high() {
    return H;
  }
  
  carray_simple_set(const T l, const T h) : L(l), H(h) {   // constructor
    int num_elements = h - l;
    std::cout<<h<<" "<<l<<std::endl;
    array = (int*) malloc(num_elements / 8);
  }
  virtual ~carray_simple_set() {              // destructor
    delete array;
  }
  virtual carray_simple_set<T>& operator+=(const T item) {
    if (item - L >= H || item < L) {
      throw out_of_bounds();
    }
    
    set_bit(array, item - L);
    return *this;
  }
  virtual carray_simple_set<T>& operator-=(const T item) {
    clear_bit(array, item - L);
    return *this;
  }
  virtual bool contains(const T& item) const {
    return test_bit(array, item - L);
  }
};

//---------------------------------------------------------------

// Naive hash function object.
// Provides a default for any type that can be cast to int.
//
template<typename T>
class cast_to_int {
public:
  int operator()(const T n) {
    return (int) n;
  }
};

// Hash table implementation of set.
// Requires instantiation with guaranteed upper bound on number of elements
// that may be placed in set.  Throws overflow if bound is exceeded.
// Can be instantiated without second generic parameter if element type
// can be cast to int; otherwise requires hash function object.
//
class overflow { };         // exception
template<typename T, typename F = cast_to_int<T> >
class hashed_simple_set : public virtual simple_set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
  // You'll need some data members here.

  // I recommend you pick a hash table size p that is a prime
  // number >= n, use F(e) % p as your hash function, and rehash
  // with kF(e) % p after the kth collision.  (But make sure that
  // F(e) is never 0.)
  HashMap<T> map;
  const int max_elements;
  int num_elements;
  int* array;
  int prime;
public:
  hashed_simple_set<T, F>(const int n) : max_elements(n), num_elements(0), map(HashMap<T>(n)) {   // constructor
    array = (int*) malloc(max_elements / 8);
   
  }
  virtual ~hashed_simple_set<T, F>() { }    // destructor
  virtual hashed_simple_set<T, F>& operator+=(const T item) {
    if (num_elements == max_elements) {
      throw overflow();
    }
    map.put(item);
    num_elements++;
    return *this;
  }
  virtual hashed_simple_set<T, F>& operator-=(const T item) {
    // returns true if value was removed
    if (map.remove(item)) {
      num_elements--;
    }
    return *this;
  }
  virtual bool contains(const T& item) const {
    return map.get(item);
  }
};

//---------------------------------------------------------------

// Sorted array implementation of set; supports binary search.
// Requires instantiation with guaranteed upper bound on number of
// elements that may be placed in set.  Throws overflow if bound is
// exceeded.
//
template<typename T, typename C = comp<T> >
class bin_search_simple_set : public virtual simple_set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
  // You'll need some data members here.
  int max_elements;
  int num_elements;
  T* array;

  void insert(T* array, T elem, int location) {
    T temp = array[location];
    array[location] = elem;
    for (int i = location + 1; i < num_elements; i++) {
      T temp2 = array[i];
      array[i] = temp;
      temp = temp2;
    }
  }

  void remove (T* array, int location) {
    if (location > -1) {
      for (int i = location; i < num_elements; i++) {
	array[i] = array[i+1];
      }
      num_elements--;
    }
  }
  
public:
  bin_search_simple_set(const int n) : max_elements(n), num_elements(0) {    // constructor
    array = (T*) malloc(n * sizeof(T));
    for (int i = 0; i < n; i++) {
      array[i] = (T)MIN_INT;
    }
  }
  virtual ~bin_search_simple_set() { delete array; }    // destructor
  virtual bin_search_simple_set<T>& operator+=(const T item) {
    if (num_elements == 0) {
      num_elements++;
      array[0] = item;
    } else if (array[num_elements - 1] < item) {
      array[num_elements] = item;
      num_elements++;
    } else {
      for (int i = 0; i < num_elements; i++) {
	if (array[i] > item) {
	  insert(array, item, i);
	  break;
	} else if (array[i] == item){
	  break;
	}
      }
    }
    return *this;
  }
  virtual bin_search_simple_set<T>& operator-=(const T item) {
    remove(array, contains_at(item));
    return *this;
  }

   

  /**************source:https://www.geeksforgeeks.org/binary-search**************/
  int binary_search(int l, int r, const T& item) const { 
    if (r >= l) { 
      int mid = l + (r - l) / 2; 
  
      // If the element is present at the middle 
      // itself 
      if (item == array[mid]) 
	return mid; 
  
      // If element is smaller than mid, then 
      // it can only be present in left subarray 
      if (item < array[mid]) 
	return binary_search(l, mid - 1, item); 
  
      // Else the element can only be present 
      // in right subarray 
      return binary_search(mid + 1, r, item); 
    } 
  
    // We reach here when element is not 
    // present in array 
    return -1; 
  }
  /**************************************************************************************/
  virtual bool contains_at(const T& item) const {
    return binary_search(0, num_elements-1, item);
  }
  virtual bool contains(const T& item) const {
    return contains_at(item) == -1 ? false : true;
  }
   
};

//---------------------------------------------------------------

// Tree Simple Set
template<typename T, typename C = comp<T> >
class tree_simple_set : public virtual simple_set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
  // You'll need some data members here.
public:
  tree_simple_set(const int n) {    // constructor
    // replace this line:
    (void) n;
  }
  virtual ~tree_simple_set() { }    // destructor
  virtual tree_simple_set<T>& operator+=(const T item) {
    // replace this line:
    (void) item;  return *this;
  }
  virtual tree_simple_set<T>& operator-=(const T item) {
    // replace this line:
    (void) item;  return *this;
  }
  virtual bool contains(const T& item) const {
    // replace this line:
    (void) item;  return false;
  }
};



//===============================================================
// RANGE SETS

// Function object for incrementing.
// Provides a default for any integral type.
//
template<typename T>
class increment {
  //static_assert(std::is_integral<T>::value, "Integral type required.");
public:
  T operator()(T a) const {
    return ++a;
  }
};

// Range type.  Uses comp<T> by default, but you can provide your
// own replacement if you want, e.g. for C strings.
//
class empty_range {};    // exception

template<typename T, typename C = comp<T> >
class range {
  
protected:
  T L;        // represents all elements from L
  bool Linc;  // inclusive?
  T H;        // through H
  bool Hinc;  // inclusive?
  C cmp;      // can't be static; needs explicit instantiation
public:
  range(const T l, const bool linc, const T h, const bool hinc)
    : L(l), Linc(linc), H(h), Hinc(hinc)/*, cmp()*/ {
    // std::cout<<"comparing ";
    // std::cout<<l;
    // std::cout<<" and ";
    // std::cout<<h<<std::endl;
    //cmp.precedes(h, l);
    
    cmp.equals(h, l);
    //std::cout<<"fine here\n";
    if (cmp.precedes(h, l)
	|| (cmp.equals(l, h) && (!Linc || !Hinc))) {
      // std::cout<<h<<std::endl;
      // std::cout<<l<<std::endl;
		   
      // std::cout<<cmp.precedes(h, l)<<std::endl;
      // std::cout<<cmp.equals(l, h)<<std::endl;
      // std::cout<<(!Linc || !Hinc)<<std::endl;
		   
      throw empty_range();
    }
  }

  void set_closed_low(bool b) {
    Linc = b;
  }

  void set_closed_high(bool b) {
    Hinc = b;
  }
  // no destructor needed
  T low() const { return L; }
  bool closed_low() const { return Linc; }
  T high() const { return H; }
  bool closed_high() const {return Hinc; }
  bool contains(const T& item) const {
    return ((cmp.precedes(L, item) || (Linc && cmp.equals(L, item)))
	    && (cmp.precedes(item, H) || (Hinc && cmp.equals(item, H))));
  }

  // string to_string() const {
  // 	 return "";
  // 	 //return (Linc ? "(" : "[") +  L + ", " + H + (Hinc ? ")" : "]"); 
  // }
  // You may also find it useful to define the following:
  // bool precedes(const range<T, C>& other) const { ...
  // bool overlaps(const range<T, C>& other) const { ...
};

// You may find it useful to define derived types with two-argument
// constructors that embody the four possible combinations of open and
// close-ended:
//
// template<typename T, typename C = comp<T>>
// class CCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class COrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OOrange : public range<T, C> { ...

// This is the abstract class from which all range-supporting sets are derived.
//
template<typename T, typename C = comp<T> >
class range_set : public virtual simple_set<T> {
  // 'virtual' on simple_set ensures single copy if multiply inherited
public:
  virtual range_set<T, C>& operator+=(const range<T, C> r) = 0;
  virtual range_set<T, C>& operator-=(const range<T, C> r) = 0;
  //virtual bool contains(const range<T> r) = 0;
};

//---------------------------------------------------------------

// As implemented in the standard library, sets contain individual
// elements, not ranges.  (There are range insert and erase operators, but
// (a) they use iterators, (b) they take time proportional to the number of
// elements in the range, and (c) they require, for deletion, that the
// endpoints of the range actually be in the set.  An std_range_set, as
// defined here, avoids shortcomings (a) and (c), but not (b).  Your
// bin_search_range_set should avoid (b), though it will have slow insert
// and remove operations.  A tree_range_set (search tree -- extra credit)
// would have amortized log-time insert and remove for individual elements
// _and_ ranges.
//
template<typename T, typename C = comp<T>, typename I = increment<T> >
class std_range_set : public virtual range_set<T, C>,
		      public std_simple_set<T> {
  // 'virtual' on range_set ensures single copy if multiply inherited
  static_assert(std::is_integral<T>::value, "Integral type required.");
  I inc;
 
public:
  // The first three methods below tell the compiler to use the
  // versions of the simple_set methods already found in std_simple_set
  // (given true multiple inheritance it can't be sure it should do that
  // unless we tell it).
  virtual std_simple_set<T>& operator+=(const T item) {
    return std_simple_set<T>::operator+=(item);
  }
  virtual std_simple_set<T>& operator-=(const T item) {
    return std_simple_set<T>::operator-=(item);
  }
  virtual bool contains(const T& item) const {
    return std_simple_set<T>::contains(item);
  }
  virtual range_set<T>& operator+=(const range<T, C> r) {
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this += i;
    }
    return *this;
  }
  virtual range_set<T>& operator-=(const range<T, C> r) {
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this -= i;
    }
    return *this;
  }
};

//---------------------------------------------------------------

// insert an appropriate carray_range_set declaration here
template<typename T, typename C = comp<T>, typename I = increment<T> >
class carray_range_set : public virtual range_set<T, C>,
			 public carray_simple_set<T> {
  // 'virtual' on range_set ensures single copy if multiply inherited
  static_assert(std::is_integral<T>::value, "Integral type required.");
  I inc;
  T L;
  T H;
  
public:

  
  // The first three methods below tell the compiler to use the
  // versions of the simple_set methods already found in carray_simple_set
  // (given true multiple inheritance it can't be sure it should do that
  // unless we tell it).
  carray_range_set(const T l, const T h) : carray_simple_set<T>(l, h) {   // constructor
    L = l;
    H = h;
  }
			       
  virtual carray_simple_set<T>& operator+=(const T item) {
    return carray_simple_set<T>::operator+=(item);
  }
  virtual carray_simple_set<T>& operator-=(const T item) {
    return carray_simple_set<T>::operator-=(item);
  }
  virtual bool contains(const T& item) const { 
    return carray_simple_set<T>::contains(item);
  }

  // virtual bool contains(const range<T>& r) {
  // 	 // TODO
  // 	 return true;
  // }
  virtual range_set<T>& operator+=(const range<T, C> r) {
    ///
    if (r.low() > L || r.high() > H) {
      throw out_of_bounds();
    }

    ///
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this += i;
    }
    return *this;
  }
  virtual range_set<T>& operator-=(const range<T, C> r) {
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this -= i;
    }
    return *this;
  }
};

//---------------------------------------------------------------

// insert an appropriate hashed_range_set declaration here
template<typename T, typename F= cast_to_int<T>, typename C = comp<T>, typename I = increment<T> >
class hashed_range_set : public virtual range_set<T, C>,
			 public hashed_simple_set<T> {
  // 'virtual' on range_set ensures single copy if multiply inherited
  //static_assert(std::is_integral<T>::value, "Integral type required.");
  I inc;
  int max_elements;
public:
  // The first three methods below tell the compiler to use the
  // versions of the simple_set methods already found in hashed_simple_set
  // (given true multiple inheritance it can't be sure it should do that
  // unless we tell it).
  hashed_range_set<T, F>(int n) : hashed_simple_set<T, F>(n), max_elements(n) {
				 
  }
  virtual hashed_simple_set<T>& operator+=(const T item) {
    return hashed_simple_set<T>::operator+=(item);
  }
  virtual hashed_simple_set<T>& operator-=(const T item) {
    return hashed_simple_set<T>::operator-=(item);
  }
  virtual bool contains(const T& item) const {
    return hashed_simple_set<T>::contains(item);
  }
  virtual range_set<T>& operator+=(const range<T, C> r) {
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this += i;
    }
    return *this;
  }
  virtual range_set<T>& operator-=(const range<T, C> r) {
    for (T i = (r.closed_low() ? r.low() : inc(r.low()));
	 r.contains(i); i = inc(i)) {
      *this -= i;
    }
    return *this;
  }
};


// // insert an appropriate bin_search_range_set declaration here
template<typename T, typename C = comp<T> >
class bin_search_range_set : public virtual range_set<T, C>,
			     public bin_search_simple_set<T> {
  // 'virtual' on range_set ensures single copy if multiply inherited
  //static_assert(std::is_integral<T>::value, "Integral type required.");
  int max_ranges;
  
  int num_elements;
public:
  range<T, C>* array;
  int num_ranges() {
    return num_elements;
  }
  bin_search_range_set<T, C>(int n) : bin_search_simple_set<T>(n), max_ranges(n), num_elements(0) {
    array = (range<T, C>*) malloc(n * sizeof(range<T, C>));
  }

  void insert(range<T, C>* array, range<T, C> elem, int location) {
    range<T, C> temp = array[location];
    array[location] = elem;
    for (int i = location + 1; i < num_elements; i++) {
      range<T, C> temp2 = array[i];
      array[i] = temp;
      temp = temp2;
    }
  }

  range<T, C>*  split(range<T, C> r, range<T, C> item) {
    range<T, C>* split_range = (range<T, C>*) malloc(2 * sizeof(range<T, C>));
    
    split_range[0] = range<T, C> (r.low(), r.closed_low(), item.low(), !item.closed_low());
    try {
      //std::cout<<" r low "<<r.low()<<std::endl;
      //std::cout<<" item high "<<item.high()<<" "<<"r high "<<r.high()<<std::endl;
      split_range[1] = range<T, C> (item.high(), !item.closed_low(), r.high(), r.closed_high());
    } catch(empty_range e) {
      std::cout<<"caught~"<<std::endl;
    }
    return split_range;
  }
  
  void remove (range<T, C>* array, int location, range<T, C> item) {
    
    if (location > -1) {
      if (equal(array[location], item)) {
	std::cout<<"equal\n";
	for (int i = location; i < num_elements; i++) {
	  array[i] = array[i+1];
	}
	num_elements--;
      }
      else {
	//std::cout<<"not equal\n"<<num_elements<<" "<<max_ranges;
       
	if (num_elements == max_ranges) throw overflow();
	//std::cout<<"splitting "<<array[location].low()<<" "<<array[location].high()<<" "<<item.low()<<" "<<item.high()<<std::endl;
	range<T, C>* split_range = split(array[location], item);
	array[location] = split_range[0];
	range<T, C> temp = split_range[1];
	for (int i = location + 1; i < num_elements; i++) {
	  range<T, C> temp2 = array[i];
	  array[i] = temp;
	  temp = temp2;
	}
	num_elements++;
      }
    }
    
  }
  
  // The first three methods below tell the compiler to use the
  // versions of the simple_set methods already found in bin_search_simple_set
  // (given true multiple inheritance it can't be sure it should do that
  // unless we tell it).

  virtual bin_search_range_set<T, C>& operator+=(const range<T, C> item) {
    if (num_elements == 0) {
      num_elements++;
      array[0] = item;
      
    } else if (less_than(array[num_elements - 1], item)) {
      array[num_elements] = item;
      num_elements++;
    } else {
      for (int i = 0; i < num_elements; i++) {
  	if (greater_than(array[i], item)) {
  	  insert(array, item, i);
  	  break;
  	} else if (equal(array[i], item) || contained(item, array[i])) {
  	  break;
  	} else if (contained(array[i], item)) {
	  array[i] = item;
	} else if (precedes(array[i], item)) {
	  
	  range<T, C> r(array[i].low(), array[i].closed_low(), item.high(), item.closed_high());
	  array[i] = r;
	}
	else if (precedes(item, array[i])) {
	  range<T, C> r(item.low(), item.closed_low(), array[i].high(), array[i].closed_high());
	  array[i] = r;
	}
      }
    }
    // std::cout<<"added "<<array[0].low()<<" "<<array[0].closed_low()<<" "<<array[0].high()<<" "<<array[0].closed_high()<<std::endl;
    return *this;
  }

  virtual bin_search_range_set<T, C>& operator-=(const range<T, C> item) {
    remove(array, contains_at(item), item);
    return *this;
  }

  virtual bin_search_range_set<T, C>& operator-=(T item) {
    
    remove(array, contains_at(item), item);
    
    return *this;
  } 
  
  virtual bool less_than(range<T, C> a, range<T, C> b) const {
    if (a.high() < b.low()) return true;
    
    return false;
  }

  virtual bool greater_than(range<T, C> a, range<T, C> b) {
    if (a.low() > b.low()) return true; 
    return false;
  }

  virtual bool edge_in(range<T, C> which, range<T, C> in) const {
    // std::cout<<which.low()<<" "<<which.high()<<" "<<which.closed_low()<<" "<<which.closed_high()<<std::endl;
    // std::cout<<in.low()<<" "<<in.high()<<" "<<in.closed_low()<<" "<<in.closed_high()<<std::endl;
    return ((!which.closed_low() && !which.closed_high()) || (which.closed_low() && in.closed_low() && which.closed_high() && in.closed_low()) || (!which.closed_low() && !in.closed_low() && which.closed_high() && in.closed_high()) || (which.closed_low() && in.closed_low() && !which.closed_high() && !in.closed_high()));
  }
  virtual bool contained(range<T, C> which, range<T, C> in) const {
    // std::cout<<"is "<<which.low()<<" "<<which.closed_low()<<" "<<which.high()<<" "<<which.closed_high()<<" contained in "<<in.low()<<" "<<in.closed_low()<<" "<<in.high()<<" "<<in.closed_high()<<" "<<(which.low() >= in.low() && which.high() <= in.high())<<" edge "<<edge_in(which, in)<<std::endl;
    if (which.low() >= in.low() && which.high() <= in.high() && edge_in(which, in)) return true;
    return false;
  }

  virtual bool equal(range<T, C> a, range<T, C> b) const {
    if (a.low() == b.low() && a.high() == b.high() && a.closed_high() == b.closed_high() && a.closed_low() == b.closed_low()) {
      return true;
    }
    return false;
  }

  virtual bool precedes(range<T, C> a, range<T, C> b) {
    if (a.low() < b.low()) {
      return true;
    }
    return false;
  }
  virtual bool contains(const range<T, C>& item) const {
   
    return contains_at(item) == -1 ? false : true;
  }

  virtual bool contains_at(const range<T, C>& item) const {
    return binary_search(0, num_elements - 1, item);
  }

  virtual bool less_than(T item, range<T, C> r) const {
    if (item < r.low()) return true;
    return false;
  }
  virtual bool contained(T which, range<T, C> in) const {
    // std::cout<<"is "<<which.low()<<" "<<which.closed_low()<<" "<<which.high()<<" "<<which.closed_high()<<" contained in "<<in.low()<<" "<<in.closed_low()<<" "<<in.high()<<" "<<in.closed_high()<<" "<<(which.low() >= in.low() && which.high() <= in.high())<<" edge "<<edge_in(which, in)<<std::endl;
    if (which >= in.low() && which <= in.high() && edge_in(which, in)) return true;
    return false;
  }
  virtual bool contains_at(const T& item) const {
    return binary_search(0, num_elements - 1, item);
  }

  /**************source:https://www.geeksforgeeks.org/binary-search**************/
  int binary_search(int l, int r, const T& item) const { 
    if (r >= l) { 
      int mid = l + (r - l) / 2; 
  
      // If the element is present at the middle 
      // itself 
      if (contained(item, array[mid])) 
	return mid; 
  
      // If element is smaller than mid, then 
      // it can only be present in left subarray 
      if (less_than(item, array[mid])) 
	return binary_search(l, mid - 1, item); 
  
      // Else the element can only be present 
      // in right subarray 
      return binary_search(mid + 1, r, item); 
    } 
  
    // We reach here when element is not 
    // present in array 
    return -1; 
  }

  int binary_search(int l, int r, const range<T, C>& item) const { 
    if (r >= l) { 
      int mid = l + (r - l) / 2; 
  
      // If the element is present at the middle 
      // itself
        
      if (contained(item, array[mid])) 
	return mid; 
	
      // If element is smaller than mid, then 
      // it can only be present in left subarray

      if (less_than(item, array[mid])) 
	return binary_search(l, mid - 1, item); 

      // Else the element can only be present 
      // in right subarray 
      return binary_search(mid + 1, r, item); 
    } 
  
    // We reach here when element is not 
    // present in array 
    return -1; 
  } 
  /**************************************************************************/



  virtual bool edge_in(T which, range<T, C> in) const {
    return ((which == in.low() && in.closed_low()) || (which == in.high() && in.closed_high()) || (which != in.low() && which != in.high()));
  }


  void remove (range<T, C>* array, int location, T item) {
    //std::cout<<"removing from location "<<location<<std::endl;
      
    if (location > -1) {
      if (item == array[location].low()) {
	  
	array[location].set_closed_low(false);
      } else
	if (item == array[location].high()) {
	    
	  array[location].set_closed_high(false);
	} else {
	  if (num_elements == max_ranges) throw overflow();
	  //std::cout<<"splitting "<<array[location].low()<<" "<<array[location].high()<<" "<<item.low()<<" "<<item.high()<<std::endl;
	  range<T, C>* split_range;
	  //try {
	    
	  split_range = split(array[location], item);
	  //} catch (empty_range e) {std::cout<<"caught!\n";}
	  array[location] = split_range[0];
	  range<T, C> temp = split_range[1];
	  for (int i = location + 1; i < num_elements; i++) {
	    range<T, C> temp2 = array[i];
	    array[i] = temp;
	    temp = temp2;
	  }
	    
	  num_elements++;
	}
    }
      
    
  }

  // this split function splits a range into two
  range<T, C>*  split(range<T, C> r, T item) {
    range<T, C>* split_range = (range<T, C>*) malloc(2 * sizeof(range<T, C>));
    //std::cout<<"->"<<r.low()<<" "<<r.high()<<std::endl;
    
    //	std::cout<<"item "<<item<<std::endl;
    try {
      split_range[0] = range<T, C> (r.low(), r.closed_low(), item, false);
    } catch(empty_range e) {std::cout<<"caught"<<std::endl;}
    try {
      //std::cout<<" r low "<<r.low()<<std::endl;
      //std::cout<<" item high "<<item.high()<<" "<<"r high "<<r.high()<<std::endl;
      split_range[1] = range<T, C> (item, false, r.high(), r.closed_high());
    } catch(empty_range e) {
      std::cout<<"caught"<<std::endl;
    }
    return split_range;
      
  }


  
};

// comparator for C strings
//
class lexico_less {
public:
  bool precedes(const char *a, const char *b) const {
    return strcmp(a, b) < 0;
  }
  bool equals(const char *a, const char *b) const {
    return strcmp(a, b) == 0;
  }
};

typedef enum{mon, tue, wed, thu, fri} weekday;
