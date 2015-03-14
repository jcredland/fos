#pragma once
#include <std_types.h>

template <typename T, typename Allocator>
class SortedList
{
public:
    struct Node
    {
        T data;
        Node * next_node; 
    };

    class iterator
    {
    public:
        iterator(Node * starting_node) : n(starting_node) {}
        iterator(const iterator & i) : n(i.n) {}
        const iterator & operator=(const iterator & i) { n = i.n; return *this; }

        iterator & operator++()
        {
            n = n->next_node;
            return *this;
        }
        iterator operator++(int)
        {
            iterator c(*this); 
            ++(*this); 
            return c;
        }
        T operator*() { return n->data; }
        bool operator==(const iterator & rhs) { return n == rhs.n; }
        bool operator!=(const iterator & rhs) { return ! ((*this) == rhs.n); }
            
    private:
        Node * n; 
    };

    SortedList(Allocator & allocator)
        :
            first(nullptr),
            allocator(allocator)
    {}

    iterator begin() 
    {
        return iterator(first); 
    }

    iterator end() const
    {
        return nullptr;
    }

    void insert_sorted(const T & item)
    {
        insert_sorted(item, [](const T & a, const T & b){ return a > b; });
    }

    template <typename Predicate>
    void insert_sorted(const T & item, Predicate ordering_function)
    {
        Node * p; 
        p = first; 

        if (p == nullptr)
        {
            first = (Node *) allocator.malloc(sizeof(Node)); 
            first->next_node = nullptr;
            first->data = item;
        }
        else
        {
            do
            {
                Node * next = p->next_node;

                if (next == nullptr || ordering_function(next->data, item))
                {
                    insert_after(p, item); 
                    break;
                }

                p = p->next_node;
            }
            while (p != nullptr);
        }

        return;
    };


    T operator[](size_t index) const
    {
        int i = 0; 
        Node * n = first;
        while (n != nullptr)
        {
            if (i == index)
                return n->data;

            n = n->next_node;
            i++;
        }
        return  T();
    }

private:
    void insert_after(Node * p, const T & item)
    {
        Node * new_node = (Node *) allocator.malloc(sizeof(Node)); 

        if (new_node == nullptr)
            return;

        Node * nxt = p->next_node;
        p->next_node = new_node;
        new_node->next_node = nxt;

        new_node->data = item;
    }

    Node * first; 
    Allocator & allocator;
};


