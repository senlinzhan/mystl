#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <memory>

// In C++14 you can use std::make_unique(), but not in C++11
// so this is our own version
template <typename T, typename... Ts>
std::unique_ptr<T> make_unique( Ts&&... params ) {
    return std::unique_ptr<T>( new T( std::forward<Ts>(params)... ) );
}

#endif /* _MEMORY_H_ */
