#ifndef _ITERATOR_H_
#define _ITERATOR_H_

#include <iterator>

template <typename InputIterator>
using RequireInputIterator = typename std::enable_if<
    std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category, 
                        std::input_iterator_tag
                        >::value
    >::type;

#endif /* _ITERATOR_H_ */
