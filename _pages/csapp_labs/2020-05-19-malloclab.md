---
layout: post
title: Malloc Lab 解析
excerpt: "CSAPP的七个实验中的一个。模拟malloc，free和realloc"
categories: [CS]
comments: true
---



根据CSAPP原书的介绍，GNU malloc采用的是Segregated Fits方法。为了使效果尽可能好，我也打算实现Segregated Fits方法。原文还提到，在segreated free list上做first fit，效果接近于在整个堆上做best-fit，因此放置策略选择first fit。每个空闲链表使用显式空闲链表实现。

定义该堆共有`NUM_LISTS`个空闲链表，每个链表的头节点的首地址放置在堆起始处。起始也可以在堆起始处为各个空闲链表建一个虚拟头结点，能够略微提高吞吐量，但每个节点4个字的开销可能不是很划得来……`NUM_LISTS`的最佳值主要受请求块大小影响。从给出的测试tracefile来看，什么请求块大小都有。这个`NUM_LISTS`的设置就像机器学习的超参数，通过调它去（过）拟合测试集。我选择了`NUM_LISTS = 16`。

这种较底层的程序debug非常艰辛，一连串的比特位让人眼花。写一个`mm_check()`方法是很有必要的。`gcc -g`编译程序，在gdb中通过`x`操作观察指定地址存储情况，以及`call`调用`mm_check()`，能让debug过程轻松许多。以下是我使用的`mm_check()`函数，它把堆视作隐式空闲链表，从头到尾地遍历了一次堆，并且遍历了所有非空的分离空闲链表：

```c
void mm_check() {
    printf("------------- list -----------------\n");
    for (int i = 0; i < NUM_LISTS; ++i) {
        if (GET(heap_listp + i * WSIZE) != NULL) {
            printf("list %d {%d ~ %d}:", i, (4<<i), (8<<i));
            unsigned int *ptr = GET(heap_listp + i * WSIZE);
            int count = 0;      // maximum nodes. avoid infinite loop
            while (ptr != NULL && count++ < 10) {   // traverse list
                printf(" (block %x size: %d) -> ", ptr, GET_SIZE(HDRP(ptr)));
                ptr = GET(ptr + 1);
            }
            printf("\n");
        }
    }
    printf("------------- heap -----------------\n");
    // Prologue block
    unsigned int *ptr = heap_listp + (NUM_LISTS + 2) * WSIZE;
    while (GET_SIZE(HDRP(ptr))) {
        printf("block %x size: %d, allocated: %d.\n",
               ptr, GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)));
        ptr = NEXT_BLKP(ptr);
    }
    printf("Epilogue block %x size: %d, allocated: %d.\n",
           ptr, GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)));
    printf("-------------------------------------\n");
}
```

下面是一个简单的分离适配版本，首先是初始化函数。初始化函数首先申请`NUM_LISTS + 4`个字的空间，用来存放分离链表的首节点地址，以及Prologue block的头部脚部，以及Epilogue的头部（还有一个字用于padding）。然后，函数申请一个空闲块，大小为`CHUNKSIZE = 4096`字节。这个空闲块会被添加进对应分离链表中（在`extend_heap`中实现）。

```c
int mm_init(void) {
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk((NUM_LISTS + 4) * WSIZE)) == (void *)-1)
        return -1;
    
    for (int i = 0; i < NUM_LISTS; ++i) {
        PUT(heap_listp + i * WSIZE, NULL); // initialize lists
    }

    PUT(heap_listp + NUM_LISTS * WSIZE, 0); /* Alignment padding */
    PUT(heap_listp + ((NUM_LISTS + 1) * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + ((NUM_LISTS + 2) * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + ((NUM_LISTS + 3) * WSIZE), PACK(0, 1));     /* Epilogue header */

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    char *block_ptr;
    if ((block_ptr = extend_heap(CHUNKSIZE/WSIZE)) == NULL)
        return -1;
    return 0;
}
```

`mem_sbrk`和`extend_heap`的实现和书上的一样。唯一不同的是合并连续空闲块的函数`coalesce`有所改变。`coalesce`不仅要把相邻的空闲块合并，还要将被合并的空闲块从分离链表中移除，然后将合并后的大空闲块加入分离链表中。加入链表和从链表中移除的操作下面会介绍。

```c
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && !next_alloc) { // Coalesce with next
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        delete_block(NEXT_BLKP(bp));    // delete next from its list
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));
    } else if (!prev_alloc && next_alloc) { // Coalesce with prev
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        delete_block(PREV_BLKP(bp));    // delete prev from its list
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } else if (!prev_alloc && !next_alloc) { // Coalesce prev and next
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        delete_block(PREV_BLKP(bp));    // delete prev and next from free lists
        delete_block(NEXT_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    insert_block(bp);       // insert bp back to free list
    return bp;
}
```

把空闲块插入对应的分离链表，首先要确定它对应的分离链表是哪一个。`get_list_index`函数将空闲块大小`size`映射到所属大小类的下标。我的大小类设置比较简单，分别是`{1~4}, {5~8}, {9~16}, ... {2^(k-1), 2^k}`。第`NUM_LISTS`个链表存储所有大小过大的空闲块。确定对应分离链表后，找到该链表的首节点地址。如果该地址为空，说明链表暂时为空，只需要将首节点地址设为被插入的空闲块地址即可。如果首节点地址不为空，我们把空闲块插入到链表的头部，只需要把空闲块的后继改成之前的首节点，之前的首节点的前驱改成空闲块，并将链表首节点改成空闲块即可。

将空闲块从对应分离链表中移除也是同理。首先要判断它在链表中的情况（链表中的唯一节点/链表最后一个节点/链表第一个节点/链表的中间节点），根据不同情况进行不同的操作。

```c
int get_list_index(size_t size) {
    // size class: {1~4}, {5~8}, {9~16}, ...
    for (int idx = 0, end = NUM_LISTS - 1; idx < end; ++idx) {
        if (size <= (4 << idx))
            return idx;
    }
    return NUM_LISTS - 1;
}

void insert_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_list_index(size);  // index of corresponding list
    unsigned int *list_ptr = GET(heap_listp + idx * WSIZE); // address of head node of list
    if (list_ptr == NULL) { // empty list
        PUT(heap_listp + idx * WSIZE, bp); // set head of list to the added block
        PUT(bp, NULL);    // the prev and succ of the added block should be NULL
        PUT((unsigned int*) bp + 1, NULL);
    } else {    // add at head
        PUT(bp, NULL);                         // prev of added block is NULL
        PUT((unsigned int *)bp + 1, list_ptr); // succ of added block is the previous head
        PUT(list_ptr, bp);                     // prev of the previous head is added block
        PUT(heap_listp + idx * WSIZE, bp);     // set head of list to the added block
    }
}

void delete_block(void *bp) {
    // prev and succ blocks
    unsigned int *prev = GET(bp), *succ = GET((unsigned int*) bp + 1);
    int idx = get_list_index(GET_SIZE(HDRP(bp)));
    if (prev == NULL && succ == NULL) { // the only node in list
        PUT(heap_listp + idx * WSIZE, NULL);    // empty the list
    } else if (prev != NULL && succ == NULL) {  // last node in list
        PUT(prev + 1, NULL);    // set the succ of prev to NULL
    } else if (prev == NULL && succ != NULL) {  // first node in list
        PUT(succ, NULL);        // set the prev of succ to NULL
        PUT(heap_listp + idx * WSIZE, succ);    // reset the head of list
    } else { // intermediate node
        PUT(prev + 1, succ);    // connect succ and prev
        PUT(succ, prev);
    }
}
```

`mm_malloc`的代码和书上的无异，只是`find_fit`和`place`这两个函数需要实现。`find_fit`在所有可能的分离链表中（即大小类范围不小于请求块的大小）找到一个合适的空闲块（即大小不小于请求块的大小）。由于是用First Fit策略，所以只需要遍历分离链表，返回第一个找到的合适空闲块即可。`place`函数将一个空闲块分配给一个请求。如果空闲块大小减去请求大小，剩下的大小不小于4个字（足以存储头部脚部+两个链表指针），那么剩下的这段空间将被划分为一个新的空闲块。否则，将整个空闲块都分配给请求。

```c
void *mm_malloc(size_t size) {
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

void *find_fit(size_t size) {
    int idx = get_list_index(size); // index of corresponding list
    // skip empty list
    unsigned int *ptr;
    while (idx < NUM_LISTS) {
        if ((ptr = GET(heap_listp + idx * WSIZE)) != NULL) {
            // find a block of suitable size
            while (ptr != NULL) {
                if (GET_SIZE(HDRP(ptr)) >= size)
                    return ptr;
                ptr = GET(ptr + 1);     // succ
            }
        }
        ++idx;
    }
    return NULL;          // fail to find a fit
}

void place(void *bp, size_t size) {
    size_t block_size = GET_SIZE(HDRP(bp));
    delete_block(bp);
    if (block_size - size >= 2 * DSIZE) { // split block
        PUT(HDRP(bp), PACK(size, 1));     // build head and foot
        PUT(FTRP(bp), PACK(size, 1));
        bp = NEXT_BLKP(bp);
        size = block_size - size;
        PUT(HDRP(bp), PACK(size, 0)); // build head and foot
        PUT(FTRP(bp), PACK(size, 0));
        insert_block(bp); // insert to corresponding free list
    } else {    // should allocate the whole free block
        PUT(HDRP(bp), PACK(block_size, 1)); // build head and foot
        PUT(FTRP(bp), PACK(block_size, 1));
    }
}
```

`mm_free`的代码和书上的一样，没有什么特别，不一样的操作都在`coalesce`中实现了。

```c
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));   // modify the head and foot
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}
```

`mm_realloc`是最有优化空间的。根据参数的不同情况，`mm_realloc`会有不同行为。首先，如果`ptr == NULL`，它等同于`mm_malloc(size)`；其次，如果`size == 0`，它等同于`mm_free(ptr)`。如果`size`等于原有分配块的大小`old_size`或者等于`old_size - DSIZE`（相当于想把块大小缩小`DSIZE`，但是缩小的空间无法构成一个新的块。因此拒绝此操作），那么直接返回`ptr`。

如果`size < old_size`，我们就把原有分配块收缩到`size`大小。收缩很简单，因为尾部数据会被抛弃，所以只用修改头部脚部，并将收缩空间构成新的空闲块就可以了。如果`size > old_size`，我们需要放大原有分配块。最简单的方法是申请一个`size`的块，然后将原有分配块上的数据迁移过去，在释放掉原有块。但是这样的效率可能不高。

```c
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return mm_malloc(size);
    }
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        size = 2 * DSIZE;
    else
        size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    int old_size = GET_SIZE(HDRP(ptr));
    if (size == old_size || size == old_size - DSIZE)
        return ptr;     // left size is not enough to be a new free block
    if (size < old_size) {  // shrink the block
        PUT(HDRP(ptr), PACK(size, 1));
        PUT(FTRP(ptr), PACK(size, 1));
        PUT(HDRP(NEXT_BLKP(ptr)), PACK(old_size-size, 0));  // new free block
        PUT(FTRP(NEXT_BLKP(ptr)), PACK(old_size-size, 0));
        coalesce(NEXT_BLKP(ptr));
    } else {
        void *new_ptr = mm_malloc(size);
        if (new_ptr == NULL)
            return NULL;
        // copy data from old block to new block (except head and foot)
        memmove(new_ptr, ptr, old_size - DSIZE);
        mm_free(ptr);
        ptr = new_ptr;
    }
    return ptr;
}
```

至此，一个简单的分离适配实现就已经完成了。跑分如下：

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   98%    5694  0.000566 10053
 1       yes   97%    5848  0.000592  9875
 2       yes   97%    6648  0.000735  9047
 3       yes   99%    5380  0.000628  8564
 4       yes   66%   14400  0.001086 13257
 5       yes   93%    4800  0.000581  8264
 6       yes   90%    4800  0.000575  8349
 7       yes   55%   12000  0.000914 13123
 8       yes   51%   24000  0.001721 13943
 9       yes   30%   14401  0.034854   413
10       yes   30%   14401  0.001547  9311
Total          73%  112372  0.043799  2566

Perf index = 44 (util) + 40 (thru) = 84/100
```

结果比较一般。可以看到，realloc（第9 10个tracefile）的表现非常差。下面对这个实现做一些优化。

首先是First Fit，我们将它改成内存利用率更好的Best Fit。我们只需要在将空闲块放入空闲链表时，维持空闲链表的顺序（节点的大小按从小到大排）即可。在`find_fit`时，第一个匹配的块即是Best Fit。

```c
void insert_block(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_list_index(size);  // index of corresponding list
    unsigned int *list_ptr = GET(heap_listp + idx * WSIZE); // address of head node of list
    if (list_ptr == NULL) { // empty list
        PUT(heap_listp + idx * WSIZE, bp); // set head of list to the added block
        PUT(bp, NULL);    // the prev and succ of the added block should be NULL
        PUT((unsigned int*) bp + 1, NULL);
    } else {    // keep the order of list
        unsigned int *prev_ptr = NULL;     // prev of list_ptr
        while (list_ptr && GET_SIZE(HDRP(list_ptr)) < size) {
            prev_ptr = list_ptr;
            list_ptr = GET(list_ptr + 1); // next node
        }
        if (list_ptr) {
            if (prev_ptr)
                PUT(prev_ptr + 1, bp);             // next of prev_ptr is added block
            PUT(bp, prev_ptr);                     // prev of added block is prev_ptr
            PUT((unsigned int *)bp + 1, list_ptr); // succ of added block is list_ptr
            PUT(list_ptr, bp);                     // prev of list_ptr is added block
            if (GET(heap_listp + idx * WSIZE) == list_ptr) // update head
                PUT(heap_listp + idx * WSIZE, bp);
        } else { // add block at the tail of list
            PUT(prev_ptr + 1, bp);                // succ of original tail is added block
            PUT(bp, prev_ptr);                    // prev of added block is original tail
            PUT((unsigned int *)bp + 1, NULL);    // next of added block is NULL
        }
    }
}
```

得分稍稍提高，结果为85分，前几个tracefile的util上升到99%甚至100%了。然后，`mm_realloc`也需要优化一下。如果`ptr`临近的空闲块（前驱后继都可以）和`ptr`块合起来大小能超过`size`，那么就不必申请新的空间，只需要把临近的空闲块和块`ptr`合并起来就可以。合并的代码如下：

```c
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return mm_malloc(size);
    }
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        size = 2 * DSIZE;
    else
        size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    int old_size = GET_SIZE(HDRP(ptr));
    if (size <= old_size) { // do nothing
        return ptr;
    } else {
        void *next_ptr = NEXT_BLKP(ptr), *prev_ptr = PREV_BLKP(ptr);
        size_t next_alloc = GET_ALLOC(HDRP(next_ptr)),
               next_size = GET_SIZE(HDRP(next_ptr)),
               prev_alloc = GET_ALLOC(HDRP(prev_ptr)),
               prev_size = GET_SIZE(HDRP(prev_ptr));

        if (!next_alloc && next_size + old_size >= size) { // allocate the next free block
            delete_block(next_ptr);
            old_size += next_size;
            PUT(HDRP(ptr), PACK(old_size, 1));
            PUT(FTRP(ptr), PACK(old_size, 1));
        } else if (!prev_alloc && prev_size + old_size >= size) { // allocate the prev free block
            delete_block(prev_ptr);
            memmove(prev_ptr, ptr, old_size);
            old_size += prev_size;
            ptr = prev_ptr;
            PUT(HDRP(ptr), PACK(old_size, 1));
            PUT(FTRP(ptr), PACK(old_size, 1));
        } else if (!next_alloc && !prev_alloc && prev_size + old_size + next_size >= size) {
            // allocate both prev and next
            delete_block(prev_ptr), delete_block(next_ptr);
            memmove(prev_ptr, ptr, old_size);
            old_size += prev_size + next_size;
            ptr = prev_ptr;
            PUT(HDRP(ptr), PACK(old_size, 1));
            PUT(FTRP(ptr), PACK(old_size, 1));
        } else {    // must move to another place
            void *new_ptr = mm_malloc(size);
            if (new_ptr == NULL)
                return NULL;
            // copy data from old block to new block (except head and foot, but it doesn't matter)
            memmove(new_ptr, ptr, old_size);
            mm_free(ptr);
            ptr = new_ptr;
        }
    }
    return ptr;
}
```

可以看到，我把收缩`ptr`块的部分去掉了……如果没有去掉，分数只有86分，仅提高1分，而去掉了这段代码，分数就提高到了91分……不明白是为什么。在之前那个简单的`mm_realloc`上去掉这部分，分数并没有提高（-_-||）。

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.000228 24996
 1       yes   99%    5848  0.000224 26084
 2       yes   99%    6648  0.000264 25191
 3       yes  100%    5380  0.000211 25486
 4       yes   66%   14400  0.000357 40381
 5       yes   96%    4800  0.000365 13165
 6       yes   95%    4800  0.000362 13245
 7       yes   55%   12000  0.000371 32389
 8       yes   51%   24000  0.000827 29035
 9       yes   97%   14401  0.000215 67044
10       yes   75%   14401  0.000179 80408
Total          85%  112372  0.003602 31201

Perf index = 51 (util) + 40 (thru) = 91/100
```

书本上还提到一种优化：如果上一个块（地址连续）是分配块，就在当前块的头部标记（头部为`size | alloc`，由于`size >= 4 * WIZE`，在32位系统下最低三位是没有利用的。最低位用于`alloc`，次低位可以用来标记上一个块是否分配）。通过这个标记位，可以在合并相邻空闲块时判断上一个块是否为空闲块。如果是，则利用上一个空闲块的脚部进行合并。如果不是，则不用管上一个块。因此，实际上分配块是不需要脚部的。通过这个优化，可以另分配块节省脚部的空间。我在实现了这个优化以后，发现效果不是很好。按[这个人](https://blog.csdn.net/pku_zzy/article/details/53483978)的说法，要让得分超过98分最好是使用BST的实现。由于没有了解过，这次实验就到此为止了。总得来说，这次实验还是比较困难的。主要是debug太繁琐了……累得仿佛多年后正在加班的我。





