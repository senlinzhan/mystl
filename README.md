My STL
====
## 简介
	My STL 实现了 C++11 标准库的部分容器以及算法，同时又对标准库进行扩充。

## 标准库容器与算法
|  标准库容器   |       文件        |
|:-------:|:-----------------:|
|向量     |[vector.hpp](https://github.com/senlinzhan/mystl/blob/master/vector.hpp)|
|栈       |[stack.hpp](https://github.com/senlinzhan/mystl/blob/master/stack.hpp)|
|单向链表 |[forward_list.hpp](https://github.com/senlinzhan/mystl/blob/master/forward_list.hpp)|
|双向链表 |[list.hpp](https://github.com/senlinzhan/mystl/blob/master/list.hpp)|
|队列     |[queue.hpp](https://github.com/senlinzhan/mystl/blob/master/queue.hpp)|
|优先队列 |[priority_queue.hpp](https://github.com/senlinzhan/mystl/blob/master/priority_queue.hpp)|
|散列表|[unordered_set.hpp](https://github.com/senlinzhan/mystl/blob/master/unordered_set.hpp)|

| 标准库组件 |       文件        |
|:-----------:|:-----------------:|
|   迭代器     |[iterator.hpp](https://github.com/senlinzhan/mystl/blob/master/iterator.hpp)|

| 标准库算法 |       文件        |
|:-----------:|:-----------------:|
|堆算法|[heap.hpp](https://github.com/senlinzhan/mystl/blob/master/heap.hpp)|
|数值算法|[numeric.hpp](https://github.com/senlinzhan/mystl/blob/master/numeric.hpp)|
|通用算法|[algorithm.hpp](https://github.com/senlinzhan/mystl/blob/master/algorithm.hpp)|

## 线程安全容器
|   容器     |       文件         |
|:-----------:|:-----------------:|
|阻塞队列|[ThreadQueue.hpp](https://github.com/senlinzhan/mystl/blob/master/ThreadQueue.hpp)|

## 自定义容器与算法
| 自定义容器 |       文件        |
|:-------:|:-----------------:|
|二叉搜索树|[binary_tree.hpp](https://github.com/senlinzhan/mystl/blob/master/binary_tree.hpp)|
|AVL 树|[avl_tree.hpp](https://github.com/senlinzhan/mystl/blob/master/avl_tree.hpp)|


| 自定义算法 |||||       文件        |
|:-------:|:-----------------:|
|冒泡排序|选择排序|归并排序|快速排序|插入排序|[sort.hpp](https://github.com/senlinzhan/mystl/blob/master/sort.hpp)|
