---
layout: post
title:  腾讯2018春招技术类编程题汇总
excerpt: "牛客网： 腾讯2018春招技术类编程题汇总"
categories: [Algorithm]
comments: true

---

题目链接：https://www.nowcoder.com/test/10611931/summary



## 翻转数列

> 小Q定义了一种数列称为翻转数列:
> 给定整数n和m, 满足n能被2m整除。对于一串连续递增整数数列1, 2, 3, 4..., 每隔m个符号翻转一次, 最初符号为'-';。
> 例如n = 8, m = 2, 数列就是: -1, -2, +3, +4, -5, -6, +7, +8.
> 而n = 4, m = 1, 数列就是: -1, +2, -3, + 4.
> 小Q现在希望你能帮他算算前n项和为多少。

如果把`m`个数视为一块，第`2i`块的第一个数加上第`2i-1`块的第一个数为`m`，第`2i`块的第二个数加上第`2i-1`块的第二个数也为`m`……所以第`2i`块加上第`2i-1`块的和为`m^2`，刚好是`m`。`n`个数总共可以分成`n/m`块，所以和为`n/m/2*m^2 = nm/2`。注意`n`取值很大，要防止溢出。

```c++
#include <iostream>
using namespace std;

int main() {
    long long n, m;
    cin >> n >> m;
    cout << n / 2 * m;
}
```



## 纸牌游戏

> 牛牛和羊羊正在玩一个纸牌游戏。这个游戏一共有n张纸牌, 第i张纸牌上写着数字ai。
> 牛牛和羊羊轮流抽牌, 牛牛先抽, 每次抽牌他们可以从纸牌堆中任意选择一张抽出, 直到纸牌被抽完。
> 他们的得分等于他们抽到的纸牌数字总和。
> 现在假设牛牛和羊羊都采用最优策略, 请你计算出游戏结束后牛牛得分减去羊羊得分等于多少。

最优策略就是每次只取最大的牌。这么一来，先手的人会取第一大、第三大、第五大……的牌，而后手的人会取第二大、第四大……的牌。只需要排序后按顺序取就可以了。

```c++
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    int n;
    cin >> n;
    int a[n];
    for (int i = 0; i < n; ++i)
        cin >> a[i];
    sort(a, a + n, [](int p1, int p2) { return p1 > p2; });
    long long rtn = 0;
    for (int i = 1; i < n; i += 2) {
        rtn += a[i-1] - a[i];
    }
    if (n % 2 == 1) rtn += a[n-1];
    cout << rtn;
}
```



## 贪吃的小Q

> 小Q的父母要出差N天，走之前给小Q留下了M块巧克力。小Q决定每天吃的巧克力数量不少于前一天吃的一半，但是他又不想在父母回来之前的某一天没有巧克力吃，请问他第一天最多能吃多少块巧克力

这是一个二分查找问题。首先，要让第一天能吃得最多，那么之后的那些天，必须尽量吃得少。设第一天吃`x`块，第二天就只能吃`ceil(x/2)`块，第三天只能吃`ceil(ceil(x/2))`块……因此，确定了第一天吃几块后，`N`天内总共吃几块也是能确定的，设为`total_choc(x)`。因此，我们在`1`到`M`之间查找`x`，使用`total_choc(x)`和`M`比较来作为条件。

```c++
#include <iostream>
#include <algorithm>
using namespace std;

int total_choc(int n, int m) {
    int total = 0;
    while (n) {
        total += m;
        m = (m + 1) >> 1;
        --n;
    }
    return total;
}

int main() {
    int N, M;
    cin >> N >> M;

    int left = 1, right = M, mid, total;
    while (left <= right) {
        mid = (left + right) / 2;
        total = total_choc(N, mid);
        if (total == M) {
            right = mid;
            break;
        } else if (total < M) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    cout << right;
}
```



## 小Q的歌单

> 小Q有X首长度为A的不同的歌和Y首长度为B的不同的歌，现在小Q想用这些歌组成一个总长度正好为K的歌单，每首歌最多只能在歌单中出现一次，在不考虑歌单内歌曲的先后顺序的情况下，请问有多少种组成歌单的方法。

这是一个简单的01背包问题变种。由于这些歌都是不同的，所以可以把它们看做是`N = X + Y`首不同的歌。设`dp[n][k]`为用前`n`首歌正好组成总长度为`k`的歌单的方法数。我们可以只用前`n-1`首来组成长度`k`的歌单，这样方法数为`dp[n-1][k]`，也可以使用第`n`首，这样方法数为`dp[n-1][k-len(n)]`。因此，转移方程为：

```c++
dp[n][k] = dp[n-1][k] + dp[n-1][k-len(n)];
```

```c++
#include <iostream>
using namespace std;

inline int len(int n, int X, int A, int B) {
    return n < X ? A : B;
}

int main() {
    int K, A, X, B, Y;
    cin >> K >> A >> X >> B >> Y;
    int N = X + Y;

    long dp[N + 1][K + 1];
    for (int k = 1; k <= K; ++k) dp[0][k] = 0;
    for (int n = 0; n <= N; ++n) dp[n][0] = 1;

    for (int n = 1; n <= N; ++n) {
        for (int k = 1; k <= K; ++k) {
            dp[n][k] = dp[n - 1][k];
            if (k >= len(n - 1, X, A, B)) {
                dp[n][k] += dp[n - 1][k - len(n - 1, X, A, B)];
            }
            dp[n][k] %= 1000000007;
        }
    }
    cout << dp[N][K];
}
```

下面是代码的优化版，参考了[背包问题九讲](https://www.kancloud.cn/kancloud/pack/70125)。值得一提的是，原文中

> 由于只需要最后f[v]的值，倒推前一个物品，其实只要知道f[v-w[n]]即可。

是错的，应该是要知道`f[v-c[n]]`。因此，`bound`也应该为

```c++
bound=max{V-sum{c[i..n]},c[i]};
```

```c++
#include <bits/stdc++.h>
using namespace std;

inline int len(int n, int X, int A, int B) { return n < X ? A : B; }

int main() {
    int K, A, X, B, Y;
    cin >> K >> A >> X >> B >> Y;
    int N = X + Y, len_, bound;

    long dp[K + 1];
    dp[0] = 1;
    for (int k = 1; k <= K; ++k) dp[k] = 0;
    long sum = K - (A * X + B * Y);

    for (int n = 1; n <= N; ++n) {
        len_ = len(n - 1, X, A, B);
        sum += len_;
        for (int k = K; k >= sum; --k) {
            if (k >= len_) dp[k] = (dp[k] + dp[k - len_]) % 1000000007;
        }
    }
    cout << dp[K];
}
```



## 安排机器

> 小Q的公司最近接到m个任务, 第i个任务需要xi的时间去完成, 难度等级为yi。
> 小Q拥有n台机器, 每台机器最长工作时间zi, 机器等级wi。
> 对于一个任务,它只能交由一台机器来完成, 如果安排给它的机器的最长工作时间小于任务需要的时间, 则不能完成,如果完成这个任务将获得200 * xi + 3 * yi收益。
>
> 对于一台机器,它一天只能完成一个任务, 如果它的机器等级小于安排给它的任务难度等级, 则不能完成。
>
> 小Q想在今天尽可能的去完成任务, 即完成的任务数量最大。如果有多种安排方案,小Q还想找到收益最大的那个方案。小Q需要你来帮助他计算一下。

这题不会做，是一个有两个目标的贪心问题。网友给的答案也是错的。所以先空着。



## 画家小Q

> 画家小Q又开始他的艺术创作。小Q拿出了一块有NxM像素格的画板, 画板初始状态是空白的,用'X'表示。
> 小Q有他独特的绘画技巧,每次小Q会选择一条斜线, 如果斜线的方向形如'/',即斜率为1,小Q会选择这条斜线中的一段格子,都涂画为蓝色,用'B'表示;如果对角线的方向形如'\\',即斜率为-1,小Q会选择这条斜线中的一段格子,都涂画为黄色,用'Y'表示。
> 如果一个格子既被蓝色涂画过又被黄色涂画过,那么这个格子就会变成绿色,用'G'表示。
> 小Q已经有想画出的作品的样子, 请你帮他计算一下他最少需要多少次操作完成这幅画。

这是一个贪心算法问题。我们从左上角开始按从左到右、从上到下的顺序遍历画板，并记录一个虚拟的正在画的画板`visited`，初始时都为`X`。如果`board[n][m]`为`B`而`visited[n][m]`为`X`，那说明至少有一条蓝色斜线会穿过`board[n][m]`这个位置。我们便从`board[n][m]`这个地方开始画一条'\\'线。线要尽量长，直到遇到了一个黄色或无色的点为止。不需要考虑左上角的点，因为如果它们需要涂蓝色的画，早就会被涂了。`Y`同理。如果碰到了`G`，则画一条蓝线，再画一条黄线。如果我们遍历到了`board[n][m]`是一个`G`，而`visited[n][m]`上仅为`Y`，说明我们需要补一条`B`线。其他情况同理。

```c++
#include <iostream>
#include <vector>
using namespace std;

int total;

void color_B(vector<vector<char>> &board, vector<vector<int>> &visited, int n, int m, int N) {
    ++total;
    visited[n][m] += 1;
    ++n, --m;
    while (n < N && m >= 0 && (board[n][m] == 'B' || board[n][m] == 'G')) {
        visited[n][m] += 1;
        ++n, --m;
    }
}

void color_Y(vector<vector<char>> &board, vector<vector<int>> &visited, int n, int m, int N, int M) {
    ++total;
    visited[n][m] += 2;
    ++n, ++m;
    while (n < N && m < M && (board[n][m] == 'Y' || board[n][m] == 'G')) {
        visited[n][m] += 2;
        ++n, ++m;
    }
}

int main() {
    total = 0;
    int N, M;
    cin >> N >> M;
    vector<vector<char>> board(N, vector<char>(M));
    // visited[n][m] == 0 --> X
    // visited[n][m] == 1 --> B
    // visited[n][m] == 2 --> Y
    // visited[n][m] == 3 --> G
    vector<vector<int>> visited(N, vector<int>(M, 0));
    char c;
    for (int n = 0; n < N; ++n) {
        for (int m = 0; m < M; ++m) {
            cin >> board[n][m];
        }
    }
    for (int n = 0; n < N; ++n) {
        for (int m = 0; m < M; ++m) {
            if (!visited[n][m]) {
                if (board[n][m] == 'B') color_B(board, visited, n, m, N);
                else if (board[n][m] == 'Y') color_Y(board, visited, n, m, N, M);
                else if (board[n][m] == 'G') {
                    color_B(board, visited, n, m, N);
                    color_Y(board, visited, n, m, N, M);
                }
            } else if (board[n][m] == 'G') {
                if (visited[n][m] == 1) color_Y(board, visited, n, m, N, M);
                else if (visited[n][m] == 2) color_B(board, visited, n, m, N);
            }
        }
    }
    cout << total;
}
```

