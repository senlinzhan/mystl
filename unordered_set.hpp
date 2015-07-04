#ifndef _UNORDERED_SET_H_
#define _UNORDERED_SET_H_

#include <utility>
#include <cstddef>
#include "vector.hpp"
#include "forward_list.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include <initializer_list>        
#include <iostream>

namespace mystl {

class unordered_set_exception : public std::exception
{
public:
    explicit unordered_set_exception( const std::string &message ) 
        : message_( message )
        {  }
    
    virtual const char * what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

template <typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>>
class unordered_set
{
private:
    using bucket_type     = mystl::forward_list<T>;
    using bucket_vector   = mystl::vector<bucket_type>;
    
public:
    using key_type             = T;
    using value_type           = T;
    using hasher               = Hash;
    using key_equal            = Equal;
    using pointer              = T *;
    using const_pointer        = const T*;
    using reference            = T&;
    using const_reference      = const T&;
    using size_type            = std::size_t;
    using difference_type      = std::ptrdiff_t;

    /*
      both local_iterator and const_local_iterator are forward_list's const_iterator
      because we don't want user modify element by using iterator 
     */
    using local_iterator       = typename mystl::forward_list<value_type>::const_iterator;
    using const_local_iterator = typename mystl::forward_list<value_type>::const_iterator;

    class const_iterator
    {
        friend class unordered_set;
    public:
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using iterator_category = std::forward_iterator_tag;
        
        const_iterator() 
            : ptr_( nullptr ), index_( 0 )
        {  }

        reference operator*() const { 
            return *iter_;
        }

        pointer operator->() const {
            return &( operator*() );
        }
        
        const_iterator &operator++() noexcept {
            ++iter_;
            while( iter_ == (*ptr_)[index_].cend() && ++index_ < ptr_->size() ) {
                iter_ = (*ptr_)[index_].cbegin();
            }
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        
        bool operator==( const const_iterator &other ) const noexcept {
            return ptr_ == other.ptr_ && index_ == other.index_ && iter_ == other.iter_;
        }
        
        bool operator!=( const const_iterator &other ) const noexcept {
            return !( *this == other );
        }

    protected:
        const_iterator( bucket_vector *ptr, bool end )
            : ptr_( ptr ) {
            if( end ) {
                index_ = ptr_->size();
            } else {
                index_ = 0;
                iter_ = (*ptr_)[index_].cbegin();
                while( iter_ == (*ptr_)[index_].cend() && ++index_ < ptr_->size() ) {
                    iter_ = (*ptr_)[index_].cbegin();
                }
            }
        }
        
        const_iterator( bucket_vector *ptr, size_type index, local_iterator iter ) 
            : ptr_( ptr ), index_( index ), iter_( iter ){
        }

    private:
        const bucket_vector *ptr_;
        size_type index_;
        local_iterator iter_;
    };

    /**
       iterator is same as const_iterator
       because we don't want user modify element by using iterator
    **/
    using iterator = const_iterator;
    
private:
    hasher         hash_;                           // hash function
    key_equal      equal_;                          // for test value' equality
    bucket_vector  buckets_;                        // a vector contains all buckets
    size_type      size_ = 0;                       // elements number
    float          max_load_factor_ = 1.0;          // max load factor

    static const size_type PRIME_SIZE = 28;
    static const size_type prime_[PRIME_SIZE];
    
    size_type next_prime( size_type n ) const {
        auto first = std::begin( prime_ );
        auto last = std::end( prime_ );

        auto iter = std::lower_bound( first, last,  n );
        return iter == last ? *( last - 1 ) : *iter;
    }

public:
    unordered_set() {
        const size_type num = next_prime( 0 );
        buckets_.reserve( num );
        buckets_.insert( buckets_.cend(), num, bucket_type() );
    }
    
    explicit unordered_set( size_type bucket_num, const hasher &hash = hasher(), const key_equal &equal = key_equal() )
        : hash_( hash ), 
          equal_( equal ), 
          buckets_( next_prime( bucket_num ), bucket_type() )
    {  }    

    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
    unordered_set( InputIterator first, InputIterator last, size_type bucket_num = 0, 
                   const hasher &hash = hasher(), const key_equal &equal = key_equal() ) 
        : hash_( hash ), 
          equal_( equal ),
          buckets_( next_prime( bucket_num ), bucket_type() )
    {
        insert( first, last );
    }

	
	unordered_set( std::initializer_list<value_type> lst, size_type bucket_num = 0, 
				   const hasher &hash = hasher(), const key_equal &equal = key_equal() )
        : unordered_set( lst.begin(), lst.end(), bucket_num, hash, equal ) 
    {  }
    
    unordered_set( const unordered_set &other ) {
        buckets_ = other.bucket_;
        size_ = other.size_;
    }

    unordered_set( unordered_set &&other ) noexcept {
        swap( other );
    }
    
    virtual ~unordered_set() = default;
    
    unordered_set &operator=( const unordered_set &other ) {
        auto copy = other;
        swap( other );
        return *this;
    }

    unordered_set &operator=( unordered_set &&other ) noexcept {
        if( this != &other ) {
            clear();
            swap( other );
        }
        return *this;
    }

    unordered_set &operator=( std::initializer_list<value_type> lst ) {
        clear();
        buckets_.
        insert( lst.begin(), lst.end() );
        return *this;
    }


    void swap( unordered_set &other ) noexcept {
        using std::swap;
        swap( buckets_, other.buckets_ );
    }

    size_type bucket_count() const noexcept {
        return buckets_.size();
    }

    /**
       If size_hint is equal or lower than bucket_count(), this function do nothing.
       If size_hint is greater than bucket_count(), a rehash is forced, the new bucket count is greater than size_hint.
    **/
    void rehash( size_type size_hint ) {
        if( size_hint <= bucket_count() ) {
            return;
        }
        const size_type new_bucket_count = next_prime( size_hint );

        if( new_bucket_count > bucket_count() ) 
        {
            unordered_set other( new_bucket_count );
            for( auto &elem : *this ) {
                other.insert( std::move( elem ) );
            }
            swap( other );
        }
    }

    float load_factor() const noexcept {
        return static_cast<float>( size() ) / static_cast<float>( bucket_count() );
    }

    void print( std::ostream &os = std::cout, const std::string &delim = " " ) const {
        for( const auto &lst : buckets_ ) {
            for( const auto &elem : lst ) {
                os << elem << delim;
            }
        }
    }
    
    void clear() noexcept {
        buckets_.clear();
        size_ = 0;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
    
    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return prime_[PRIME_SIZE - 1];
    }

    // this is the maximum potential number of buckets the container can have 
    size_type max_bucket_count() const noexcept {
        return prime_[PRIME_SIZE - 1];
    }
    
    float max_load_factor() const noexcept {
        return max_load_factor_;
    }

    void max_load_factor( float factor ) {
        max_load_factor_ = factor;
    }
    
    /**
       if n is greater than the current bucket_count multiplied by the max_load_factor
       then the container's bucket_count is increased and a rehash is forced.
       If n is lower than that, the function may have no effect.
    **/
    void reserve( size_type elem_num ) {
        if( elem_num > static_cast<size_type>( bucket_count() * max_load_factor() ) ) {
            rehash( bucket_count() + 1 );
        }
    }
    
    /***
        returns the bucket number where the element with value k is located.
    ***/
    size_type bucket( const value_type &value ) const {
        return hash_( value ) % bucket_count();
    }

    std::pair<iterator, bool> insert( const value_type &value ) {
        auto copy = value;
        return insert( std::move( copy ) );
    }

    std::pair<iterator, bool> insert( value_type &&value ) {
        // if current load factor greater than max load factor
        // then we need to rehash the container 
        if( load_factor() > max_load_factor() ) {
            rehash( bucket_count() + 1 );         // force rehash
        }

        // compute the bucket index for that value
        const size_type pos = bucket( value );
        
        auto iter = mystl::find( cbegin( pos ), cend( pos ), value );

        // if container doesn't contains the specify value, then we insert it
        // otherwise we don't
        if( iter == cend( pos ) ) {
            buckets_[pos].push_front( std::move( value ) );
            ++size_;
            return { iterator{ &buckets_, pos, begin( pos ) }, true };
        } else {
            return { iterator{ &buckets_, pos, begin( pos ) }, false };
        }
    }

    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
	void insert( InputIterator first, InputIterator last ) {
        while( first != last ) {
            insert( *(first++) );
        }
    }
    void insert( std::initializer_list<value_type> lst ) {
        insert( lst.begin(), lst.end() );
    }

    template<typename... Args> 
    std::pair<iterator, bool> emplace( Args&&... args ) {
        // if current load factor greater than max load factor
        // then we need to rehash the container 
        if( load_factor() > max_load_factor() ) {
            rehash( bucket_count() + 1 );         // force rehash
        }

        value_type value( std::forward<Args>( args )... );

        // compute the bucket index for that value
        const size_type pos = bucket( value );
        
        auto iter = mystl::find( begin( pos ), end( pos ), value );
        
        // if container doesn't contains the specify value, then we insert it
        // otherwise we don't
        if( iter == buckets_[pos].cend() ) {
            buckets_[pos].push_front( std::move( value ) );
            ++size_;
            return { iterator{ &buckets_, pos, begin( pos ) }, true };
        } else {
            return { iterator{ &buckets_, pos, begin( pos ) }, false };
        }
    }

    iterator begin() noexcept {
        return { &buckets_, false };
    }

    const_iterator begin() const noexcept {
        return const_cast<unordered_set *>( this )->begin();
    }

    iterator end() noexcept {
        return { &buckets_, true };
    }

    const_iterator end() const noexcept {
        return const_cast<unordered_set *>( this )->end();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }
    
    const_iterator cend() const noexcept {
        return end();
    }

    local_iterator begin( size_type bucket_index ) {
        return buckets_[bucket_index].begin();
    }
    
    const_local_iterator begin( size_type bucket_index ) const {
        return buckets_[bucket_index].begin();
    }
    
    local_iterator end( size_type bucket_index ) {
        return buckets_[bucket_index].end();
    }

    const_local_iterator end( size_type bucket_index ) const {
        return buckets_[bucket_index].end();
    }
    
    const_local_iterator cbegin( size_type bucket_index ) const {
        return buckets_[bucket_index].cbegin();
    }
    
    const_local_iterator cend( size_type bucket_index ) const {
        return buckets_[bucket_index].cend();
    }

    hasher hash_function() const {
        return hasher();
    }
    
    key_equal key_eq() const {
        return key_equal();
    }
    
    // returns the number of elements in the specify bucket
    size_type bucket_size( size_type bucket_index ) const {
        return buckets_[bucket_index].size();
    }

    /***
        returns 1 if an element with that value exists in the container, and zero otherwise.
    ***/
    size_type count( const value_type &value ) const {
        const size_type pos = bucket( value );
        auto iter = mystl::find( cbegin( pos ), cend( pos ), value );
        return iter == cend( pos ) ? 0 : 1;
    }

    const_iterator find( const value_type &value ) const {
        return const_cast<unordered_set *>( this )->find( value );
    }

    /***
        searches the container for the specify value and returns an iterator to it if found
        otherwise it returns an iterator to unordered_set::end()
    ***/
    iterator find( const value_type &value ) {
        size_type pos = bucket( value );
        auto iter = mystl::find( cbegin( pos ), cend( pos ), value );
        if( iter == cend( pos ) ) {
            return end();
        } else {
            return { &buckets_, pos, iter };
        }
    }

    /**
       return an iterator pointing to the position immediately following the the element erased
     **/
    iterator erase( const_iterator position ) {
        if( position == cend() ) {
            throw unordered_set_exception( "unordered_set::erase(): the specify iterator is an off-the-end iterator!" );
        }
        size_type pos = position.index_;
        buckets_[pos].remove( *position );
        return ++position;
    }
    
    /**
       returns the number of elements erased, this is 1 if an element with value existed 
       ( and thus was subsequently erased ), and zero otherwise.
    **/
    size_type erase( const value_type &value ) {
        size_type pos = bucket( value );
        auto iter = mystl::find( cbegin( pos ), cend( pos ) );
        if( iter == cend( pos ) ) {
            return 0;
        } else {
            buckets_[pos].remove( value );   
            return 1;
        }
    }

    /**
       removes from the unordered_set container a range of elements 
       return an iterator pointing to the position immediately following the last of the elements erased
     **/
    iterator erase( const_iterator first, const_iterator last ) {
        if( first == last ) {
            return last;
        }
        for( auto iter = first; iter != last; ++iter ) {
            erase( iter );
        }
        return last;
    }


    bool operator==( const unordered_set &other ) const noexcept {
        if( this == &other ) {    // the same forward_list
            return true;
        }
        if( size_ != other.size_ ) {
            return false;
        }

        return mystl::equal( cbegin(), cend(), other.cbegin() );
    }

    bool operator!=( const unordered_set &other ) const noexcept {
        return !(*this == other);
    }
};

// prime number serve as bucket count
template <typename T, typename Hash, typename Equal>
const typename unordered_set<T, Hash, Equal>::size_type 
unordered_set<T, Hash, Equal>::prime_[PRIME_SIZE] = 
{
    53u, 97u, 193u, 389u, 769u, 1543u, 3079u, 6151u, 12289u, 24593u, 49157u, 
    98317u, 196613u, 393241u, 786433u, 1572869u, 3145739u, 6291469u, 12582917u, 
    25165843u, 50331653u, 100663319u, 201326611u, 402653189u, 805306457u, 
    1610612741u, 3221225473u, 4294967291u,
};

template <typename T, typename Hash, typename Equal>
inline std::ostream &operator<<( std::ostream &os, const unordered_set<T, Hash, Equal> &coll ) {
    coll.print( os );
    return os;
}

template <typename T, typename Hash, typename Equal>
inline void swap( unordered_set<T, Hash, Equal> &left, const unordered_set<T, Hash, Equal> &right ) noexcept {
    left.swap( right );
}

};



#endif /* _UNORDERED_SET_H_ */
    
