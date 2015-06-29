/***
    单向链表
        1. 使用智能指针自动管理内存
        2. 使用虚析构函数，允许定义 forward_list 的派生类
        3. 引入异常，对于不合法的操作会抛出异常
        4. 不允许使用 allocator 来分配内存

    版本 1.0
    作者：詹春畅
    博客：senlinzhan.github.io
 ***/

#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_

#include "algorithm.hpp"
#include "memory.hpp"     
#include "iterator.hpp"         
#include <string>
#include <exception>               // for std::exception
#include <cstddef>                 // for std::size_t
#include <iostream>                // for debug
#include <functional>              // for std::less<>
#include <initializer_list>        // for std::initializer_list<>

namespace mystl {

class forward_list_exception : public std::exception
{
public:
    explicit forward_list_exception( const std::string &message ) 
        : message_( message )
        {  }
    
    virtual const char * what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

template <typename T>
class forward_list
{
private:
    struct node;
    using node_ptr = std::unique_ptr<node>; 
    using node_raw_ptr = node *;
    
    struct node 
    {
        node( const T &value, node_ptr &&next ) 
            : value_( value ), next_( std::move( next ) )
        {  }

        node( T &&value, node_ptr &&next ) 
            : value_( std::move( value ) ), next_( std::move( next ) ) 
        {  }

        explicit node( const T &value = T{} ) 
            : value_( value ), next_( nullptr ) 
        {  }
        
        node( const node & ) = delete;
        node &operator=( const node & ) = delete;
        
        node( node && ) = default;
        node &operator=( node && ) = default;

        T value_;
        node_ptr next_;
    };

public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    class const_iterator
    {
        friend class forward_list<T>;
    public:
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        const_iterator() noexcept
            : ptr_( nullptr ) 
        {  }

        reference operator*() const noexcept { 
            return ptr_->value_;
        }

        pointer operator->() const noexcept {
            return &( operator*() );
        }
        
        const_iterator &operator++() noexcept {
            ptr_ = (ptr_->next_).get();
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==( const const_iterator &other ) const noexcept {
            return ptr_ == other.ptr_;
        }
        
        bool operator!=( const const_iterator &other ) const noexcept {
            return ptr_ != other.ptr_;
        }

    protected:
        const_iterator( node_raw_ptr ptr ) noexcept
            : ptr_( ptr )
        {  }

        node_raw_ptr ptr_;
    };

    class iterator : public const_iterator
    {
        friend class forward_list<T>;
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        iterator() noexcept = default;

        reference operator*() const noexcept { 
            return this->ptr_->value_;
        }

        pointer operator->() const noexcept {
            return &( operator*() );
        }
        
        iterator &operator++() noexcept {
            this->ptr_ = (this->ptr_->next_).get();
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
/*
        the fowllowing two function is the same as it's parent :

        bool operator==( const iterator &other ) const noexcept;

        bool operator!=( const iterator &other ) const noexcept;
*/
    protected:
        iterator( node_raw_ptr ptr ) noexcept
            : const_iterator( ptr )
        {  }
    };

private:
    node_ptr head_ = make_unique<node>();     // dummy node
    size_type size_ = 0;

public:
    forward_list() = default;             
    
    explicit forward_list( size_type n, const value_type &value = value_type() ) {
        for( size_type i = 0; i < n; ++i ) {
            push_front( value );
        }
    }

    template <class InputIterator, typename = RequireInputIterator<InputIterator>>
    forward_list( InputIterator first, InputIterator last ) {
        auto new_list = copy_forward_list( first, last );
        head_->next_ = std::move( new_list );
    }

    forward_list( const forward_list &other ) {
        if( *this != other ) {
            auto new_list = copy_forward_list( other.begin(), other.end() );
            head_->next_ = std::move( new_list );
        }
    }

    forward_list( forward_list &&other ) noexcept {
        swap( other );
    }

    forward_list( std::initializer_list<value_type> lst ) 
        : forward_list( lst.begin(), lst.end() ) 
    {  }
    
    // call head_'s destructor, all nodes' memory will be free 
    virtual ~forward_list() = default;

    // can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    forward_list &operator=( const forward_list &other ) {
        auto copy = other;
        swap( copy );
        return *this;
    }

    forward_list &operator=( forward_list &&other ) noexcept {
        if( this != &other ) {
            clear();
            swap( other );
        }
        return *this;
    }

    forward_list &operator=( std::initializer_list<value_type> lst ) {
        size_ = 0;
        auto new_list = copy_forward_list( lst.begin(), lst.end() );
        head_->next_ = std::move( new_list );
        return *this;
    }
    
    void push_front( const value_type &value ) {
        auto ptr = make_unique<node>( value, std::move( head_->next_ ) );
        head_->next_ = std::move( ptr );
        ++size_;
    }

    void push_front( value_type &&value ) {
        auto ptr = make_unique<node>( std::move( value ), std::move( head_->next_ ) );
        head_->next_ = std::move( ptr );
        ++size_;
    }

    template <class... Args>
    void emplace_front( Args&&... args ) {
        auto ptr = make_unique<node>( value_type( std::forward<Args>(args)... ), 
                                      std::move( head_->next_ ) );
        head_->next_ = std::move( ptr );
        ++size_;
    }

    size_type size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void pop_front() {
        if( empty() ) {
            throw forward_list_exception( "forward_list::pop_front(): forward_list is empty!" );
        }
        head_->next_ = std::move( head_->next_->next_ );
        --size_;
    }
    
    reference front() {
        if( empty() ) {
            throw forward_list_exception( "forward_list::front(): forward_list is empty!" );
        }
        return head_->next_->value_;
    }

    const_reference front() const {
        return const_cast<forward_list *>( this )->front();
    }

    iterator before_begin() noexcept {
        return { head_.get() };
    }

    const_iterator before_begin() const noexcept {
        return { head_.get() };
    }

    iterator begin() noexcept {
        return { (head_->next_).get() };
    }

    const_iterator begin() const noexcept {
        return { (head_->next_).get() };
    }

    iterator end() noexcept {
        return { };
    }

    const_iterator end() const noexcept {
        return { };
    }

    const_iterator cbefore_begin() const noexcept {
        return { head_.get() };
    }

    const_iterator cbegin() const noexcept {
        return { (head_->next_).get() };
    }

    const_iterator cend() const noexcept {
        return { };
    }
    
    template <typename InputIterator, typename = RequireInputIterator<InputIterator>>
    void assign( InputIterator first, InputIterator last ) {
        size_ = 0;
        auto new_list = copy_forward_list( first, last );
        head_->next_ = std::move( new_list );
    }

    void assign( size_type n, const value_type &value ) {
        size_ = 0;
        head_->next_ = nullptr;
        for( size_type i = 0; i < n; ++i ) {
            push_front( value );
        }
    }

    void assign( std::initializer_list<value_type> lst ) {
        assign( lst.begin(), lst.end() );
    }

    void swap( forward_list &other ) noexcept {
        using std::swap;
        swap( size_, other.size_ );
        swap( head_, other.head_ );
    }

    void clear() noexcept {
        head_->next_ = nullptr;
        size_ = 0;
    }

    iterator erase_after( const_iterator position ) {
        node_raw_ptr ptr = position.ptr_;
        if( ptr == nullptr || ptr->next_ == nullptr ) {
            throw forward_list_exception( "forward_list::erase_after(): no element after that iterator" );   
        }
        ptr->next_ = std::move( ptr->next_->next_ );
        --size_;
        return { (ptr->next_).get() };
    }

    iterator erase_after( const_iterator position, const_iterator last ) {
        node_raw_ptr ptr = position.ptr_;
        if( ptr == nullptr || ptr->next_ == nullptr ) {
            throw forward_list_exception( "forward_list::erase_after(): no element after that iterator" );   
        }
        
        size_type removeNum = 0;
        for( auto iter = ++position; iter != last; ++iter ) {
            ++removeNum;
        }
        for( size_type i = 0; i < removeNum; ++i ) {
            ptr->next_ = std::move( ptr->next_->next_ );
        }
        size_ -= removeNum;

        return { (ptr->next_).get() };
    }

    iterator insert_after( const_iterator position, const value_type &value ) {
        return insert_after( position, 1, value );
    }

    iterator insert_after( const_iterator position, value_type &&value ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::insert_after(): the specity iterator is  a off-the-end iterator" );
        }
        node_raw_ptr ptr = position.ptr_;
        ptr->next_ = make_unique<node>( std::move( value ), std::move( ptr->next_ ) );
        ++size_;
        return { (ptr->next_).get() };        
    }    

    iterator insert_after( const_iterator position, size_type n, const value_type &value ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::insert_after(): the specity iterator is  a off-the-end iterator" );
        }
        node_raw_ptr ptr = position.ptr_;
        for( size_type i = 0; i < n; ++i ) {
            ptr->next_ = make_unique<node>( value, std::move( ptr->next_ ) );
            ptr = (ptr->next_).get();
        }
        size_ += n;
        return { ptr };
    }

    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
    iterator insert_after( const_iterator position, InputIterator first, InputIterator last ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::insert_after(): the specity iterator is  a off-the-end iterator" );
        }
        
        node_raw_ptr ptr = position.ptr_;
        for( auto iter = first; iter != last; ++iter ) {
            ptr->next_ = make_unique<node>( *iter, std::move( ptr->next_ ) );
            ptr = (ptr->next_).get();
            ++size_;
        }
        
        return { ptr };
    }

    iterator insert_after( const_iterator position, std::initializer_list<value_type> lst ) {
        return insert_after( position, lst.begin(), lst.end() );
    }

    template <class... Args>
    iterator emplace_after( const_iterator position, Args&&... args ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::emplace_after(): the specity iterator is  a off-the-end iterator" );
        }

        node_raw_ptr ptr = position.ptr_;
        ptr->next_ = make_unique<node>( value_type( std::forward<Args>(args)... ),
                                        std::move( ptr->next_ ) );
        ++size_;
        return { (ptr->next_).get() };        
    }

    void remove( const value_type &value ) {
        remove_if( [&value]( const value_type &elem ) { 
                       return elem == value;
                   } );
    }

    template <class Predicate>
    void remove_if( Predicate pred ) {
        if( empty() ) {
            return;
        }
        auto previous = cbefore_begin();
        auto current = begin();
        auto last = end();
        
        while( current != last ) {
            if( pred( *current ) ) {
                current = erase_after( previous );
                --size_;
            } else {
                ++previous;
                ++current;
            }
        }
    }

    // merge two sorted forward_list    
    template <class Compare = std::less<value_type>>
    void merge( forward_list &other, Compare comp = Compare{} ) {
        merge( std::move( other ), comp );
    }

    template <class Compare = std::less<value_type>>
    void merge( forward_list &&other, Compare comp = Compare{} ) {
        size_ += other.size();
        auto ptr = merge( std::move( head_->next_ ), 
                          std::move( (other.head_)->next_ ), 
                          comp );
        head_->next_ = std::move( ptr );
        other.head_->next_ = nullptr;
        other.size_ = 0;
    }

    void reverse() noexcept {
        if( size() < 2 ) {
            return;
        }
        node_ptr previous;
        auto current = std::move( head_->next_ );
        while( current != nullptr ) {
            auto next = std::move( current->next_ );
            current->next_ = std::move( previous );
            previous = std::move( current );
            current = std::move( next );
        }

        head_->next_ = std::move( previous );
    }

    void resize( size_type n, const value_type &value = value_type() ) {
        if( n == size_ ) {
            return;
        }

        if( n < size_ ) {
            auto iter = before_begin();
            for( size_type i = 0; i < n; ++i ) {
                ++iter;
            }
            (iter.ptr_)->next_ = nullptr;
        }

        if( n > size_ ) {
            auto iter = cbefore_begin();
            for( size_type i = 0; i < size_; ++i ) {
                ++iter;
            }
            insert_after( iter, n - size_, value );
        }

        size_ = n;
    }

    void splice_after( const_iterator position, forward_list &other ) {
        splice_after( position, std::move( other ) );
    }

    void splice_after( const_iterator position, forward_list &&other ) {
        splice_after( position, other, other.cbefore_begin(), other.cend() );
    }
    
    void splice_after( const_iterator position, forward_list &other, const_iterator i ) {
        splice_after( position, std::move( other ), i );
    }

    void splice_after( const_iterator position, forward_list &&other, const_iterator i ) {
        auto first = i;
        auto last = ++(++i);
        return splice_after( position, std::move( other ), first, last );
    }
    
    void splice_after( const_iterator position, forward_list &other,
                       const_iterator first, const_iterator last ) {
        return splice_after( position, std::move( other ), first, last );
    }

    void splice_after( const_iterator position, forward_list &&other,
                       const_iterator first, const_iterator last ) {
        if( position == nullptr ) {
            throw forward_list_exception( "forward_list::splice_after(): "
                                          "the first parameter is an off-the-end const_iterator" );
        }
        if( first == last ) {
            return;
        }

        size_type range_size = 0;
        auto current = first;
        auto before_last = current;
        while( ++current != last ) {
            before_last = current;
            ++range_size;
        }
        
        node_raw_ptr ptr = position.ptr_;
        auto next = std::move( ptr->next_ );
        
        node_raw_ptr first_ptr = first.ptr_;
        ptr->next_ = std::move( first_ptr->next_ );
        
        node_raw_ptr before_last_ptr = before_last.ptr_;
        first_ptr->next_ = std::move( before_last_ptr->next_ );
        before_last_ptr->next_ = std::move( next );
        
        size_ += range_size;
        other.size_ -= range_size;
    }

    template <typename BinaryPredicate = std::equal_to<value_type>>
    void unique( BinaryPredicate binary_pred = BinaryPredicate{} ) {
        if( size_ < 2 ) {
            return;
        }
        auto last = end();
        auto previous = cbegin();
        auto current = begin();
        ++current;

        while( current != last ) {
            if( binary_pred( *previous, *current ) ) {
                current = erase_after( previous );
            } else {
                ++previous;
                ++current;
            }
        }
    }

    // use merge sort
    template <typename Compare = std::less<value_type>>
    void sort( Compare comp = Compare{} ) {
        head_->next_ = merge_sort( std::move( head_->next_ ), comp );
    }

    void print( std::ostream &os = std::cout, const std::string &delim = " " ) const {
        for( const auto &elem : *this ) {
            os << elem << delim;
        }
        os << std::endl;
    }

private:
    template <typename InputIterator, typename = RequireInputIterator<InputIterator>>
    node_ptr copy_forward_list( InputIterator first, InputIterator last ) {
        if( first == last ) {
            return nullptr;
        }
        auto front = make_unique<node>( *first );
        auto rest = copy_forward_list( ++first, last );
        front->next_ = std::move( rest );
        ++size_;
        return front;
    }
 
    template <typename Compare>
    node_ptr merge_sort( node_ptr link, Compare comp ) {
        if( link == nullptr || link->next_ == nullptr ) {
            return link;
        }
        auto left = std::move( link );
        node_raw_ptr current = left.get();
        node_raw_ptr next = (current->next_).get();
        
        while( next != nullptr && next->next_ != nullptr ) {
            current = next;
            next = (next->next_->next_).get();
        }
        auto right = std::move( current->next_ );

        left = merge_sort( std::move( left ), comp );
        right = merge_sort( std::move( right ), comp );
        return merge( std::move( left ), std::move( right ), comp );
    }

    // left point to the first element in the first forward_list
    // right point to the first element in the second forward_list
    template <typename Comp>
    node_ptr merge( node_ptr left, node_ptr right, Comp comp ) {
        node head_node;
        node_raw_ptr head_ptr = &head_node;
        node_raw_ptr current = head_ptr;
        while( left != nullptr && right != nullptr ) {
            if( comp( left->value_, right->value_ ) ) {
                current->next_ = std::move( left );
                current = (current->next_).get();
                left = std::move( current->next_ );
            } else {
                current->next_ = std::move( right );
                current = (current->next_).get();
                right = std::move( current->next_ );
            }
        }
        
        if( left != nullptr ) {
            current->next_ = std::move( left );
        } else {
            current->next_ = std::move( right );            
        }
        
        return std::move( head_ptr->next_ );
    }

public:
    bool operator==( const forward_list<value_type> &other ) const noexcept {
        if( this == &other ) {    // the same forward_list
            return true;
        }
        if( size_ != other.size_ ) {
            return false;
        }

        return mystl::equal( cbegin(), cend(), other.cbegin() );
    }

    bool operator!=( const forward_list<value_type> &other ) const noexcept {
        return !(*this == other);
    }
};

template <typename T>
inline void swap( forward_list<T> &left, forward_list<T> &right ) noexcept {
    left.swap( right );
}

template <typename T>
inline std::ostream &operator<<( std::ostream &os, const forward_list<T> &lst ) {
    for( const auto &elem : lst ) {
        os << elem << " ";
    }
    return os;
}

};    // end of namespace mystl

#endif /* _FORWARD_LIST_H_ */
