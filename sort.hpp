#ifndef _SORT_H_
#define _SORT_H_

#include "iterator.hpp"
#include <functional>


namespace mystl {


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

/**
   Bubble sort base on array
 **/
template <typename T>
void bubble_sort( T arr[], int size )
{
    using std::swap;
    
    if( size < 2 )
    {
        return;
    }
    
    for( int i = 0; i < size; ++i )
    {
        for( int j = 1; j < size - i; ++j )
        {
            if( arr[j] < arr[j - 1] )
            {
                swap( arr[j], arr[j - 1] );
            }
        }
    }
}

/**
   Bubble sort in STL style using the user defined comparator
   the iterator require at least be forward iterator because we must traverse the container more than once
 **/
template <typename ForwardIterator, typename Comp, typename = mystl::RequireForwardIterator<ForwardIterator>>
void bubble_sort( ForwardIterator first, ForwardIterator last, Comp comp )
{
    using std::swap;
    
    if( first == last )
    {
        return;
    }

    while( last != first )
    {
        auto curr = first;
        auto next = first;
        
        for( ++next; next != last; ++next )
        {
            if( comp( *next, *curr ) )
            {
                swap( *next, *curr );
            }
            curr = next;
        }
        last = curr;
    }
}

/**
   Bubble sort in STL style using the default comparator
   the iterator require at least be forward iterator because we must traverse the container more than once
 **/
template <typename ForwardIterator, typename = mystl::RequireForwardIterator<ForwardIterator>>
void bubble_sort( ForwardIterator first, ForwardIterator last )
{
    using value_type = typename mystl::iterator_traits<ForwardIterator>::value_type;
    bubble_sort( first, last, std::less<value_type>() );
}

/**
   Merge two sorted array
 **/
template <typename T>
void merge( T arr[], T l[], int lsize, T r[], int rsize )
{
    int size = lsize + rsize;

    for( int i = 0, j = 0, k = 0; k < size; ++k )
    {
        if( i == lsize )
        {
            arr[k] = std::move( r[j++] );
        } 
        else if( j == rsize )
        {
            arr[k] = std::move( l[i++] );
        }
        else if( r[j] < l[i] )
        {
            arr[k] = std::move( r[j++] );
        }
        else 
        {
            arr[k] = std::move( r[i++] );
        }
    }
}

/**
   Merge sort base on array
 **/
template <typename T>
void merge_sort( T arr[], int lo, int hi )
{
    // we must sure range [lo, hi] is valid
    if( hi <= lo )
    {
        return;
    }

    int mid = lo + ( hi - lo ) / 2;
    
    auto left  = new T[mid - lo + 1];
    auto right = new T[hi - mid];
    
    int cnt = 0;
    for( int i = lo; i <= mid; ++i )
    {
        left[cnt++] = std::move( arr[i] );
    }

    cnt = 0;
    for( int i = mid + 1; i <= hi; ++i )
    {
        right[cnt++] = std::move( arr[i] );
    }

    merge_sort( left, 0, mid - lo );
    merge_sort( right, 0, hi - mid - 1 );
    
    merge( arr, left, mid - lo + 1, right, hi - mid );

    delete[] left;
    delete[] right;
}

/**
   Merge sort base on array
 **/
template <typename T>
void merge_sort( T arr[], int size )
{
    merge_sort( arr, 0, size - 1 );
}


};    // namespace mystl


#endif /* _SORT_H_ */
