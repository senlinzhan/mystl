#ifndef _TRIETREE_H_
#define _TRIETREE_H_

#include <vector>
#include <string>
#include <iostream>

template <typename T>
class trie_tree
{
public:
    explicit trie_tree()
        : root_( new node )
    {
    }
    
    ~trie_tree()
    {
        clear();
    }
    
    void clear()
    {
        clear( root_ );
        root_ = new node;
    }

    trie_tree( const trie_tree & ) = delete;
    trie_tree &operator=( const trie_tree & ) = delete;
    
private:
    struct node;
    using  node_ptr  = node *;
    using  value_ptr = T *;
    
    static constexpr int R = 128;
 
    node_ptr root_;

    struct node
    {
        explicit node()
            : value( nullptr ), next( R, nullptr )
        {
        }
        
        ~node()
        {
            if( value )
            {
                delete value;
            }
        }

        value_ptr              value;
        std::vector<node_ptr>  next;
    };

public:
    void insert( const std::string &key, const T &value )
    {
        insert( root_, key, value, 0 );
    }
    
    bool contains( const std::string &key ) const 
    {
        return get( key ) != nullptr;
    }

    value_ptr get( const std::string &key ) const
    {
        node_ptr ptr = get( root_, key, 0 );
        
        if( !ptr )
        {
            return nullptr;
        }
        
        // ptr->value may be a null pointer, that means no value corresponds to the key
        return ptr->value;
    }

    std::vector<std::string> keys() const
    {
        return keysWithPrefix( "" );
    }

    std::vector<std::string> keysWithPrefix( const std::string &prefix ) const
    {
        std::vector<std::string> keys;
        auto ptr = get( root_, prefix, 0 );
        collect( ptr, prefix, keys );
        return keys;
    }

    void collect( node_ptr ptr, const std::string &prefix, std::vector<std::string> &keys ) const
    {
        if( ptr == nullptr )
        {
            return;
        }
        if( ptr->value != nullptr )
        {
            keys.push_back( prefix );
        }
        
        char c = 0;
        for( auto p : ptr->next )
        {
            collect( p, prefix + c, keys );
            ++c;
        }
    }

private:
    void insert( node_ptr &ptr, const std::string &key, const T &value, int i )
    {
        if( !ptr )
        {
            ptr = new node;
        }
        if( i == key.size() )
        {
            ptr->value = new T( value );
            return;
        }

        char index = key[i];
        insert( ptr->next[index], key, value, i + 1 );
    }


    node_ptr get( node_ptr ptr, const std::string &key, int i ) const
    {
        if( !ptr )
        {
            return nullptr;
        }
        if( i == key.size() )
        {
            return ptr;
        }

        char index = key[i];
        return get( ptr->next[index], key, i + 1 );
    }

    void clear( node_ptr ptr )
    {
        if( ptr )
        {
            for( auto p : ptr->next )
            {
                clear( p );
            }

            delete ptr;
        }
    }
};

#endif /* _TRIETREE_H_ */
