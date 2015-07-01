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
            ptr_ = get_raw( ptr_->next_ );
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
            this->ptr_ = get_raw( this->ptr_->next_ );
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        /**
           The fowllowing two function inherit from const_iterator:

           bool operator==( const iterator &other ) const noexcept;
           bool operator!=( const iterator &other ) const noexcept;
        **/

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
    
    explicit forward_list( size_type n ) 
        : forward_list( n, value_type() )
    {  }

    forward_list( size_type n, const value_type &value ) {
        insert_after( cbefore_begin(), n, value );
    }

    template <class InputIterator, typename = RequireInputIterator<InputIterator>>
    forward_list( InputIterator first, InputIterator last ) {
        insert_after( cbefore_begin(), first, last );
    }

    forward_list( const forward_list &other ) 
        : forward_list( other.cbegin(), other.cend() )
    {  }

    forward_list( forward_list &&other ) noexcept {
        swap( other );
    }

    forward_list( std::initializer_list<value_type> lst ) 
        : forward_list( lst.begin(), lst.end() ) 
    {  }
    
    /**
       call head_'s destructor, all nodes' memory will be free 
    **/
    virtual ~forward_list() = default;

    /**
       can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    **/
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
        assign( lst.begin(), lst.end() );
        return *this;
    }
    
    void push_front( const value_type &value ) {
        auto copy = value;
        push_front( std::move( copy ) );
    }

    void push_front( value_type &&value ) {
        emplace_front( std::move( value ) );
    }

    template <class... Args>
    void emplace_front( Args&&... args ) {
        emplace_after( cbefore_begin(), std::forward<Args>(args)... );
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
        erase_after( cbefore_begin() );
    }
    
    reference front() {
        if( empty() ) {
            throw forward_list_exception( "forward_list::front(): forward_list is empty!" );
        }
        return *begin();
    }

    const_reference front() const {
        return const_cast<forward_list *>( this )->front();
    }

    iterator before_begin() noexcept {
        return { get_raw( head_ ) };
    }

    const_iterator before_begin() const noexcept {
        return { get_raw( head_ ) };
    }

    iterator begin() noexcept {
        return { get_raw( head_->next_ ) };
    }

    const_iterator begin() const noexcept {
        return { get_raw( head_->next_ ) };
    }

    iterator end() noexcept {
        return { };
    }

    const_iterator end() const noexcept {
        return { };
    }

    const_iterator cbefore_begin() const noexcept {
        return before_begin();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }
    
    template <typename InputIterator, typename = RequireInputIterator<InputIterator>>
    void assign( InputIterator first, InputIterator last ) {
        clear();
        insert_after( cbefore_begin(), first, last );
    }

    void assign( size_type n, const value_type &value ) {
        clear();
        insert_after( cbefore_begin(), n, value );
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

    /** 
        erase one element after position and return iterator pointing to the next element after the erased one. 
        if position is an off-the-end iterator or no element after position, then throw exception
     **/
    iterator erase_after( const_iterator position ) 
    {
        auto ptr = position.ptr_;
        if( !ptr ) {
            throw forward_list_exception( "forward_list::erase_after(): can't erase element after an off-the-end iterator" );   
        }
        if( !ptr->next_ ) {
            throw forward_list_exception( "forward_list::erase_after(): no element after the specify iterator" );   
        }

        ptr->next_ = std::move( ptr->next_->next_ );
        --size_;

        return to_non_const( ++position );
    }

    /**
       erase elements in range ( position, last ), but not include position and last
       you must insure this range is valid, that means the range must be empty or contain at least one element, otherwise throw exception
    **/
    iterator erase_after( const_iterator position, const_iterator last ) 
    {
        // if range ( position, last ) is empty, then we just return 
        auto pos = position;
        if( ++pos == last ) {
            return to_non_const( last );
        }
        
        auto next_erased = erase_after( position );
        while( next_erased != last ) {
            next_erased = erase_after( position );
        }

        return to_non_const( last );
    }

    iterator insert_after( const_iterator position, const value_type &value ) {
        auto copy = value;
        return insert_after( position, std::move( copy ) );
    }

    /** 
        insert value after position and return iterator pointing to the new element
        if position is an off-the-end iterator, then throw exception
    **/
    iterator insert_after( const_iterator position, value_type &&value ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::insert_after(): can't insert element after an off-the-end iterator" );
        }
        return emplace_after( position, std::move( value ) );
    }    

    /**
       insert n values after position and return iterator pointing to the last inserted element
       if position is an off-the-end iterator, then throw exception
    **/
    iterator insert_after( const_iterator position, size_type n, const value_type &value ) {   
        for( size_type i = 0; i < n; ++i ) {
            position = insert_after( position, value );
        }

        return to_non_const( position );
    }

    /**
       insert n values after position and return iterator pointing to the last inserted element
       if position is an off-the-end iterator, then throw exception
    **/
    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
    iterator insert_after( const_iterator position, InputIterator first, InputIterator last ) {
        for( auto iter = first; iter != last; ++iter ) {
            position = insert_after( position, *iter );
        }
        
        return to_non_const( position );
    }

    iterator insert_after( const_iterator position, std::initializer_list<value_type> lst ) {
        return insert_after( position, lst.begin(), lst.end() );
    }

    template <class... Args>
    iterator emplace_after( const_iterator position, Args&&... args ) {
        if( position == cend() ) {
            throw forward_list_exception( "forward_list::emplace_after(): can't emplace element after an off-the-end iterator" );
        }

        auto ptr = position.ptr_;
        ptr->next_ = make_unique<node>( value_type( std::forward<Args>(args)... ),
                                        std::move( ptr->next_ ) );
        ++size_;
        return to_non_const( ++position );        
    }

    void resize( size_type new_size ) {
        resize( new_size, value_type() );
    }
    
    void resize( size_type new_size, const value_type &value )
    {
        // if new_size equals to size_, do nothing
        if( new_size < size_ ) 
        {
            auto iter = cbefore_begin();
            std::advance( iter, new_size );
            erase_after( iter, end() );
        } 
        else if( new_size > size_ ) 
        {
            auto iter = cbefore_begin();
            std::advance( iter, size_ );
            insert_after( iter, new_size - size_, value );            
        }
    }

    void remove( const value_type &value ) {
        remove_if( [&value]( const value_type &elem ) {  return elem == value;  } );
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
            } else {
                ++previous;
                ++current;
            }
        }
    }

    void reverse() noexcept 
    {
        if( size() < 2 ) { 
            return; 
        }
        node_ptr previous;
        auto current = std::move( head_->next_ );
        
        while( current ) 
        {
            auto next = std::move( current->next_ );
            current->next_ = std::move( previous );
            previous = std::move( current );
            current = std::move( next );
        }

        head_->next_ = std::move( previous );
    }

    void unique() {
        unique( std::equal_to<value_type>() );
    }

    template <typename BinaryPredicate>
    void unique( BinaryPredicate binary_pred ) 
    {
        if( size_ < 2 ) {
            return;
        }

        auto last = end();
        auto previous = cbegin();
        auto current = begin();

        ++current;
        while( current != last ) 
        {
            if( binary_pred( *previous, *current ) ) {
                current = erase_after( previous );
            } else {
                ++previous;
                ++current;
            }
        }
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

    /**
       moves the element behind iterator i in other right after position of this forward list
       two forward_list may be identical
    **/
    void splice_after( const_iterator position, forward_list &&other, const_iterator i ) {
        // in the case two forward_list are identical
        // if position and i are equal or i just before position, then do nothing   
        auto iter = i;
        ++iter;
        if( position == i || position == iter ) {
            return;
        }

        // otherwise 
        auto first = i;
        auto last = ++iter;
        return splice_after( position, std::move( other ), first, last );
    }
    
    void splice_after( const_iterator position, forward_list &other,
                       const_iterator first, const_iterator last ) {
        return splice_after( position, std::move( other ), first, last );
    }

    /**
       moves all elements between first and last ( both not included ) 
       in other right after position of this forward_list 
       two forward_list may be identical
     **/
    void splice_after( const_iterator position, forward_list &&other, 
                       const_iterator first, const_iterator last ) 
    {
        if( position == cend() ) 
        {
            throw forward_list_exception( "forward_list::splice_after(): can't splice after an off-the-end const_iterator" );
        }

        size_type length = std::distance( first, last );

        // if range ( first, last ) doesn't contains any element, then just return
        if( length < 2 ) {
            return;
        }

        size_type elem_num = length - 1;  // element number in range ( first, last )
        size_ += elem_num;
        other.size_ -= elem_num;

        // before_last points to the last element in range ( first, last )
        auto before_last = first;            
        std::advance( before_last, elem_num );
        
        auto ptr = position.ptr_;
        auto remain = std::move( ptr->next_ );
        
        ptr->next_ = std::move( first.ptr_->next_ );
        first.ptr_->next_ = std::move( before_last.ptr_->next_ );
        before_last.ptr_->next_ = std::move( remain );
    }

    void print( std::ostream &os = std::cout, const std::string &delim = " " ) const {
        for( const auto &elem : *this ) {
            os << elem << delim;
        }
        os << std::endl;
    }

    void merge( forward_list &other ) {
        merge( std::move( other ) );
    }

    void merge( forward_list &&other ) {
        merge( std::move( other ), std::less<value_type>() );
    }
    
    template <typename Comp> 
    void merge( forward_list &other, Comp comp ) {
        merge( std::move( other ), comp );
    }
    
    /**
       merge another forward_list's content into this forward_list as sorted order
       you must insure two forward_list are sorted before merge them
    **/
    template <typename Comp>
    void merge( forward_list &&other, Comp comp )
    {
        size_ += other.size();
        other.size_ = 0;
        
        head_->next_ = merge( head_->next_, other.head_->next_, comp );
    }

    void sort() {
        sort( std::less<value_type>() );
    }

    /**
       sort forward_list using merge sort
     **/
    template <typename Compare>
    void sort( Compare comp ) {
        head_->next_ = merge_sort( head_->next_, comp );
    }

private: 
    /**
       get the raw pointer in unique_ptr
     **/
    static node_raw_ptr get_raw( const node_ptr &ptr ) noexcept {
        return ptr.get();
    }

    iterator to_non_const( const_iterator iter ) noexcept {
        return { iter.ptr_ };
    }

    /**
       lst points to the first element in that forward_list to be sorted
     **/
    template <typename Compare>
    node_ptr merge_sort( node_ptr &lst, Compare comp ) 
    {
        // if lst is empty or contains only one element, then it is already sorted
        if( lst == nullptr || lst->next_ == nullptr ) {
            return std::move( lst );
        }
        
        auto left = std::move( lst );
        auto current = get_raw( left );
        auto next = get_raw( current->next_ );
        
        while( next && next->next_ ) 
        {
            current = next;
            next = get_raw( next->next_->next_ );
        }
        
        auto right = std::move( current->next_ );

        left = merge_sort( left, comp );
        right = merge_sort( right, comp );
        
        return merge( left, right, comp );
    }

    /**
       left point to the first element in the first forward_list
       right point to the first element in the second forward_list
    **/
    template <typename Comp>
    node_ptr merge( node_ptr &left, node_ptr &right, Comp comp ) 
    {
        node head_node;                          // a dummy node             
        node_raw_ptr current = &head_node;

        while( left && right ) 
        {
            if( comp( left->value_, right->value_ ) ) 
            {
                current->next_ = std::move( left );
                current = get_raw( current->next_ );
                left = std::move( current->next_ );
            } 
            else 
            {
                current->next_ = std::move( right );
                current = get_raw( current->next_ );
                right = std::move( current->next_ );
            }
        }
        
        if( left ) {
            current->next_ = std::move( left );
        } else {
            current->next_ = std::move( right );            
        }
        
        return std::move( head_node.next_ );
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

    bool operator<( const forward_list<value_type> &other ) const noexcept {
        return std::lexicographical_compare( cbegin(), cend(), other.cbegin(), other.cend() );
    }
    
    bool operator>( const forward_list<value_type> &other ) const noexcept {
        return other < *this;
    }
    
    bool operator>=( const forward_list<value_type> &other ) const noexcept {
        return !( *this < other );
    }

    bool operator<=( const forward_list<value_type> &other ) const noexcept {
        return !( other < *this );
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
