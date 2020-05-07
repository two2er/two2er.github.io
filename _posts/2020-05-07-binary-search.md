---
layout: post
title: 二分查找的几种情形
excerpt: "对二分查找的几种返回位置进行讨论。"
categories: [CS]
comments: true

---

---



二分查找是一个很基础的算法，但是它的边界条件值得注意。

```c++
int binary_search(vector<int>& nums, int low, int high, int target) {
    int mid;
    while (low <= high) {
        mid = (low + high) >> 1;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
```

这个是最原始的二分查找版本。在发现一个`nums[mid] == target`后，立即返回；如果`nums`中不存在`target`，则返回`-1`。假如`nums`中存在多个`target`，则返回的`target`所处位置不确定。程序的思路如下（记`end = nums.size()`)：

- 循环的每一轮，都在使范围`[low, high]`缩减。
- 如果`nums`中所有数都大于/小于`target`，那么在范围大小缩减为负数后，循环停止，返回`-1`。
- 如果`nums`中存在`target`：在循环过程中，程序保证`nums[low] <= target <= nums[high]`。这意味着，对于`num in nums[0, low)`，都有`num < target`；对于`num in nums[high + 1, end)`，都有`num > target`。因此，如果`nums[mid] < target`，说明`nums[0, mid]`里面不存在`target`，需要令`low = mid + 1`。`high`同理，如果`nums[mid] > target`，说明`nums[mid, end)`中不存在`target`，需要令`high = mid - 1`。范围`[low, high]`持续在缩减，`mid`总在`[low, high]`中取，所以总有一轮会有`nums[mid] == target`。
- 如果`nums[0] < target < nums.back()`但是`nums`中不存在`target`：程序在保证`nums[low] <= target <= nums[high]`的同时不断缩减`[low, high]`。由于`nums[mid]`不可能等于`target`，所以程序会在`low > high`之后结束循环返回`-1`。

这个版本的二分查找虽然简单，但是功能不够强大。现在我们要为二分查找增加一个新的功能：在`nums`中不存在`target`时返回大于`target`的最小数的下标。

```c++
int binary_search(vector<int>& nums, int low, int high, int target) {
    int mid;
    while (low <= high) {
        mid = (low + high) >> 1;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return low;
}
```

这个版本和上一个版本仅有返回值的区别，返回值`low`指向`nums`中最小的比`target`大的数（如果这个数存在）。依旧分情况讨论：

- 如果`nums`中所有数都大于`target`，那么范围会持续以`high = mid - 1`的方式缩减。最终，`low == 0, high == -1`。`nums[low]`是`nums`中最小的数，因此也是`nums`中最小的比`target`大的数。
- 如果`nums`中所有数都小于`target`，那么范围会持续以`low = mid + 1`的方式缩减。最终，`low == end, high == end - 1`。`low`并不指向`nums`中的数，这也说明了`nums`中不存在比`target`大的数。
- 如果`nums`中存在`target`，情况和上个版本分析的一样。
- 如果`nums[0] < target < nums.back()`但是`nums`中不存在`target`：程序保证`nums[low] <= target <= nums[high]`，同时也保证对于`num in nums[0, low)`，有`num < target`；对于`num in nums[high+1, end)`，有`num > target`。在循环结束的时候，有`low = high + 1`。由于`low in [high+1, end)`，所以也有`nums[low] > target`。结合`nums`是有序的这一点，可以知道`nums[low]`是`nums`中最小的比`target`大的数。同理，`nums[high]`是`nums`中最大的比`target`小的数。

前面有提到，当`nums`中存在多个`target`时，返回的`target`是第几个是不确定的。那么是否可以让二分查找程序返回指定的`target`下标呢？答案是可以的。下面这个版本的二分查找，固定返回第一个`target`的下标：

```c++
int binary_search(vector<int>& nums, int low, int high, int target) {
    int mid;
    while (low < high) {
        mid = (low + high) >> 1;
        if (nums[mid] < target) low = mid + 1;
        else high = mid;
    }
    return low;
}
```

这个版本的二分查找和上面的很相似，区别是在`nums[mid] == target`时，令`high = mid`，而不直接返回`mid`。这样能够保证，对于`num in nums[0, low)`，有`num < target`；对于`num in nums[high, end)`，有`num >= target`。程序只有在`low == high`时才会结束循环。在结束时，`low`确定了一个边界：在它左边的数都小于`target`，在它右边（包括它）的数都不小于`target`。如果`nums`中存在多个`target`，那么`low`指向的就会是第一个`target`。

如果在`nums`中不存在`target`时，由于上面描述的`low`作为边界的特性，可以知道：`nums[low]`是`nums`中最小的大于`target`的数。显然,`nums[low-1]`也是`nums`中最大的小于`target`的数。这和上一个版本的返回值很像，但是有一个区别：在`nums`中所有数都小于`target`时，返回的会是`end - 1`而非`end`。这是循环判断条件为`low < high`而非`low <= high`导致的。这个瑕疵无法避免，因为若判断条件是`low <= high`，在`nums`中所有数都大于`target`时程序会陷入死循环。

下面代码是上面的对称版，返回最后一个`target`的下标：

```c++
int binary_search(vector<int>& nums, int low, int high, int target) {
    int mid;
    while (low < high) {
        mid = ((low + high) >> 1) + 1;
        if (nums[mid] > target) high = mid - 1;
        else low = mid;
    }
    return high;
}
```

由于是对称，`mid`应该向上取整。这里并不是严格意义的向上取整，但是只要让`mid`不超出`[low, high]`范围即可。其他的和上面的没有本质区别，不再赘述。

