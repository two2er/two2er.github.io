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

因为是二叉搜索树，所以不必遍历每一个节点。比如说，一个节点的值小于等于L，那么它的左子树上的节点都小于L，因为不必再遍历它的左子树。按照这个思路，可以在DFS时剪枝。时间复杂度平均情况下为$O(N)$，空间复杂度为$O(H)$，其中$N$为节点数，$H$为树高度。

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

