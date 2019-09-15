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

因此时间复杂度的下界为$\Omega(2^N)$。空间复杂度应该跟最大层数有关，应为$O(\log N)$，不是很明白为什么LeetCode的标准答案那是$O(2^N)$。