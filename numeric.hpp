#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include "iterator.hpp"

namespace mystl {

template <typename ForwardIterator, typename T>
inline void iota( ForwardIterator first, ForwardIterator last, T value )
{
    for( ; first != last; ++first )
    {
        *first = value;
        ++value;
    }
}

template <typename InputIterator, typename T>
inline T accumulate( InputIterator first, InputIterator last, T initValue )
{
    for( ; first != last; ++first )
    {
        initValue = initValue + *first;
    }

    return initValue;
}

template <typename InputIterator, typename T, typename BinaryFunction>
inline T accumulate( InputIterator first, InputIterator last, T initValue, BinaryFunction binaryFunc )
{
    for( ; first != last; ++first )
    {
        initValue = binaryFunc( initValue, *first );
    }
    return initValue;
}

template <typename InputIteratorL, typename InputIteratorR, typename T>
inline T inner_product( InputIteratorL firstL, InputIteratorL lastL, InputIteratorR firstR, T initValue )
{
    for( ; firstL != lastL; ++firstL, ++firstR )
    {
        initValue = initValue + ( *firstL ) * ( *firstR );
    }

    return initValue;
}

template <typename InputIteratorL, typename InputIteratorR, typename T, typename BinaryFunctionL, typename BinaryFunctionR>
inline T inner_product( InputIteratorL firstL, InputIteratorL lastL, InputIteratorR firstR, T initValue, 
                        BinaryFunctionL binaryFuncL, BinaryFunctionR binaryFuncR )
{
    for( ; firstL != lastL; ++firstL, ++firstR )
    {
        initValue = binaryFuncL( initValue, binaryFuncR( *firstL, *firstR ) );
    }

    return initValue;
}


template <typename InputIterator, typename OutputIterator>
OutputIterator partial_sum( InputIterator first, InputIterator last, OutputIterator dest )
{
    if( first == last )
    {
        return dest;
    }

    auto total = *first;
    *dest++ = total;
    
    while( ++first != last )
    {
        total = total + *first;
        *dest++ = total;
    }
    
    return dest;
}

template <typename InputIterator, typename OutputIterator, typename BinaryFunction>
OutputIterator partial_sum( InputIterator first, InputIterator last, OutputIterator dest, BinaryFunction binaryFunc )
{
    if( first == last )
    {
        return dest;
    }

    auto total = *first;
    *dest++ = total;
    
    while( ++first != last )
    {
        total = binaryFunc( total, *first );
        *dest++ = total;
    }
    
    return dest;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference( InputIterator first, InputIterator last, OutputIterator dest )
{
    if( first == last )
    {
        return dest;
    }

    auto prev = *first;
    *dest++ = prev;
    
    while( ++first != last )
    {
        auto curr = *first;
        *dest++ = curr - prev;
        prev = std::move( curr );
    }

    /**
       Note: because the iterator is input iterator, we can't write the code as following:
       
       while( ++first != last )
       {
           *dest++ = *first - prev;
           prev = *first;
       }
    **/
    
    return dest;
}

template <typename InputIterator, typename OutputIterator, typename BinaryFunction>
OutputIterator adjacent_difference( InputIterator first, InputIterator last, OutputIterator dest, BinaryFunction binaryFunc )
{
    if( first == last )
    {
        return dest;
    }

    auto prev = *first;
    *dest++ = prev;
    
    while( ++first != last )
    {
        auto curr = *first;
        *dest++ = binaryFunc( curr,  prev );
        prev = std::move( curr );
    }
    
    return dest;
}



};    // namespace mystl


#endif /* _NUMERIC_H_ */
