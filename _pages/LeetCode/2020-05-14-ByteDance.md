---
layout: post
title: LeetCode-字节跳动高频题
excerpt: "LeetCode问题的解答：字节跳动高频题"
categories: [Algorithm]
comments: true
---



![bytedance1](https://two2er.github.io/img/frequent_leetcode/bytedance1.png)

## 440. K-th Smallest in Lexicographical Order

> Given integers `n` and `k`, find the lexicographically k-th smallest integer in the range from `1` to `n`.
>
> Note: 1 ≤ k ≤ n ≤ 109.
>
> **Example:**
>
> ```
> Input:
> n: 13   k: 2
> 
> Output:
> 10
> 
> Explanation:
> The lexicographical order is [1, 10, 11, 12, 13, 2, 3, 4, 5, 6, 7, 8, 9], so the second smallest number is 10.
> ```

将`1`到`n`的数字以前缀树的形式组织：

![0_1477293053966_upload-40379731-118a-4753-bed9-1cb372790d4b](https://leetcode.com/uploads/files/1477293057263-upload-40379731-118a-4753-bed9-1cb372790d4b.png)

可以发现，所谓的字典序，其实就是前缀树的前序遍历而已。所以，要找到字典序的第`k`小的元素，即是找在前缀树前序遍历序列中第`k`个元素。假设我们现在处于元素`curr`，它离第`k`小的元素还有距离`d`。我们需要计算以`curr`为根节点的子树上的节点个数`count`。如果`count >= d`，那第`k`小的元素不会在这棵子树上，我们应该移动到以`curr+1`为根节点的子树上另做打算。如果`count < d`则说明第`k`小的元素在这棵子树上。我们令`next = curr + 1`。下面的一段代码中：

```c++
int count = 0; // count of nodes on subtree
while (curr <= n) {
    count += next - curr;
    next *= 10, curr *= 10;
}
```

在循环体中的每一步，计算了下一层中，子树拥有的节点数。在最后一层上，有可能`n < next`以致该层没有满节点，因此循环体中应该改为：

```c++
int count = 0; // count of nodes on subtree
while (curr <= n) {
    count += min(next, n+1) - curr;
    next *= 10, curr *= 10;
}
```

（注意，这段代码要计算的是，以`curr`为根节点的子树上的节点数量，而非讨论区高赞所说的“从`curr`移动到`curr+1`所需步数。以它的方法理解就会有一个逻辑上的问题：如果`curr = 9`，那么移动到`curr+1`便不是移动到右兄弟而是移动到下一层。虽然“移动”到`curr = 9`以后就实际上不可能再向右移动，但还是会带来思维上的困难）

在获取了当前子树的节点数后，如果`count < d`，那我们就向下搜索，移动到`curr`的第一个孩子。然后重复上面的操作，直到找到第`k`小的元素为止。

```c++
class Solution {
    int nodesNum(long curr, long next, int n) {
        int count = 0;
        while (curr <= n) {
            count += (n+1 > next ? next : n+1) - curr;
            curr *= 10, next *= 10;
        }
        return count;
    }
public:
    int findKthNumber(int n, int k) {
        int curr = 1, count;
        --k; // 1 is the first element
        while (k > 0) {
            count = nodesNum(curr, curr+1, n);
            if (count <= k) ++curr, k -= count; // move to curr+1
            else curr *= 10, --k;       // move to child
        }
        return curr;
    }
};
```

















