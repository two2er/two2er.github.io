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

## 726. Number of Atoms

<https://leetcode.com/problems/number-of-atoms/>

> Given a chemical `formula` (given as a string), return the count of each atom.
>
> An atomic element always starts with an uppercase character, then zero or more lowercase letters, representing the name.
>
> 1 or more digits representing the count of that element may follow if the count is greater than 1. If the count is 1, no digits will follow. For example, H2O and H2O2 are possible, but H1O2 is impossible.
>
> Two formulas concatenated together produce another formula. For example, H2O2He3Mg4 is also a formula.
>
> A formula placed in parentheses, and a count (optionally added) is also a formula. For example, (H2O2) and (H2O2)3 are formulas.
>
> Given a formula, output the count of all elements as a string in the following form: the first name (in sorted order), followed by its count (if that count is more than 1), followed by the second name (in sorted order), followed by its count (if that count is more than 1), and so on.

这道题很繁琐，但是思路很清晰，遇到括号，把括号里的东西递归求解就可以了。我的编程很面向过程，因此不倡导，在[这里](<https://leetcode.com/problems/number-of-atoms/discuss/140802/Python-20-lines-very-readable-simplest-and-shortest-solution-36-ms-beats-100>)有一个使用栈的非常简洁的回答，可以学习学习，美化代码。

时间复杂度跟字符串长度有关，$O(S)$。

```python
def countOfAtoms(self, formula: str) -> str:
	def recursive(formula):
	    # if meet a formula in parentheses, count its elements recursively
	    count = {}
	    i = 0
	    while i < len(formula):
	        if i == len(formula) - 1:
	            if formula[-1] not in count:
	                count[formula[-1]] = 1
	            else:
	                count[formula[-1]] += 1
	            break
	        j = i + 1
	        if 'A' <= formula[i] <= 'Z':
	            # scan lowercase letters
	            while j < len(formula) and 'a' <= formula[j] <= 'z':
	                j += 1
	            element_name = formula[i:j]
	            # scan numbers
	            i = j
	            while j < len(formula) and '0' <= formula[j] <= '9':
	                j += 1
	            element_num = int(formula[i:j]) if j > i else 1
	            if element_name not in count:
	                count[element_name] = element_num
	            else:
	                count[element_name] += element_num
	        else:  # meet a parentheses
	            parentheses_count = 1
	            while parentheses_count > 0:
	                if formula[j] == ')':
	                    parentheses_count -= 1
	                    if parentheses_count == 0:
	                        break
	                elif formula[j] == '(':
	                    parentheses_count += 1
	                j += 1
	            sub_count = recursive(formula[i + 1:j])
	            # scan numbers
	            j += 1
	            i = j
	            while j < len(formula) and '0' <= formula[j] <= '9':
	                j += 1
	            element_num = int(formula[i:j]) if j > 1 else 1
	            # combine sub_count
	            for k, v in sub_count.items():
	                if k not in count:
	                    count[k] = element_num * sub_count[k]
	                else:
	                    count[k] += element_num * sub_count[k]
	        i = j
	    return count

	count = recursive(formula)
	sorted_count = sorted([(k, v) for k, v in count.items()], key=lambda x: x[0])
	res = ''
	for each in sorted_count:
	    res += each[0] + (str(each[1]) if each[1] > 1 else '')
	return res
```

## 698. Partition to K Equal Sum Subsets

<https://leetcode.com/problems/partition-to-k-equal-sum-subsets/>

这个题，最经典的解法，居然是DFS暴力搜索。单纯这样写代码是会超时的，看了[这位大神](<https://leetcode.com/problems/partition-to-k-equal-sum-subsets/discuss/108741/Solution-with-Reference>)的代码，意识到可以将`nums`事先排好序，再进行搜索，可以节省很多时间（因为`False`的那些测试用例会更早返回）。加上了排序的代码击败了5%的Python3代码。看了一眼讨论区，有另[一位大神](<https://leetcode.com/problems/partition-to-k-equal-sum-subsets/discuss/146579/Easy-python-28-ms-beats-99.5>)，用非常类似的代码，击败了95% （我提交的时候是75%），原因是多了这么一段

```python
if bucket[j] == bound:
	return False
```

我初看时，以为是在某个数字大于桶容量时返回，但仔细想想才发现不完全是，这是因为DFS搜索是按桶从左到右放的，前若干个桶为非空桶，后几个为空桶（如果有的话）。如果用一个空桶装`nums[i]`，而无法完美装下剩下的数字的话，那用下一个空桶装，也是不行的。而那些非空桶，因为排在空桶前面，早就试过了，肯定不行（不然早返回`True`了），因此在`bucket[j] == bound`时就能判断出这组测试用例是`False`的了。

膜拜大神……

```python
class Solution:
    def canPartitionKSubsets(self, nums: List[int], k: int) -> bool:
        sums = sum(nums)
        if len(nums) < k or sums % k != 0:
            return False
        bound = sums//k
        bucket = [bound] * k
        nums.sort(reverse=True)
        
        def dfs(i):
            if i == len(nums):
                return True
            for j in range(k):
                if nums[i] <= bucket[j]:
                    bucket[j] -= nums[i]
                    if dfs(i+1):
                        return True
                    bucket[j] += nums[i]
                if bucket[j] == bound:
                    return False
            return False
            
        return dfs(0)
```

