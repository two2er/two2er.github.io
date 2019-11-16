---
layout: post
title: LeetCode-链表
excerpt: "LeetCode问题的解答：链表"
categories: [Algorithm]
comments: true

---

## 160. Intersection of Two Linked Lists 

< https://leetcode.com/problems/intersection-of-two-linked-lists/ >

>  Write a program to find the node at which the intersection of two singly linked lists begins. 

这道题在[牛客网面经]( https://www.nowcoder.com/discuss/328830?type=2&order=0&pos=10&page=4 )上见到了。比较暴力的做法是，把其中一个链表连成一个环，然后从另一个链表的起点处出发，找环的入口，如同[剑指Offer第55题]( https://two2er.github.io/pages/jianzhi/2019-09-29-linkedlist)一样。但是，这道题有另一种巧妙得多的方法。假设两个指针`p1`和`p2`分别从两个链表的起点处出发，每次前进一个节点。在到达链表尾部时，跳转到另一个链表的起点处继续走。如此一来，如果这两个链表有交点的话，它们相遇的地方一定是交点。假设第一个链表不重叠的部分长度为`A`，第二个链表不重叠部分的长度为`B`，两个链表重叠部分的长度为`C`。在走了`A+B+C`步后，两个指针都会到达交点处，因为交点距离两个链表的起点距离分别是`A`和`B`。因此，在`p1`和`p2`相等时返回它们所指节点即可。如果两个链表没有交点，那它们在走完`A+B+2*C`距离后，都会到达某个链表的尾部。这时依然可以按之前的逻辑返回，因为它们是`None`值。

```python
def getIntersectionNode(self, headA, headB):
	"""
	:type head1, head1: ListNode
	:rtype: ListNode
	"""
    p1, p2 = headA, headB
    while p1 != p2:
    	p1 = p1.next if p1 else headB
    	p2 = p2.next if p2 else headA
    return p1
```

