---
layout: post
title: 剑指offer-动态规划
excerpt: "剑指offer问题的解答：动态规划"
categories: [Algorithm]
comments: true
---



---



## 08-跳台阶

> 一只青蛙一次可以跳上1级台阶，也可以跳上2级。求该青蛙跳上一个n级的台阶总共有多少种跳法（先后次序不同算不同的结果）。

超级简单的动态规划问题。设`dp[i]`为跳上第`i`级的跳法。那么有：

`dp[i] = dp[i-1] + dp[i-2]`。

讨论区说是斐波那契数列，确实是。不过斐波那契数列可以看作动态规划的一个特例，用斐波那契数列来想可能能减小空间复杂度（虽然通过动态规划优化也可以）。

```python
def jumpFloor(self, number):
	dp = [0] * (number+1)
	dp[0], dp[1] = 1, 1
	for i in range(2, number+1):
	    dp[i] = dp[i-1] + dp[i-2]
	return dp[-1]
```



---



## 09-变态跳台阶

> 一只青蛙一次可以跳上1级台阶，也可以跳上2级……它也可以跳上n级。求该青蛙跳上一个n级的台阶总共有多少种跳法。

这题是上一题的延伸。既然青蛙可以跳任意级台阶，那么`dp[i]`就变成了：

`dp[i] = dp[0] + dp[1] + ... + dp[i-1]`。

```python
def jumpFloorII(self, number):
	dp = [0] * (number+1)
	dp[0], dp[1] = 1, 1
	for i in range(2, number+1):
	    dp[i] = sum(dp[:i])
	return dp[-1]
```

不过，由于

`dp[i-1] = dp[0] + dp[1] + ... + dp[i-2] = dp[i] - dp[i-1]`，

所以`dp[i] = 2 * dp[i-1]`，可以免去求和步骤。

```python
def jumpFloorII(self, number):
	dp = [0] * (number+1)
	dp[0], dp[1] = 1, 1
	for i in range(2, number+1):
	    dp[i] = 2 * dp[i-1]
	return dp[-1]
```



---



## 10-矩形覆盖

> 我们可以用2\*1的小矩形横着或者竖着去覆盖更大的矩形。请问用n个2\*1的小矩形无重叠地覆盖一个2\*n的大矩形，总共有多少种方法？

这题的代码跟青蛙跳台阶那一题是完全一样的。但是这题比较难想。设`dp[i]`为覆盖一个`2*i`的大矩形的方法数。我们已经知道了`dp[1], dp[2], ..., dp[n-1]`，在覆盖`2*n`矩形时，可以有两种选择：

1. 用一块竖着的矩形插到最右一列上。这种方式的覆盖方法有`dp[n-1]`种。
2. 用两块横着的矩形去覆盖`n-1, n`两列。这种方式的覆盖方法有`dp[n-2]`种。

因此，

`dp[i] = dp[i-1] + dp[i-2]`。

```python
def rectCover(self, number):
    if number == 0:
        return 0
    dp = [0] * (number+1)
    dp[0], dp[1] = 1, 1
    for i in range(2, number+1):
        dp[i] = dp[i-1] + dp[i-2]
    return dp[-1]
```



---



## 30-连续子数组的最大和

> 给一个数组，返回它的最大连续子序列的和。例如:{6,-3,-2,7,-15,1,2,2},连续子向量的最大和为8(从第0个开始,到第3个为止)。

删掉了题目前面半段莫名其妙的话。仍记得这题是本科学校ACM俱乐部宣讲会上介绍的问题，当时我没想出来（当时就是个弟弟）。这题可以用动态规划想，也可以用贪心算法的思路想。设`dp[i]`是以`array[i]`结尾的最大连续子序列的和。那么，在已知`dp[i-1]`后，如果`dp[i-1] <= 0`，那么以`array[i]`结尾的连续子序列不要`dp[i-1]`对应的子序列也罢，因为它只有反效果（和变小了，还不如序列中只有一个`array[i]`。因此，

`dp[i] = dp[i-1] + array[i] if dp[i-1] > 0 else array[i]`。

在计算动态规划的递推式时，其实已经用贪心算法的思维在思考了。把动态规划的代码优化一下（用两个变量来存储`dp[i-1]`和`dp[i]`），就变得更像是贪心算法了。殊途同归。

```python
def FindGreatestSumOfSubArray(self, array):
	dp = [0] * len(array)
	dp[0] = array[0]
	for i in range(1, len(array)):
	    dp[i] = dp[i-1] + array[i] if dp[i-1] > 0 else array[i]
	return max(dp)
```



---



## 52-正则表达式匹配

[LeetCode相同问题](<https://leetcode.com/problems/regular-expression-matching>)

这题我曾经在LeetCode上做过，使用了状态机的方法。由于我个人很喜欢状态机的做法，所以下面贴出当时用状态机解答问题做的（英文）笔记：

![状态机](https://two2er.github.io/img/others/state_machine.jpg)

> This version of my state machine is easy to understand. However, this is a NFA (Nondeterminism Finite Automaton). Without further optimazation, my code got Time Limit Exceeded when submitted to LeetCode. A good solution is to transform it to a DFA (Deterministic Finite Automation).
>
> Here is a DFA written by [cbmbbz](https://leetcode.com/problems/regular-expression-matching/discuss/295184/20-line-state-machine-solution-no-dp-beats-99.7):

```python
from collections import defaultdict
class Solution:
    def isMatch(self, s, p):
        transfer = defaultdict(set)
        curr_states = [0]
        for i,c in enumerate(p):
            if c=='*':
                continue
            # an id for new state
            new_state = curr_states[-1] + 1
            for state in curr_states:
                transfer[state, c].add(new_state)
            if i < len(p) - 1 and p[i + 1] == '*':  # if next c is *
                transfer[new_state, c] = {new_state}
                curr_states.append(new_state)
            else:
                curr_states = [new_state]
        success = curr_states  # final states
        curr_states = {0}
        for c in s:
            next_states = set()
            for state in curr_states:
                next_states.update(transfer[state, c] | transfer[state, '.'])
            curr_states = next_states
        return any(state in curr_states for state in success)  # check any curr_states is success
```

动态规划的做法更为直观。设`dp[i][j]`为`s[:i]`和`p[:j]`是否匹配。如果匹配`dp[i][j]`则为`True`，否则为`False`。如果`s[i] == p[j]`，那么一定是因为`p[j]`是一个字母，刚好与`s[i]`相同。这种情况下，如果`s[:i]`和`p[:j]`匹配上，那么`s[:i+1]`和`p[:j+1]`就能匹配上。即：

`dp[i+1][j+1] = dp[i][j]`。

如果`s[i] != p[j]`，那么就要看`p[j]`是什么了。如果`p[j]`是一个字母，那么`s[i]`和`p[j]`就不可能匹配上了（因为结尾部分不一样）。因此，`dp[i+1][j+1] = False`。如果`p[j]`不是字母，而是通配符`.`，那么不管`s[i]`是什么都能匹配上，因此`dp[i+1][j+1] = dp[i][j]`，只需要`s[:i]`和`p[:j]`匹配上就可以了。如果`p[j]`是`*`，那么有三种情况：

1. `x*`(`x`是`p[j-1]`)不和`s[:i+1]`中的字母匹配。那么`s[:i+1]`和`p[:j+1]`要匹配上，必须要让`s[:i+1]`和`p[:j-1]`匹配上，因此`dp[i+1][j+1] = dp[i+1][j-1]`。
2. `x*`和`s[i]`匹配，即只匹配一个字母。这种情况的前提是`x == s[i]`。满足前提后，如果`s[:i]`能和`p[:j-1]`匹配上的话，`s[:i+1]`和`p[:j+1]`就能匹配上。因此，`dp[i+1][j+1] = dp[i][j-1]`。
3. `x*`和若干个`s`中的字母匹配。在这种情况下，如果`s[:i+1]`和`p[:j+1]`匹配上了，肯定有`s[:i]`也和`p[:j+1]`能匹配上，因为`x*`能和`s`尾部若干个（大于1个）字母匹配。因此，`dp[i+1][j+1] = dp[i][j+1]`。

综上，代码如下：

```python
def isMatch(self, s: str, p: str) -> bool:
	dp = [[False] * (len(p)+1) for _ in range(len(s)+1)]
	# initialize
	dp[0][0] = True
	for j in range(1, len(p), 2):
	    if p[j] == '*':
	        dp[0][j+1] = True
	    else:
	        break
	for i in range(len(s)):
	    for j in range(len(p)):
	        if s[i] == p[j] or p[j] == '.':
	            dp[i+1][j+1] = dp[i][j]
	        elif p[j] == '*':
                case1, case2, case3 = False, False, False
	            if j > 0:
	                case1 = dp[i+1][j-1]
	                if p[j-1] == s[i] or p[j-1] == '.':
	                    case2 = dp[i][j-1]
	                    case3 = dp[i][j+1]
	            dp[i+1][j+1] = case1 or case2 or case3
            # else:
            #     # s[i-1] != p[j-1]
            #     dp[i][j] = False
	return dp[len(s)][len(p)]
```
