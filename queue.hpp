/***
    队列
        1. 引入异常，对于不合法的操作会抛出异常
        2. 使用 list 作为底层容器，而不使用 deque

    版本 1.0
    作者：詹春畅
    博客：senlinzhan.github.io
 ***/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "list.hpp"
#include <string>
#include <exception>

namespace mystl {


class queue_exception : public std::exception
{
public:
    explicit queue_exception( const std::string &message ) 
        : message_( message )
    {
    }
    
    virtual const char * what() const noexcept override 
    {
        return message_.c_str();
    }
    
private:
    std::string message_;
};


template <typename T, typename Container = mystl::list<T>>
class queue
{
public:
    using container_type   = Container;
    using value_type       = typename Container::value_type;
    using reference        = typename Container::reference;
    using const_reference  = typename Container::const_reference;
    using size_type        = typename Container::size_type;

protected:
    Container container_;

public:
    explicit queue( const Container &container ) 
        : container_( container )
    {
    }

    explicit queue( Container &&container = Container() ) 
        : container_( std::move( container ) )
    {    
    }

    ~queue() = default;

    bool empty() const
    {
        return container_.empty();
    }
    
    size_type size() const
    {
        return container_.size();
    }
    
    reference front() 
    {
        if( empty() ) 
        {
            throw queue_exception( "queue::front(): the queue is empty!" );
        }
        return container_.front();
    }

    
    const_reference front() const
    {
        if( empty() ) 
        {
            throw queue_exception( "queue::front(): the queue is empty!" );
        }
        return container_.front();        
    }

    reference back()
    {
        if( empty() ) 
        {
            throw queue_exception( "queue::front(): the queue is empty!" );
        }
        return container_.back();   
    }

    const_reference back() const
    {
        if( empty() ) 
        {
            throw queue_exception( "queue::front(): the queue is empty!" );
        }
        return container_.back();
    }

    void push( const value_type &value ) {
        auto copy = value;
        push( std::move( copy ) );
    }

    void push( value_type &&value ) {
        emplace( std::move( value ) );
    }

    template<typename... Args>
    void emplace( Args&&... args ) {
        container_.emplace_back( std::forward<Args>( args )... );
    }

    void pop()
    {
        if( empty() ) 
        {
            throw queue_exception( "queue::pop(): the queue is empty!" );
        }
        container_.pop_front();
    }
    
    void swap( queue &other ) noexcept( noexcept( swap( container_, other.container_ ) ) )
    {
        using std::swap;
        swap( container_, container_ );
    }

    bool operator==( const queue &other ) 
    {
        return container_ == other.container_;
    }

    bool operator!=( const queue &other ) 
    {
        return !( *this == other );
    }
    
    bool operator<( const queue &other ) 
    {
        return container_ < other.container_;
    }
    
    bool operator>=( const queue &other ) 
    {
        return !( *this < other );
    }

    bool operator>( const queue &other ) 
    {
        return other < *this;
    }
    
    bool operator<=( const queue &other ) 
    {
        return !( *this > other );
    }
};

template <typename T, typename Container>
inline void swap( queue<T, Container> &left, queue<T, Container> &right ) 
    noexcept( noexcept( left.swap( right ) ) )
{
    left.swap( right );
}


};    // namespace mystl

#endif /* _QUEUE_H_ */
