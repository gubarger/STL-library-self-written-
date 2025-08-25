#ifndef SOURCE_GUBARGERVECTOR_H_
#define SOURCE_GUBARGERVECTOR_H_

#include <memory>
#include <algorithm>
#include <stdexcept>
#include <cassert>

#include "MyIterator.h"

/**
 * @brief Dynamic array with the ability to change size.
 * 
 * @tparam T_ %vector element type.
 * @tparam Alloc_ allocator for memory management (default %std::allocator<T_>).
 * 
 * @note Implements a container with random-access iterators, similar to %std::vector.
 * Supports moving, copying, and working with different allocators.
 */
template<typename T_, typename Alloc_ = std::allocator<T_>>
class MyVector 
{
    T_* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
    Alloc_ alloc_;

    using AllocTraits_ = std::allocator_traits<Alloc_>;

public:
    using iterator = MyIterator<T_>;
    using const_iterator = MyIterator<const T_>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Default constructor. Creates an empty vector.
     */
    MyVector() = default;

    /**
     * @brief Constructor with a given size and value.
     * 
     * @param n Number of elements.
     * @param value Value to initialize elements with (default is %T_()).
     */
    explicit MyVector(size_t n, const T_& value = T_()) 
    {
        reserve(n);
        alloc_.allocate(n);
        
        for (size_t i = 0; i < n; ++i)
        {
            AllocTraits_::construct(alloc_, data_ + i, value);
        }

        size_ = n;
    }

    /**
     * @brief Constructor from a range of iterators.
     * 
     * @tparam Iter_ iterator type.
     * 
     * @param first Start of range.
     * @param last End of range.
     */
    template<typename Iter_>
    MyVector(Iter_ first, Iter_ last) 
    {
        size_t count = std::distance(first, last);
        reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            AllocTraits_::construct(alloc_, data_ + i, *first);
            ++first;
        }
        
        size_ = count;
    }

    /**
     * @brief Constructor from the initialization list.
     * 
     * @param init List of values.
     */
    MyVector(std::initializer_list<T_> init) : MyVector(init.begin(), init.end()) {}

    /**
     * @brief Destructor. Frees memory and destroys elements.
     */
    ~MyVector() 
    {
        clear();
        if (data_) AllocTraits_::deallocate(alloc_, data_, capacity_);
    }
    
    /**
     * @brief Copy constructor
     * 
     * @param other Vector for copying.
     * 
     * @note Uses `select_on_container_copy_construction` to choose the correct allocator:
     * - If the allocator is stateful, copies the allocator from other.
     * - If the allocator is stateless, it uses the standard mechanism.
     * 
     * This ensures correct behavior when working with different types of allocators.
     */
    MyVector(const MyVector& other) 
    : alloc_(AllocTraits_::select_on_container_copy_construction(other.alloc_))
    {
        reserve(other.capacity_);

        for (size_t i = 0; i < other.size_; ++i)
        {
            AllocTraits_::construct(alloc_, data_ + i, other.data_[i]);
        }

        size_ = other.size_;
    }

    /**
     * @brief Move constructor.
     * 
     * @param other Vector for move.
     */
    MyVector(MyVector&& other) noexcept 
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_), alloc_(std::move(other.alloc_)) 
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /**
     * @brief Copy assignment operator.
     * 
     * @param other Vector for copying.
     * 
     * @note Used to assign a copy of the other object to an existing object.
     */
    MyVector& operator=(const MyVector& other) 
    {
        if (this != &other)
        {
            clear();
            if (AllocTraits_::propagate_on_container_copy_assignment::value) alloc_ = other.alloc_;
            
            reserve(other.capacity_);

            for (size_t i = 0; i < other.size_; ++i)
            {
                AllocTraits_::construct(alloc_, data_ + i, other.data_[i]);
            }

            size_ = other.size_;
        }

        return *this;
    }
    
    /**
     * @brief Move assignment constructor.
     * 
     * @param other Vector for move.
     * 
     * @note Used to "intercept" resources from an r-value object.
     */
    MyVector& operator=(MyVector&& other) noexcept 
    {
        if (this != &other)
        {
            clear();
            AllocTraits_::deallocate(alloc_, data_, capacity_); // Freeing up current memory
            
            /** @todo The implementation was removed because it was redundant. */
            // if (data_) AllocTraits_::deallocate(alloc_, data_, capacity_);
            
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            if (AllocTraits_::propagate_on_container_move_assignment::value) alloc_ = std::move(other.alloc_);

            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }

        return *this;
    }

    /**
     * @brief `Iterators` for getting the last and initial element.
     */
    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + size_); }

    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + size_); }
    
    const_iterator cbegin() const { return const_iterator(data_); }
    const_iterator cend() const { return const_iterator(data_ + size_); }
    
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    T_& operator[](size_t index) { return data_[index]; }
    const T_& operator[](size_t index) const { return data_[index]; }

    /**
     * @brief Accessing an element with bounds checking.
     * 
     * @param index Element index.
     * 
     * @return Link to the element.
     * @throw std::out_of_range if index is out of range.
     */
    constexpr T_& at(size_t index) 
    {
        if (index >= size_) throw std::out_of_range("Index out of range");
        return data_[index];
    }

    /**
     * @brief Accessing an element with bounds checking.
     * 
     * This function is a copy of T_& at(size_t index), but has a const form.
     * @see T_& at(size_t index)
     */
    constexpr const T_& at(size_t index) const 
    {
        if (index >= size_) throw std::out_of_range("Index out of range");
        return data_[index];
    }

    void push_back(const T_& value)
    {
        if (size_ >= capacity_)
        {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }

        AllocTraits_::construct(alloc_, data_ + size_, value);
        size_++;
    }

    void push_back(T_&& value)
    {
        if (size_ >= capacity_)
        {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        
        AllocTraits_::construct(alloc_, data_ + size_, std::move(value));
        size_++;
    }

    void pop_back() 
    {
        if (size_ > 0)
        {
            AllocTraits_::destroy(alloc_, data_ + size_ - 1);
            size_--;
        }
    }

    void resize(size_t new_size) 
    {
        if (new_size < size_)
        {
            for (size_t i = new_size; i < size_; ++i)
            {
                AllocTraits_::destroy(alloc_, data_ + i);
            }
        }
        else if (new_size > size_)
        {
            for (size_t i = new_size; i < size_; ++i)
            {
                AllocTraits_::construct(alloc_, data_ + i);
            }
        }

        size_ = new_size;
    }
    
    void reserve(size_t n) 
    {
        if (n <= capacity_) return;
        reallocate(n);
    }

    void clear() 
    {
        for (size_t i = 0; i < size_; ++i)
        {
            AllocTraits_::destroy(alloc_, data_ + i);
        }

        size_ = 0;
    }

    size_t max_size() const noexcept 
    {
        const size_t diffmax = __gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(T_);
        const size_t allocmax = AllocTraits_::max_size(alloc_);
        
        return (std::min)(diffmax, allocmax);
    }

    void swap(MyVector& other) noexcept 
    {
#ifndef NDEBUG
    assert(alloc_ == other.alloc_);
#endif // NDEBUG

        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);

        if constexpr (AllocTraits_::propagate_on_container_swap::value) std::swap(alloc_, other.alloc_);
    }

    void shrink_to_fit()
    {
        if (capacity_ == size_) return;

        T_* new_data = AllocTraits_::allocate(alloc_, size_);
        size_t new_capacity = size_;

        try
        {
            for (size_t i = 0; i < size_; ++i)
            {
                AllocTraits_::construct(alloc_, new_data + i, std::move(data_[i]));
            }
        }
        catch(...)
        {
            AllocTraits_::deallocate(alloc_, new_data, new_capacity);
            throw;
        }

        for (size_t i = 0; i < size_; ++i)
        {
            AllocTraits_::destroy(alloc_, data_ + i);
        }

        AllocTraits_::deallocate(alloc_, data_, capacity_);
        
        data_ = new_data;
        capacity_ = size_;
    }

    constexpr Alloc_ get_allocator() const noexcept {
        return Alloc_(alloc_);
    }

    /**
     * @brief Assigns a given value to a %vector.
     * 
     * @param n Number of elements to be assigned.
     * @param val Value to be assigned.
     */
    constexpr void assing(size_t n, const T_& val)
    {
        this->clear();
        
        for (size_t i = 0; i < n; ++i)
        {
            push_back(std::move(val));
        }
    }

    /**
     * @brief Assigns a range to a %vector.
     * 
     * @param first An input iterator.
     * @param last An input iterator.
     */
    constexpr void assing(iterator first, iterator last)
    {
        this->clear();
     
        for (auto iter = first; iter != last; ++iter)
        {
            push_back(std::move(*iter));
        }
    }

    /**
     * @brief Assigns an initializer list to a %vector.
     * 
     * @param list An initializer_list.
     */
    constexpr void assing(std::initializer_list<T_> list)
    {
        this->clear();
     
        for (auto iter = list.begin(); iter != list.end(); ++iter)
        {
            push_back(std::move(*iter));
        }
    }

    /**
     * Returns a read-only (constant) reference to the data at the first
     * element of the %vector.
     * 
     * @throw 'std::runtime_error' if %vector is empty.
     */
    [[__nodiscard__]]
    constexpr T_& front()
    {
        if (empty()) throw std::runtime_error("Unable to get reference to last element. Vector is empty.");
        return *begin();
    }

    [[__nodiscard__]]
    constexpr const T_& front() const
    {
        if (empty()) return throw std::runtime_error("Unable to get reference to last element. Vector is empty.");
        return *begin();
    }

    /**
     * Returns a read/write reference to the data at the last
     * element of the %vector.
     * 
     * @throw 'std::runtime_error' if %vector is empty.
     */
    [[__nodiscard__]]
    constexpr T_& back()
    {
        if (empty()) throw std::runtime_error("Unable to get reference to last element. Vector is empty.");
        return *(end() - 1);
    }

    [[__nodiscard__]]
    constexpr const T_& back() const
    {
        if (empty()) return throw std::runtime_error("Unable to get reference to last element. Vector is empty.");
        return *(end() - 1);
    }

    /**
     * @returns
     * - If %vector is empty - nullptr.
     * 
     * @note returns a pointer to the first element of a vector,
     *       which is stored in a contiguous block of memory, 
     *       similar to an array.
     */
    [[__nodiscard__]]
    constexpr T_* data() noexcept
    {
        if (empty()) return nullptr;
        return std::__to_address(begin());
    }

    [[__nodiscard__]]
    constexpr const T_* data() const noexcept
    {
        if (empty()) return nullptr;
        return std::__to_address(begin());
    }

    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    operator T_() const { return data_; }

private:
    /**
     * @brief Reallocates memory to a new size.
     * 
     * @param new_capacity The new capacity.
     * 
     * @note Used for %reserve() and %vector growth.
     */
    void reallocate(size_t new_capacity) 
    {
        T_* new_data = AllocTraits_::allocate(alloc_, new_capacity);
        size_t i = 0;

        try 
        {
            for (i = 0; i < size_; ++i)
            {
                AllocTraits_::construct(alloc_, new_data + i, std::move(data_[i]));
                AllocTraits_::destroy(alloc_, data_ + i);
            }
        } 
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                AllocTraits_::destroy(alloc_, new_data + j);
            }
            
            AllocTraits_::deallocate(alloc_, new_data, new_capacity);
            throw;
        }

        if (data_) AllocTraits_::deallocate(alloc_, data_, capacity_);

        data_ = new_data;
        capacity_ = new_capacity;
    }
};

#endif // SOURCE_GUBARGERVECTOR_H_