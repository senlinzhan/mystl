#ifndef _UNORDERED_SET_H_
#define _UNORDERED_SET_H_

#include <stdint.h>
#include <utility>
#include "vector.hpp"
#include "forward_list.hpp"
#include "algorithm.hpp"
#include <iostream>

namespace mystl {

template <typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>>
class unordered_set
{
private:
    using bucket_type     = mystl::forward_list<T>;
    using bucket_vector   = mystl::vector<bucket_type>;
    
public:
    using value_type           = T;
    using pointer              = T*;
    using const_pointer        = const T*;
    using reference            = T&;
    using const_reference      = const T&;
    using size_type            = size_t;
    using difference_type      = std::ptrdiff_t;

    /*
      both local_iterator and const_local_iterator are forward_list's const_iterator
      because we don't want user modify element by using iterator 
     */
    using local_iterator       = typename mystl::forward_list<value_type>::const_iterator;
    using const_local_iterator = typename mystl::forward_list<value_type>::const_iterator;


    // iterator is same as const_iterator
    // because we don't want user modify element by using iterator
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


    class iterator : public const_iterator 
    {
        friend class unordered_set;
    public:
        iterator() = default;
    protected:
        iterator( bucket_vector *ptr, bool end )
            : const_iterator( ptr, end )
        {  }
        iterator( bucket_vector *ptr, size_type index, local_iterator iter ) 
            : const_iterator( ptr, index, iter ) 
        {  }
    };





private:
    Hash          hash_;
    Equal         equal_;
    bucket_vector buckets_;
    size_type     size_ = 0;

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
    
    explicit unordered_set( size_type bucket_num, const Hash &hashFunc = Hash(), 
                            const Equal &equalFunc = Equal() )
        : hash_( hashFunc ), equal_( equalFunc ) 
    {
        const size_type num = next_prime( bucket_num );
        buckets_.reserve( num );
        buckets_.insert( buckets_.cend(), num, bucket_type() );
    }

    template<typename InputIterator>
	unordered_set( InputIterator first, InputIterator last, size_type bucket_num = 0, 
                   const Hash &hashFunc = Hash(), const Equal &equalFunc = Equal() ) {
        const size_type num = next_prime( bucket_num );
        buckets_.reserve( num );
        buckets_.insert( buckets_.cend(), num, bucket_type() );
        insert( first, last );
    }

	unordered_set( std::initializer_list<value_type> lst, size_type bucket_num = 0, 
                   const Hash &hashFunc = Hash(), const Equal &equalFunc = Equal() )
        : unordered_set( lst.begin(), lst.end(), bucket_num, hashFunc, equalFunc ) 
    {  }
    
    unordered_set( const unordered_set &other ) {
        buckets_ = other.bucket_;
        size_ = other.size_;
    }

    unordered_set( unordered_set &&other ) noexcept {
        swap( other );
    }
    
    virtual ~unordered_set() = default;
    
    void swap( unordered_set &other ) noexcept {
        using std::swap;
        swap( buckets_, other.buckets_ );
    }

    size_type bucket_count() const noexcept {
        return buckets_.size();
    }
    
    void rehash( size_type size_hint ) {
        if( size_hint <= bucket_count() ) {
            return;
        }
        
        const size_type new_bucket_count = next_prime( size_hint );
        if( new_bucket_count > bucket_count() ) {
            unordered_set other( new_bucket_count );
            for( auto &lst : buckets_ ) {
                for( auto &elem : lst ) {
                    other.insert( std::move( elem ) );
                }
            }
            swap( other );
        }
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
    }

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
        insert( lst.begin(), lst.end() );
        return *this;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
    
    
    
    size_type size() const noexcept {
        return size_;
    }


    template<typename... Args> 
    std::pair<iterator, bool> emplace( Args&&... args ) {
        value_type value( std::forward<Args>( args )... );        
        rehash( size_ + 1 );
        const size_type pos = hash_( value ) % bucket_count();
        
        auto iter = mystl::find( buckets_[pos].cbegin(), buckets_[pos].cend(), value );
        if( iter == buckets_[pos].cend() ) {
            buckets_[pos].push_front( std::move( value ) );
            ++size_;
            return { iterator{ &buckets_, pos, begin( pos ) }, true };
        } else {
            return { iterator{ &buckets_, pos, begin( pos ) }, false };
        }
    }
    
    
    std::pair<iterator, bool> insert( const value_type &value ) {
        auto copy = value;
        return insert( std::move( copy ) );
    }

    std::pair<iterator, bool> insert( value_type &&value ) {
        rehash( size_ + 1 );
        const size_type pos = hash_( value ) % bucket_count();
        
        auto iter = mystl::find( buckets_[pos].cbegin(), buckets_[pos].cend(), value );
        if( iter == buckets_[pos].cend() ) {
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
    
    

    

};

template <typename T, typename Hash, typename Equal>
inline std::ostream &operator<<( std::ostream &os, const unordered_set<T, Hash, Equal> &coll ) {
    coll.print( os );
    return os;
}

template <typename T, typename Hash, typename Equal>
const typename unordered_set<T, Hash, Equal>::size_type 
unordered_set<T, Hash, Equal>::prime_[PRIME_SIZE] = 
{
    53u, 97u, 193u, 389u, 769u, 1543u, 3079u, 6151u, 12289u, 24593u, 49157u, 
    98317u, 196613u, 393241u, 786433u, 1572869u, 3145739u, 6291469u, 12582917u, 
    25165843u, 50331653u, 100663319u, 201326611u, 402653189u, 805306457u, 
    1610612741u, 3221225473u, 4294967291u,
};


};



#endif /* _UNORDERED_SET_H_ */
    
