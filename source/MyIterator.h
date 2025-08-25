#ifndef SOURCE_MYITERATOR_H_
#define SOURCE_MYITERATOR_H_

#include <stddef.h>

template<typename T_>
struct MyIterator : std::iterator<std::random_access_iterator_tag, T_>{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T_;
    using difference_type = std::ptrdiff_t;
    using pointer = T_*;
    using reference = T_&;

    pointer ptr_;

    MyIterator(pointer ptr = nullptr) : ptr_(ptr) {}
    MyIterator(const MyIterator&) = default;
    MyIterator& operator=(const MyIterator&) = default;
    ~MyIterator() = default;

    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }

    MyIterator& operator++() { ++ptr_; return *this; }
    MyIterator operator++(int) { MyIterator temp = *this; ++ptr_; return temp; }
    MyIterator& operator--() { --ptr_; return *this; }
    MyIterator operator--(int) { MyIterator temp = *this; --ptr_; return temp; }

    MyIterator& operator+=(difference_type n) { ptr_ += n; return *this; }
    MyIterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

    MyIterator operator+(difference_type n) const { return MyIterator(ptr_ + n); }
    MyIterator operator-(difference_type n) const { return MyIterator(ptr_ - n); }
    
    difference_type operator-(const MyIterator& other) const { return ptr_ - other.ptr_; }
    
    reference operator[](difference_type n) const { return ptr_[n]; }
};

template<typename T_>
bool operator==(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ == rhs.ptr_; }

template<typename T_>
bool operator!=(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ != rhs.ptr_; }

template<typename T_>
bool operator<(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ < rhs.ptr_; }

template<typename T_>
bool operator>(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ > rhs.ptr_; }

template<typename T_>
bool operator<=(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ <= rhs.ptr_; }

template<typename T_>
bool operator>=(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ >= rhs.ptr_; }

template<typename T_>
constexpr bool operator<=>(const MyIterator<T_>& lhs, const MyIterator<T_>& rhs) { return lhs.ptr_ <=> rhs.ptr_; }

template<typename T_>
MyIterator<T_> operator+(typename MyIterator<T_>::difference_type n, const MyIterator<T_>& it) { return it + n; }

#endif // SOURCE_MYITERATOR_H_