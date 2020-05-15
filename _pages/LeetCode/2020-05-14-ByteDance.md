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



## 351. 安卓系统手势解锁

> 我们都知道安卓有个手势解锁的界面，是一个 3 x 3 的点所绘制出来的网格。
>
> 给你两个整数，分别为 m 和 n，其中 1 ≤ m ≤ n ≤ 9，那么请你统计一下有多少种解锁手势，是至少需要经过 m 个点，但是最多经过不超过 n 个点的。
>
>  
>
> 先来了解下什么是一个有效的安卓解锁手势:
>
> 每一个解锁手势必须至少经过 m 个点、最多经过 n 个点。
> 解锁手势里不能设置经过重复的点。
> 假如手势中有两个点是顺序经过的，那么这两个点的手势轨迹之间是绝对不能跨过任何未被经过的点。
> 经过点的顺序不同则表示为不同的解锁手势。
>
> ![img](https://assets.leetcode.com/uploads/2018/10/12/android-unlock.png)
>
> 解释:
>
> 无效手势：4 - 1 - 3 - 6
> 连接点 1 和点 3 时经过了未被连接过的 2 号点。
>
> 无效手势：4 - 1 - 9 - 2
> 连接点 1 和点 9 时经过了未被连接过的 5 号点。
>
> 有效手势：2 - 4 - 1 - 3 - 6
> 连接点 1 和点 3 是有效的，因为虽然它经过了点 2 ，但是点 2 在该手势中之前已经被连过了。
>
> 有效手势：6 - 5 - 4 - 1 - 9 - 2
> 连接点 1 和点 9 是有效的，因为虽然它经过了按键 5 ，但是点 5 在该手势中之前已经被连过了。
>
> 示例:
>
> 输入: m = 1，n = 1
> 输出: 9

这是一个DFS回溯问题。假设当前所处点为`curr`，且已经经过的点为`len`。如果`len < n`，我们需要找到下一个点`next`，作为DFS的下一个访问目标。这道题的难点就在于如何根据当前点判断下一个点是否有效。总得来说，`next`能分以下几种情况：

- `next`已经被访问过，无效。
- `next`和`curr`相邻（上下左右 左上左下右上右下），有效。
- `next`和`curr`之间的移动是国际象棋中马的移动，即日字。有效。
- `next`和`curr`连线穿过第三个点，这个点被访问过->有效；这个点还未被访问过->无效。

```c++
class Solution {
    int numberOfPatternsByLength(int curr, int len, vector<bool> &visited) {
        // last visited node is curr, the length of path going to visit is len
        if (len == 0) return 1; // end of path
        int rtn = 0;
        for (int i = 0; i < 9; ++i) {
            if (isValid(curr, i, visited)) {
                visited[i] = true;
                rtn += numberOfPatternsByLength(i, len-1, visited);
                visited[i] = false;
            }
        }
        return rtn;
    }
    bool isValid(int curr, int next, vector<bool> &visited) {
        // whether next is the valid point next to curr
        if (visited[next]) return false;
        if (curr == -1) return true;    // first point
        // knight moves or adjacent cells (in a row or in a column)
        if ((curr + next) % 2) return true;
        // curr and next are at 0 & 8 or 3 & 7
        int mid = (curr + next) / 2;
        if (mid == 4) return visited[mid];
        // adjacent cells (on diagonal)
        if ((curr % 3 != next % 3) && (curr / 3 != next / 3)) return true;
        return visited[mid];
    }
public:
    int numberOfPatterns(int m, int n) {
        vector<bool> visited(9, false);
        int rtn = 0;
        for (int len = m; len <= n; ++len) {
            rtn += numberOfPatternsByLength(-1, len, visited);
        }
        return rtn;
    }
};
```



## 726. Number of Atoms

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
>
> **Example 1:**
>
> ```
> Input: 
> formula = "H2O"
> Output: "H2O"
> Explanation: 
> The count of elements are {'H': 2, 'O': 1}.
> ```
>
> 
>
> **Example 2:**
>
> ```
> Input: 
> formula = "Mg(OH)2"
> Output: "H2MgO2"
> Explanation: 
> The count of elements are {'H': 2, 'Mg': 1, 'O': 2}.
> ```
>
> 
>
> **Example 3:**
>
> ```
> Input: 
> formula = "K4(ON(SO3)2)2"
> Output: "K4N2O14S4"
> Explanation: 
> The count of elements are {'K': 4, 'N': 2, 'O': 14, 'S': 4}.
> ```

这道题和[LeetCode 394](https://leetcode.com/problems/decode-string/)有点类似，使用栈来保存之前尚未添加进结果集的元素，并用一个有序表来保存在当前层（和当前访问元素在同个括号内的元素算同一层）的元素。过程比较繁琐，尽量直接看代码：

```c++
class Solution {
public:
    string countOfAtoms(string s) {
        stack<map<string, int>> st;
        map<string, int> curr;
        int i = 0, count = 0; // index of current char, count of current element
        // name of current element
        string name;
        while (i < s.length()) {
            if (s[i] >= 'A' && s[i] <= 'Z') {
                // get element name
                name = s[i++];
                if (i < s.length() && s[i] >= 'a' && s[i] <= 'z') {
                    name += s[i++];
                }
                // no decimal number -> count of element is 1
                if (i >= s.length() || (i < s.length() && (s[i] < '0' || s[i] > '9')))
                    ++curr[name];
            } else if (s[i] >= '0' && s[i] <= '9') {
                // count of current element
                count = s[i++] - '0';
                while (s[i] >= '0' && s[i] <= '9')
                    count = count * 10 + (s[i++] - '0');
                curr[name] += count;
            } else if (s[i] == '(') {
                // next level. push current level into stack
                st.push(curr), curr.clear(), ++i;
            } else if (s[i] == ')') {
                ++i;
                // count of current level
                count = 0;
                while (s[i] >= '0' && s[i] <= '9')
                    count = count * 10 + (s[i++] - '0');
                if (count > 0) {
                    for (map<string, int>::iterator it = curr.begin(); it != curr.end(); ++it)
                        it->second *= count;
                }
                // append current level to previous level
                for (map<string, int>::iterator it = st.top().begin(); it != st.top().end(); ++it)
                        curr[it->first] += it->second;
                st.pop();
            }
        }
        // empty the stack
        if (!st.empty())
            for (map<string, int>::iterator it = st.top().begin(); it != st.top().end(); ++it)
                curr[it->first] += it->second;
        // the result string
        string rtn = "";
        for (map<string, int>::iterator it = curr.begin(); it != curr.end(); ++it) {
            rtn += it->first;
            if (it->second > 1) 
                rtn += to_string(it->second);
        }
            
        return rtn;
    }
};
```



## 135. Candy

> There are *N* children standing in a line. Each child is assigned a rating value.
>
> You are giving candies to these children subjected to the following requirements:
>
> - Each child must have at least one candy.
> - Children with a higher rating get more candies than their neighbors.
>
> What is the minimum candies you must give?
>
> **Example 1:**
>
> ```
> Input: [1,0,2]
> Output: 5
> Explanation: You can allocate to the first, second and third child with 2, 1, 2 candies respectively.
> ```
>
> **Example 2:**
>
> ```
> Input: [1,2,2]
> Output: 4
> Explanation: You can allocate to the first, second and third child with 1, 2, 1 candies respectively.
>              The third child gets 1 candy because it satisfies the above two conditions.
> ```

这是一个拓扑排序问题。对于当前小朋友`i`，如果`ratings[i] > ratings[i-1]`，那么`i`获得的糖果数量就依赖于`i-1`获得的糖果数，要比后者多。这就像一个拓扑排序，`i`必须排在`i-1`后面。计算最少的糖果数，其实就是计算拓扑排序图中有多少层，以及每层有多少人。真的按拓扑排序的思路做，可以解决这个问题，不过耗时会比较久。更好的方法是这个[答案](https://leetcode.com/problems/candy/discuss/42794/Simple-O(n)-Java-solution-with-comments)里提到的，从左到右遍历一趟`ratings`，计算每个小朋友最低能获得多少个糖果。再从右到左遍历一趟，更新小朋友们能获得的最低糖果数。其实这是我上面拓扑排序思维的一个变种。链接下面评论有人称其为“wave”，我觉得很形象。

```c++
class Solution {
public:
    int candy(vector<int>& ratings) {
        vector<int> nums(ratings.size(), 1);
        for (int i = 1; i < ratings.size(); ++i) {
            if (ratings[i] > ratings[i-1]) nums[i] = nums[i-1] + 1;
        }
        for (int i = ratings.size() - 2; i >= 0; --i) {
            if (ratings[i] > ratings[i+1]) nums[i] = nums[i] > (nums[i+1] + 1) ? nums[i] : (nums[i+1] + 1);
        }
        return accumulate(nums.begin(), nums.end(), 0);
    }
};
```













