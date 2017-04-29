/***
    vector
        1. 引入异常，对于不合法的操作会抛出异常
        2. 使用 std::allocator 来管理内存

    版本 1.0
    作者：詹春畅
    博客：senlinzhan.github.io
 ***/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "algorithm.hpp"
#include "iterator.hpp"         
#include "memory.hpp"
#include <string>
#include <iostream>
#include <exception>
#include <initializer_list> 

namespace mystl { 

template <typename T>
class vector
{
public:
    using value_type               = T;
    using pointer                  = T*;
    using const_pointer            = const T*;
    using reference                = T&;
    using const_reference          = const T&;
    using size_type                = std::size_t;
    using difference_type          = std::ptrdiff_t;
    using allocator_type           = std::allocator<T>;
    using iterator                 = T*;
    using const_iterator           = const T*;
    using reverse_iterator         = std::reverse_iterator<T*>;
    using const_reverse_iterator   = std::reverse_iterator<const T*>;

private:
    pointer elem_ = nullptr;    // pointer to the first element in the allocated space
    pointer free_ = nullptr;    // pointer to the first free element in the allocated space
    pointer last_ = nullptr;    // pointer to one past the end of the allocated space
    std::allocator<T> alloc_;

public:
    vector() noexcept = default;

    explicit vector(size_type n)
    {
        create_elements(n, value_type());
    }

    vector(size_type n, const value_type &value)
    {
        create_elements(n, value);
    }
    
    vector(std::initializer_list<value_type> values)
        : vector(values.begin(), values.end())
    {        
    }
    
    template<typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>> 
    vector(InputIterator first, InputIterator last)        
    {
        create_elements(first, last);
    }

    vector(const vector &other)
    {
        create_elements(other.cbegin(), other.cend());
    }
    
    /**
       can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    **/
    vector &operator=(const vector &other) 
    {
        auto copy = other;
        swap(copy);
        return *this;
    }

    vector(vector &&other) noexcept
    {
        swap(other);
    }
    
    vector &operator=(vector &&other) noexcept 
    {
        // handle the problem of self-move-assignment        
        if (this != &other)
        {
            clear_elements();
            swap(other);
        }
        return *this;
    }

    vector &operator=(std::initializer_list<value_type> values) 
    {
        assign(values.begin(), values.end());
    }

    ~vector() noexcept
    {
        clear_elements();
    }

    /**
       removes all elements from the vector, leaving the container with a size of 0
       but we don't deallocate memory, vector's capacity doesn't change
    **/
    void clear() 
    {
        erase(cbegin(), cend());
    }

    template<typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    void assign(InputIterator first, InputIterator last) 
    {     
        clear_elements();
        create_elements(first, last);
    }

    void assign(std::initializer_list<value_type> lst) 
    {
        assign(lst.begin(), lst.end());
    }

    void assign(size_type n, const value_type &value) 
    {
        clear_elements();
        create_elements(n, value);
    }

    iterator begin() noexcept 
    {
        return elem_;
    }
    
    const_iterator begin() const noexcept 
    {
        return elem_;
    }
    
    iterator end() noexcept 
    {
        return free_;
    }
    
    const_iterator end() const noexcept 
    {
        return free_;
    }

    reverse_iterator rbegin() noexcept 
    {
        return reverse_iterator(free_);
    }
    
    const_reverse_iterator rbegin() const noexcept 
    {
        return const_reverse_iterator(free_);
    }

    reverse_iterator rend() noexcept 
    {
        return reverse_iterator(elem_);
    }
    
    const_reverse_iterator rend() const noexcept 
    {
        return const_reverse_iterator(elem_);
    }

    const_iterator cbegin() const noexcept 
    {
        return begin();
    }

    const_iterator cend() const noexcept 
    {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept 
    {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept 
    {
        return rend();
    }

    size_type size() const noexcept 
    {
        return free_ - elem_;
    }

    void resize(size_type new_size) 
    {
        return resize(new_size, value_type());
    }

    void resize(size_type new_size, const value_type &value) 
    {
        if(new_size < size()) 
        {
            erase(begin() + new_size, end());
        }
        else if(new_size > size()) 
        {
            insert(end(), size() - new_size, value);
        }
    }

    /** 
        push_back() is exception safe because if exception throw by value_type's copy constructor
        all element will be destroy and all memory will free by vector's destructor
    **/
    void push_back(const value_type &value) 
    {
        auto copy = value;                 // value_type's copy constructor may throw 
        push_back(std::move(copy));
    }

    void push_back(value_type &&value) 
    {
        emplace_back(std::move(value));
    }

    template<typename... Args> 
    void emplace_back(Args&&... args) 
    {
        if(free_ == last_) 
        {
            expand_double();
        }
        alloc_.construct(free_, std::forward<Args>( args )...);
        ++free_;        
    }

    void shrink_to_fit() 
    {
        auto other = *this;
        swap(other);
    }

    size_type capacity() const noexcept 
    {
        return last_ - elem_;
    }
    
    bool empty() const noexcept 
    {
        return elem_ == free_;
    }

    void reserve(size_type n) 
    {
        if(n > capacity()) 
        {
            expand_to(n);
        }
    }

    reference operator[](size_type n) 
    {
        return at(n);
    }

    const_reference operator[](size_type n) const 
    {
        return at(n);
    }

    reference at(size_type n) 
    {
        if(n < size()) 
        {
            return elem_[n];
        } 
        else 
        {
            throw std::out_of_range("vector::at() - the specify index is out of bound");
        }        
    }

    const_reference at(size_type n) const 
    {
        return const_cast<vector *>(this)->at();
    }

    reference front() 
    {
        if(!elem_) 
        {
            throw std::length_error("vector::front() - the vector is empty");
        }
        return *begin();
    }

    const_reference front() const 
    {
        return const_cast<vector *>(this)->front();
    }

    reference back() 
    {
        if(!elem_) 
        {
            throw std::length_error("vector::back() - the vector is empty");            
        }
        return *rbegin();        
    }
      
    const_reference back() const noexcept 
    {
        return const_cast<vector *>(this)->back();
    }

    void pop_back() 
    {
        if(!elem_) 
        {
            throw std::length_error("vector::pop_back() - the vector is empty"); 
        }
        alloc_.destroy(--free_);
    }

    template<typename... Args>
    iterator emplace(const_iterator position, Args&&... args) 
    {
        if(position < cbegin() || position > cend()) 
        {
            throw std::out_of_range("vector::emplace() - parameter \"position\" is out of bound");            
        }
        
        if(position == cend()) 
        {
            emplace_back(std::forward<Args>(args)...);
            return free_ - 1;
        }
        
        difference_type diff = position - cbegin();

        // if we expand vector's size, then position will be invalid
        if(free_ == last_) 
        {
            expand_double();
        }
        
        // Note: we can't use position now, because position may be invalid, we use pos instead
        auto pos = elem_ + diff;
        
        // we construct a new element as the last element
        alloc_.construct(free_, *(free_ - 1));
        ++free_;

        // move elements
        for(auto iter = free_ - 2; iter != pos; --iter) 
        {
            *iter = std::move(*(iter - 1));
        }
        
        *pos = value_type(std::forward<Args>(args)...);

        return pos;
    }
    
    iterator insert(const_iterator position, const value_type &value) 
    {
        auto copy = value;
        return insert(position, std::move(copy));
    }

    iterator insert(const_iterator position, value_type &&value) 
    {
        return emplace(position, std::move(value));
    }

    iterator insert(const_iterator position, std::initializer_list<value_type> lst) 
    {
        return insert(position, lst.begin(), lst.end());
    }

    /**
       inserts n copies of value before the specify iterator 
       returns the position of the first new element 
       or return the origin iterator if there is no new element ( n equals to zero )
    **/    
    iterator insert(const_iterator position, size_type n, const value_type &value) 
    {
        auto pos = to_non_const(position);

        if(n == 0)
        {
            return pos;
        }
        
        // we must always update iterator because iterator may be in invalid state
        for(size_type i = 0; i < n; ++i) 
        {
            pos = insert(pos, value);            
        }        
        return pos;
    }

    /**
       inserts a copy of all elements of the range [first, last) before the specify iterator 
       returns the position of the first new element 
       or return the origin iterator if there is no new element ( n equals to zero )
    **/
    template<typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    iterator insert(const_iterator position, InputIterator first, InputIterator last) 
    {
        auto pos = to_non_const(position);

        if(first == last) 
        {
            return pos;
        }
        // we must always update iterator pos because pos may be in invalid state
        for(auto iter = first; iter != last; ++iter) 
        {
            pos = insert(pos, *iter);
            ++pos;
        }
        pos -= std::distance(first, last);
        
        return pos;
    }
    
    /** 
        removes the element at iterator position pos and returns the position of the next element
     **/
    iterator erase(const_iterator position)
    {
        // if iterator points to invalid range, then throw exception
        if(position < cbegin() || position >= cend()) 
        {
            throw std::out_of_range("vector::erase() - parameter \"position\" is out of bound");                        
        }        
        auto pos = to_non_const(position);
        
        if(position + 1 != cend()) 
        {
            std::move(position + 1, cend(), pos);
        }
        // destroy the last element
        alloc_.destroy(--free_);
        
        return pos;
    }

    iterator erase(const_iterator first, const_iterator last) 
    {
        if(!( first >= cbegin() && last <= cend())) 
        {
            throw std::out_of_range("vector::erase() - parameter \"first\" or \"last\" is out of bound");            
        }
        auto iter = std::move(to_non_const(last), free_, to_non_const(first));
        destroy_elements(iter, free_);
        
        free_ = iter;
        return to_non_const(first);
    }

    void swap(vector &other) noexcept 
    {
        using std::swap;
        swap(elem_, other.elem_);
        swap(free_, other.free_);
        swap(last_, other.last_);
    }

    void print(std::ostream &os = std::cout, const std::string &delim = " ") const 
    {
        for(const auto &elem : *this)
        {
            os << elem << delim;
        }
    }
    
    void sort() 
    {
        sort(std::less<value_type>());
    }
    
    template <typename Comp>
    void sort(Comp comp) 
    {
        std::sort(begin(), end(), comp);
    }

private:
    void expand_double() 
    {
        auto new_size = empty() ? 1 : size() * 2;
        expand_to(new_size);
    }

    void expand_to(size_type new_size) 
    {
        if(new_size <= size())
        {
            return;
        }
        
        auto new_elem = alloc_.allocate(new_size);
        auto new_free = new_elem;
        
        // if value_type's move constructor is noexcept, then move elements
        // otherwise copy elements
        if(std::is_nothrow_move_constructible<value_type>()) 
        {
            for(auto iter = elem_; iter != free_; ++iter) 
            {
                alloc_.construct(new_free++, std::move( *iter ));
            }
        } 
        else 
        {
            try 
            {
                new_free = std::uninitialized_copy(elem_, free_, new_elem);
            }
            catch(...)        // catch the exception throw by value_type's copy constructor
            {    
                alloc_.deallocate(new_elem, new_size);
                throw;
            }
        }
        
        // remember to clear the origin vector's content
        clear_elements();
        
        elem_ = new_elem;
        free_ = new_free;
        last_ = new_elem + new_size;
    }

    
    // Note: before call this function, you must sure that the container is empty!
    void create_elements(size_type n, const value_type &value) 
    {
        auto new_elem = alloc_.allocate(n);

        try 
        {
            std::uninitialized_fill(new_elem, new_elem + n, value);
        }
        catch(...)     // catch the exception throw by value_type's copy constructor
        {
            alloc_.deallocate(new_elem, n);    // avoid memory leak
            throw;
        }
        
        elem_ = new_elem;
        last_ = free_ = new_elem + n;
    }

    // Note: before call this function, you must sure that the container is empty!
    template <typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    void create_elements(InputIterator first, InputIterator last)
    {
        if(first == last) 
        {
            return;
        }

        auto n = std::distance(first, last);
        auto new_elem = alloc_.allocate(n);

        pointer new_free;
        try 
        {
            new_free = std::uninitialized_copy(first, last, new_elem);
        }
        catch(...)     // catch the exception throw by value_type's copy constructor
        {
            alloc_.deallocate(new_elem, n);    // avoid memory leak
            throw;
        }
        
        elem_ = new_elem;
        last_ = free_ = new_free;
    }

    void clear_elements() noexcept 
    {
        if(elem_) 
        {
            destroy_elements(elem_, free_);
            alloc_.deallocate(elem_, capacity());
            elem_ = free_ = last_ = nullptr;
        }
    }
    
    // destruct elements in allocated memory
    void destroy_elements(iterator first, iterator last) noexcept 
    {
        for (auto iter = first; iter != last; )
        {
            // assume value_type's destructor will not throw exception            
            alloc_.destroy(iter);
        }
    }

    iterator to_non_const(const_iterator iter) 
    {
        return const_cast<iterator>(iter);
    }

public:
    bool operator==(const vector &other) const noexcept 
    {
        if(this == &other) // equals to itself
        {     
            return true;
        }
        if(size() != other.size()) 
        {
            return false;
        }
        
        return mystl::equal(cbegin(), cend(), other.cbegin());
    }

    bool operator!=(const vector &other) const noexcept 
    {
        return !(*this == other);
    }

    bool operator<(const vector &other) const noexcept 
    {
        return std::lexicographical_compare(cbegin(), cend(), other.cbegin(), other.cend());
    }
    
    bool operator>(const vector &other) const noexcept 
    {
        return other < *this;
    }
    
    bool operator>=(const vector &other) const noexcept 
    {
        return !(*this < other);
    }

    bool operator<=(const vector &other) const noexcept 
    {
        return !( other < *this );
    }
};

template <typename T>
void swap(vector<T> &first, vector<T> &second ) noexcept 
{
    first.swap(second);
}
    
template <typename T>
std::ostream &operator<<(std::ostream &os, const vector<T> &vec) 
{
    vec.print(os, " ");
    return os;
}


}; // namespace mystl


#endif /* _VECTOR_H_ */
