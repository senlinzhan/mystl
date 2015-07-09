#ifndef _NUMERIC_H_
#define _NUMERIC_H_

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
        initValue += *first;
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

template <typename InputIteratorL, typename InputIteratorR, typename T, 
          typename = mystl::RequireInputIterator<InputIteratorL>, typename = mystl::RequireInputIterator<InputIteratorR>>
inline T inner_product( InputIteratorL firstL, InputIteratorL lastL, InputIteratorR firstR, T initValue )
{
    for( ; firstL != lastL; ++firstL, ++firstR )
    {
        initValue += ( *firstL ) * ( *firstR );
    }

    return initValue;
}


template <typename InputIteratorL, typename InputIteratorR, typename T, 
          typename BinaryFunctionL, typename BinaryFunctionR,
          typename = mystl::RequireInputIterator<InputIteratorL>, typename = mystl::RequireInputIterator<InputIteratorR>>
inline T inner_product( InputIteratorL firstL, InputIteratorL lastL, InputIteratorR firstR, T initValue, 
                        BinaryFunctionL binaryFuncL, BinaryFunctionR binaryFuncR )
{
    for( ; firstL != lastL; ++firstL, ++firstR )
    {
        initValue = binaryFuncL( initValue, binaryFuncR( *firstL, *firstR ) );
    }

    return initValue;
}


};


#endif /* _NUMERIC_H_ */
