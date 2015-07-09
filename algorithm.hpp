#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <functional>
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>

namespace mystl {

template<typename InputIterator>
inline typename std::iterator_traits<InputIterator>::difference_type
distance( InputIterator first, InputIterator last ) 
{
    return mystl::distance( first, last, 
                            typename std::iterator_traits<InputIterator>::iterator_category() );
}

template<typename RandomAccessIterator>
inline typename std::iterator_traits<RandomAccessIterator>::difference_type
distance( RandomAccessIterator first, RandomAccessIterator last, std::random_access_iterator_tag )
{
    return last - first;
}

template<typename InputIterator>
inline typename std::iterator_traits<InputIterator>::difference_type
distance( InputIterator first,  InputIterator last, std::input_iterator_tag )
{
    typename std::iterator_traits<InputIterator>::difference_type n = 0;
    while( first != last )
    {
        ++first;
        ++n;
    }
    return n;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy( InputIterator beg, InputIterator end, OutputIterator desBeg ) 
{
    while( beg != end ) 
    {
        *desBeg++ = *beg++;
    }
    return desBeg;
}

template <typename InputIterator, typename T>
InputIterator find( InputIterator beg, InputIterator end, const T &value ) 
{
    for( ; beg != end && *beg != value; ++beg )
    {        
    }
    return beg;
}

template <typename InputIterator, typename UnaryPredicate>
InputIterator find_if( InputIterator beg, InputIterator end, UnaryPredicate predicate ) 
{
    for( ; beg != end && !predicate( *beg ); ++beg )
    {    
    }
    return beg;
}

template <typename InputIterator, typename UnaryPredicate>
InputIterator find_if_not( InputIterator beg, InputIterator end, UnaryPredicate predicate ) 
{
    for( ; beg != end && predicate( *beg ); ++beg )
    {   
    }
    return beg;
}

template <typename ForwardIterator, typename Comp>
ForwardIterator min_element( ForwardIterator beg, ForwardIterator end, Comp comp ) 
{
    if( beg == end ) 
    {
        return beg;
    }
    auto min = beg;
    while( ++beg != end ) 
    {
        if( comp( *beg, *min ) ) 
        {
            min = beg;
        }
    }
    return min;
}

template <typename ForwardIterator>
ForwardIterator min_element( ForwardIterator beg, ForwardIterator end ) 
{
    return min_element( beg, end, std::less<decltype(*beg)>() );
}

template <typename ForwardIterator, typename Comp>
ForwardIterator max_element( ForwardIterator beg, ForwardIterator end, Comp comp ) 
{
    if( beg == end ) 
    {
        return beg;
    }
    auto max = beg;
    while( ++beg != end ) 
    {
        if( comp( *max, *beg ) ) 
        {
            max = beg;
        }
    }
    return max;
}

template <typename ForwardIterator>
ForwardIterator max_element( ForwardIterator beg, ForwardIterator end ) 
{
    return max_element( beg, end, std::less<decltype(*beg)>() );
}

template <typename ForwardIterator, typename Comp>
std::pair<ForwardIterator, ForwardIterator>
minmax_element( ForwardIterator beg, ForwardIterator end, Comp comp ) 
{
    if( beg == end ) 
    {
        return { beg, beg };
    }
    auto min = beg, max = beg;
    while( ++beg != end ) 
    {
        if( comp( *beg, *min ) ) 
        {
            min = beg;
        }
        if( comp( *max, *beg ) ) 
        {
            max = beg;
        }
    }
    return { min, max };
}

template <typename ForwardIterator>
std::pair<ForwardIterator, ForwardIterator>
minmax_element( ForwardIterator beg, ForwardIterator end ) 
{
    return minmax_element( beg, end, std::less<decltype(*beg)>() );
}


template <typename InputIterator1, typename InputIterator2>
bool equal( InputIterator1 beg1, InputIterator1 end1, InputIterator2 beg2 ) 
{
    while( beg1 != end1 ) 
    {
        if( *beg1++ != *beg2++ ) 
        {
            return false;
        }
    }
    return true;
}

template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
bool equal( InputIterator1 beg1, InputIterator1 end1, InputIterator2 beg2, BinaryPredicate predicate ) 
{
    while( beg1 != end1 ) 
    {
        if( !predicate( *beg1++, *beg2++ ) ) 
        {
            return false;
        }
    }
    return true;
}

template <typename InputIterator, typename T>
auto count( InputIterator beg, InputIterator end, const T &value ) -> decltype( beg - beg )
{
    auto num = beg - beg;
    while( beg != end ) 
    {
        if( *beg++ == value ) 
        {
            ++num;
        }  
    }
    return num;
} 


template <typename InputIterator, typename UnaryPredicate>
auto count_if( InputIterator beg, InputIterator end, UnaryPredicate predicate ) -> decltype( beg - beg )
{
    auto num = beg - beg;
    while( beg != end ) 
    {
        if( predicate( *beg++ ) ) 
        {
            ++num;
        }
    }
    return num;
}


template<typename ForwardIterator1, typename ForwardIterator2>
bool is_permutation( ForwardIterator1 beg1, ForwardIterator1 end1, ForwardIterator2 beg2 ) 
{
    for (; beg1 != end1; ++beg1, ++beg2 ) 
    {
        if(  *beg1 != *beg2 )         
        {
            break;            
        }
    }
    if( beg1 == end1 ) 
    {
        return true;
    }

    auto end2 = beg2;
    std::advance( end2, std::distance( beg1, end1 ) );
    for( auto scan = beg1; scan != end1; ++scan ) 
    {
        if( scan != mystl::find( beg1, scan, *scan ) ) 
        {
            continue; // We've seen this one before.          
        }

        auto matches = mystl::count( beg2, end2, *scan );
        if( matches == 0 || mystl::count( scan, end1, *scan ) != matches ) 
        {
            return false;            
        }
	}
    return true;
}

template<typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
bool is_permutation( ForwardIterator1 beg1, ForwardIterator1 end1, ForwardIterator2 beg2, BinaryPredicate predicate ) 
{
    for( ; beg1 != end1; ++beg1, ++beg2 ) 
    {
        if( !bool( predicate( *beg1, *beg2 ) ) ) 
        {
            break;
        }
    }

    if( beg1 == end1 ) 
    {
        return true;
    }
    auto end2 = beg2;
    std::advance( end2, std::distance( beg1, end1 ) );
    for( auto scan = beg1; scan != end1; ++scan ) 
    {
        using std::placeholders::_1;

        if( scan != mystl::find_if( beg1, scan, std::bind( predicate, _1, *scan ) ) ) 
        {
            continue;             
        }
        auto matches = mystl::count_if( beg2, end2, std::bind( predicate, _1, *scan ) );
        if( matches == 0 || matches != mystl::count_if( scan, end1, std::bind( predicate, _1, *scan ) ) ) 
        {
            return false;            
        }

	}
    return true;
}

template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2>
mismatch( InputIterator1 beg1, InputIterator1 end1, InputIterator2 beg2 ) 
{
    for( ; beg1 != end1; ++beg1, ++beg2 ) 
    {
        if( *beg1 != *beg2 ) 
        {
            break;
        }
    }
    return { beg1, beg2 };
}

template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
std::pair<InputIterator1, InputIterator2>
mismatch( InputIterator1 beg1, InputIterator1 end1, InputIterator2 beg2, BinaryPredicate predicate ) 
{
    for( ; beg1 != end1; ++beg1, ++beg2 ) 
    {
        if( !predicate( *beg1, *beg2 ) ) 
        {
            break;
        }
    }
    return { beg1, beg2 };   
}

template <typename ForwardIterator, typename T>
ForwardIterator remove( ForwardIterator beg, ForwardIterator end, const T &elem ) 
{
    auto pos = mystl::find( beg, end, elem );
    if( pos == end ) 
    {                          
        return end;
    }
    auto newEnd = pos;
    for( ++pos; pos != end; ++pos ) 
    {
        if( *pos != elem ) 
        {
            *newEnd++ = std::move( *pos ) ;
        }
    }
    return newEnd;
}
     
template <typename ForwardIterator, typename UnaryPredicate>
ForwardIterator remove_if( ForwardIterator beg, ForwardIterator end, UnaryPredicate predicate ) 
{
    auto pos = mystl::find_if( beg, end, predicate );
    if( pos == end ) 
    {
        return end;
    }
    auto newEnd = pos;
    for( ++pos; pos != end; ++pos ) 
    {
        if( !predicate( *pos ) ) 
        {
            *newEnd++ = std::move( *pos );
        }
    }
    return newEnd;
}
    
template <typename InputIterator, typename OutputIterator, typename T>
OutputIterator remove_copy( InputIterator srcBeg, InputIterator srcEnd, OutputIterator desBeg, const T &elem ) 
{
    while( srcBeg != srcEnd ) 
    {
        auto copy = *srcBeg++;
        if( copy != elem ) 
        {
            *desBeg++ = copy;
        } 
    }
    return desBeg;
}

template <typename InputIterator, typename OutputIterator, typename UnaryPredicate>
OutputIterator remove_copy_if( InputIterator srcBeg, InputIterator srcEnd, OutputIterator desBeg, UnaryPredicate predicate ) 
{
    while( srcBeg != srcEnd ) 
    {
        auto copy = *srcBeg++;
        if( !predicate( copy ) ) 
        {
            *desBeg++ = copy;
        } 
    }
    return desBeg;
}

  

template <typename ForwardIterator, typename BinaryPredicate>
ForwardIterator adjacent_find( ForwardIterator beg, ForwardIterator end, BinaryPredicate predicate ) 
{
    if( beg == end ) 
    {
        return end;
    }
    auto previous = beg;
    while( ++beg != end ) 
    {
        if( predicate( *previous, *beg ) ) 
        {
            return previous;
        }
        ++previous;
    }
    return end;
}

template <typename ForwardIterator>
ForwardIterator adjacent_find( ForwardIterator beg, ForwardIterator end ) 
{
    return adjacent_find( beg, end, std::equal_to<decltype(*beg)>{} );
}

template <typename ForwardIterator, typename BinaryPredicate>
ForwardIterator unique( ForwardIterator beg, ForwardIterator end, BinaryPredicate predicate ) 
{
    beg = mystl::adjacent_find( beg, end, predicate );
    if( beg == end ) 
    {
        return end;
    }
    
    auto newEnd = beg++;
    while( ++beg != end ) 
    {
        if( !predicate( *newEnd, *beg ) ) 
        {
            *(++newEnd) = std::move( *beg );
        }
    }
    return ++newEnd;
}
    
template <typename ForwardIterator>
ForwardIterator unique( ForwardIterator beg, ForwardIterator end ) 
{
    return mystl::unique( beg, end, std::equal_to<decltype(*beg)>() );
}
    
template <typename InputIterator, typename OutputIterator, typename BinaryPredicate>
OutputIterator unique_copy( InputIterator srcBeg, InputIterator srcEnd, OutputIterator desBeg, BinaryPredicate predicate ) 
{
    if( srcBeg == srcEnd ) 
    {
        return desBeg;
    }
    auto lastOutputElem = *srcBeg++;
    *desBeg++ = lastOutputElem;
    while( srcBeg != srcEnd ) 
    {
        auto currentElem = *srcBeg++;
        if( !predicate( lastOutputElem, currentElem ) ) 
        {
            *desBeg++ = currentElem;
            lastOutputElem = std::move( currentElem );
        }
    } 
    return desBeg;
} 

template <typename InputIterator, typename OutputIterator>
OutputIterator unique_copy( InputIterator srcBeg, InputIterator srcEnd, OutputIterator desBeg ) 
{
    return mystl::unique_copy( srcBeg, srcEnd, desBeg, std::equal_to<decltype(*srcBeg)>{} );
}


template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 search( ForwardIterator1 beg, ForwardIterator1 end, 
                         ForwardIterator2 searchBeg, ForwardIterator2 searchEnd ) 
{
    if( searchBeg == searchEnd ) 
    {
        return beg;
    }
    auto start = beg;
    while( ( start = mystl::find( start, end, *searchBeg ) ) != end ) 
    {
        auto i = start;
        auto j = searchBeg;
        while( ++j != searchEnd && ++i != end && *i == *j )
            ;
        if( j == searchEnd ) {
            return start;
        }
        ++start;
    }
    return end; 
}

template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 search( ForwardIterator1 beg, ForwardIterator1 end, ForwardIterator2 searchBeg, ForwardIterator2 searchEnd,
                         BinaryPredicate predicate ) 
{
    using std::placeholders::_1;
    if( searchBeg == searchEnd ) 
    {
        return beg;
    }
    auto start = beg;
    while( ( start = mystl::find( start, end, std::bind( predicate, _1, *searchBeg ) ) ) != end )
    {
        auto i = start;
        auto j = searchBeg;
        while( ++j != searchEnd && ++i != end && predicate( *i, *j ) )
        {
        }

        if( j == searchEnd ) 
        {
            return start;
        }
        ++start;
    }
    return end;
}

template <typename InputIterator, typename ForwardIterator>
InputIterator find_first_of( InputIterator beg, InputIterator end, ForwardIterator searchBeg, ForwardIterator searchEnd ) 
{
    for( ; beg != end; ++beg ) 
    {
        for( auto iter = searchBeg; iter != searchEnd; ++iter ) 
        {
            if( *beg == *iter ) 
            {
                return beg;
            }
        }
    }
    return end;
}

template <typename InputIterator, typename ForwardIterator, typename BinaryPredicate>
InputIterator find_first_of( InputIterator beg, InputIterator end, ForwardIterator searchBeg, ForwardIterator searchEnd,
                             BinaryPredicate predicate ) 
{
    for( ; beg != end; ++beg ) 
    {
        for( auto iter = searchBeg; iter != searchEnd; ++iter ) 
        {
            if( predicate( *beg, *iter ) ) 
            {
                return beg;
            }
        }
    }
    return end;
}

template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 find_end( ForwardIterator1 beg, ForwardIterator1 end, ForwardIterator2 searchBeg, ForwardIterator2 searchEnd ) 
{
    auto pos = mystl::search( beg, end, searchBeg, searchEnd );
    if( pos == end ) 
    {
        return end;
    }
    auto nextPos = pos;
    while( ( nextPos = mystl::search( nextPos, end, searchBeg, searchEnd ) ) != end ) 
    {
        pos = nextPos;
        ++nextPos;
    } 
    return pos;
}
    
template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 find_end( ForwardIterator1 beg, ForwardIterator1 end, ForwardIterator2 searchBeg, ForwardIterator2 searchEnd,
                           BinaryPredicate predicate ) 
{
    auto pos = mystl::search( beg, end, searchBeg, searchEnd, predicate );
    if( pos == end ) 
    {
        return end;
    } 
    auto nextPos = pos;
    while( ( nextPos = mystl::search( nextPos, end, searchBeg, searchEnd, predicate ) ) != end ) 
    {
        pos = nextPos;
        ++nextPos;
    }
    return pos;
}


template <typename ForwardIterator, typename T>
bool binary_search( ForwardIterator beg, ForwardIterator end, const T &elem ) 
{
    if( beg == end ) 
    {
        return false;
    }
    auto size = std::distance( beg, end );
    auto middle = beg;
    std::advance( middle, size / 2 );

    if( elem < *middle ) 
    {
        return mystl::binary_search( beg, middle, elem );
    } 
    else if( *middle < elem ) 
    {
        return mystl::binary_search( ++middle, end, elem );
    } 
    else 
    {
        return true;
    }
}

template <typename ForwardIterator, typename T, typename Comp>
bool binary_search( ForwardIterator beg, ForwardIterator end, const T &elem, Comp comp ) 
{
    if( beg == end ) 
    {
        return false;
    }
    
    auto size = std::distance( beg, end );
    auto middle = beg;
    std::advance( middle, size / 2 );
    
    if( comp( elem, *middle ) ) 
    {
        return mystl::binary_search( beg, middle, elem, comp );
    } 
    else if( comp( *middle, elem ) ) 
    {
        return mystl::binary_search( ++middle, end, elem, comp );
    } 
    else 
    {
        return true;
    }
}
    
template <typename InputIterator1, typename InputIterator2>
bool includes( InputIterator1 beg, InputIterator1 end, InputIterator2 searchBeg, InputIterator2 searchEnd ) 
{
    if( searchBeg == searchEnd ) 
    {
        return true;
    }
    while( ( beg = mystl::find( beg, end, *searchBeg ) ) != end ) 
    {
        if( ++searchBeg == searchEnd ) 
        {
            return true;
        }
        ++beg;
    }
    return false;
}
    
template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
bool includes( InputIterator1 beg, InputIterator1 end, InputIterator2 searchBeg, InputIterator2 searchEnd,
               BinaryPredicate predicate ) 
{
    using std::placeholders::_1;
    if( searchBeg == searchEnd ) 
    {
        return true;
    }
    while( ( beg = mystl::find_if( beg, end, std::bind( predicate, _1, *searchBeg ) ) ) != end ) 
    {
        if( ++searchBeg == searchEnd ) 
        {
            return true;
        }
        ++beg;
    }
    return false;
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Comp>
OutputIterator set_union( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2,
                          OutputIterator desBeg, Comp comp ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( comp( *srcBeg1, *srcBeg2 ) ) 
        {
            *desBeg++ = *srcBeg1++;
        } 
        else if( comp( *srcBeg2, *srcBeg1 ) ) 
        {
            *desBeg++ = *srcBeg2++;
        } 
        else 
        {
            *desBeg++ = *srcBeg1++;
            ++srcBeg2;
        }
    }
    if( srcBeg1 != srcEnd1 ) 
    {
        return mystl::copy( srcBeg1, srcEnd1, desBeg );
    } 
    else 
    {
        return mystl::copy( srcBeg2, srcEnd2, desBeg );
    }
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_union( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2,
                          OutputIterator desBeg ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( *srcBeg1 < *srcBeg2 ) 
        {
            *desBeg++ = *srcBeg1++;
        } 
        else if( *srcBeg2 < *srcBeg1 ) 
        {
            *desBeg++ = *srcBeg2++; 
        } 
        else 
        {
            *desBeg++ = *srcBeg1++;
            ++srcBeg2;
        }
    }
    if( srcBeg1 != srcEnd1 ) 
    {
        return mystl::copy( srcBeg1, srcEnd1, desBeg );
    } 
    else 
    {
        return mystl::copy( srcBeg2, srcEnd2, desBeg );
    }
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Comp>
OutputIterator set_difference( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2, 
                               OutputIterator desBeg, Comp comp ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( comp( *srcBeg1, *srcBeg2 ) ) 
        {
            *desBeg++ = *srcBeg1++;
        } 
        else if( comp( *srcBeg2, *srcBeg1 ) ) 
        {
            ++srcBeg2;
        } 
        else 
        {
            ++srcBeg1;
            ++srcBeg2;
        }
    }
    return mystl::copy( srcBeg1, srcEnd1, desBeg );
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_difference( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2, 
                               OutputIterator desBeg ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( *srcBeg1 < *srcBeg2 ) 
        {
            *desBeg++ = *srcBeg1++;
        } 
        else if( *srcBeg2 < *srcBeg1 ) 
        {
            ++srcBeg2;
        } 
        else 
        {
            ++srcBeg1;
            ++srcBeg2;
        }
    }
    return mystl::copy( srcBeg1, srcEnd1, desBeg );
}


template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Comp>
OutputIterator set_intersection( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2,
                                 OutputIterator desBeg, Comp comp ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( comp( *srcBeg1, *srcBeg2 ) ) 
        {
            ++srcBeg1;
        }
        else if( comp( *srcBeg2, *srcBeg1 ) ) 
        {
            ++srcBeg2;
        } 
        else 
        {
            *desBeg++ = *srcBeg1++;
            ++srcBeg2;
        }
    }
    return desBeg;
}


template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_intersection( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2,
                                 OutputIterator desBeg ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( *srcBeg1 < *srcBeg2 ) 
        {
            ++srcBeg1;
        } 
        else if( *srcBeg2 < *srcBeg1 ) 
        {
            ++srcBeg2;
        } 
        else 
        {
            *desBeg++ = *srcBeg1++;
            ++srcBeg2;
        }
    }
    return desBeg;
}

template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator set_symmetric_difference( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2, 
                                         OutputIterator desBeg ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( *srcBeg1 < *srcBeg2 ) 
        {
            *desBeg++ = *srcBeg1++;
        }
        else if( *srcBeg2 < *srcBeg1 ) 
        {
            *desBeg++ = *srcBeg2++;
        } 
        else 
        {
            ++srcBeg1;
            ++srcBeg2;
        }
    }
    if( srcBeg1 != srcEnd1 ) 
    {
        return mystl::copy( srcBeg1, srcEnd1, desBeg );
    } 
    else 
    {
        return mystl::copy( srcBeg2, srcEnd2, desBeg );
    }
}

template <typename InputIterator1, typename InputIterator2, 
          typename OutputIterator, typename Comp>
OutputIterator set_symmetric_difference( InputIterator1 srcBeg1, InputIterator1 srcEnd1, InputIterator2 srcBeg2, InputIterator2 srcEnd2,
                                         OutputIterator desBeg, Comp comp ) 
{
    while( srcBeg1 != srcEnd1 && srcBeg2 != srcEnd2 ) 
    {
        if( comp( *srcBeg1, *srcBeg2 ) ) 
        {
            *desBeg++ = *srcBeg1++;
        } 
        else if( comp( *srcBeg2, *srcBeg1 ) ) 
        {
            *desBeg++ = *srcBeg2++;
        } 
        else 
        {
            ++srcBeg1;
            ++srcBeg2;
        }
    }
    if( srcBeg1 != srcEnd1 ) 
    {
        return mystl::copy( srcBeg1, srcEnd1, desBeg );
    }
    else 
    {
        return mystl::copy( srcBeg2, srcEnd2, desBeg );
    }
}

template <typename BidirectionalIterator>
void reverse( BidirectionalIterator beg, BidirectionalIterator end ) 
{
    using std::swap; 
    for( auto size = std::distance( beg, end ); size >= 2; size -= 2 ) 
    {
        swap( *beg++, *--end );            
    }
}

template <typename BidirectionalIterator, typename OutputIterator>
OutputIterator reverse_copy( BidirectionalIterator beg, BidirectionalIterator end, OutputIterator desBeg ) 
{
    while( end != beg ) 
    {
        *desBeg++ = *--end;
    }
    return desBeg;
}

template <typename ForwardIterator, typename OutputIterator>
OutputIterator rotate_copy( ForwardIterator beg, ForwardIterator newBeg, ForwardIterator end, OutputIterator desBeg ) 
{
    if( beg == end ) 
    {
        return desBeg;
    }
    auto current = newBeg;

    do {
        *desBeg++ = *current++;
        if( current == end ) 
        {
            current = beg;
        }
    } while( current != newBeg );
        
    return desBeg;
}

template <typename BidirectionalIterator, typename Comp>
bool next_permutation( BidirectionalIterator beg, BidirectionalIterator end, Comp comp ) 
{
    using std::swap;
    if( beg == end ) 
    {
        return false;
    }
    auto current = beg;
    if( ++current == end ) 
    {
        return false;
    }
    current = end;
    --current;                       // current points to the last elemt 
    while( current != beg ) 
    {
        auto next = current;
        --current;
        if( comp( *current, *next ) ) 
        {
            auto iter = end;
            while( !comp( *current, *--iter ) )
            {
            }

            swap( *current, *iter );
            mystl::reverse( next, end );
            return true;
        }
    }
    return false;
}
    
template <typename BidirectionalIterator>
bool next_permutation( BidirectionalIterator beg, BidirectionalIterator end ) 
{
    return mystl::next_permutation( beg, end, std::less<decltype(*beg)>{} );
}

template <typename BidirectionalIterator, typename Comp>
bool prev_permutation( BidirectionalIterator beg, BidirectionalIterator end, Comp comp ) 
{
    using std::swap;
    if( beg == end ) 
    {
        return false;
    }
    auto current = beg;
    if( ++current == end ) 
    {
        return false;
    }
    current = end;
    --current;                       // current points to the last element
    while( current != beg ) 
    {
        auto next = current;
        --current;
        if( comp( *next, *current ) ) 
        {
            auto iter = end;
            while( !comp( *--iter, *current ) )
            {
            }
            swap( *current, *iter );
            mystl::reverse( next, end );
            return true;
        }
    }
    return false;
}
    
template <typename BidirectionalIterator>
bool prev_permutation( BidirectionalIterator beg, BidirectionalIterator end ) 
{
    return mystl::prev_permutation( beg, end, std::less<decltype(*beg)>{} );
}
    
template <typename RandomIterator>
void random_shuffle( RandomIterator beg, RandomIterator end ) 
{
    if( beg == end ) 
    {
        return;
    }
    for( auto iter = beg + 1; iter != end; ++iter ) 
    {
        std::iter_swap( iter, beg + ( std::rand() % ( ( iter - beg )  + 1 ) ) );
    }
}
    
template <typename RandomIterator, typename RandomNumberGenerator>
void random_shuffle( RandomIterator beg, RandomIterator end, RandomNumberGenerator &&rand ) 
{
    if( beg == end ) 
    {
        return;
    }
    for( auto iter = beg + 1; iter != end; ++iter ) 
    {
        std::iter_swap( iter, beg + rand( ( iter - beg )  + 1 ) );
    }
}
    
template <typename RandomIterator, typename UniformRandomNumberGenerator>
void shuffle( RandomIterator beg, RandomIterator end, UniformRandomNumberGenerator &&generator )
{
    if( beg == end ) 
    {
        return;
    }
    std::uniform_int_distribution<unsigned> distribution;
    using range_type = decltype( distribution )::param_type;
    for( auto iter = beg + 1; iter != end; ++iter ) 
    {
        std::iter_swap( iter, beg + distribution( generator, range_type( 0, iter - beg ) ) );
    }
}


};    // namespace mystl

#endif /* _ALGORITHM_H_ */
