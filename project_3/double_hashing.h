#ifndef DOUBLE_HASHING_H
#define DOUBLE_HASHING_H

#include <vector>
#include <algorithm>
#include <functional>


namespace {

// Internal method to test if a positive number is prime.
bool double_IsPrime(size_t n) {
  if( n == 2 || n == 3 )
    return true;
  
  if( n == 1 || n % 2 == 0 )
    return false;
  
  for( int i = 3; i * i <= n; i += 2 )
    if( n % i == 0 )
      return false;
  
  return true;
}


// Internal method to return a prime number at least as large as n.
int double_NextPrime(size_t n) {
  if (n % 2 == 0)
    ++n;  
  while (!double_IsPrime(n)) n += 2;  
  return n;
}

}  // namespace


// Quadratic probing implementation.
template <typename HashedObj>
class HashTableDouble {
 public:
  enum EntryType {ACTIVE, EMPTY, DELETED};

  explicit HashTable(size_t size = 101) : array_(NextPrime(size))
    { MakeEmpty(); }
  
  bool Contains(const HashedObj & x) const {
    
    return IsActive(FindPos(x));
  }
  
  void MakeEmpty() {
    current_size_ = 0;
    for (auto &entry : array_)
      entry.info_ = EMPTY;
  }

  bool Insert(const HashedObj & x) {
    // Insert x as active
    size_t current_pos = FindPos(x);
    if (IsActive(current_pos))
      return false;
    
    array_[current_pos].element_ = x;
    array_[current_pos].info_ = ACTIVE;
    
    
    // Rehash; see Section 5.5
    if (++current_size_ > array_.size() / 2)
      Rehash();    
    return true;
  }
    
  bool Insert(HashedObj && x) {
    // Insert x as active
    size_t current_pos = FindPos(x); // finds a position thats empty to insert the  element
    if (IsActive(current_pos))
      return false;
    
    array_[current_pos] = std::move(x);
    array_[current_pos].info_ = ACTIVE;
    

    // Rehash; see Section 5.5
    if (++current_size_ > array_.size() / 2)
      Rehash();

    return true;
  }

  bool Remove(const HashedObj & x) {
    size_t current_pos = FindPos(x);
    if (!IsActive(current_pos))
      return false;

    array_[current_pos].info_ = DELETED;
    return true;
  }
  
  size_t Public_Count_Collisions(){
    return collisions;
  }
  int table_size(){
    return array_.size();
  }

  int Num_elements(){
    return current_size_;
  }

  float Loadfactor(){
    return Num_elements()/table_size();
  }

  void reset_probes(){
    probes = 0;
  }

  int get_probes(){
    return probes;
  }

  size_t public_get_probes(const HashedObj& x){

    return private_get_probes(x);
  }

  

 private:        
  struct HashEntry {
    HashedObj element_;
    EntryType info_;
    
    HashEntry(const HashedObj& e = HashedObj{}, EntryType i = EMPTY)
    :element_{e}, info_{i} { }
    
    HashEntry(HashedObj && e, EntryType i = EMPTY)
    :element_{std::move(e)}, info_{ i } {}
  };
  mutable size_t collisions = 0;

  std::vector<HashEntry> array_;
  size_t current_size_;
 
  size_t private_get_probes(const HashedObj& x) const{
    size_t offset = 1;
    size_t current_pos = InternalHash(x);
      int probes = 0;
    while (array_[current_pos].info_ != EMPTY &&
	   array_[current_pos].element_ != x) {
      probes++
      current_pos += offset;  // Compute ith probe.
      offset += 2;

      if (current_pos >= array_.size())
	current_pos -= array_.size();  
    }
    return probes + 1;
  }


// hash2 (x) = R – (x mod R) to find the next index you add hash1 to hash2
  bool IsActive(size_t current_pos) const
  { return array_[current_pos].info_ == ACTIVE; }
  
  
  size_t FindPos(const HashedObj & x) const {
    size_t current_pos = InternalHash(x);
     static std::HashTable<HashedObj> hf;
    //setting R valuse to 97 just pick random prime less that table size
    size_t offset = ((97 - (hf(x) % 97))% array_.size());      
    while (array_[current_pos].info_ != EMPTY &&
	   array_[current_pos].element_ != x) {
       collisions++;
      current_pos += offset;  // Compute ith probe.
      if (current_pos >= array_.size())
	current_pos -= array_.size();
    }
    return current_pos;
  }

  void Rehash() {
    std::vector<HashEntry> old_array = array_;

    // Create new double-sized, empty table.
    array_.resize(NextPrime(2 * old_array.size()));
    for (auto & entry : array_)
      entry.info_ = EMPTY;
    
    // Copy table over.
    current_size_ = 0;
    for (auto & entry :old_array)
      if (entry.info_ == ACTIVE)
	Insert(std::move(entry.element_));
  }
  
  size_t InternalHash(const HashedObj & x) const {
    static std::hash<HashedObj> hf;
    return hf(x) % array_.size( );
  }
};

#endif  // DOUBLE_HASHING_H
