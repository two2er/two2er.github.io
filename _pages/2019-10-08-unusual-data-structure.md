---
layout: post
title: 较不常见数据结构总结
excerpt: "你听过但不熟的数据结构/算法。"
categories: [Algorithm]
comments: true

---

---

### 异或链表

[维基百科]([https://zh.wikipedia.org/wiki/%E5%BC%82%E6%88%96%E9%93%BE%E8%A1%A8](https://zh.wikipedia.org/wiki/异或链表))

**异或链表**是双向链表的一种变体。它并不需要像双向链表那样，每个节点保存两个地址，一个指向前节点一个指向后节点。它只需要保存前节点地址和后节点地址的异或值。在从前向后遍历中，已知两个相邻节点的地址（用B和C代表前后两个节点）。C的异或值和B的地址异或，就得到了C下一个节点的地址。因此，遍历能够继续进行。异或链表的缺点就是必须知道相邻至少两个节点的地址，才能完成对链表的遍历。



### Trie树

[维基百科](https://zh.wikipedia.org/wiki/Trie)

![Trie Tree](https://two2er.github.io/img/unusual_algo/trie_tree.png)

**Trie树**翻译叫前缀树、字典树，可以用来快速地检索单词。Trie树非常简单易懂，每个节点对应一个字母，每个字典里的单词都是Trie树从根节点都某个叶子节点路径上的字母的拼凑。因为Trie树的特点，它在自动补全输入单词时有很大的用处。有一题[LeetCode](<https://leetcode.com/problems/stream-of-characters/>)涉及了Trie树的构造和应用，可以看看当时[我](https://leetcode.com/problems/stream-of-characters/discuss/379232/Python-Trie-Tree.-easy-to-understand)自己是怎么做的。



