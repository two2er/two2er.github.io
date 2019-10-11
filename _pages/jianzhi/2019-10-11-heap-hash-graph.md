---
layout: post
title: 剑指offer-堆&哈希表&图
excerpt: "剑指offer问题的解答：堆&哈希表&图"
categories: [Algorithm]
comments: true
---


## 29-最小的K个数

> 输入n个整数，找出其中最小的K个数。例如输入4,5,1,6,2,7,3,8这8个数字，则最小的4个数字是1,2,3,4,。

这道题使用Python，可以很方便地一行解决（不过由于牛客网无聊的测试用例加到了三行啦……）：

```python
class Solution:
    def GetLeastNumbers_Solution(self, tinput, k):
        if len(tinput) < k:
            return []
        return sorted(tinput)[:k]
```

或者也可以用堆来做。

```python
import heapq
class Solution:
    def GetLeastNumbers_Solution(self, tinput, k):
        if len(tinput) < k:
            return []
        heap = []
        for each in tinput:
            heapq.heappush(heap, each)
        rtn = []
        for _ in range(k):
            rtn.append(heapq.heappop(heap))
        return rtn
```



---



## 34-第一个只出现一次的字符

> 在一个字符串(0<=字符串长度<=10000，全部由字母组成)中找到第一个只出现一次的字符,并返回它的位置, 如果没有则返回 -1（需要区分大小写）.

这道题可以使用一个哈希表来存储出现过的字符第一次出现的位置。由于题目的字符串只有'a-zA-Z' 52个大小写字母，所以用一个长度为52的一维数组就可以了。数组初始化为-1，每碰到一个字符串的单词，就将其记录到对应位置上；如果字符对应位置之前有记录过的话，说明该字符之前在字符串中已经出现过，便将其设为-2。

```python
def FirstNotRepeatingChar(self, s):
	if not s:
	    return -1
	place = [-1] * 52
	for i, c in enumerate(s):
	    idx = ord(c)-97 if 'a' <= c else ord(c) - 65 + 26
	    if place[idx] == -1:
	        place[idx] = i
	    else:
	        place[idx] = -2
	return min([p for p in place if p >= 0])
```



---



## 65-矩阵中的路径

> 请设计一个函数，用来判断在一个矩阵中是否存在一条包含某字符串所有字符的路径。路径可以从矩阵中的任意一个格子开始，每一步可以在矩阵中向左，向右，向上，向下移动一个格子。如果一条路径经过了矩阵中的某一个格子，则该路径不能再进入该格子。 例如 a b c e s f c s a d e e 矩阵中包含一条字符串"bcced"的路径，但是矩阵中不包含"abcb"路径，因为字符串的第一个字符b占据了矩阵中的第一行第二个格子之后，路径不能再次进入该格子。

这是一个很经典的回溯问题。做法很简单，对矩阵的每个位置进行DFS，尝试是否有一条路径是和字符串一样的。对走过的位置，将其设置为`None`，表示走过，不能再走到这个位置。再探索完一条道路后，将道路上的`None`设回原值，以便在其他位置继续DFS。

```python
def hasPath(self, matrix, rows, cols, path):
    def dfs(i, j, path_):
        if not path_:
            return True
        for x, y in [(-1, 0), (1, 0), (0, 1), (0, -1)]:
            ix, iy = i + x, j + y
            if rows > ix >= 0 and cols > iy >= 0:
                if matrix[ix*cols+iy] == path_[0]:
                    matrix[ix*cols+iy] = None
                    flag = dfs(ix, iy, path_[1:])
                    matrix[ix*cols+iy] = path_[0]
                    if flag:
                        return True
        return False
    matrix = list(matrix)
    for i in range(rows):
        for j in range(cols):
            if matrix[i*cols+j] == path[0]:
                matrix[i*cols+j] = None
                flag = dfs(i, j, path[1:])
                matrix[i*cols+j] = path[0]
                if flag:
                    return True

    return False
```



---



## 66-机器人的运动范围

> 地上有一个m行和n列的方格。一个机器人从坐标0,0的格子开始移动，每一次只能向左，右，上，下四个方向移动一格，但是不能进入行坐标和列坐标的数位之和大于k的格子。 例如，当k为18时，机器人能够进入方格（35,37），因为3+5+3+7 = 18。但是，它不能进入方格（35,38），因为3+5+3+8 = 19。请问该机器人能够达到多少个格子？

这道题可以用DFS探索可以达到的格子。不必真的分配内存创建一个方格矩阵，只需要记录走过的路径就可以了。每次走之前判断下个位置是否可以走，如果可以，就递归到下个位置；否则尝试其他位置。（我觉得这种难度的题真的没什么好说的……）

```python
def movingCount(self, threshold, rows, cols):
    if threshold < 0:
        return 0
    def val(i, j):
        s = str(i) + str(j)
        return sum([int(c) for c in s])
    path = set()
    def dfs(i, j):
        path.add((i, j))
        for x, y in [(0, 1), (1, 0), (0, -1), (-1, 0)]:
            ix, iy = i + x, j + y
            if rows > ix >= 0 and cols > iy >= 0:
                if val(ix, iy) <= threshold and (ix, iy) not in path:
                    dfs(ix, iy)
    dfs(0, 0)
    return len(path)
```