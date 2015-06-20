#ifndef _FORWARD_LIST_H_
#define _FORWARD_LIST_H_

#include "memory.hpp"
#include <exception>
#include <cstddef>
#include <iostream>

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
    friend void swap( forward_list<T> &, forward_list<T> & );
private:
    struct node;
    using node_ptr = std::unique_ptr<node>;
    
    struct node 
    {
        explicit node( const T &value, node_ptr &&next ) 
            : value_( value ), next_( std::move( next ) )
        {  }

        explicit node( T &&value, node_ptr &&next ) 
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
        friend forward_list<T>;
    public:
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        const_iterator( node *ptr = nullptr ) noexcept
            : current_( ptr ) 
        {  }

        reference operator*() const noexcept { 
            return current_->value_;
        }

        pointer operator->() const noexcept {
            return &( operator*() );
        }
        
        const_iterator &operator++() noexcept {
            current_ = (current_->next_).get();
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==( const const_iterator &other ) const noexcept {
            return current_ == other.current_;
        }
        
        bool operator!=( const const_iterator &other ) const noexcept {
            return current_ != other.current_;
        }

    private:
        node *current_;
    };

    class iterator
    {
        friend forward_list<T>;
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        iterator( node *ptr = nullptr ) noexcept 
        : current_( ptr ) 
        {  }

        reference operator*() const noexcept { 
            return current_->value_;
        }

        pointer operator->() const noexcept {
            return &( operator*() );
        }
        
        iterator &operator++() noexcept {
            current_ = (current_->next_).get();
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==( const iterator &other ) const noexcept {
            return current_ == other.current_;
        }
        
        bool operator!=( const iterator &other ) const noexcept {
            return current_ != other.current_;
        }

    private:
        node *current_;
    };

private:
    node_ptr head_ = make_unique<node>();     // dummy node
    size_type size_ = 0;

public:
    forward_list() = default;             
    
    explicit forward_list( size_type n, const value_type &value = T{} ) {
        for( size_type i = 0; i < n; ++i ) {
            push_front( value );
        }
    }
    
    template <class InputIterator>
    forward_list( InputIterator first, InputIterator last ) {
        auto new_list = copy_forward_list( first, last );
        head_->next_ = std::move( new_list );
    }
    
    forward_list( const forward_list &other ) {
        auto new_list = copy_forward_list( other.begin(), other.end() );
        head_->next_ = std::move( new_list );
    }

    forward_list( forward_list &&other ) noexcept = default ;


    forward_list( std::initializer_list<value_type> lst ) 
        : forward_list( lst.begin(), lst.end() ) 
    {  }
    
    // call head_'s destructor, all nodes' memory will be free 
    virtual ~forward_list() = default;

    // can handle the problem of self-assignment
    // Note: pass by value
    // see C++ Primer 5th section 13.3
    forward_list &operator=( forward_list other ) {
        using std::swap;
        swap( *this, other );
        return *this;
    }

    forward_list &operator=( forward_list &&other ) noexcept = default;

    forward_list &operator=( std::initializer_list<value_type> lst ) {
        size_ = 0;
        auto new_list = copy_forward_list( lst.begin(), lst.end() );
        head_->next_ = std::move( new_list );
        return *this;
    }
    
    void push_front( const T &value ) {
        auto ptr = make_unique<node>( value, std::move( head_->next_ ) );
        head_->next_ = std::move( ptr );
        ++size_;
    }

    void push_front( T &&value ) {
        auto ptr = make_unique<node>( std::move( value ), std::move( head_->next_ ) );
        head_->next_ = std::move( ptr );
        ++size_;
    }

    template <class... Args>
    void emplace_front( Args&&... args ) {
        T value( std::forward<Args>(args)... );
        auto ptr = make_unique<node>( std::move( value ), std::move( head_->next_ ) );
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
    
    template <class InputIterator>
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
        size_ = 0;
        head_->next_ = nullptr;
    }

    iterator erase_after( const_iterator position ) {
        node *ptr = position.current_;
        if( ptr == nullptr || ptr->next_ == nullptr ) {
            throw forward_list_exception( "forward_list::erase_after(): no element after that iterator" );   
        }
        ptr->next_ = std::move( ptr->next_->next_ );
        --size_;
        return { (ptr->next_).get() };
    }

    iterator erase_after( const_iterator position, const_iterator last ) {
        node *ptr = position.current_;
        if( ptr == nullptr || ptr->next_ == nullptr ) {
            throw forward_list_exception( "forward_list::erase_after(): no element after that iterator" );   
        }
        
        size_type removeNum = 0;
        for( auto iter = ++position; iter != last; ++iter ) {
            ++removeNum;
        }

        for( size_type i = 0; i < removeNum; ++i ) {
            ptr->next_ = std::move( ptr->next_->next_ );
            --size_;
        }

        return { (ptr->next_).get() };
    }

private:
    template <typename InputIterator>
    std::unique_ptr<node> copy_forward_list( InputIterator first, InputIterator last ) {
        if( first == last ) {
            return nullptr;
        }
        auto front = make_unique<node>( *first );
        auto rest = copy_forward_list( ++first, last );
        front->next_ = std::move( rest );
        ++size_;
        return front;
    }
};


template <typename T>
inline void swap( forward_list<T> &left, forward_list<T> &right ) noexcept {
    left.swap( right );
}

#endif /* _FORWARD_LIST_H_ */
