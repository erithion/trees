# Structures and algorithms

## Status

* Binary search tree
  * [x] Manually-balancing tree (Warren-Stout algorithm)
  * [x] Red-black tree
  * [x] AVL tree
  * [x] Interval tree (based on red-black tree)
* [x] Fenwick tree
* [x] Segment tree

## Usage

### Lightweight binary trees
#### Simple but pretty useless tree 
```cpp
#include "tree_search/tree.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::empty_augment>;
```
It is useless as you will not be able to perform any operations on it except a couple of elementary ones:
```cpp
int main() {
    tree<char> bin;
    
    assert(size(bin) == 0);
    assert(height(bin) == 0);
    assert(balanced(bin) == true);
    assert(perfect(bin) == true);
    
    return 0;
}
```
In order to do more than that you need **capabilities**. Simply put, a capability enables a particular function like insert/remove/... for your tree. 

#### A tree with insert/remove capabilites

Capabilities are specified as additional template parameters. 
For insert/remove it looks as follows:
```cpp
#include "tree_search/tree.hpp"

#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_remove.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::empty_augment
                                , tree_search::capability_insert
                                , tree_search::capability_remove>;

```
After that you are eligible to use **insert/remove** operations on the tree via corresponding functions:
```cpp
int main() {
    tree<int> bin;
    insert(bin, { 5, 7, 3, 6, 8, 2, 4, 1 });
    remove(bin, { 3, 7 });
    return 0;
}
```
The motivation behind such a design is to have lightweight and less error-prone trees where you could not, say, accidentally remove an element(s) if the tree was initially insert-only by design.

#### More capabilities at hand
```cpp
#include "tree_search/tree.hpp"

#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_search.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::empty_augment
                                , tree_search::capability_traverse
                                , tree_search::capability_search
                                , tree_search::capability_insert>;
                                
```
Now you can **traverse** the tree:
```cpp
int main() {
    tree<char> bin;
    insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
    
    // iterators; pre-order
    auto rg1 = traverse(tree_search::tag_preorder{}, bin);
    std::string res1(rg1.begin_, rg1.end_);
    assert(res1 == "FBADCEGIH");
    
    // range-loop; in-order
    std::string res2 = {};
    for (auto v : traverse(tree_search::tag_inorder{}, bin)) res2.push_back(v);
    assert(res2 == "ABCDEFGHI");

    // iterator functions; post-order
    auto rg2 = traverse(tree_search::tag_postorder{}, bin);
    std::string res3(begin(rg2), end(rg2));
    assert(res3 == "ACEDBHIGF");

    return 0;
}
```
Or you can **search** within the tree:
```cpp
int main() {
    tree<char> bin;
    insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
        
   // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, [](int v) {return v == 'F' || v == 'D' || v == 'E'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'F', 'D', 'E' }));
    }
    // inorder
    {
        auto it = search(ts::tag_inorder{}, bin, [](int v) {return v == 'D' || v == 'E' || v == 'F'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'D', 'E', 'F' }));
    }
    // postorder
    {
        auto it = search(ts::tag_postorder{}, bin, [](int v) {return v == 'D' || v == 'E' || v == 'F'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'E', 'D', 'F' }));
    }
     return 0;
}
```

#### Manually balancing tree

Binary tree may loose its efficiency in searching once it renders unbalanced. One possible option to mitigate this issue is to manually balance the tree. And that's how you declare that you'd like that capability:
```cpp
#include "tree_search/tree.hpp"
#include "tree_search/tree_balance.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_rotate.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::empty_augment
                                , tree_search::capability_insert
                                , tree_search::capability_rotate  // required for balancing to work
                                , tree_search::capability_balance_manual>;
```
Now you a free to use *balance* function once you've made any modification to your tree. Internally *balance* is an implementation of the Warren-Stout algorithm.
```cpp
int main() {
    tree<int> bin;
    insert(bin, { 7, 4, 6, 3, 8, 1 }); // initializer_list

    balance(bin);
    
    return 0;
}
```
#### Auto-balancing trees
Alternative to manual balancing is to use either AVL or Red-black tree.
The following enables AVL tree:
```cpp
#include "tree_search/tree_avl.hpp"
#include "tree_search/tree.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::avl_augment
                                , tree_search::capability_insert_avl
                                , tree_search::capability_remove_avl>;
```
And Red-black tree:
```cpp
#include "tree_search/tree_redblack.hpp"
#include "tree_search/capability_insert_redblack.hpp"
#include "tree_search/capability_remove_redblack.hpp"
#include "tree_search/tree.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::redblack_augment
                                , tree_search::capability_insert_redblack
                                , tree_search::capability_remove_redblack>;

```
Now every insert or remove will automatically balance your tree. Other capabilities as template parameters you may specify at will.

#### Interval tree
Useful if you work with intervals.
```cpp
#include "tree_search/tree_interval.hpp"
#include "tree_search/capability_search_interval.hpp"
#include "tree_search/tree.hpp"

template <typename T>
using tree = tree_search::tree<std::pair< T, T>, tree_search::interval_augment
                                               , tree_search::capability_insert_interval
                                               , tree_search::capability_remove_interval
                                               , tree_search::capability_search_interval>;
```
And for example having a set of intervals you would like to get all intervals that are within a specified range:
```cpp
int main() {
    tree<int> bin;
    insert(bin
        , { std::make_pair(15, 20)
          , std::make_pair(10, 30)
          , std::make_pair(17, 19)
          , std::make_pair(5, 20)
          , std::make_pair(12, 15)
          , std::make_pair(30, 40) });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, std::make_pair(12, 16));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(15, 20)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(5, 20)
                                                , std::make_pair(12, 15) };
        assert(res == truth);
    }
    return 0;
}
```