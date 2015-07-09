#ifndef _ITERATOR_H_
#define _ITERATOR_H_

#include <iterator>

namespace mystl {


/**
   define five iterator category
 **/
struct output_iterator_tag 
{ 
};
 
struct input_iterator_tag 
{
};

struct forward_iterator_tag : public input_iterator_tag 
{
};

struct bidirectional_iterator_tag : public forward_iterator_tag 
{
};

struct random_access_iterator_tag : public bidirectional_iterator_tag 
{
};


template <typename T>
struct iterator_traits 
{
    using iterator_category = typename T::iterator_category;
    using value_type        = typename T::value_type;
    using difference_type   = typename T::difference_type;
    using pointer           = typename T::pointer;
    using reference         = typename T::reference;
};

template <typename T>
struct iterator_traits<T*> 
{
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

template <typename T>
struct iterator_traits<const T*> 
{
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;
};

template <typename Category, typename T, typename Distance = std::ptrdiff_t, 
          typename Pointer = T*, typename Reference = T&>
struct iterator
{
    using iterator_category = Category;
    using value_type        = T;
    using difference_type   = Distance;
    using pointer           = Pointer;
    using reference         = Reference;
};

template <typename Iterator>
class reverse_iterator : public iterator<typename iterator_traits<Iterator>::iterator_category,
                                         typename iterator_traits<Iterator>::value_type,
                                         typename iterator_traits<Iterator>::difference_type,
                                         typename iterator_traits<Iterator>::pointer,
                                         typename iterator_traits<Iterator>::reference>
{
    using difference_type   = typename iterator_traits<Iterator>::difference_type;
    using pointer           = typename iterator_traits<Iterator>::pointer;
    using reference         = typename iterator_traits<Iterator>::reference;
    
protected:
    Iterator current_;
    
public:
    using iterator_type = Iterator;
    
    reverse_iterator() = default;

    explicit reverse_iterator( iterator_type iter )
        : current_( iter ) 
    {
    }

    reverse_iterator( const reverse_iterator &other ) 
        : current_( other.current_ )    
    {
    }

    /**
       a reverse_iterator across other types can be copied if the underlying iterator can be converted to the type of current_.
     **/
    template <typename Iter>
    reverse_iterator( const reverse_iterator<Iter> &other )
        : current_( other.base() )
    {
    }

    iterator_type base() const 
    {
        return current_;
    } 

    reference operator*() const 
    {
        auto tmp = current_;
        return *--tmp;
    }

    pointer operator->() const 
    {
        return &( operator*() );
    }

    reverse_iterator &operator++()
    {
        --current_;
        return *this;
    } 

    reverse_iterator operator++(int)
    {
        auto tmp = *this;
        --current_;
        return tmp;
    }

    reverse_iterator &operator--()
    {
        ++current_;
        return *this;
    }

    reverse_iterator operator--(int)
    {
        auto tmp = *this;
        ++current_;
        return tmp;
    }

    reverse_iterator operator+( difference_type diff ) const
    {
        return reverse_iterator( current_ - diff ); 
    }

    reverse_iterator& operator+=( difference_type diff )
    {
        current_ -= diff;
        return *this;
    }

    reverse_iterator operator-( difference_type diff ) const
    { 
        return reverse_iterator( current_ + diff ); 
    }

    reverse_iterator& operator-=( difference_type diff )
    {
        current_ += diff;
        return *this;
    }

    reference operator[]( difference_type diff ) const
    {
        return *( *this + diff ); 
    }
};


template<typename Iterator>
inline bool operator==( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return left.base() == right.base();
}

template<typename Iterator>
inline bool operator<( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return right.base() < left.base();
}

template<typename Iterator>
inline bool operator!=( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return !( left == right );
}

template<typename Iterator>
inline bool operator>( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return right < left;
}


template<typename Iterator>
inline bool operator<=( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return !( right < left );
}

template<typename Iterator>
inline bool operator>=( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return !( left < right );
}

template<typename Iterator>
inline typename reverse_iterator<Iterator>::difference_type 
operator-( const reverse_iterator<Iterator> &left, const reverse_iterator<Iterator> &right )
{
    return right.base() - left.base();
}

template<typename Iterator>
inline reverse_iterator<Iterator>
operator+( typename reverse_iterator<Iterator>::difference_type diff, const reverse_iterator<Iterator> &iter )
{ 
    return reverse_iterator<Iterator>( iter.base() - diff ); 
}


template<typename IteratorL, typename IteratorR>
inline bool operator==(const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{ 
    return left.base() == right.base();
}

template<typename IteratorL, typename IteratorR>
inline bool operator<( const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{
    return right.base() < left.base();
}

template<typename IteratorL, typename IteratorR>
inline bool operator!=(const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{ 
    return !( left == right );
}

template<typename IteratorL, typename IteratorR>
inline bool operator>(const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{ 
    return right < left;
}

template<typename IteratorL, typename IteratorR>
inline bool operator<=(const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{ 
    return !( right < left );
}

template<typename IteratorL, typename IteratorR>
inline bool operator>=(const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right )
{ 
    return !( left < right );
}

template<typename IteratorL, typename IteratorR>
inline auto operator-( const reverse_iterator<IteratorL> &left, const reverse_iterator<IteratorR> &right ) -> decltype( right.base() - left.base() )
{
    return right.base() - left.base();
}

template<typename Iterator>
inline reverse_iterator<Iterator> make_reverse_iterator( Iterator iter )
{ 
    return reverse_iterator<Iterator>( iter ); 
}

template <typename OutputIterator>
using RequireOutputIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<OutputIterator>::iterator_category, 
                        std::output_iterator_tag
                        >::value
    >::type;

template <typename InputIterator>
using RequireInputIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, 
                        std::input_iterator_tag
                        >::value
    >::type;

template <typename ForwardIterator>
using RequireForwardIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<ForwardIterator>::iterator_category, 
                        std::forward_iterator_tag
                        >::value
    >::type;

template <typename BidirectionalIterator>
using RequireBidirectionalIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<BidirectionalIterator>::iterator_category, 
                        std::bidirectional_iterator_tag
                        >::value
    >::type;

template <typename RandomIterator>
using RequireRandomIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<RandomIterator>::iterator_category, 
                        std::random_access_iterator_tag
                        >::value
    >::type;

};    // namespace mystl

#endif /* _ITERATOR_H_ */
