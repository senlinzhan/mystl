#ifndef _LIST_H_
#define _LIST_H_

#include "algorithm.hpp"
#include "memory.hpp"     
#include "iterator.hpp"         
#include <string>
#include <exception>               // for std::exception
#include <cstddef>                 // for std::size_t
#include <iostream>                // for debug
#include <functional>              // for std::less<>
#include <initializer_list>        // for std::initializer_list<>

namespace mystl{

class list_exception : public std::exception
{
public:
    explicit list_exception( const std::string &message ) 
        : message_( message )
        {  }
    
    virtual const char * what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

template <typename T>
class list
{
private:
    struct node;
    using node_raw_ptr = node *;
    using node_ptr = std::unique_ptr<node>;

    struct node 
    {
        explicit node( const T &value, node_raw_ptr prev, node_ptr &&next ) 
            : value_( value ), previous_( prev ), next_( std::move( next ) )
        {  }

        explicit node( T &&value, node_raw_ptr prev, node_ptr &&next ) 
            : value_( std::move( value ) ), previous_( prev ), next_( std::move( next ) ) 
        {  }

        explicit node( const T &value = T{} ) 
            : value_( value ), previous_( nullptr ), next_( nullptr ) 
        {  }
        
        node( const node & ) = delete;
        node &operator=( const node & ) = delete;
        
        node( node && ) = default;
        node &operator=( node && ) = default;

        T value_;
        node_raw_ptr previous_;                    // raw pointer points to the previous node
        node_ptr next_;                            // smart pointer points to the next node
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
        friend class list<T>;
    public:
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        
        const_iterator() noexcept
            : ptr_( nullptr ) 
        {  }

        reference operator*() const { 
            return ptr_->value_;
        }

        pointer operator->() const {
            return &( operator*() );
        }
        
        const_iterator &operator++() {
            ptr_ = get_raw( ptr_->next_ );
            return *this;
        }

        const_iterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator &operator--() {
            ptr_ = ptr_->previous_;
            return *this;
        }

        const_iterator operator--(int) {
            auto tmp = *this;
            --*this;
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
        friend class list<T>;
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        
        iterator() noexcept = default;

        reference operator*() const { 
            return this->ptr_->value_;
        }

        pointer operator->() const {
            return &( operator*() );
        }
        
        iterator &operator++() {
            this->ptr_ = get_raw( this->ptr_->next_ );
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        iterator &operator--() {
            this->ptr_ = this->ptr_->previous_;
            return *this;
        }

        iterator operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }

    protected:
        iterator( node_raw_ptr ptr ) noexcept
            : const_iterator( ptr )
        {  }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    node_ptr head_;
    node_raw_ptr tail_;
    size_type size_;
    
public:    
    list() {
        init();
    }
    
    explicit list( size_type n ) 
        : list( n, value_type() )
    {  }

    list( size_type n, const value_type &value ) {
        init();
        insert( cend(), n, value );
    }

    template <class InputIterator, typename = RequireInputIterator<InputIterator>>
    list( InputIterator first, InputIterator last ) {
        init();
        insert( cend(), first, last );
    }

    list( std::initializer_list<value_type> lst ) 
        : list( lst.begin(), lst.end() ) 
    {  }


    list( const list &other )
        : list( other.cbegin(), other.cend() ) 
    {  }

    // can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    list &operator=( const list &other ) {
        auto copy = other;
        swap( copy );
        return *this;
    }

    list( list &&other ) noexcept {
        swap( other );
    }

    list &operator=( list &&other ) noexcept {
        if( this != &other ) {
            clear();
            swap( other );
        }
        return *this;
    }

    // call head_'s destructor, all nodes' memory will be free 
    virtual ~list() = default;

    void swap( list &other ) noexcept {
        using std::swap;
        swap( head_, other.head_ );
        swap( tail_, other.tail_ );
        swap( size_, other.size_ );
    }

    // we assume value_type's destructor will not throw exception
    void clear() noexcept {
        head_->next_ = std::move( tail_->previous_->next_ );
        tail_->previous_ = get_raw( head_ );
        size_ = 0;
    }

    list &operator=( std::initializer_list<value_type> lst ) {
        assign( lst.begin(), lst.end() );
        return *this;
    }

    void assign( size_type n, const value_type &value ) {
        clear();
        insert( cend(), n, value );
    }

    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
    void assign( InputIterator first, InputIterator last ) {
        clear();
        insert( cend(), first, last );
    }

    void assign( std::initializer_list<value_type> lst ) {
        assign( lst.begin(), lst.end() );
    }

    iterator begin() noexcept {
        return { get_raw( head_->next_ ) };
    }
    
    const_iterator begin() const noexcept {
        return { get_raw( head_->next_ ) };
    }
    
    iterator end() noexcept { 
        return { tail_ };
    }
    
    const_iterator end() const noexcept {
        return { tail_  };   
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator( end() );
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator( end() ); 
    }
    
    reverse_iterator rend() noexcept { 
        return reverse_iterator( begin() ); 
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator( begin() ); 
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }
    
    const_iterator cend() const noexcept {
        return end();
    }
    
    const_reverse_iterator crbegin() const noexcept { 
        return rbegin();
    }
    
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    void push_back( const value_type &value ) {
        auto copy = value;
        push_back( std::move( copy ) );
    }

    void push_back( value_type &&value ) {
        emplace_back( std::move( value ) );
    }

    void push_front( const value_type &value ) {
        auto copy = value;
        push_front( std::move( copy ) );
    }

    void push_front( value_type &&value ) {
        emplace_front( std::move( value ) );
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
     
    size_type size() const noexcept {
        return size_;
    }

   void resize( size_type new_size ) {
       resize( new_size, value_type() );
   }

    void resize( size_type new_size, const value_type &value ) {
        // if new_size equals to size_, do nothing
        if( new_size < size_ ) 
        {
            auto iter = begin();
            std::advance( iter, new_size );             // move iter to the first illegal node
            erase( iter, end() );
        } 
        else if( new_size > size_ ) 
        {
            insert( cend(), new_size - size_, value );   
        }
    }

    reference front() {
        if( empty() ){
            throw list_exception( "list::front(): list is empty!" );
        }
        return *begin(); 
    }

    // we know that reference can convert to const_reference
    const_reference front() const {
        return const_cast<list *>( this )->front();
    }

    reference back() { 
        if( empty() ){
            throw list_exception( "list::back(): list is empty!" );
        }
        return *rbegin();
    }

    // we know that reference can convert to const_reference
    const_reference back() const { 
        return const_cast<list *>( this )->back();
    }
    
    template<typename... Args>
    void emplace_front( Args&&... args ) {
        emplace( cbegin(), std::forward<Args>( args )... );
    }

    template<typename... Args>
    void emplace_back( Args&&... args ) {
        emplace( cend(), std::forward<Args>( args )... );
    }

    void pop_front() {
        if( empty() ){
            throw list_exception( "list::pop_front(): list is empty!" );
        }
        erase( begin() );
    }

    void pop_back() {
        if( empty() ){
            throw list_exception( "list::pop_back(): list is empty!" );
        }
        erase( --end() );
    }

    template<typename... Args>
    iterator emplace( const_iterator position, Args&&... args ) 
        {
        auto prev_node = position.ptr_->previous_;
        auto curr_node = std::move( prev_node->next_ );
        
        auto new_node = make_unique<node>( value_type( std::forward<Args>( args )... ),
                                           prev_node, std::move( curr_node ) );

        new_node->next_->previous_ = get_raw( new_node );
        prev_node->next_ = std::move( new_node );
        ++size_;
        return to_non_const( --position );
    }

    iterator insert( const_iterator pos, const value_type &value ) {
        auto copy = value;
        return insert( pos, std::move( copy ) );
    }

    iterator insert( const_iterator pos, value_type &&value ) {
        return emplace( pos, std::move( value ) ); 
    }
        
    iterator insert( const_iterator pos, std::initializer_list<value_type> lst ) {
        return insert( pos, lst.begin(), lst.end() );
    }

    /**
       inserts n copies of value before iterator position pos 
       returns the position of the first new element or pos if there is no new element ( n equals to zero )
    **/
    iterator insert( const_iterator pos, size_type n, const value_type &value ) {
        if( n == 0 ) {
            return to_non_const( pos );
        }
        auto first_inserted = insert( pos, value );

        for( size_type i = 1; i < n; ++i ) {
            insert( pos, value );            
        }        
        return first_inserted;        
    }

    /**
       inserts a copy of all elements of the range [first, last) before iterator position pos 
       returns the position of the first new element or pos if there is no new element
    **/
    template<typename InputIterator, typename = RequireInputIterator<InputIterator>> 
    iterator insert( const_iterator pos, InputIterator first, InputIterator last ) {
        if( first == last ) {
            return to_non_const( pos );
        }
        
        auto first_inserted = insert( pos, *first );

        for( auto iter = ++first; iter != last; ++iter ) {
            insert( pos, *iter );
        }
        
        return first_inserted;
    }

    void remove( const value_type &value ) {
        remove_if( [&value]( const value_type &elem ) {  return elem == value;  } );
    }

    template<typename Predicate>
    void remove_if( Predicate pred ) {
        if( empty() ) {
            return;
        }
        auto iter = begin();
        auto last = end();
        while( iter != last ) {
            if( pred( *iter ) ) {
                iter = erase( iter );
            } else {
                ++iter;
            }
        }        
    }

    void unique() {
        unique( std::equal_to<value_type>() );
    }

    template<typename BinaryPredicate>
    void unique( BinaryPredicate bin_pred ) 
    {
        if( size_ < 2 ) {
            return;
        }
        auto last = end();
        auto current = begin();
        auto previous = current;
        ++current;
        
        while( current != last ) {
            if( bin_pred( *previous, *current ) ) {
                current = erase( current );
            } else {
                previous = current;                
                ++current;
            }
        }
    }

    iterator erase( const_iterator position ) {
        if( position == cend() ) {
            throw list_exception( "list::erase(): the specify const_iterator is an off-the-end iterator!" );
        }
        auto prev_node = position.ptr_->previous_;
        prev_node->next_ = std::move( position.ptr_->next_ );
        prev_node->next_->previous_ = prev_node;
        --size_;
        
        return { get_raw( prev_node->next_ ) };
    }

    /**
       removes all elements of the range [fist, end) 
       returns the position of the next element
    **/
    iterator erase( const_iterator first, const_iterator last ) {
        while( first != last ) {
            first = erase( first );
        }
        return to_non_const( last );
    }

    void merge( list &&lst ) {
        merge( std::move( lst ), std::less<value_type>() );
    }
    
    void merge( list &lst ) { 
        merge( std::move( lst ) ); 
    }

    template<typename Comp>
    void merge( list &lst, Comp comp ) {
        merge( std::move( lst ), comp );
    }

    template<typename Comp>
    void merge( list &&lst, Comp comp )
    {
        list new_list;        

        auto iter1 = begin(), last1 = end();
        auto iter2 = lst.begin(), last2 = lst.end();
        
        while( iter1 != last1 && iter2 != last2 ) 
        {
            if( comp( *iter1, *iter2 ) ) 
            {
                new_list.push_back( std::move( *iter1 ) );
                ++iter1;
            } 
            else 
            {
                new_list.push_back( std::move( *iter2 ) );
                ++iter2;
            }
        }
        
        if( iter1 != last1 ) {
            new_list.insert( new_list.end(), iter1, last1 );
        } else {
            new_list.insert( new_list.end(), iter2, last2 );
        }

        swap( new_list );
    }

    void reverse() noexcept {
        if( size() < 2 ) {
            return;
        }
        tail_ = get_raw( head_ );
        auto curr = std::move( head_->next_ );
        auto prev = std::move( head_ );
        
        while( curr ) {
            auto next = std::move( curr->next_ );
            prev->previous_ = get_raw( curr );            
            curr->next_ = std::move( prev );
            prev = std::move( curr );
            curr = std::move( next );
        }

        head_ = std::move( prev );
    }

    void splice( const_iterator position, list &&other ) noexcept {
        splice( position, std::move( other ), other.cbegin(), other.cend() );
    }

    void splice( const_iterator position, list &other ) noexcept {
        splice( position, std::move( other ) );
    }
    

    void splice( const_iterator position, list &other, const_iterator i ) noexcept {
        splice( position, std::move( other ), i );
    }

    void splice( const_iterator position, list &other, 
                 const_iterator first, const_iterator last ) noexcept {
        splice( position, std::move( other ), first, last );
    }

    void splice( const_iterator position, list &&other, const_iterator i ) noexcept {
        // TODO
    }

    void splice( const_iterator position, list &&other, 
                 const_iterator first, const_iterator last ) noexcept {
        // TODO
    }

    void sort() {
        sort( std::less<value_type>() );
    }

    template<typename Comp>
    void sort( Comp comp ) {
        // TODO
    }

private:
    static node_raw_ptr get_raw( const node_ptr &ptr ) noexcept {
        return ptr.get();
    }

    void init() {
        head_ = make_unique<node>();
        head_->next_ = make_unique<node>( value_type(), get_raw( head_ ), nullptr );
        tail_ = get_raw( head_->next_ );
        size_ = 0;
    }

    iterator to_non_const( const_iterator iter ) noexcept {
        return { iter.ptr_ };
    }

public:
    bool operator==( const list<value_type> &other ) const noexcept {
        if( this == &other ) {    // the same list
            return true;
        }
        if( size_ != other.size_ ) {
            return false;
        }

        return mystl::equal( cbegin(), cend(), other.cbegin() );
    }

    bool operator!=( const list<value_type> &other ) const noexcept {
        return !(*this == other);
    }

    bool operator<( const list<value_type> &other ) const noexcept {
        return std::lexicographical_compare( cbegin(), cend(), other.cbegin(), other.cend() );
    }
    
    bool operator>( const list<value_type> &other ) const noexcept {
        return other < *this;
    }
    
    bool operator>=( const list<value_type> &other ) const noexcept {
        return !( *this < other );
    }

    bool operator<=( const list<value_type> &other ) const noexcept {
        return !( other < *this );
    }
};

template <typename T>
inline void swap( list<T> &left, list<T> &right ) noexcept {
    left.swap( right );
}

template <typename T>
inline std::ostream &operator<<( std::ostream &os, const list<T> &lst ) {
    for( const auto &elem : lst ) {
        os << elem << " ";
    }
    return os;
}

};    // namespace mystl

#endif /* _LIST_H_ */
