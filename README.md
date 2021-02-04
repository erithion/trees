A lightweight header-only STL-like library for AVL/Red-Black/interval binary trees and more, where you could do this:

```cpp
tree<char> bin;
insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
    
auto range = traverse(tree_search::tag_postorder{}, bin);
std::string r(begin(range), end(range));
assert(r == "ACEDBHIGF");
```
For more details see below.

## Status

* Binary search tree
  * [x] Manually-balancing tree (Warren-Stout algorithm)
  * [x] Red-black tree
  * [x] AVL tree
  * [x] Interval tree (based on red-black tree)
* [x] Fenwick tree
* [x] Segment tree

## Usage

### Binary trees
<details>
 <summary>A simple but pretty useless tree</summary>
 
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
</details>

<details>
 <summary>A tree with insert/remove capabilites</summary>

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
The motivation behind such a design is to have less error-prone trees where you could not, say, accidentally remove the element(s) if the tree was initially insert-only by design, and let the compiler make sure that your decisions are followed throughout the entire code that would use your tree.
</details>

<details>
 <summary>More capabilities at hand</summary>

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
        
    auto it = search(tree_search::tag_inorder{}, bin, [](int v) {return v == 'D' || v == 'E' || v == 'F'; });
    std::vector<int> res(begin(it), end(it));
    assert(res == std::vector<int>({ 'D', 'E', 'F' }));

    return 0;
}
```
</details>

<details>
 <summary>Manually balancing tree</summary>

Binary tree looses its efficiency as a search means once it renders unbalanced. One possible option to fix this is to manually balance the tree. Below is how you might declare that you'd like that capability:
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
</details>

<details>
 <summary>Auto-balancing trees</summary>

Alternative to manual balancing is to use trees that automatically and efficiently balance themselves upon modification. In particular, AVL or Red-black tree.
The following is a type declaration of AVL tree:
```cpp
#include "tree_search/tree_avl.hpp"
#include "tree_search/tree.hpp"

template <typename T>
using tree = tree_search::tree<T, tree_search::avl_augment
                                , tree_search::capability_insert_avl
                                , tree_search::capability_remove_avl>;
```
And this is how you would declare Red-black tree type:
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
</details>

<details>
 <summary>Interval tree</summary>

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
</details>

### Fenwick tree

### Segment tree

