/***
    二叉搜索树
        1. 使用智能指针自动管理内存
        2. 使用虚析构函数，允许定义 binary_tree 的派生类
        3. 引入异常，对于不合法的操作会抛出异常
        4. 支持双向迭代器

    版本 1.0
    作者：詹春畅
    博客：senlinzhan.github.io
 ***/

#ifndef _BINARY_TREE_H_
#define _BINARY_TREE_H_

#include "memory.hpp"
#include "iterator.hpp"
#include <stack>
#include <functional>
#include <iostream>

class binary_tree_exception : public std::exception
{
public:
    explicit binary_tree_exception( const std::string &message ) 
        : message_( message )
        {  }
    
    virtual const char * what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

template <typename T, typename Comp = std::less<T>>
class binary_tree
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    struct node;
    using node_ptr = std::unique_ptr<node>; 
    using node_raw_ptr = node *;
    using node_path = std::shared_ptr<mystl::vector<node_raw_ptr>>;   // for record node path

    struct node 
    {
        node( const T &value, node_ptr &&left, node_ptr &&right ) 
            : value_( value ), left_( std::move( left ) ), right_( std::move( right ) )
        {  }

        node( T &&value, node_ptr &&left, node_ptr &&right ) 
            : value_( std::move( value ) ), left_( std::move( left ) ), right_( std::move( right ) ) 
        {  }

        explicit node( const T &value = T{} ) 
            : value_( value ), left_( nullptr ), right_( nullptr ) 
        {  }
        
        node( const node & ) = delete;
        node &operator=( const node & ) = delete;
        
        node( node && ) = default;
        node &operator=( node && ) = default;

        T value_;
        node_ptr left_;
        node_ptr right_;
    };

public:

    // bidirectional const_iterator
    class const_iterator
    {
        friend class binary_tree<T, Comp>;
    public:
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        
        reference operator*() const { 
            return ( path_->operator[]( index_ ) )->value_;
        }

        pointer operator->() const {
            return &( operator*() );
        }
        
        const_iterator &operator++() noexcept {
            ++index_;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator &operator--() noexcept {
            --index_;
            return *this;
        }

        const_iterator operator--(int) noexcept {
            auto tmp = *this;
            --*this;
            return tmp;
        }
        
        bool operator==( const const_iterator &other ) const noexcept {
            return tree_ == other.tree_ && index_ == other.index_;
        }
        
        bool operator!=( const const_iterator &other ) const noexcept {
            return !( *this == other );
        }

    protected:
        // only binary_tree can access this constructor
        const_iterator( const binary_tree *tree, size_type index, node_path path = nullptr )
            : tree_( tree ), index_( index ), path_( path ) 
        {  }

        const binary_tree *tree_;
        size_type index_;
        node_path path_;
    };

    // iterator is same as const_iterator
    // because we don't want user modify element by using iterator
    class iterator : public const_iterator 
    {
        friend class binary_tree<T, Comp>;
    protected:
        iterator( const binary_tree *tree, size_type index, node_path path = nullptr )
            : const_iterator( tree, index, path )
        {  }        
    };    

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    node_ptr root_ = nullptr;             // point to root node
    size_type size_ = 0;                  // number of nodes
    Comp less_;                           // for compare elements
    
public:
    binary_tree() = default;              
    
    binary_tree( const binary_tree &tree ) {
        root_ = clone_tree( tree.root_ );
        size_ = tree.size_;
    }

    binary_tree( binary_tree &&tree ) noexcept {
        swap( tree );
    }

    // can handle the problem of self-assignment, see C++ Primer 5th section 13.3
    binary_tree &operator=( const binary_tree &tree ) {
        auto copy = tree;
        swap( copy );
        return *this;
    }
    
    // root_ will be set to nullptr, thus free all the memory
    // then root_ will be set to tree.root_ 
    // and tree.root_ will be set to nullptr 
    binary_tree &operator=( binary_tree &&tree ) noexcept {
        // handle the problem of self-move-assignment
        if( *this != tree ) {
            root_ = std::move( tree.root_ );     
            size_ = tree.size_;
            tree.size_ = 0;
        }
        return *this;
    }

    binary_tree &operator=( std::initializer_list<value_type> lst ) {
        // clear all elemtns and free memory
        clear();                        
        // insert all elemtns from lst
        for( const auto &elem : lst ) {
            insert( elem );
        }
        return *this;
    }

    template <typename InputIterator, typename = RequireInputIterator<InputIterator>>
    binary_tree( InputIterator first, InputIterator last ) {
        for( auto iter = first; iter != last; ++iter ) {
            insert( *iter );
        }
    }

    binary_tree( std::initializer_list<value_type> lst ) 
        : binary_tree( lst.begin(), lst.end() )
    {  }

    // call root_'s destructor, all nodes' memory will be free 
    virtual ~binary_tree() = default;
    
    void swap( binary_tree &tree ) noexcept {
        using std::swap;
        swap( root_, tree.root_ );
        swap( size_, tree.size_ );
    }
    
    template<typename InputIterator, typename = RequireInputIterator<InputIterator>>
    void assign( InputIterator first, InputIterator last ) {
        clear();
        for( auto iter = first; iter != last; ++iter ) {
            insert( *iter );
        }
    }

    void assign( std::initializer_list<value_type> lst ) {
        assign( lst.begin(), lst.end() );
    }

    void insert( const value_type &value ) {
        auto copy = value;
        insert( std::move( copy ) );
    }

    // insert value in tree, and increment the size by one
    // if value already in this tree, then just return
    void insert( value_type &&value )
    {
        auto new_node = make_unique<node>( std::move( value ) );
        // if the tree is empty, then store value in the root node
        if( !root_ ) {
            root_ = std::move( new_node );
            ++size_;
            return;
        }
        
        node_raw_ptr parent = nullptr;
        node_raw_ptr child = get_raw( root_ );
        while( child ) 
        {
            parent = child;
            if( less_( value, child->value_ ) ) {
                child = get_raw( child->left_ );
            } else if( less_( child->value_, value ) ) {
                child = get_raw( child->right_ );
            } else {
                return;              // if value already exist, then just return
            }
        }
        
        if( less_( value, parent->value_ ) ) {
            parent->left_ = std::move( new_node );
        } else {
            parent->right_ = std::move( new_node );
        }
        ++size_;
    }

    void insert( std::initializer_list<value_type> lst ) {
        for( const auto &elem : lst ) {
            insert( elem );
        }
    }

    void clear() {
        root_ = nullptr;
        size_ = 0;
    }

    // linear time operation
    iterator begin() {
        auto path = get_path();            // path contains all nodes' raw pointer
        return { this, 0, path };
    }

    // linear time operation
    reverse_iterator rbegin() {
        auto path = get_path();
        iterator last( this, size(), path );
        return reverse_iterator( last );
    }

    const_reverse_iterator rbegin() const {
        return const_cast<binary_tree *>( this )->rbegin();
    }

    // constant time operation
    reverse_iterator rend() noexcept {
        iterator first( this, 0, nullptr );
        return reverse_iterator( first );
    }

    // constant time operation
    const_reverse_iterator rend() const noexcept {
        return const_cast<binary_tree *>( this )->rend();
    }

    // linear time operation
    const_iterator begin() const {
        return const_cast<binary_tree *>( this )->begin();
    }

    // constant time operation
    iterator end() noexcept {
        return { this, size(), nullptr };
    }

    // constant time operation
    const_iterator end() const noexcept {
        return const_cast<binary_tree *>( this )->end();
    }

    // linear time operation
    const_iterator cbegin() const {
        return begin();
    }

    // constant time operation
    const_iterator cend() const noexcept {
        return end();
    }    
    
    // linear time operation
    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    // constant time operation
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    size_type size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
    
    // non-recursive version
    bool contains( const value_type &value ) const noexcept {
        if( !root_ ) {
            return false;
        }

        node_raw_ptr parent = nullptr;
        node_raw_ptr child = get_raw( root_ );

        while( child ) 
        {
            parent = child;
            if( less_( value, child->value_ ) ) {
                child = get_raw( child->left_ );
            } else if( less_( child->value_, value ) ) {
                child = get_raw( child->right_ );
            } else {
                return true;                
            }
        }
        return false;
    }

    void print( std::ostream &os = std::cout, const std::string &delim = " " ) const {
        for( const auto &elem : *this ) {
            os << elem << delim;
        }
    }

    // if value is not in this tree, then do nothing
    // thus this function will not throw exception
    void remove( const value_type &value ) noexcept {
        remove( value, root_ );
    }

    // return by copy beacuse we must ensure that client can't modify this value
    value_type min() const {
        if( empty() ) {
            throw binary_tree_exception( "binary_tree::min(): the tree is empty!" );
        }
        return finMin( root_ )->value_;
    }

    // return by copy beacuse we must ensure that client can't modify this value
    value_type max() const {
        if( empty() ) {
            throw binary_tree_exception( "binary_tree::max(): the tree is empty!" );
        }
        return finMax( root_ )->value_;        
    }

private:
    void remove( const value_type &value, node_ptr &ptr ) noexcept {
        if( !ptr ) {
            return;
        }
        
        if( less_( value, ptr->value_ ) ) {
            remove( value, ptr->left_ );
        } else if( less_( ptr->value_, value ) ) {
            remove( value, ptr->right_ );
        } else { 
            // value equals to ptr->value_
            if( ptr->left_ && ptr->right_ ) {
                ptr->value_ = finMin( ptr->right_ )->value_;
                remove( ptr->value_, ptr->right_ );
            } else {
                ptr = ptr->left_ ? std::move( ptr->left_ ) : std::move( ptr->right_ );
                --size_;
            }
        }
    }

    // if ptr equals to nullptr, then return nullptr
    node_raw_ptr finMin( const node_ptr &ptr ) const noexcept {
        node_raw_ptr min = get_raw( ptr );
        
        if( min ) {
            while( min->left_ ) {
                min = get_raw( min->left_ );
            }
        }
        
        return min;
    }

    // if ptr equals to nullptr, then return nullptr
    node_raw_ptr finMax( const node_ptr &ptr ) const noexcept {
        node_raw_ptr max = get_raw( ptr );
        
        if( max ) {
            while( max->right_ ) {
                max = get_raw( max->right_ );
            }
        }
        
        return max;
    }

    node_raw_ptr get_raw( const node_ptr &ptr ) const noexcept {
        return ptr.get();
    }

    node_ptr clone_tree( const node_ptr &r ) {
        if( !r ) {
            return nullptr;
        } else {
            return make_unique<node>( r->value_, clone_tree( r->left_ ), clone_tree( r->right_ ) );   
        }
    }

    node_path get_path() const {
        auto path = std::make_shared<mystl::vector<node_raw_ptr>>();
        fill_path( path, root_ );
        return path;
    }

    void fill_path( node_path &path, const node_ptr &ptr ) const {
        if( !ptr ) {
            return;
        }
        fill_path( path, ptr->left_ );
        path->push_back( get_raw( ptr ) );
        fill_path( path, ptr->right_ );
    }

public:
    bool operator==( const binary_tree &other ) const {
        if( this == &other ) {     // equals to itself
            return true;
        }
        if( size() != other.size() ) {
            return false;
        }
        
        return mystl::equal( cbegin(), cend(), other.cbegin() );
    }

    bool operator!=( const binary_tree &other ) const {
        return !(*this == other);
    }
};

template <typename T, typename Comp>
void swap( binary_tree<T, Comp> &first, binary_tree<T, Comp> &second ) noexcept {
    first.swap( second );
}

template <typename T, typename Comp>
std::ostream &operator<<( std::ostream &os, const binary_tree<T, Comp> &tree ) {
    tree.print( os, " " );
    return os;
}


#endif /* _BINARY_TREE_H_ */
