---
layout: post
title: 剑指offer-链表
excerpt: "剑指offer问题的解答：链表"
categories: [Algorithm]
comments: true
---

## 15-反转链表

> 输入一个链表，反转链表后，输出新链表的表头。

[LeetCode相同问题](<https://leetcode.com/problems/reverse-linked-list/>)

翻转链表是很经典的题目，正常的程序员都做过它。翻转的思路很简单：用三个指针`prev, curr, next`来记录前一个操作节点、当前操作节点和下一个操作节点。每次迭代，就把当前操作节点的指针指向上一个操作节点。而因为记录了下一个操作节点，所以修改`curr.next`不会有什么副作用。修改完成后，将三个指针移到对应的下个位置，重复此操作直至链表末尾。

```python
def reverseList(self, head):
	if not head or not head.next:
	    return head
	prev, curr = None, head
	while curr:
	    nex = curr.next
	    curr.next = prev
	    prev = curr
	    curr = nex
	return prev
```

```c++
class Solution {
   public:
    ListNode *ReverseList(ListNode *pHead) {
        if (!pHead || !head.next) return pHead;
        ListNode *prev = nullptr, *curr = pHead, *next;
        while (curr) {
            next = curr->next;
            curr->next = prev;
            prev = curr, curr = next;
        }
        return prev;
    }
};
```



第二种解法也很经典：使用递归，将当前节点之后的子链表翻转后，和当前节点接上——这样从当前节点开始的链表就是一个完整的翻转链表了。

```python
def ReverseList(self, head):
	if head is None or head.next is None:
	    return head
	new_head = self.ReverseList(head.next)
	head.next.next = head
	head.next = None
	return new_head
```

```c++
class Solution {
   public:
    ListNode *ReverseList(ListNode *pHead) {
        if (!pHead || !pHead->next) return pHead;
        ListNode *new_head = ReverseList(pHead->next);
        pHead->next->next = pHead;
        pHead->next = nullptr;
        return new_head;
    }
};
```



顺便吐槽一句：牛客网的在线编程框太烂了，甚至比本科使用的课程OJ还烂，讨论区质量也很低。不过毕竟只有67题，暂时忍一下。或者移步到LeetCode的相似问题做提交。



---



## 14-链表中倒数第k个结点

> 输入一个链表，输出该链表中倒数第k个结点。

这个类型的题我也是见过很多次了。使用两个指针`ptr1, ptr2`，让`ptr2`先走`k`步，这样`ptr1`和`ptr2`的距离就是`k`个节点。然后，`ptr1`和`ptr2`一起前进，等`ptr2`到达尾后时，`ptr1`离末尾节点的距离是`k-1`个节点，因此`ptr1`指的就是倒数第`k`个节点了。

```python
def FindKthToTail(self, head, k):
	ptr1, ptr2 = head, head
	for _ in range(k):
	    if not ptr2:
	        return None
	    ptr2 = ptr2.next
	while ptr2:
	    ptr1 = ptr1.next
	    ptr2 = ptr2.next
	return ptr1
```

```c++
class Solution {
   public:
    ListNode *FindKthToTail(ListNode *pListHead, unsigned int k) {
        ListNode *ptr1 = pListHead, *ptr2 = pListHead;
        for (int i = 0; i < k; ++i) {
            if (!ptr1) return nullptr;
            ptr1 = ptr1->next;
        }
        while (ptr1) ptr1 = ptr1->next, ptr2 = ptr2->next;
        return ptr2;
    }
};
```



---



## 16-合并两个或k个有序链表

> 输入两个单调递增的链表，输出两个链表合成后的链表，当然我们需要合成后的链表满足单调不减规则。

[LeetCode相同问题](<https://leetcode.com/problems/merge-two-sorted-lists/>)

首先创建一个虚拟的头节点（简化代码），然后用两个指针来分别扫描两个链表。每次迭代中，将当前节点值小的节点添加到头结点跟着的链表中，并让指针前进一位。如果有一个指针到头了，那么就将另外一个链表剩下的节点依次添加进去。the code is quite intuitive.

```python
def Merge(self, pHead1, pHead2):
	pHead = ListNode(0)
	ptr, ptr1, ptr2 = pHead, pHead1, pHead2
	while ptr1 or ptr2:
	    if ptr1 and (not ptr2 or ptr1.val < ptr2.val):
	        ptr.next = ptr1
	        ptr1 = ptr1.next
	    else:
	        ptr.next = ptr2
	        ptr2 = ptr2.next
	    ptr = ptr.next
	return pHead.next
```

```c++
class Solution {
   public:
    ListNode *Merge(ListNode *pHead1, ListNode *pHead2) {
        ListNode *rtn = new ListNode(0), *curr = rtn;
        while (pHead1 && pHead2) {
            if (pHead1->val < pHead2->val) curr->next = new ListNode(pHead1->val), pHead1 = pHead1->next;
            else curr->next = new ListNode(pHead2->val), pHead2 = pHead2->next;
            curr = curr->next;
        }
        while (pHead1)
            curr->next = new ListNode(pHead1->val), pHead1 = pHead1->next, curr = curr->next;
        while (pHead2)
            curr->next = new ListNode(pHead2->val), pHead2 = pHead2->next, curr = curr->next;
        curr = rtn->next;
        delete rtn;
        return curr;
    }
};
```

​    

---



## 25-复杂链表的复制

> 输入一个复杂链表（每个节点中有节点值，以及两个指针，一个指向下一个节点，另一个特殊指针指向任意一个节点），返回结果为复制后复杂链表的head。（注意，输出结果中请不要返回参数中的节点引用，否则判题程序会直接返回空）

这题在讨论区中，占主流的是一种来自书上的做法：在原链表每个节点后插入一个复制节点，再根据原节点的随机关系设置复制节点的随机关系。然后将原链表和复制链表拆分。不过我利用Python的优势，使用字典来映射原节点跟复制节点，这样就不用在同一个链表上操作了。

```python
def Clone(self, pHead):
	if not pHead:
	    return None
	root = RandomListNode(pHead.label)
	maps = {pHead: root, None: None}
	ptr1, ptr2 = pHead, root
	while ptr1.next:
	    ptr2.next = RandomListNode(ptr1.next.label)
	    ptr1, ptr2 = ptr1.next, ptr2.next
	    maps[ptr1] = ptr2
	ptr1, ptr2 = pHead, root
	while ptr2:
	    ptr2.random = maps[ptr1.random]
	    ptr1, ptr2 = ptr1.next, ptr2.next
	return root
```

另外还要吐槽一下牛客网：这样的[代码](<https://www.nowcoder.com/profile/689521/codeBookDetail?submissionId=9315265>)为什么能通过测试？答案完全错误（用一个单节点链表、根节点的`random`自指测试一下就知道了），而且我将无用的第15行删去后提交，居然说内存越界？这不是Python吗，报错说内存越界？牛客网的OJ真的做得太一般了。

```c++
class Solution {
   public:
    RandomListNode *Clone(RandomListNode *pHead) {
        if (!pHead) return nullptr;
        RandomListNode *curr = pHead;
        while (curr) {
            RandomListNode *tmp = new RandomListNode(curr->label);
            tmp->next = curr->next, curr->next = tmp;
            curr = tmp->next;
        }
        RandomListNode *rtn = pHead->next, *curr2 = rtn;
        curr = pHead;
        while (curr) {
            if (curr->random) curr2->random = curr->random->next;
            curr = curr->next->next;
            if (curr) curr2 = curr->next;
        }
        curr = pHead, curr2 = rtn;
        while (curr) {
            if (curr2->next) {
                curr->next = curr2->next, curr2->next = curr2->next->next;
            } else {
                curr->next = nullptr;
            }
            curr = curr->next, curr2 = curr2->next;
        }
        return rtn;
    }
};
```



---



## 36-两个链表的第一个公共结点

> 输入两个链表，找出它们的第一个公共结点。

假设我们从第一个链表的`head`出发，走到末尾时跳转到第二个链表的`head`，直到第一个公共节点停下，走的路程是`len1 + com2`，其中`len1`是链表1的长度，`com2`是链表2 `head`到第一个公共节点距离。然后，我们从第二个链表`head`出发，走到末尾时跳转到第一个链表的`head`，直到第一个公共节点停下，走的路程是`len2 + com1`，`com1` 是链表1 `head`到第一个公共节点的距离。容易看出来 `len1 + com2 == len2 + com1`，所以在这种走法下，最终它们会在公共节点处相遇。

```c++
class Solution {
   public:
    ListNode *FindFirstCommonNode(ListNode *pHead1, ListNode *pHead2) {
        if (!pHead1 || !pHead2) return nullptr;
        ListNode *curr1 = pHead1, *curr2 = pHead2;
        while (curr1 != curr2) {
            curr1 = curr1->next, curr2 = curr2->next;
            if (!curr1 && !curr2) return nullptr;
            if (!curr1) curr1 = pHead2;
            if (!curr2) curr2 = pHead1;
        }
        return curr1;
    }
};
```



---



## 55-链表中环的入口结点

> 给一个链表，若其中包含环，请找出该链表的环的入口结点，否则，输出null。

[LeetCode相似题目](<https://leetcode.com/problems/linked-list-cycle/>)

我以前知道快慢指针（核心：一个每次走两步的快指针，和一个每次走一步的慢指针，一起在一个环里运动。如果快指针看做是跟在慢指针后面跑，那么每次运动它们之间的距离都会少1。因此它们终将相遇），但是我不知道如何求环的入口点。下面是一个转自[牛客网评论区](`http:``//kekecv.com/2016/06/08/Linked-List-Cycle-%E5%88%A4%E6%96%AD%E9%93%BE%E8%A1%A8%E6%98%AF%E5%90%A6%E6%9C%89%E7%8E%AF%EF%BC%8C%E5%A6%82%E6%9E%9C%E6%9C%89%E7%8E%AF%EF%BC%8C%E6%89%BE%E5%88%B0%E7%8E%AF%E7%9A%84%E5%85%A5%E5%8F%A3/`)的解释：

> ![img](https://uploadfiles.nowcoder.com/images/20170311/646147_1489199577531_5B1EFEB8A03B04A4781BC78AC00B89CE)
>
> 假设x为环前面的路程（黑色路程），a为环入口到相遇点的路程（蓝色路程，假设顺时针走）， c为环的长度（蓝色+橙色路程）
>
> 当快慢指针相遇的时候：
>
> 此时慢指针走的路程为 S_slow = x + m * c + a
> 快指针走的路程为 S_fast = x + n * c + a
> 2 S_slow = S_fast
> 2 * ( x + m*c + a ) = (x + n *c + a)
> 从而可以推导出：
> x = (n - 2 * m )*c - a
> = (n - 2 *m -1 )*c + c - a
> 即环前面的路程 = 数个环的长度（为可能为0） + c - a
> 什么是c - a？这是相遇点后，环后面部分的路程。（橙色路程）
> 所以，我们可以让一个指针从起点A开始走，让一个指针从相遇点B开始继续往后走，
> 2个指针速度一样，那么，当从原点的指针走到环入口点的时候（此时刚好走了x）
> 从相遇点开始走的那个指针也一定刚好到达环入口点。
> 所以2者会相遇，且恰好相遇在环的入口点。

```python
def EntryNodeOfLoop(self, pHead):
	slow, fast = pHead, pHead
	while fast and fast.next:
	    slow, fast = slow.next, fast.next.next
	    if slow == fast:
	        ptr = pHead
	        while slow != ptr:
	            slow, ptr = slow.next, ptr.next
	        return ptr
	return None
```

```c++
class Solution {
   public:
    ListNode *EntryNodeOfLoop(ListNode *pHead) {
        ListNode *fast = pHead, *low = pHead;
        while (fast && fast->next) {
            fast = fast->next->next, low = low->next;
            if (fast == low) {
                ListNode *fast = pHead;
                while (fast != low) fast = fast->next, low = low->next;
                return fast;
            }
        }
        return nullptr;
    }
};
```



---



## 56-删除链表中重复的结点

> 在一个排序的链表中，存在重复的结点，请删除该链表中重复的结点，重复的结点不保留，返回链表头指针。 例如，链表1->2->3->3->4->4->5 处理后为 1->2->5

题目表述不太清楚，重复节点是跟在一起的。创建一个虚拟头节点。在原链表中，如果发现一个节点的值，和它下一个节点的值一样，那么就让指针前进，直到到达一个值不等于先前节点值的节点。这些跳过的节点是要被删去的。剩下那些不被删去的，就和虚拟头结点串联，然后返回由虚拟头结点下个节点做头的链表就可以了。

```python
def deleteDuplication(self, pHead):
	if not pHead:
	    return None
	vHead = ListNode(0)
	vptr = vHead
	while pHead:
	    if not pHead.next or pHead.val != pHead.next.val:
	        vptr.next = pHead
	        vptr = vptr.next
	    tmp_val = pHead.val
	    while pHead and pHead.val == tmp_val:
	        pHead = pHead.next
	vptr.next = None
	return vHead.next
```

```c++
class Solution {
   public:
    ListNode *deleteDuplication(ListNode *pHead) {
        if (!pHead) return nullptr;
        ListNode *vHead = new ListNode(0), *vptr = vHead, *tptr;
        vHead->next = pHead;
        int tmp;
        
        while (vptr->next) {
            if (vptr->next->next && vptr->next->val == vptr->next->next->val) {
                tmp = vptr->next->val;
                while (vptr->next && (vptr->next->val == tmp)) {
                    tptr = vptr->next, vptr->next = vptr->next->next;
                    delete tptr;
                }
            } else {
                vptr = vptr->next;
            }
        }
        
        pHead = vHead->next;
        delete vHead;
        return pHead;
    }
};
```

