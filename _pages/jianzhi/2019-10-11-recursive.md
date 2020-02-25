---
layout: post
title: 剑指offer-递归
excerpt: "剑指offer问题的解答：递归"
categories: [Algorithm]
comments: true
---



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

```c++
class Solution {
    int rows_, cols_;
    bool DFS(char *matrix, int x, int y, char *path) {
        if (!*path) return true;
        int moves[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        int ix, iy;
        for (auto move: moves) {
            ix = x + move[0], iy = y + move[1];
            if (ix >= 0 && ix < rows_ && iy >= 0 && iy < cols_ && (matrix[ix * cols_ + iy] == *path)) {
                matrix[ix * cols_ + iy] = 0;
                bool flag = DFS(matrix, ix, iy, path+1);
                matrix[ix * cols_ + iy] = *path;
                if (flag) return true;
            }
        }
        return false;
    }
   public:
    bool hasPath(char* matrix, int rows, int cols, char* str) {
        if (!*str) return false;
        rows_ = rows, cols_ = cols;
        for(int x = 0; x < rows; ++x) {
            for (int y = 0; y < cols; ++y) {
                if (*str == matrix[x*cols+y]) {
                    matrix[x * cols + y] = 0;
                    bool flag = DFS(matrix, x, y, str+1);
                    matrix[x * cols + y] = *str;
                    if (flag) return true;
                }
            }
        }
        return false;
    }
};
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

```c++
#include <set>
#include <utility>
class Solution {
    int val(int x, int y) {
        int sum = 0;
        for (int e: {x, y}) {
            while (e) {
                sum += e % 10;
                e /= 10;
            }
        }
        return sum;
    }
    std::set<std::pair<int, int>> path;
    int rows_, cols_, threshold_;
    void DFS(int x, int y) {
        int moves[][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        int ix, iy;
        for (auto move: moves) {
            ix = x + move[0], iy = y + move[1];
            if (ix >= 0 && ix < rows_ && iy >= 0 && iy < cols_ && val(ix, iy) <= threshold_ && path.find(std::make_pair(ix, iy)) == path.end()) {
                path.insert(std::make_pair(ix, iy));
                DFS(ix, iy);
            }
        }
    }
   public:
    int movingCount(int threshold, int rows, int cols) {
        if (threshold < 0) return 0;
        rows_ = rows, cols_ = cols, threshold_ = threshold;
        path = {std::make_pair(0, 0)};
        DFS(0, 0);
        return path.size();
    }
};
```



---



##  027-字符串的排列 

>  输入一个字符串,按字典序打印出该字符串中字符的所有排列。例如输入字符串abc,则打印出由字符a,b,c所能排列出来的所有字符串abc,acb,bac,bca,cab和cba。 

最基础最基础的回溯问题了吧。

```python
class Solution:
    def Permutation(self, ss):
        def recursive(chars, s):
            if not chars and s:
                return {s}
            ps = set()
            for i in range(len(chars)):
                ps.update(recursive(chars[:i]+chars[i+1:], s+chars[i]))
            return ps
        return sorted(list(recursive(list(ss), '')))
```

```c++
#include <vector>
#include <string>
#include <set>
#include <algorithm>
class Solution {
    std::set<string> rtn;
    void recursive(string left, string substr) {
        if (!left.length()) {
            rtn.insert(substr);
        }
        for (int i = 0; i < left.length(); ++i) {
            string left_ = left.substr(0, i) + left.substr(i+1, left.length());
            recursive(left_, substr + left[i]);
        }
    }
   public:
    vector<string> Permutation(string str) {
        if (!str.length()) return {};
        rtn.clear();
        recursive(str, "");
        vector<string> rtn_(rtn.begin(), rtn.end());
        std::sort(rtn_.begin(), rtn_.end());
        return rtn_;
    }
};
```

