#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "heap.hpp"
#include "vector.hpp"
#include <exception>
#include <string>

namespace mystl {


class priority_queue_exception : public std::exception
{
public:
    explicit priority_queue_exception( const std::string &message ) 
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


template <typename T, typename Container = mystl::vector<T>, typename Comp = std::less<T>>
class priority_queue
{
public:
    using container_type   = Container;
    using value_type       = typename Container::value_type;
    using reference        = typename Container::reference;
    using const_reference  = typename Container::const_reference;
    using size_type        = typename Container::size_type;

protected:
    Comp      comp_;                                // for compare elements 
    Container container_;                           // the underlying container

public:
    explicit priority_queue( const Comp &comp = Comp{}, const Container &c = Container{} )
        : comp_( comp ), 
          container_( c ) 
    {
        mystl::make_heap( container_.begin(), container_.end(), comp_ );
    }
    
    explicit priority_queue( const Comp &comp, Container &&container ) 
        : comp_( comp ), 
          container_( std::move( container ) ) 
    {
        mystl::make_heap( container_.begin(), container_.end(), comp_ );
    }
    
    template <typename InputIterator>
    priority_queue( InputIterator beg, InputIterator end, const Comp &comp = Comp(), const Container &container = Container() )
        : comp_( comp ), 
          container_( container ) 
    {
        container_.insert( container_.end(), beg, end );
        mystl::make_heap( container_.begin(), container_.end(), comp_ );
    }

    template <typename InputIterator>
    priority_queue( InputIterator beg, InputIterator end, const Comp &comp, Container &&container )
        : comp_( comp ), 
          container_( std::move( container ) ) 
    {
        container_.insert( container_.end(), beg, end );
        mystl::make_heap( container_.begin(), container_.end(), comp_ );
    }

    ~priority_queue() = default;
    
    bool empty() const 
    {
        return container_.empty();
    }
    
    size_type size() const 
    {
        return container_.size();
    }
    
    /*   
        priority_queue don't provide reference top();
        because it don't want user change elemtn by reference
    */
    const_reference top() const 
    {
        if( empty() )
        {
            throw priority_queue_exception( "priority_queue::top(): the container is empty!" );
        }
        return container_.front();
    }
 

    void pop()
    {
        if( empty() )
        {
            throw priority_queue_exception( "priority_queue::pop(): the container is empty!" );
        }
        mystl::pop_heap( container_.begin(), container_.end(), comp_ );
        container_.pop_back();
    }

    void push( const value_type &elem ) 
    {
        auto copy = elem;
        push( std::move( copy ) );
    }

    void push( value_type &&elem ) 
    {
        emplace( std::move( elem ) );
    }
    
    template <typename... Args>
    void emplace( Args&&... args ) 
    {
        container_.emplace_back( std::forward<Args>( args )... );
        mystl::push_heap( container_.begin(), container_.end(), comp_ );
    }
    
    void swap( priority_queue &other ) 
        noexcept( noexcept( swap( container_, other.container_ ) ) && noexcept( swap( comp_, other.comp_ ) ) ) 
    {
        using std::swap;
        swap( container_, other.container_ );
        swap( comp_, other.comp_ );
    }
};

template <typename T, typename Container, typename Comp>
void swap( priority_queue<T, Container, Comp> &x, priority_queue<T, Container, Comp> &y )
    noexcept( noexcept( x.swap( y ) ) ) 
{
    x.swap( y );
}

};    // namespace mystl


#endif /* _PRIORITY_QUEUE_H_ */
