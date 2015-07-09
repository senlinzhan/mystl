#ifndef _SORT_H_
#define _SORT_H_

#include "iterator.hpp"
#include <functional>

/**
   Selection sort base on array
 **/
template <typename T>
void selection_sort( T arr[], int size ) 
{
    using std::swap;
    
    if( size < 2 ) 
    {
        return;
    }
    
    for( int i = 0; i < size; ++i ) 
    {
        int min = i;
        for( int j = i + 1; j < size; ++j ) 
        {
            if( arr[j] < arr[min] ) 
            {
                min = j;
            }
        }
        if( min != i ) 
        {
            swap( arr[i], arr[min] );   
        }
    }
}

/**
   Selection sort in STL style using the user defined comparator
   the iterator require at least be forward iterator because we must traverse the container more than once
 **/
template <typename ForwardIterator, typename Comp, typename = mystl::RequireForwardIterator<ForwardIterator>>
void selection_sort( ForwardIterator first, ForwardIterator last, Comp comp )
{
    using std::swap;
    if( first == last ) 
    {
        return;
    }    

    for( ; first != last; ++first ) 
    {
        auto min = first;
        auto cur = first;
        for( ++cur; cur != last; ++cur ) 
        {
            if( comp( *cur, *min ) )
            {
                min = cur;
            }
        }
        if( min != first )
        {
            swap( *min, *first );
        }
    }
}

/**
   Selection sort in STL style using the default comparator
   the iterator require at least be forward iterator because we must traverse the container more than once
 **/
template <typename ForwardIterator, typename = mystl::RequireForwardIterator<ForwardIterator>>
void selection_sort( ForwardIterator first, ForwardIterator last ) 
{
    using value_type = typename mystl::iterator_traits<ForwardIterator>::value_type;
    selection_sort( first, last, std::less<value_type>() );
}




#endif /* _SORT_H_ */
