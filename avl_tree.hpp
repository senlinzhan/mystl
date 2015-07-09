/***
    AVL 树
        1. 使用智能指针自动管理内存
        2. 引入异常，对于不合法的操作会抛出异常
        3. 支持输入迭代器

    版本 1.0
    作者：詹春畅
    博客：senlinzhan.github.io
 ***/

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include "stack.hpp"
#include "memory.hpp"
#include "algorithm.hpp"
#include "iterator.hpp"
#include <string>
#include <exception>
#include <iostream>
#include <functional>
#include <initializer_list>        

namespace mystl {


class avl_tree_exception : public std::exception
{
public:
    explicit avl_tree_exception( const std::string &message ) 
        : message_( message )
    {
    }
    
    virtual const char * what() const noexcept override 
    {
        return message_.c_str();
    }
    
private:
    std::string message_;
};


template <typename T, typename Comp = std::less<T>>
class avl_tree
{
public:
    public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    struct node;
    using node_ptr        = std::unique_ptr<node>; 
    using node_raw_ptr    = node *;

    struct node 
    {
        node( const value_type &value, node_ptr &&left, node_ptr &&right, size_type height = 1 ) 
            : value_( value ), 
              left_( std::move( left ) ), 
              right_( std::move( right ) ), 
              height_( height )
        {  
        }

        node( value_type &&value, node_ptr &&left, node_ptr &&right, size_type height = 1 ) 
            : value_( std::move( value ) ), 
              left_( std::move( left ) ), 
              right_( std::move( right ) ),
              height_( height )
        {
        }

        explicit node( const value_type &value = value_type() ) 
            : value_( value ), 
              left_( nullptr ), 
              right_( nullptr ),
              height_( 1 )
        { 
        }
        
        node( const node & )            = delete;
        node &operator=( const node & ) = delete;
        
        node( node && )                 = default;
        node &operator=( node && )      = default;

        value_type       value_;
        node_ptr         left_;
        node_ptr         right_;
        size_type        height_;
    };

public:
    class const_iterator
    {
        friend class avl_tree;
    public:
        using value_type          = T;
        using pointer             = const T*;
        using reference           = const T&;
        using difference_type     = std::ptrdiff_t;
        using iterator_category   = std::input_iterator_tag;
        
        const_iterator() = default;
        
        reference operator*() const
        {
            return stack_.top()->value_;
        }

        pointer operator->() const 
        {
            return &( operator*() );
        }
        
        const_iterator &operator++() noexcept 
        {
            auto cur = stack_.top();
            stack_.pop();

            cur = get_raw( cur->right_ );
            if( cur != nullptr ) 
            {
                stack_.push( cur );
                cur = get_raw( cur->left_ );
                while( cur != nullptr ) 
                {
                    stack_.push( cur );
                    cur = get_raw( cur->left_ );
                }
            }
            ++index_;
            return *this;
        }

        const_iterator operator++(int) noexcept 
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        
        bool operator==( const const_iterator &other ) const noexcept 
        {
            return tree_ == other.tree_ && index_ == other.index_;
        }
        
        bool operator!=( const const_iterator &other ) const noexcept
        {
            return !( *this == other );
        }

    protected:
        const_iterator( const avl_tree *tree, bool end ) 
            : tree_( tree )
        {
            if( end || tree->size() == 0 )
            {
                index_ = tree->size();
            } 
            else 
            {
                index_ = 0;
                auto cur = get_raw( tree->root_ );
                while( cur ) 
                {
                    stack_.push( cur );
                    cur = get_raw( cur->left_ );
                }
            }
        }

        const avl_tree               *tree_;
        size_type                    index_;
        mystl::stack<node_raw_ptr>     stack_;
    };

    /**
       iterator is same as const_iterator
       because we don't want user modify element by using iterator
    **/
    using iterator = const_iterator;
    
private:
    Comp                         less_;                           // for compare elements
    node_ptr                     root_;                           // point to root node
    size_type                    size_ = 0;                       // number of nodes
    static constexpr size_type   ALLOWED_IMBALANCE = 1;

public:
    avl_tree( Comp comp = Comp() ) 
        : less_( comp )
    { 
    }

    template <typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    avl_tree( InputIterator first, InputIterator last, Comp comp = Comp() ) 
        : less_( comp ) 
    {
        insert( first, last );
    }
    
    avl_tree( std::initializer_list<value_type> lst, Comp comp = Comp() ) 
        : avl_tree( lst.begin(), lst.end(), comp )
    { 
    }

    avl_tree( const avl_tree &tree )
    {
        root_ = clone_tree( tree.root_ );
        size_ = tree.size_;
    }

    avl_tree( avl_tree &&tree ) noexcept
    {
        swap( tree );
    }

    // can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    avl_tree &operator=( const avl_tree &tree ) 
    {
        auto copy = tree;
        swap( copy );
        return *this;
    }
    
    /**
       root_ will be set to nullptr, thus free all the memory
       then root_ will be set to tree.root_ 
       and tree.root_ will be set to nullptr 
    **/
    avl_tree &operator=( avl_tree &&tree ) noexcept 
    {
        // handle the problem of self-move-assignment
        if( *this != tree ) 
        {
            clear();
            swap( tree );
        }
        return *this;
    }

    avl_tree &operator=( std::initializer_list<value_type> lst ) 
    {
        assign( lst.begin(), lst.end() );
        return *this;
    }

    // call root_'s destructor, all nodes' memory will be free 
    virtual ~avl_tree() = default;

    void swap( avl_tree &tree ) noexcept 
    {
        using std::swap;
        swap( root_, tree.root_ );
        swap( size_, tree.size_ );
    }

    template<typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    void assign( InputIterator first, InputIterator last ) 
    {
        clear();
        insert( first, last );
    }

    void assign( std::initializer_list<value_type> lst ) 
    {
        assign( lst.begin(), lst.end() );
    }
    
    void assign( size_type size, value_type &value ) 
    {
        clear();
        insert( size, value );
    }

    /**
       insert value in tree, and increment the size by one
       if value already in this tree, then just return
    **/
    void insert( const value_type &value ) 
    {
        auto copy = value;
        insert( std::move( copy ) );
    }

    void insert( value_type &&value ) 
    {
        insert( std::move( value ), root_ );
    }

    void insert( std::initializer_list<value_type> lst ) 
    {
        insert( lst.begin(), lst.end() );
    }

    template<typename InputIterator, typename = mystl::RequireInputIterator<InputIterator>> 
    void insert( InputIterator first, InputIterator last ) 
    {
        std::for_each( first, last, [=]( const value_type &elem ) {  insert( elem );  } );
    }

    void insert( size_type size, value_type &value ) 
    {
        for( size_type i = 0; i < size; ++i ) 
        {
            insert( value );
        }
    }

    /**
       insert value in tree, and increment the size by one
       if value already in this tree, then just return
    **/
    template<typename... Args>
    void emplace( Args&&... args )
    {
        insert( value_type( std::forward<Args>( args )... ), root_ );
    }

    /**
       we assume value_type's destructor will not throw exception
       so clear() is noexcept
    **/
    void clear() noexcept
    {
        root_ = nullptr;
        size_ = 0;
    }

    size_type height() const noexcept 
    {
        return height( root_ );
    }

    iterator begin()
    {
        return { this, false };
    }

    const_iterator begin() const 
    {
        return const_cast<avl_tree *>( this )->begin();
    }

    iterator end() noexcept 
    {
        return { this, true };
    }

    const_iterator end() const noexcept 
    {
        return const_cast<avl_tree *>( this )->end();
    }

    const_iterator cbegin() const 
    {
        return begin();
    }

    const_iterator cend() const noexcept 
    {
        return end();
    }    

    size_type size() const noexcept 
    {
        return size_;
    }

    bool empty() const noexcept 
    {
        return size_ == 0;
    }

    void print( std::ostream &os = std::cout, const std::string &delim = " " ) const
    {
        for( const auto &elem : *this ) 
        {
            os << elem << delim;
        }
    }

    // non-recursive version
    bool contains( const value_type &value ) const noexcept 
    {
        if( !root_ ) 
        {
            return false;
        }

        node_raw_ptr parent = nullptr;
        node_raw_ptr child = get_raw( root_ );

        while( child ) 
        {
            parent = child;
         
            if( less_( value, child->value_ ) ) 
            {
                child = get_raw( child->left_ );
            } 
            else if( less_( child->value_, value ) ) 
            {
                child = get_raw( child->right_ );
            } 
            else 
            {
                return true;                
            }
        }
        return false;
    }

    /**
       if value is not in this tree, then do nothing
       thus this function will not throw exception
    **/
    void remove( const value_type &value ) noexcept 
    {
        remove( value, root_ );
    }

    /**
       return by copy beacuse we must ensure that client can't modify this value
    **/
    value_type min() const 
    {
        if( empty() ) 
        {
            throw avl_tree_exception( "avl_tree::min(): the tree is empty!" );
        }
        return finMin( root_ )->value_;
    }

    value_type max() const 
    {
        if( empty() ) 
        {
            throw avl_tree_exception( "avl_tree::max(): the tree is empty!" );
        }
        return finMax( root_ )->value_;        
    }


private:

    static node_raw_ptr get_raw( const node_ptr &ptr ) noexcept
    {
        return ptr.get();
    }

    node_ptr clone_tree( const node_ptr &r ) 
    {
        if( !r ) 
        {
            return nullptr;
        } 
        else 
        {
            return make_unique<node>( r->value_, clone_tree( r->left_ ), clone_tree( r->right_ ), r->height_ ); 
        }
    }

    void insert( value_type &&value, node_ptr &ptr )
    {
        if( !ptr ) 
        {
            ptr = make_unique<node>( std::move( value ) );
            ++size_;
        } 
        else 
        {
            if( less_( value, ptr->value_ ) ) 
            {
                insert( std::move( value ), ptr->left_ );
            } 
            else if( less_( ptr->value_, value ) ) 
            {
                insert( std::move( value ), ptr->right_ );
            } 
            else
            {
                return;
            }
        }
        balance( ptr );
    }

    size_type height( const node_ptr &ptr ) const 
    {
        return ptr ? ptr->height_ : 0;
    }
    
    void balance( node_ptr &ptr ) 
    {
        if( !ptr ) 
        {
            return;
        }
        
        if( height( ptr->left_ ) > height( ptr->right_ ) + ALLOWED_IMBALANCE ) 
        {
            if( height( ptr->left_->left_ ) >= height( ptr->left_->right_ ) ) 
            {
                single_rotate_left( ptr );
            } 
            else 
            {
                double_rotate_left( ptr );
            }
        } 
        else if( height( ptr->right_ ) > height( ptr->left_ ) + ALLOWED_IMBALANCE ) 
        {
            if( height( ptr->right_->right_ ) >= height( ptr->right_->left_ ) ) 
            {
                single_rotate_right( ptr );
            }
            else 
            {
                double_rotate_right( ptr );
            }
        }

        ptr->height_ = std::max( height( ptr->left_ ), height( ptr->right_ ) ) + 1;
    }

    void single_rotate_left( node_ptr &ptr ) 
    {
        auto left = std::move( ptr->left_ );
        ptr->left_ = std::move( left->right_ );
        
        ptr->height_ = std::max( height( ptr->left_ ), height( ptr->right_ ) ) + 1;
        left->height_ = std::max( height( left->left_ ), height( ptr ) ) + 1;

        left->right_ = std::move( ptr );
        ptr = std::move( left );
    }
    
    void single_rotate_right( node_ptr &ptr )
    {
        auto right = std::move( ptr->right_ );
        ptr->right_ = std::move( right->left_ );
        
        ptr->height_ = std::max( height( ptr->left_ ), height( ptr->right_ ) ) + 1;
        right->height_ = std::max( height( right->right_ ), height( ptr ) ) + 1;
        
        right->left_ = std::move( ptr );
        ptr = std::move( right );        
    }

    void double_rotate_left( node_ptr &ptr )
    {
        single_rotate_right( ptr->left_ );
        single_rotate_left( ptr );
    }
    
    void double_rotate_right( node_ptr &ptr )
    {
        single_rotate_left( ptr->right_ );
        single_rotate_right( ptr );
    }

    void remove( const value_type &value, node_ptr &ptr ) noexcept 
    {
        if( !ptr ) 
        {
            return;
        }
        
        if( less_( value, ptr->value_ ) ) 
        {
            remove( value, ptr->left_ );
        } 
        else if( less_( ptr->value_, value ) ) 
        {
            remove( value, ptr->right_ );
        }
        else 
        { 
            // value equals to ptr->value_
            if( ptr->left_ && ptr->right_ ) 
            {
                ptr->value_ = finMin( ptr->right_ )->value_;
                remove( ptr->value_, ptr->right_ );
            } 
            else 
            {
                ptr = ( ptr->left_ ? std::move( ptr->left_ ) : std::move( ptr->right_ ) );
                --size_;
            }
        }

        balance( ptr );
    }

    // if ptr equals to nullptr, then return nullptr
    node_raw_ptr finMin( const node_ptr &ptr ) const noexcept 
    {
        node_raw_ptr min = get_raw( ptr );

        if( min ) 
        {
            while( min->left_ ) 
            {
                min = get_raw( min->left_ );
            }
        }
        return min;
    }

    // if ptr equals to nullptr, then return nullptr
    node_raw_ptr finMax( const node_ptr &ptr ) const noexcept 
    {
        node_raw_ptr max = get_raw( ptr );
        
        if( max ) 
        {
            while( max->right_ ) 
            {
                max = get_raw( max->right_ );
            }
        }
        return max;
    }

public:
    bool operator==( const avl_tree &other ) const 
    {
        if( this == &other )      // equals to itself
        {
            return true;
        }
        if( size() != other.size() ) 
        {
            return false;
        }
        
        return mystl::equal( cbegin(), cend(), other.cbegin() );
    }

    bool operator!=( const avl_tree &other ) const
    {
        return !(*this == other);
    }

    bool operator<( const avl_tree &other ) const noexcept 
    {
        return std::lexicographical_compare( cbegin(), cend(), other.cbegin(), other.cend() );
    }
    
    bool operator>( const avl_tree &other ) const noexcept 
    {
        return other < *this;
    }
    
    bool operator>=( const avl_tree &other ) const noexcept 
    {
        return !( *this < other );
    }

    bool operator<=( const avl_tree &other ) const noexcept 
    {
        return !( other < *this );
    }

};

template <typename T, typename Comp>
void swap( avl_tree<T, Comp> &first, avl_tree<T, Comp> &second ) noexcept 
{
    first.swap( second );
}

template <typename T, typename Comp>
std::ostream &operator<<( std::ostream &os, const avl_tree<T, Comp> &tree ) 
{
    tree.print( os );
    return os;
}


};    // namespace mystl    


#endif /* _AVL_TREE_H_ */
