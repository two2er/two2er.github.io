---
layout: post
title: LeetCode-递归
excerpt: "LeetCode问题的解答：递归"
categories: [Algorithm]
comments: true
---

## 938. Range Sum of BST

<https://leetcode.com/problems/range-sum-of-bst/>

> Given the `root` node of a binary search tree, return the sum of values of all nodes with value between `L` and `R` (inclusive).
>
> The binary search tree is guaranteed to have unique values.

因为是二叉搜索树，所以不必遍历每一个节点。比如说，一个节点的值小于等于$L$，那么它的左子树上的节点都小于$L$，因为不必再遍历它的左子树。按照这个思路，可以在DFS时剪枝。时间复杂度平均情况下为$O(N)$，空间复杂度为$O(H)$，其中$N$为节点数，$H$为树高度。

```python
def rangeSumBST(self, root: TreeNode, L: int, R: int) -> int:
	if not root:
	    return 0
	ans = 0
	if L <= root.val <= R:
	    ans += root.val
	if root.val > L:
	    ans += self.rangeSumBST(root.left, L, R)
	if root.val < R:
	    ans += self.rangeSumBST(root.right, L, R)
	return ans
```

## 894. All Possible Full Binary Trees

<https://leetcode.com/problems/all-possible-full-binary-trees>

> A *full binary tree* is a binary tree where each node has exactly 0 or 2 children.
>
> Return a list of all possible full binary trees with `N` nodes.  Each element of the answer is the root node of one possible tree.
>
> Each `node` of each tree in the answer **must** have `node.val = 0`.
>
> You may return the final list of trees in any order.

一个FBT一定有单数个节点。所以如果给的数字$N$是偶数，那么直接返回空列表。对于一个单数$N$，首先分配$1$给根节点，然后依次给左子树分配$2M$，给右子树分配$N-1-2M$，其中$1\leq M\leq (N-1)/2$。

```python
class Solution:
    def allPossibleFBT(self, N: int):
        # in each FBT, there must be an odd number of nodes
        if N % 2 == 0:
            return []
        if N == 1:
            # only a root
            return [TreeNode(0)]
        res = []        # result
        # a root + 2*n internal nodes/leaves
        for left in range(1, N, 2):
            # we can build the left sub FBT with odd number of nodes
            left_subtree = self.allPossibleFBT(left)
            right_subtree = self.allPossibleFBT(N-1-left)
            # Cartesian product of left_subtree and right_subtree
            for ls in left_subtree:
                for rs in right_subtree:
                    root = TreeNode(0)
                    root.left = ls
                    root.right = rs
                    res.append(root)
        return res
```

时间复杂度设为$f(N)$，可以通过观察循环体，发现：

$$\begin{array}f(N)=f(1)+f(N-1-1)+f(3)+f(N-1-3)+\cdots+f(N-1-1)+f(1)\\=2[f(1)+f(3)+\cdots+f(N-2)]\\\ge 2f(N-2)\ge 2^2f(N-4)\cdots \ge 2^{N/2} \end{array}$$

因此时间复杂度的下界为$\Omega(2^N)$。空间复杂度与FST数目有关，也可以用类似的方法求出。

## 761*. Special Binary String

<https://leetcode.com/problems/special-binary-string/>

> *Special* binary strings are binary strings with the following two properties:
>
> - The number of 0's is equal to the number of 1's.
>
> - Every prefix of the binary string has at least as many 1's as 0's.
>
> Given a special string `S`, a *move* consists of choosing two consecutive, non-empty, special substrings of `S`, and swapping them. *(Two strings are consecutive if the last character of the first string is exactly one index before the first character of the second string.)*
>
> At the end of any number of moves, what is the lexicographically largest resulting string possible?

这道题我不会做，是看[这个答案](<https://leetcode.com/problems/special-binary-string/discuss/113211/JavaC%2B%2BPython-Easy-and-Concise-Recursion>)的。我在看这篇答案之后，想了一会才明白是怎么回事。我认为这道题最关键的一点，就是要清楚：通过代码中**计算count**的方法找出的`S[i:j]`如果是一个special string，那么`S[i+1:j-1]`的任何一个子串，都不会和`S[:i+1]`，`S[j:]`的子串进行交换，即，special string内部的子串不会“跑出”这个special string。这是因为：

1. special string `S[i:j]`的内部子串`S[i+1:j-1]`，仍然是一个special string。这是由于special string两个条件的限制。`S[i:j]`的首尾两个字符肯定是1和0，因为它的前缀总是要保持1的数目不小于0。同时，由于`S[i:k]`，`i < k < j`，中，1的数目总是大于0，所以`S[i+1:k]`中1的数目不小于0，满足第二个条件。且`S[i+1:j-1]`中1的数目等于0，可以推出它是一个special string。
2. `S[k:v]`，`i < k < j < v`，不可能是一个special string。假设它是一个special string，那么`S[k:j]`中1的数目不小于0。由于`S[j-1]`是0，所以`S[k:j-1]`中1的数目大于0。同时，`S[i+1:j-1]`中1的数目等于0的数目，因此`S[i:k]`中0的数目大于1。这与第二个条件矛盾。因此，`S[k:v]`不是一个special string，它就不会跟后面的special string交换，`S[k:j]`就保留在`S[i:j]`内部。`S[w:k]`，`w < i < k < j`也是同理。

因此，就可以通过计算count的方法，将`S`划分成若干special string。这些special string可以两两换位，因此它们之间的排序是随意的。只需要处理好它们的内部，然后用字典序将这些special string排好就可以了。

```python
def makeLargestSpecial(self, S: str) -> str:
	start = 0   # start of a special string
	count = 0   # count of 1s in the current special string
	res = []
	for i, c in enumerate(S):
	    count = count + 1 if c == '1' else count - 1
	    if count == 0:
	        res.append('1'+self.makeLargestSpecial(S[start+1:i])+'0')
	        start = i + 1
	return ''.join(sorted(res)[::-1])
```

