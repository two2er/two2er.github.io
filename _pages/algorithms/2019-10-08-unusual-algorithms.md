---
layout: post
title: 较不常见算法总结
excerpt: "你听过但不熟的数据结构/算法。"
categories: [Algorithm]
comments: true

---

---

## Morris Traversal

[参考文章](https://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion-and-without-stack/)

**Morris Traversal**是一种时间复杂度为`O(n)`，空间复杂度为**O(1)**的中序遍历方法。它的特点是，在到达一个节点后，找它的“predecessor”：它左子树下最靠右的节点。也就是在中序遍历序列中，访问当前节点之前访问的节点。这个predecessor的右孩子应该是空的，所以可以把它设成当前节点。当遍历过程到达predecessor那里后，可以直接通过右孩子指针回到中序遍历下的下一个节点。这样就不需要额外的栈空间来存储它要返回的节点。

在Morris Traversal中，每条边最多被访问两次（一次用来找predecessor，一次用来确认predecessor已经访问过）。在最坏的情况下，有`|V|`条边被创建，其中`|V|`是节点数。

```python
# Iterative function for inorder tree traversal 
def MorrisTraversal(root):
    # Set current to root of binary tree
    current = root

    while current:

        if not current.left:
            # reach the leftmost node
            print(current.val)
            # go to the right subtree or back to root
            current = current.right
        else:
            # Find the inorder predecessor of current
            pre = current.left
            while (pre.right and pre.right != current):
                pre = pre.right
            if not pre.right:
                # Make current as right child of its inorder predecessor
                pre.right = current
                # go to next node
                current = current.left
            else:
                # we have built the link between the current node and its predecessor.
                # The only case is that we visited the predecessor in the last step,
                # and came to the current node by its right child pointer.
                # Therefore, visit current node and go to its right subtree
                pre.right = None
                print(current.val)
                current = current.right
```



## KMP算法

[参考文章](https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/)

著名的看毛片算法。**KMP算法**用来做子字符串匹配。具体地说，就是在一个字符串`s`中，找出所有与另一字符串`p`相等的子串。KMP算法能做到在`O(n)`时间复杂度里完成匹配。
KMP算法先算一个跟`p`有关的`lps`数组（longest prefix, suffix），其中`lps[i]`是`p[:i+1]`的最长的共同前缀后缀子串的长度。如果有一个子字符串是`p[:i+1]`的前缀，同时也是后缀，那么它就称为共同前缀后缀子串（简称前后缀。前后缀不能是`p[:i+1]`本身，起码要比`p[:i+1]`少一个字符。比如`abcd`的前缀有`a, ab, abc`。）这个`lps`的好处是，当我们匹配时，`p[:i]`和`s[j:j+i]`都匹配上了，但是`p[i] != s[j+i]`，我们不必回到`s[j+1]`和`p[0]`那里重新匹配过，而是向前移动`i-lps[i-1]`距离:

![KMP_1](https://two2er.github.io/img/unusual_algo/kmp_1.jpg)

因为`p[:i]`中有长度为`lps[i-1]`的前后缀，所以移动距离`i-lps[i-1]`能让`p[:i]`再次匹配上`i-lps[i-1]`个字符。（注意，移动`p`其实是移动指针`j`。`p`首部和`s[j]`对齐）为什么是移动这么多距离，而不能是更少呢？因为移动更少距离，`p[:i]`没有机会和`s[j:]`匹配上。设想一下，如果真的匹配上了，`p[:i]`应该有一个更长的最长前后缀才对。在移动完`j`以后，`s[j:j+lps[i-1]]`跟`p[:lps[i-1]]`是匹配上的，下一步我们应该考虑`s[j+lps[i-1]]`是否等于`p[lps[i-1]]`，所以我们要把`p`的指针`i`移到`lps[i-1]`。

```python
def KMPSearch(p, s):
    M = len(p)
    N = len(s)
    lps = [0] * M
    i = 0  # index for p
    # Preprocess the pattern (calculate lps[] array)
    computeLPSArray(p, M, lps)
    j = 0  # index for s
    while j < N:
        if p[i] == s[j]:
            j += 1
            i += 1
            if i == M:
                print("Found pattern at index " + str(j - i))
                i = lps[i - 1]
        else:
            # mismatch after i matches
            # Do not match lps[0..lps[i-1]] characters, they will match anyway
            if i != 0:
                i = lps[i - 1]
            else:
                # p[0] != s[j]. move j forward
                j += 1
```

在求`lps`是也可以做优化。比方说，我们已经算出`p[:i]`的`lps`值是`len`，那么如果`p[len]==p[i]`，就能确定`lps[i] = len+1`。这看图很容易看出来。

![KMP_2](https://two2er.github.io/img/unusual_algo/kmp_3.jpg)

如果`p[len]!=p[i]`，说明`p[:i+1]`的最长前后缀的前缀（下面就简称最前缀和最后缀）不是`p[:len+1]`，而是包含在`p[:len]`里。我们用a b c d来标明`p`的最前缀和最后缀上的四个长度相等子串（可能是空串）。

![KMP_3](https://two2er.github.io/img/unusual_algo/kmp_2.jpg)

我们想找一个最长的a，来使得它后一个字符是`p[i]`，并且`a+p[i]`是`p[:i+1]`的最前缀。但是呢，a b和c d是在`p[:i]`的最前后缀中，所以有`a==c`和`b==d`。这样的话，a的长度就不能超过`p[:len]`的最前缀。否则，`a!=b` -> `a!=d` -> `a+p[i]!=d+p[i]`，这个`a+p[i]`就不是`p[:i+1]`的最前缀。因此，`a`被包含在`p[:len]`的最前缀中。因此，在计算`lps[i]`时，不用再考虑其最前缀长度大于`p[:len]`最前缀长度（即`lps[len-1]`）的情况。

```python
def computeLPSArray(p, M, lps):
    len = 0  # length of the previous longest prefix suffix
    # lps[0] is always 0. the loop calculates lps[i] for i = 1 to M-1
    i = 1
    while i < M:
        if p[i] == p[len]:
            # situation in figure 2.
            len += 1
            lps[i] = len
            i += 1
        else:
            if len != 0:
                # like recursion!
                len = lps[len - 1]
            else:
                # p[i] != p[0]. no common prefix-suffix
                lps[i] = 0
                i += 1
```



Floyd-Warshall算法

**Floyd-Warshall算法**是一个动态规划的求最短路径的算法。和Dijkstra算法不同，它能求出所有顶点到所有顶点的最短距离，时间复杂度是`O(|V|^3)`，如果要用Dijkstra实现求所有顶点到所有顶点的最短路径，时间复杂度是`O(|E||V|^2)`（其中`|V|`是节点数，`|E|`是边数）。FW算法用`dist(i, j, k)`来表示，从节点`i`到节点`j`，经过编号为`1~k`的中间节点，能达到的最短路径长度。在已知所有`dist(·, ·, k-1)`的情况下，我们知道，节点`i`到`j`途径`1~k`号中间节点的最短路径有两种可能：

- 经过`k`，那么`dist(i, j, k) = dist(i, k, k-1) + dist(k, j, k-1)`。这是因为这条最短路径上，只有`1~k`号点，而点`k`只会经过一次（没有负权重边，所以路径上的边都只会经过一次）
- 不经过`k`，那么`dist(i, j, k) = dist(i, j, k-1)`

一开始，`dist(i, j, 0)`在`i`到`j`之间有边的情况下设为边权重，否则设为无穷大。然后逐步迭代：

```python
def floyd_warshall(V, E):
    N = len(V)
    dist = [[[float('inf') for _ in range(N+1)] for _ in range(N+1)] for _ in range(N+1)]
    for edge in E:
        dist[edge.start][edge.end][0] = edge.weight
    for k in range(1, N+1):
        for i in range(1, N+1):
            for j in range(1, N+1):
                dist[i][j][k] = min(dist[i][j][k-1], dist[i][k][k-1] + dist[k][j][k-1])
```



## 最大流算法

（以下复制了一些在上本科离散数学课做的笔记）

- **运输网络transport_network**是具有如下性质的一个连通有向图`N`：
  1. 存在唯一一个入度为0的结点，称为**源source**。通常对源结点标号为1。
  2. 存在唯一一个出度为0的结点，称为**汇sink**。如果`N`有n个结点，通常对汇标号n。
  3. 图N已被标号，在边`(i,j)`上的标号$C\_{ij}$是一个非负数，称为边的**容量capacity**。
     （为了简单起见，流量都往一个方向，即顶点之间只有一条有向边）
- N中的**流flow**是一个函数，使N中每条边`(i,j)`都指定一个不超过$C\_{ij}$的非负数$F\_{ij}$
- 进入结点k的边上的$F\_{ik}$之和必须等于离开结点k的边上的$F\_{kj}$之和，这称为**流的守恒conservation of flow**。离开源的流之和等于进入汇的流之和，称为**流量value of flow**，记作value(F)。

![flow_1](https://two2er.github.io/img/unusual_algo/flow_1.jpg)

（两个数字是正负超容量）

**标号算法labelling_algorithm**（找最大流）：
设`N`是有n个结点的一个网络，`G`是`N`的对称闭包。在`G`中选择一条道路和该道路上一条边`(i,j)`，如果`(i,j)`属于`N`，$e\_{ij}=C\_{ij}-F\_{ij}>0$，则称该边有正的**超容量**。如果`(i,j)`不是`N`中的边，此时如果$F\_{ji}>0$，则称`(i,j)`有超容量$e\_{ij}=F\_{ji}$，于是通过边`(i,j)`的增流将产生减少$F\_{ji}$的结果。
**算法：**

1. 设$N\_1$是通过一条具有正的超容量边与源相连接的所有结点集合，$N\_1$中每个j标记$[E\_j,1]$，其中$E\_j$是边`(1,j)`上的超容量$e\_{1j}$.
2. 设$N\_1$中的结点j是有最小节点号的结点，$N\_2(j)$表示所有未标号的（除源外）与j相连并且有正的超容量的结点集合。假设结点k属于$N\_2(j)$并且`(j,k)`是有正的超容量的边，则k标记$[E\_k,j]$，其中$E\_k$是 $E\_j$ 和 边`(j,k)`上超容量$e\_{jk}$ 的最小值。当$N\_2(j)$中所有结点都用这种方法标号后，对$N\_1$中其他结点重复该过程，设$N\_2=\cup\_{j\in N\_1}N\_2(j)$.
3. 重复2，给$N\_3,N\_4,\cdots$标号。直到下列情形之一出现：
   (i) 汇没有得到标号并且没有其他结点可以标号。 ---> 算法终止，总流就是最大流。
   (ii) 汇得到标号$[E\_n,m]$，其中$E\_n$是通过道路$\pi$到达汇的额外流量。以相反次序检查$\pi$，如果边$(i,j)\in N$，则在(i,j)上增流$E\_n$，并且减少相同量的超流量$e\_{ij}$。同时给（虚）边`(j,i)`增加超流量$E\_n$。而如果$(i,j)\not \in N$，则在`(j,i)`上减少流$E\_n$，并且增加它的超容量$E\_n$，同时在`(i,j)`上减少相同量的超容量。现在得到一个新的比前面大$E\_n$个单位的流，并回到1。

- N的**切割cut**定义为某些边的集合`K`，它的从源到汇的每条道路至少包含`K`中的一条边（就是说，一个切割把`N`分成两块，一块有源，一块有汇）。一个切割`K`的容量，即$c(K)$，是K中所有边的容量之和。

![flow_2](https://two2er.github.io/img/unusual_algo/flow_2.jpg)

任意流F的$value(F)$总不大于任意切割的$c(K)$。

- **最大流最小割定理max_flow_min_cut_theorem**：在一个网络中最大流F与该网络中最小切割容量相等。标号算法产生最大流的原因就是最大流最小割定理。

```python
from collections import defaultdict
def max_flow(node_num, edges):
    # node_num: num of nodes in the graph
    # edges[i]: {j: wj, h: wh, ...}, edges[i][j] is the capacity from i to j
    # nodes must be encoded with integers from 1 to node_num
    G = defaultdict(dict)      # transitive closure
    for i, value in edges.items():
        for j, w in value.items():
            G[i][j] = w
            G[j][i] = 0
    total_flow = 0
    while True:
        # start from N_1
        # current labels of nodes
        labels = {}
        N_next = set()
        for j in G[1].keys():
            if G[1][j] > 0:
                labels[j] = (G[1][j], 1)
                N_next.add(j)
        N_i = N_next
        N_next = set()
        while len(N_i) > 0:
            for i in N_i:
                for j in G[i].keys():
                    if j not in labels and G[i][j] > 0 and j != 1:
                        # hasn't been labelled and accessible
                        labels[j] = (min(G[i][j], labels[i][0]), i)
                        N_next.add(j)
            if node_num in labels.keys():
                break
            N_i = N_next
            N_next = set()

        if node_num in labels.keys():
            # the sink has been labelled. update the graph
            current_flow = labels[node_num][0]
            total_flow += current_flow
            a, b = labels[node_num][6], node_num
            while a != 1:
                G[a][b] -= current_flow
                G[b][a] += current_flow
                a, b = labels[a][7], a
            G[a][b] -= current_flow
            G[b][a] += current_flow
        else:
            # terminate
            return total_flow
```

在遍历图时，其实是一个BFS，可以用队列来优化一下。



## 将2维动态规划转化为1维动态规划

很多时候，2维动态规划（使用二维数组来存储中间计算值）都可以转成1维动态规划，1维动态规划可以转成常数空间复杂度。 这是因为计算`dp[i]`往往只依赖于`dp[:i]`（很多时候只依赖于`dp[i-1]`），所以用临时变量来存储上一步的结果就好了。2维动态规划转1维动态规划的也是同理，用一个一维数组来存储上一排中间计算值结果。一般来说，去掉`axis=0`那个维度比较简单。一般循环是

```
for i in range(d1):
    for j in range(d2):
        dp[i][j] = dp[i-1][j] + dp[i][j-1]
```

这样的。用一个临时数组来储存`dp[i-1]`就好了。如果想去掉`axis=1`维度，可以转置一下矩阵，反正每一步迭代，`dp`只依赖于当前位置左上方的那些位置，在计算次序上转置和不转置是一样的。
一下是一个LeetCode的[例子][https://leetcode.com/problems/wildcard-matching/]：

```python
class Solution:
    def isMatch(self, s, p):
        # dp. dp[i][j] is True if p[:j] can match s[:i]
        m, n = len(s), len(p)
        dp = [[False for _ in range(n+1)] for _ in range(m+1)]
        dp[0][0] = True
        for i in range(1, m+1):
            # empty pattern could not match any non-empty string
            dp[i][0] = False
        # only '*', '**' can match an empty string
        for j in range(1, n+1):
            if p[j-1] != '*':
                break
            dp[0][j] = True

        for i in range(1, m+1):
            for j in range(1, n+1):
                if p[j-1] != '*':
                    dp[i][j] = dp[i-1][j-1] and (p[j-1] == '?' or s[i-1] == p[j-1])
                else:
                    dp[i][j] = dp[i-1][j] or dp[i][j-1]

        return dp[m][n]
```

```python
class Solution:
    def isMatch(self, s, p):
        # dp. dp[i][j] is True if p[:i] can match s[:j]
        m, n = len(p), len(s)
        dp = [[False for _ in range(n+1)] for _ in range(m+1)]
        dp[0][0] = True
        # only '*', '**' can match an empty string
        for i in range(1, m + 1):
            if p[i - 1] != '*':
                break
            dp[i][0] = True
        for j in range(1, n+1):
            # empty pattern could not match any non-empty string
            dp[0][j] = False

        for i in range(1, m+1):
            for j in range(1, n+1):
                if p[i-1] != '*':
                    dp[i][j] = dp[i-1][j-1] and (p[i-1] == '?' or s[j-1] == p[i-1])
                else:
                    dp[i][j] = dp[i-1][j] or dp[i][j-1]

        return dp[m][n]
```



## Counting Sort 

[参考文章](https://www.geeksforgeeks.org/counting-sort/)

**Counting Sort 计数排序**使用一个数组，来计算待排序数组的元素的频数。比如说，数组`[1,3,2,2]`, 最大值为3，使用长度为4的数组`count`来统计频数。统计完后，`count[i]`表示数`i`在数组中出现的次数。对`count`每一项`count[i]`都进行：`count[i] += count[i - 1]`。这样，`count[i]`就表示数组中不大于数`i`的数的个数。因此，在排序好的数组中，数`i`应该放在位置`count[i]-1`上。

```python
def countingsort(arr):
    # can store the count of elements ranging from 0 to 10
    count = [0] * 11
    for each in arr:
        count[each] += 1
    for i in range(1, len(count)):
        count[i] += count[i - 1]
    output = [0] * len(arr)
    for each in arr:
        output[count[each] - 1] = each
        count[each] -= 1
    return output
```



## 并查集 union-find (disjoint-set)

**并查集**应该谁都知道吧……非常优美的算法。为记录而记录。

```python
class DisjointSet:
    def __init__(self, max_size):
        self.parent = list(range(max_size))
    
    def find(self, x):
        return x if self.parent[x] == x else self.find(self.parent[x])
    
    def union(self, x1, x2):
        self.parent[self.find(x1)] = self.find(x2)
    
    def is_same(self, x1, x2):
        return self.find(x1) == self.find(x2)
```



## 用栈做先序/中序/后序遍历

鉴于这个知识点还是考得挺多的，自己也没有一下子精准写出来的智商（虽然原理不难），所以就把它也写出来以供复习。

我觉得，为了让自己思维不混乱，首先要对“栈里装的是什么”这个问题有清晰的认识。

首先是先序遍历。先序遍历在访问完当前节点后，会移动到左子树，而右子树的根节点需要记录下来在栈中，等待之后访问。由于右子树们是由下至上访问的，符合栈LIFO的特质，所以把右孩子都压进栈即可。这时，栈里装的是“待遍历的（右）子树的根节点”。因此从栈中`pop`出来节点后，只需按`preorder`流程访问、压节点入栈即可。

```python
def preorderTraversal(self, root: TreeNode):
	stack = [root]
	while stack:
        # traverse the subtree whose root is node / preorder(node)
	    node = stack.pop()
	    while node:
	        if node.right:
	            stack.append(node.right)
	        visit(node)
	        node = node.left
```

中序遍历和后序遍历会稍微复杂一点。中序遍历优先访问左孩子，所以一棵子树中，最先被访问的一定是那个“最左”的节点，也就是从根节点一直向左，直到无法再左的节点。到达这个节点后，访问之，然后就去遍历它的右子树；然后，退回它的父节点，访问父节点，然后访问父节点的右子树……可以看到，这个过程中，“父节点”们是从下至上被访问的，适合储存在栈中。这时，栈中存储的是“左子树被遍历过，而根节点和右子树尚未被遍历的子树的根节点”。从栈中取出一个节点后，要先访问它，然后对它的右孩子进行`inorder`操作。为了简化代码，我们可以把“最左”节点也当成“左子树被访问过，而根节点和右子树尚未被访问的子树的根节点”，并把它们也压入栈中。

```python
def inorderTraversal(self, node: TreeNode) -> List[int]:
	stack = []
	while node or stack:
	    # if node, traverse the subtree whose root is node / inorder(node)
	    # if not node, go to stack.pop()
	    while node:
	        # go to the left most node, and store parent nodes
	        stack.append(node)
	        node = node.left
	    # now node is the left most node, or the last parent node
	    node = stack.pop()
	    # visit node, and then go to its right subtree
	    visit(node)
	    # if node.right is None, the "while node" step would just skip
	    # and the stack could pop the last parent node
	    node = node.right
```

后序遍历也是从“最左”节点开始访问。我们可以模仿中序遍历，一路直达最左节点，将路上的父节点都存在栈中。在遍历完一棵左子树后，从栈中弹出一个节点，即左子树的父节点。下一步，遍历这个父节点的右子树。这里存在一个重要的问题：父节点何时被访问呢？在遍历完左子树后，会从栈中取出父节点一次，进入它的右子树；在遍历完右子树后又需要到达父节点，访问之。这意味着被存在栈中的父节点总共需要被取出2次。因此，在第一次取出时，不能将其`pop`，第二次就无法找回它了。只有在一个父节点被访问完后，我们才能将其`pop`。另外，我们也注意到，将一个节点从栈中`pop`出来，代表着以它为根节点的子树已经完整遍历完。因此，在我们`pop`出一个节点的右孩子后，它的左、右子树都遍历完了，下一个被访问的就是它自己。它被访问完后`pop`掉。所以，如果一个栈顶节点的右孩子是上一个被`pop`的节点，那么它就会被访问，然后被`pop`；否则，意味着它的右子树还未被遍历，进入它的右子树。至此，我们可以总结：栈中存储的是“左子树被遍历过，右子树可能被遍历过，根节点尚未被访问的子树的根节点”。判断右子树是否被遍历，要看上一个被`pop`的节点是否为当前节点的右孩子。

```python
def postorderTraversal(self, node: TreeNode) -> List[int]:
	stack = []
	last = None         # last popped node
	while node or stack:
	    # if node, traverse the subtree whose root is node / postorder(node)
	    # if not node, go to stack.pop()
	    while node:
	        # go to the left most node, and store parent nodes
	        stack.append(node)
	        node = node.left
	    # now node is the left most node, or the last parent node
	    node = stack[-1]
	    # whether its right child has been visited
	    # pop all nodes whose right subtree has been traversed
	    while not node.right or node.right == last:
	        # the right child has been visited
	        self.visit(node)
	        last = stack.pop()
	        if stack:
	            # go to the last unvisited node
	            node = stack[-1]
	        else:
	            # all nodes has been visited
	            return
	    else:
	        # go to the right subtree
	        node = node.right
```





















