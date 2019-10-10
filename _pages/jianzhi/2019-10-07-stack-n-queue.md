---
layout: post
title: 剑指offer-栈&队列
excerpt: "剑指offer问题的解答：栈&队列"
categories: [Algorithm]
comments: true
---

## 05-用两个栈实现队列

> 用两个栈来实现一个队列，完成队列的Push和Pop操作。 队列中的元素为int类型。

栈是FILO（先进后出），队列是FIFO（先进先出）。对栈能进行的修改操作只有压入一个元素（push），以及弹出最后压入一个元素（pop）。而队列的pop是弹出最先进入的一个元素。如果能将栈中的元素位置倒转，那么每次pop操作时，都相当于弹出最先进入栈的元素，那么这个pop操作等同于队列的pop操作，栈也“变成”了一个队列。使用两个栈是能够翻转栈中元素的。只要将一个栈中的元素全部pop进另一个栈中，元素们在另一个栈中的次序就相当于翻转了。因此，队列可以这么实现：

1. 每次push，都将元素压入栈1中。
2. pop时，如果栈2为空，就先将栈1中元素全部倒入栈2中，相当于翻转栈1元素；然后pop栈2顶部元素。如果栈2中已有元素，则直接pop出顶部元素即可。

代码如下：

```python
class Solution:
    def __init__(self):
        self.s1, self.s2 = [], []
        
    def push(self, node):
        self.s1.append(node)
        
    def pop(self):
        if self.s2:
            return self.s2.pop()
        while self.s1:
            self.s2.append(self.s1.pop())
        return self.s2.pop()
```



---



## 20-包含min函数的栈

> 定义栈的数据结构，请在该类型中实现一个能够得到栈中所含最小元素的min函数（时间复杂度应为O（1））。

[LeetCode相同问题](<https://leetcode.com/problems/min-stack/>)

为了能在`O(1)`时间内获得栈中最小元素，只能将当前最小元素存在额外的空间里。但是，在将当前栈中最小元素`pop`出去后，栈中最小元素值会发生改变。如果这时候再来重新计算栈中最小元素值，耗费的时间就不止`O(1)`了。一个可行的办法，是在`push`进一个比当前栈中最小元素还小的元素时，将当前最小元素值也一同压入栈中。这样，在`pop`掉最小元素时，只需要从栈顶直接读出剩余元素的最小元素值即可，而不需对栈进行计算。代码如下。

我在牛客网[讨论区](<https://www.nowcoder.com/questionTerminal/4c776177d2c04c2494f2555c9fcc1e49?answerType=1&f=discussion>)看到了一种将最小元素值通过异或压缩进栈中元素的方法。这种方法让空间占用变少（不需要将最小元素值压入栈中了），但是相应地计算量会变大。总觉得意义不大，但是了解一下异或操作也是挺好的。

```python
class Solution:
    def __init__(self):
        self.min_ = float('inf')
        self.stack = []
    def push(self, node):
        if node <= self.min_:
            self.stack.append(self.min_)
            self.min_ = node
        self.stack.append(node)
    def pop(self):
        top = self.stack.pop()
        if top == self.min_:
            self.min_ = self.stack.pop()
    def top(self):
        return self.stack[-1]
    def min(self):
        return self.min_
```



---



## 21-栈的压入、弹出序列

> 输入两个整数序列，第一个序列表示栈的压入顺序，请判断第二个序列是否可能为该栈的弹出顺序。假设压入栈的所有数字均不相等。例如序列1,2,3,4,5是某栈的压入顺序，序列4,5,3,2,1是该压栈序列对应的一个弹出序列，但4,3,5,1,2就不可能是该压栈序列的弹出序列。（注意：这两个序列的长度是相等的）

[LeetCode相同问题](<https://leetcode.com/problems/validate-stack-sequences/>)

使用一个栈`stack`来模拟`pushV`和`popV`的push、pop过程。依次将`pushV`中每个元素压入`stack`中。用`i`来表示当前已经pop了的元素数量。当`stack`顶部元素和`popV[i]`相等时，必须将其pop掉。否则，压入新的元素后，必须先pop掉新压入的元素才能pop该元素。那么在`popV`中，应该是新元素排在该元素前面。但是，`popV[i]`是下一个即将被pop的元素，于此产生矛盾。所以，这个元素只能被pop掉了。

根据以上的分析，模拟栈的push和pop是很简单的。最后，如果成功在`stack`中pop掉`len(popV)`个元素的话（同时也压入了`len(pushV)`个元素），说明`pushV`和`popV`是有效的顺序。否则，`popV`不可能是`pushV`的弹出序列。

```python
def IsPopOrder(self, pushV, popV):
	stack = []
	i = 0
	for ele in pushV:
	    stack.append(ele)
	    while stack and popV[i] == stack[-1]:
	        stack.pop()
	        i += 1
	return i == len(popV)
```



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



## 44-翻转单词顺序列

> 将一个句子中的单词翻转顺序。例如，“student. a am I”变成“I am a student.”。

人生苦短，我选Python……

```python
def ReverseSentence(self, s):
    # deal with sentences with only whitespace
	if len(set(s)) == 1 and ' ' in s:
	    return s
	return ' '.join(s.split()[::-1])
```



---



## 64-滑动窗口的最大值

> 给定一个数组和滑动窗口的大小，找出所有滑动窗口里数值的最大值。例如，如果输入数组{2,3,4,2,6,2,5,1}及滑动窗口的大小3，那么一共存在6个滑动窗口，他们的最大值分别为{4,4,6,6,6,5}； 针对数组{2,3,4,2,6,2,5,1}的滑动窗口有以下6个： {[2,3,4],2,6,2,5,1}， {2,[3,4,2],6,2,5,1}， {2,3,[4,2,6],2,5,1}， {2,3,4,[2,6,2],5,1}， {2,3,4,2,[6,2,5],1}， {2,3,4,2,6,[2,5,1]}。

这道题可以用类似第20题的思想来做：记录一些关键的元素（当这些元素进入、离开窗口时，窗口内的最大值会改变）。鉴于窗口的滑动特性，可以用双向队列这一数据结构来存那些位于窗口中的关键元素。每当窗口滑到一个新元素，就将这个元素的值和队列右边的元素依次比较。如果最右边的元素比新元素小，那么最右边这个元素就应该从队列里移除（因为直到它离开窗口，它都不可能是窗口里的最大值了。它就变得不再关键，没必要留在队列里了）。反复执行这一步，直到队列为空或队列最右元素大于新元素。然后将新元素放入队列中。另外，因为窗口滑动了，所以可能队列最左的元素会离开窗口，也因此不该再存在队列里。为了方便，我们可以将关键元素的索引也一同放入队列里，这样就能很快地判断队列最左元素是否已经离开窗口。因为队列从左到右递减的特性，队列的最大值是最左元素，因此当前窗口的最大值是最左元素。每一步迭代中，代码应如下：

```python
from collections import deque
class Solution:
    def maxInWindows(self, num, size):
        if size > len(num) or size == 0:
            return []
        window = deque()
        rtn = []
        for i, n in enumerate(num):
            if window and i - size == window[0]:
                window.popleft()
                window.popleft()
            while window and window[-1] <= n:
                window.pop()
                window.pop()
            window.append(i)
            window.append(n)
            if i >= size - 1:
                rtn.append(window[1])
        return rtn
```

