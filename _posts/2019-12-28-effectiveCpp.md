---
layout: post
title: Effective C++ Note
excerpt: "Effective C++笔记"
categories: [book]
comments: true
---



## 让自己习惯C++

### 条款1：视C++为一个语言联邦

C++是多重范型编程语言（multiparadigm programming language），最好视其为一个相关语言组成的联邦，而非单一语言。它主要包含四种次语言：C, Object-Oriented C++, Template C++, STL。每个次语言都有自己的规约，切换次语言时需改变策略。

> C++高效编程守则视状况而变化，取决于你使用C++的哪一部分。

### 条款2：尽量以const、enum、inline替换#define

“宁可以编译器替换预处理器”。
```c
#define ASPECT_RATIO 1.653
// ASPECT_RATIO名字不在记号表内，难以追踪、debug ->
const double ASPECT_RATIO = 1.653

enum {numTurns = 5};
int scores[numTurns];       // 常量

#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b))
int a = 5, b = 0;
CALL_WITH_MAX(++a, b);      // a被累加2次
CALL_WITH_MAX(++a, b+10);   // a被累加1次
// 行为不一致 ->
template <typename T>
inline void callWithMax(const T& a, const T& b) {
    f(a > b ? a : b);
}
```

> 对于单纯常量，最好以const对象或enums替换#define。

> 对于形似函数的宏，最好改用inline函数替换#define。

### 条款3：尽可能使用const

> 将某些东西声明为const可帮助编译器侦测出错误用法。

> 编译器强制实施bitwise  constness（成员函数不更改对象的任何非static成员变量），但是编写程序时应该使用“概念上的常量性”（conceptual constness）（在客户端侦测不出的情况下可以修改对象的某些成员（mutable））。

> 当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const版本（static_cast添加const，再const_cast去掉const）可避免代码重复。

### 条款4：确定对象使用前已先被初始化

对于没有显式指定初始值的语句：

```c
int x;
```

C++只在某些语境下保证`x`会被初始化（为0）。一般来说，C part of C++认为初始化可能会招致运行期成本，所以不保证发生初始化（比如数组）。而STL part of C++（比如vector）却有保证。最好的方式就是总是手动初始化，不要相信编译器的默认行为。

函数体内的static对象称为local static对象，其他的static对象称为non-local static对象。如果程序包含至少两个源码文件，这两个中的non-local static对象之间互相的初始化顺序是未定义的。如果一个文件中的non-local static对象依赖另一个文件中的non-local static对象，有概率有引发问题。一个解决问题的方法是使用Singleton设计模式。即以以下形式调用一个static对象：

```c
inline int& fv() {
    static int v;
    return v;
}
```

static对象从一个non-local static对象变为一个local static对象。在首次遇上local static对象的定义式时，该对象就会被初始化。以调用函数`fv`的方法代替直接调用变量`v`，能保证在调用时`v`已经完成初始化。不仅如此，如果`v`从未被实用化，它还不会被构造/析构，节省了成本。

> 为内置型对象进行手工初始化，因为C++不保证初始化它们。

> 构造函数最好使用初始化列表而不是在构造函数体中赋值。初始化列表列出的成员变量，其排列次序应该和它们在类中的声明次序相同。

> 为免除“跨编译单元之初始化次序”问题，用local-static对象代替non-local static对象。

## 构造/析构/赋值运算

### 条款5：了解C++默认编写并调用哪些函数

> 编译器可以暗自为类创建默认构造函数、拷贝构造函数、拷贝赋值运算符和析构函数。

### 条款6：若不想使用编译器自动生成的函数，就该明确拒绝

C++11允许我们定义成员函数为`=delete`来阻止编译器自动生成它们。

> 为驳回编译器自动生成默认成员函数，可以将相应成员函数声明为private并不予实现。

### 条款7：为多态基类声明virtual析构函数

如果析构函数不是虚函数，在通过delete类的动态内存对象时，由于调用的是根据指针类型（静态类型）决定的析构函数，很可能会对派生类对象调用基类的析构函数，导致“局部销毁”的诡异异常。因此，最好就在基类中将析构函数声明为虚函数。这样delete指针时使用的析构函数能由对象的动态类型决定。

在没有必要多态时，不要将类的成员函数设为虚函数，这会使得类对象耗费额外空间存放一个`vptr`（virtual table pointer）。

如果你想要一个abstract class, 但实现的成员函数没有可以设成纯虚函数的，不妨把析构函数设成纯虚函数。不过这样就必须为析构函数提供一个定义：

```c
AWOV::~AWOV() {}
```

这是因为在析构派生类时，会顺序调用从基类到派生类的析构函数，因此需要有基类析构函数的定义（即使它是一个纯虚函数）。

> 多态的基类应该声明一个虚析构函数。如果类带有任何虚函数，它就应该拥有一个虚析构函数。

> 类的设计目的如果不是作为基类使用，或者不具备多态性，就不该声明虚析构函数。

### 条款8：别让异常逃离析构函数

如果在析构函数抛出异常，程序可能会过早结束或出现不明确行为。假设析构函数里调用了`c`函数（比如关闭连接、释放资源）。为了阻止析构函数抛出异常，有以下两种选择：

```c
~deconstruct() {
    try { c(); }
    catch (...) { std::abort(); } // 第一种选择：强制结束程序
    catch (...) { log(); } // 第二种选择：记录失败，继续运行
}
```

这两种方法其实都没有那么好，因为它们只能避免析构函数抛出异常的恶劣结果，而不能对情况作出回应。更好的设计是将`c`函数开放给用户，让用户（显式地）调用`c`函数来进行清理工作，如果在此期间抛出了异常，便由用户自己处理。在析构函数里，设一个“双保险”：

```c
~deconstruct() {
    if (!closed) {
        try { c(); }
        catch (...) { std::abort(); } // 第一种选择：强制结束程序
        catch (...) { log(); } // 第二种选择：记录失败，继续运行
    }
}
```

如此一来，用户既有机会相应清理工作中的异常情况，也不至于让析构函数不得不抛出异常。（这种设计理念很难说不是一种折中。我并不认为把复杂的`c`函数交给用户自己处理是一件好事。也许花心思设计出一个更健壮的析构函数才是关键——然而这并不容易。）

> 析构函数绝对不要抛出异常。如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕获任何异常，然后吞下它们（不传播）或结束程序。

> 如果客户需要对某个操作函数运行期间抛出的异常做出反应，那么类应该提供一个普通函数（而非析构函数）执行该操作。

### 条款9：绝不在构造和析构过程中调用virtual函数

```c
class Transaction {
public:
    Transaction() {
        logTransaction();
    }
    virtual void logTransaction() const = 0;
    // pure virtual
};

class BuyTransaction: public Transaction {
public:
    virtual void logTransaction() const;
};
```
在创建一个`BuyTransaction`对象时，先构造基类部分（`Transaction`），再构造派生类部分。在基类构造函数中，调用了虚函数`logTransaction`。这时虚函数使用的是基类版本，就像创建的对象属于基类类型一样。这样做的理由之一，是因为派生类成员还未初始化，调用派生类的虚函数版本会发生危险。

> 在构造和析构期间不要调用虚函数，因为这类调用从不下降至派生类（比起当前执行构造函数和析构函数的那层）。

### 条款10：令operator=返回一个reference to *this

为了实现右结合律连锁赋值。

### 条款11：在operator=中实现“自我赋值”

自我赋值时，如果先把原值删除再拷贝所赋的值，在碰到自我赋值的情况时会引发错误。解决方法可以是：1）临时变量储存原值；2）判断是否自我赋值；3）拷贝传递rhs实参，再和原值交换（swap）。

> 确保当对象自我赋值时operator=有良好行为。其中技术包括比较“来源对象”和“目标对象”的地址、精心周到的语句顺序、以及copy-and-swap。

> 确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为依然正确。

### 条款12：复制对象时勿忘其每一部分

> 拷贝函数/运算符应该确保赋值“对象内的所有成员变量”以及“所有基类部分”。

> 不要尝试以拷贝函数实现拷贝运算符（或反过来）。应该将共同机能放进第三个函数中，并由两者共同调用。

## 资源管理

> 所谓资源，就是一旦用了它，将来必须还给系统（的东西）。资源包括：动态分配内存，文件描述器（file descriptors），互斥锁（mutex locks），图形界面中的字型和笔刷，数据库连接，以及网络sockets。

### 条款13：以对象管理资源

“以对象管理资源”的观念也被称为“资源取得时机便是初始化时机”（Resource Acquisition Is Initialization，RAII）。典型的RAII类有智能指针。在获取资源后，我们立刻将其放入管理对象内。在管理对象析构时，确保资源得到正确释放。“引用计数型智能指针”（reference-counting smart pointer，RCSP）使用了引用计数的技术来为管理对象间的复制提供安全保障。不过，RCSP仍有环状引用（cycles of references，例如两个没有被使用的对象彼此互指）等问题，操作时依然要小心。

> 为防止资源泄漏，请使用RAII对象（Resource Acquisition Is Initialization）：它们在构造函数中获得资源并在析构函数中释放资源。

### 条款14：在资源管理类中小心coping行为

在复制RAII对象时要注意这么做的意义。如果复制是不合理的，要禁止用户复制；或者用引用计数法等提高复制的安全性。有时根据资源的特性，在复制资源管理类对象时，一并深复制它的资源。

> 复制RAII对象必须一并复制它所管理的资源，所以资源的复制行为决定RAII对象的复制行为。

> 普遍而常见的RAII类复制行为是：抑制复制、施行引用计数法（reference counting）。不过其他行为也都可能被实现。

### 条款15：在资源管理类中提供对原始资源的访问

可以显式地提供访问接口（比如`shared_ptr`的`.get()`），也可以定义资源管理类向资源的隐式转换。不过意外的隐式转换会导致不好的结果，不如显式转换安全。

> APIs往往要求访问原始资源（raw resources），所以每一个资源管理类应该提供一个“取得其所管理之资源”的办法。

### 条款16：成对使用new和delete时要采取相同形式

很多编译器对数组的内存布局是：先记录数组大小n，再依次记录每个元素。`delete[]`会读取若干内存并将它解释为“数组大小”，然后开始多次调用析构函数。

> new使用或不使用[]，delete要保持一致。

### 条款17：以独立语句将newed对象置入智能指针

考虑下列语句：
```c
process(shared_ptr<Widget>(new Widget), priority());
```
在调用`process`前，编译器会做三件事：1）调用`priority`；2）执行`new Widget`；3）调用`shared_ptr`构造函数。这三件事完成的次序不定，如果`priority`第二个被调用且发生异常，那么`new Widget`返回的指针便不能被置入`shared_ptr`内，将会遗失，造成资源泄漏。因此，更好的做法是使用分离语句：
```c
shared_ptr<Widget> pw(new Widget);
process(pw, priority());
```

> 以独立语句将newed对象置入智能指针内。否则，一旦异常被抛出，有可能导致难以察觉的资源泄漏。

## 设计与声明

### 条款18：让接口容易被正确使用，不容易被误用

理想上，如果客户企图使用某个接口而却没有获得他所预期的行为，这个代码就不该通过编译；如果代码通过了编译，它的作为就该是客户所想要的。

自定义类型，让客户能正确输入参数：

```c
Date d(Month::Mar(), Day(30), Year(1995);
// 设计年月日类，并对月份“枚举”（local-static）
// 我觉得太繁琐也太低效了
```

接口的行为应该保持一致，比如STL的`size`，所有容器的`size`成员函数都是返回元素数量。为了防止用户误用，可以给类型加const，或者用智能指针包装返回的指针（或许带上自定义的删除器）等。

> “促进正确使用”的办法包括接口的一致性，以及与内置类型的行为兼容。

> “阻止误用”的办法包括建立新类型、限制类型上的操作，束缚对象值，以及消除客户的资源管理责任。

> shared_ptr支持定制型删除器（custom deleter），可以防范DLL问题，可被用来自动解除互斥锁等等。

### 条款19：设计class犹如设计type

设计类（class）就是设计新类型（type）。为了设计高效的类，必须先考虑以下方面：

- 新类型的对象应该如何被创建和销毁？
- 对象的初始化和对象的赋值有什么样的差别？
- 新类型的对象如果被值传递，意味着什么？
- 什么是新类型的合法值？
- 你的新类型需要配合某个继承图系（inheritance graph）吗？
- 你的新类型需要什么样的类型转换？
- 什么样的操作符和函数对此新类型而言是合理的？
- 什么样的标准函数应该被delete？
- 成员的访问权限是怎么样的？
- 什么是新类型的“未声明接口”（undeclared interface）？（什么是？我也不知道……）
- 你的新类型是否更适合成为模板？
- 你真的需要一个新类型吗？

> 类的设计就是类型的设计。在定义一个类型之前，请确定你已经考虑过本条款覆盖的所有讨论问题。

### 条款20：宁以pass-by-reference-to-const替换pass-by-value

值传递需要拷贝构造一次形参再析构之，造成巨大浪费。如果可以的话，换成`const Type&`更好。这样也能支持多态。不过，C++往往以传递指针来实现引用传递，对于内置类型，值传递效率是更高的。小型类不一定就适合值传递。某些编译器可能会改变类的大小，有些也可能拒绝把小型类放进缓存器中。一般而言，可以合理假设值传递并不昂贵的唯一对象就是内置类型和STL的迭代器和函数对象。

> 尽量以pass-by-reference-to-const替换pass-by-value。前者通常比较高效，并可避免向基类的切割问题（slicing problem）。

> 以上规则并不适用于内置类型，以及STL的迭代器和函数对象。对它们而言，值传递往往比较适当。

### 条款21：必须返回对象时，别妄想返回其reference

不要为了避免返回值带来的复制析构开销而乱返回引用。这个问题在C++11里，或许可以通过右值引用来解决。

> 绝不要返回指针或引用指向一个local stack对象，或者引用指向一个heap-allocated对象，或返回指针或引用指向一个local static对象而有可能同时需要多个这样的对象。

### 条款22：将成员变量声明为private

成员变量的封装性与“成员变量的内容改变时所破坏的代码数量”成反比。比如说，取消一个public成员变量，所有使用它的客户代码都会被破坏。而取消一个protected成员变量，所有使用它的派生类代码都会被破坏。因此，public和protected一样缺乏封装性。只有private能提供封装。

> 切记将成员变量声明为private。这可赋予客户访问数据的一致性、可细微划分访问控制、允诺约束条件获得保证，并提供类作者以充分的实现弹性。

> protected不比public更具封装性。

### 条款23：宁以non-member、non-friend替换member函数

考虑以下两种`clear`的实现：
```c
class WebBrowser {
public:
    void clearCache();
    void clearHistory();
    void removeCookies();
    
    void clearEverything();
};

// 第一种
void WebBrowser::clearEverything() {
    clearCache();
    clearHistory();
    removeCookies();
}

// 第二种
void clearBrowser(WebBrowser& wb) {
    wb.clearCache();
    wb.clearHistory();
    wb.removeCookies();
}
```

根据“越少函数可以访问数据，数据的封装性就越高”的标准，第二种封装性更强，因为它不增加可访问private数据成员的函数数量。另一点，像这样的“便利函数”，可以和类放在同一个namespace下，但分在多个头文件中，实现更好的组织结构。（不过个人认为这个不是很有必要，有种流于形式的感觉）

> 宁可拿non-member、non-friend函数替换member函数。这样做可以增加封装性、包裹弹性（packaging flexibility）和机能扩充性。

### 条款24：若所有参数皆需要类型转换，请为此采用non-member函数

比如说字符串相加。如果重载了string的+成员操作符，那么`"char" + str`就无法调用该操作符。最好是将该操作符定义为`operator+(const string &lhs, const string &rhs)`，这样允许`const char*`隐式转换为string以相加。

> 如果你需要为某个函数的所有参数（包括被this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是个non-member。

### 条款25：考虑写出一个不抛出异常的swap函数

缺省的`std::swap`实现用临时变量交换两个变量。这种方法效率不高。为了提高效率，可将自定义类以“pimpl手法”（pointer to implementation）设计（见条款31）。这种类在交换时只需要交换实现指针即可。我们希望为自定义类`Widget`做一个`std::swap`的全特化：

```c
namespace std {
    template <>
    void swap<Widget>(Widget &a, Widget &b) {
        swap(a.pImpl, b.pImbl);
    }
}
```

pimpl类的实现指针一般是private的，所以可以为类添加一个public的swap成员函数，再让全特化版本调用这个swap成员函数。如果类是模板类，那么根据此思路，应该对`std::swap`进行偏特化：

```c
namespace std {
    // 注意：以下并不是正确的写法
    template <typename T>
    void swap<Widget<T>>(Widget<T> &a, Widget<T> &b) {
        a.swap(b);
    }
}
```

然而，C++不允许偏特化函数模板，只能偏特化类模板。因此，我们可以改为重载之：

```c
namespace std {
    template <typename T>
    void swap(Widget<T> &a, Widget<T> &b) {
        a.swap(b);
    }
}
```

但是，std只允许客户全特化其中的模板，但不允许添加新的模板和类、函数及其他东西。违背这点的程序仍可以编译和运行，但是行为没有定义。因此，我们只能在自定义命名空间中声明非成员函数swap。这个swap所在命名空间和`std::swap`不同，因此在对类对象进行swap操作时，可以：

```c
using std::swap;
swap(obj1, obj2);
```
在自定义swap找不到时，便使用`std::swap`（假设它比自定义版本匹配优先级低）。

> 当std::swap对你的类型效率不高时，提供一个swap成员函数，并确定这个函数不抛出异常。

> 如果你提供一个swap成员函数，也该提供一个非成员函数来调用前者。对于类（而非模板），也请特化std::swap。

> 为“用户定义类型”进行std模板全特化是好的，但千万不要尝试在std内加入某些对std而言全新的东西。

## 实现

### 条款26：尽可能延后变量定义式的出现时间

如果过早定义变量，在它被使用之前发生异常的话，它的构造和析构就没有意义了。如果它需要先被赋值再使用，那么“默认构造+赋值”会好过“拷贝/直接初始化”。如果在循环体中需要用到变量，应该考虑是否在循环体里赋值，还是在循环体里构造+初始化：

```c
int b;
for (...) {
    b = ...;
    // 或者， int b = ...;
}
```
哪种做法比较好，应该比较变量的赋值成本和构造+析构成本。

> 尽可能延后变量定义式的出现。这样做可增加程序的清晰度并改善程序效率。

### 条款27：尽量少做显式转换

C++对象在内存中如何布局是不确定的，不能自以为是地进行强制类型转换进行操作。同时，从派生类强制转到基类，只会保留它的基类部分（就是被切割了）：
```c
class Base {
   public:
    void func() { cout << v << endl; }
    int v = 10;
};

class Derived : public Base {
   public:
    void f2() { static_cast<Base>(*this).func(); }
    int v = 20;
};

Derived d;
d.func();
// 10. 如果Derived类也有一个void func() {cout << v << endl;}，
// 那么会输出20。派生类向基类的转换是很容易出错的。
d.f2();
// 10
```
如上所示，`static_cast<Base>(*this)`是一个“this对象之基类部分”的临时副本。对这个副本调用修改状态的函数会造成问题。在能用虚函数的地方尽量避免`dynamic_cast`，因为它执行速度很慢。

> 如果可以，尽量避免cast，特别是在注重效率的代码中避免dynamic_cast。如果有个设计需要cast操作，试着发展无需转型的替代设计。

> 如果cast是必要的，试着将它隐藏于某个函数背后。客户随后可以调用该函数，而不需将cast放进他们自己的代码内。

> 宁可使用C++风格cast（static_cast等），也不要用C风格cast。前者很容易辨识出来，分门别类。

### 条款28：避免返回handles指向对象内部成分

引用、指针和迭代器都是所谓的handles，返回指向对象内部成分的handles指返回封装成员的指针、引用等。这么做会降低封装性，让一些私有的成员也可以被客户直接操作。

> 避免返回handles指向对象内部。遵守这个条款可增加封装性，帮助const成员函数的行为像个const，并将发生dangling handles的可能性降到最低。

### 条款29：为“异常安全”而努力是值得的

当异常被抛出时，带有异常安全性（Exception Safety）的函数会：1）不泄漏任何资源；2）不允许数据败坏（比如说，一个记录分配了多少资源的变量，在分配失败后仍然递增）。

在异常被抛出时，异常安全函数提供以下三个保证之一：

- 基本承诺（对象仍处于有效状态，但不确定是哪种状态）。比如一个更换背景的函数抛出异常后，背景对象可能是原背景图像，也可能是缺省背景图像，用户无法预知。但是程序内的事物都是保证有效的——没有任何数据会因此败坏，所有对象都处于一种内部前后一致的状态。
- 强烈保证（回复到调用函数前状态，“全有或全无”）。类似RDBMS的Atomicity。
- nothrow状态（承诺不抛出异常）。程序总是能够完成它们承诺的功能。

“空白的异常明细”（empty exception specification）

```c
int func() throw();
```

并不能说明函数`func`不会抛出异常，而是说如果它抛出异常，将会是很严重的错误，会有意想不到的函数被调用。以上三种保证是由函数的实现决定的，无关乎它的声明。”nothrow状态“非常难满足；对大部分函数而言，抉择往往落在基本保证和强烈保证之间。

copy and swap策略能提供强烈保证。首先为打算修改的对象做一个副本，在副本上修改，最后再和原对象交换。修改过程中的异常不会影响原对象。不过，copy and swap不能总是能提供强烈保证。或许它调用了某种具有“连带影响”的函数，影响了全局状态，要在抛出异常后恢复之前的状态就变得很困难了。这个情况说明安全性遵循某种木桶效应，如果系统内有一个函数不具备异常安全性，整个系统就不具备异常安全性，而没有所谓的“局部异常安全系统”。不幸的是，许多老旧的C++代码并不具备异常安全性。今天许多系统或多或少并入了一些过去的代码，所以仍不能说是异常安全的。

copy and swap也会面临效率问题，在一些情况下不是很实际。

> 异常安全函数即使发生异常也不会泄漏资源或允许任何数据结构败坏。这样的函数区分为三种可能的保证：基本型、强烈型、不抛异常型。

> “强烈保证”往往能够以copy and swap实现出来，但“强烈保证”并非对所有函数都可实现或具备现实意义。

> 函数提供的“异常安全保证”通常最高只等于其调用之各个函数的“异常安全保证”中的最弱者。

### 条款30：透彻了解inlining的里里外外

过多的inline函数会造成目标码体积过大，造成的代码膨胀会导致额外的换页（paging）行为，降低指令高速缓存装置的击中率（instruction cache hit rate），以及伴随这些而来的效率损失。

编译器有时会拒绝函数inline。比如，要取inline函数地址，它就必须是个outline函数；虚函数也不能是inline，因为它是否调用在运行期才被决定。而有些函数（比如构造/析构函数），即使函数体内代码很少，不代表编译器不会为其生成代码（在编译期将一些执行特殊功能的代码插入构造/析构函数中，比如执行基类构造/析构等），同样不适合inline。

在inline函数被修改以后，程序需要重新编译。如果是outline函数被修改，只需要重新连接即可；如果程序库采取动态连接，修改后的outline函数甚至可以不知不觉就被程序吸纳。再者，inline函数内无法设置debug断点，难以调试。综上，设置inline时需要考虑的问题是很多的。

> 将大多数inline限制在小型、被频繁调用的函数身上。这可使日后的调试过程和二进制升级（binary upgradability，指重新连接）更容易，也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。

> 不要只因为函数模板出现在头文件，就将它们声明为inline。

### 条款31：将文件间的编译依存关系降至最低

pimpl（pointer to implementation）idiom设计把类分割为两个类，一个只提供接口（handle class），另一个负责实现该接口，“接口与实现分离”。
```c
class Person {
public:
    Person(...) {...}
    string name() const;
    // 客户接口。通过指针调用
private:
    shared_ptr<PersonImpl> pImpl;
    // 指向实现对象。实现逻辑在PersonImpl类中
};
```
在`Person`类的实现修改后，使用类对象的客户代码是不需要重新编译的。如果不进行分割，对`Person`类的修改会导致客户代码重新编译。这种handle class的方法可以解除接口和实现之间的耦合关系。这个分离的关键在于以“声明的依存性”替换“定义的依存性”。客户代码中使用`Person`类，实际只依赖`PersonImpl`类对象的声明，而对类的定义式没有依赖。将`PersonImpl`对象使用指针或引用操作是关键；如果定义了一个对象

```c
PersonImpl pImpl;
```

那么就要用到`PersonImpl`类的定义式，在修改类定义时客户代码也要重新编译。另外，如果能够，尽量要以类的声明式替换定义式：

```c
class Date;		// 前置声明
Date today();
void clear(Date d);
// 只需要Date类声明，并不需要定义
```

声明`today`和`clear`函数无需定义`Date`，但是在调用时需要用到`Date`的定义式。如此一来，那些不是真正必要的`Date`定义（比如只声明函数而不调用）就可以省略掉了，以至于减少编译的依存性。另外，一般我们会`#include`一个声明文件，而不是前置声明：

```c
#include "datefwd.h"
// include声明式。在需要定义式的时候，再include包含定义式的头文件
Date today();
void clear(Date d);
```

另一种降低编译依存性的方法是interface class。定义抽象基类（接口类），然后在派生类上实现之。客户通过基类指针调用类成员函数。就像handle class一样，除非接口类被修改了，修改派生类的话客户不需要重新编译。

> 支持“编译依存性最小化”的一般构想是：相依于声明式，不要相依于定义式。基于此构想的两种手段是handle classes和interface class。

> 程序头文件应该以“完全且仅由声明式”的形式存在。这种做法不论是否涉及模板都适用。

## 继承与面向对象设计

在OOP设计中，语法特性有其意义，表达了某种“意思”。

### 条款32：确定你的public继承塑模出is-a关系

类D public继承 类B，表达的意思是“类D is-a 类B”。B是比D更一般化的概念，而D比B更特殊化。在所有B对象可派上用场的地方，D对象一样可以（Liskov Substitution Principle）。任何期望接收类型为B的实参的函数，也愿意接受一个D实参。继承中的"is-a"，有时会反直觉。比如说企鹅"is-a"鸟，（大部分）鸟会飞，但企鹅不会飞的例子。我认为作者在分析public继承时用的企鹅和矩形的例子并不是逻辑严谨的。他认为增加矩形的宽度依然是矩形，而增加正方形的宽度就不再是正方形，以此例证public继承的反直觉。这是错的。正方形也具有“增加宽度后依然是矩形”的性质。我一直反感对“意义”的纠结。如果这种纠结无法提高开发效率，那么就是程序员的浪漫文学而已。

> "public继承"意味is-a。适用于基类身上的每一件事情一定也适用于派生类上，因为每一个派生类对象也都是一个基类对象。

### 条款33：避免遮掩继承而来的名称

里层作用域的名字会遮掩掉外层作用域中所有相同的名字（例如所有同名重载函数）。通过`using Base::fuc`让基类名字重新可见。或者通过以下转交函数（forward function）：
```c
void fuc() { Base::fuc(); }
```

> 派生类内的名词会遮掩基类中的名称。在public继承下从来没有人希望如此。

> 为了让被遮掩的名称重见天日，可使用using声明式或转交函数。

### 条款34：区分接口继承和实现继承

声明一个纯虚函数是为了让派生类只继承函数接口。派生类不对接口进行实现的便仍是抽象基类。声明一个普通纯虚数是为了让派生类继承该函数的接口和缺省实现。普通纯虚数的一个设计问题是派生类设计者可能会遗忘覆盖缺省实现，导致程序编译成功却行为异常。一种解决办法是将接口和缺省实现分离：

```c
struct Base {
    virtual void func() = 0;
    void defaultFunc() {...}
};
struct DerivedDefault: Base {
    void func() {defaultFunc();}
};
struct DerivedNotDefault: Base { };
// 遗忘覆盖将导致编译失败

Base *ptr = new DerivedNotDefault;
ptr->func();
```

如果基类的成员函数是非虚的，意味着它并不打算在派生类中有不同的行为。

> 接口继承和实现继承不同。在public继承之下，派生类总是继承基类的接口。

> 纯虚函数只具体指定接口继承。普通虚函数具体指定接口继承和缺省实现继承。非虚函数具体指定接口继承以及强制性实现继承。

### 条款35：考虑virtual函数以外的其他选择

non-virtual interface（NVI）手法是Template Method设计模式（和C++模板没有关系）的一个独特表现形式。它使用一个非虚函数——wrapper来包裹虚函数：
```c
class Base {
public:
    void wrapper() {
        ...             // 一些事前工作
        doWrapper();    // 调用虚函数
        ...             // 一些事后工作
    }
private:
    virtual void doWrapper() {...};
    // 派生类覆盖的虚函数。也可以不是private的
};
```

NVI的优点是能在调用虚函数前后做一些通用性工作（比如上锁解锁……）。基类决定虚函数何时被调用，派生类决定虚函数的机能。

另一种替代虚函数的设计模式是Strategy设计模式。类在初始化时接受一个函数指针，那个指针指向的函数便是它们的“虚函数”。这个方法的缺点是会降低类的封装性（可能要将函数声明为友元）。另外，初始化类对象的不一定要是函数指针，其他可调用对象也可以（类构造函数形参类型为function<> call signature）。在古典Strategy设计模式中，那些“虚函数”是另一些有继承关系的类的虚函数（这些类只负责像对象类提供这些“虚函数”，它们之间也用继承关系组织）。对象类只需要接受一个“虚函数”基类指针来构造即可。

### 条款36：绝不重新定义继承而来的non-virtual函数

非虚函数即使被指针调用，也是静态绑定的，只和指针类型有关。重新定义继承的非虚函数，行为不一致，意义不明，所以不应该这么做。

### 条款37：绝不要重新定义继承而来的缺省参数值

和非虚函数一样，缺省参数值也是静态绑定的。

### 条款38：通过聚合塑模出has-a或is-implemented-in-terms-of

程序中的对象可以分属于应用域（application domain，所塑造的“世界”中的某些事物）或实现域（implementation domain，实现细节用的对象，比如buffer、mutex）。如果聚合发生于应用域的对象之间，表现出has-a关系；如果发生于实现域内则表现is-implemented-in-terms-of关系。

> 聚合的意义和public继承完全不同。

> 在应用域，聚合意味着has-a。在实现域，聚合意味着is-implemented-in-terms-of。

### 条款39：明智而审慎的使用private继承

private继承让派生类对象无法自动转换为基类对象，且继承的成员（protected和public的）都变为private的。private继承意味着implemented-in-terms-of。private继承略去了接口，只继承了实现，派生类只为了采用基类已经实现了的某些特性，不一定和基类有什么意义上的联系。一般尽可能用聚合来实现implemented-in-terms-of。相比聚合，private继承允许派生类访问基类的protected对象，还能重新定义虚函数。private继承相比聚类，还有空白基类最优化（Empty base optimization，EBO）的优势。如下聚合类：
```c
class Empty {};     // 空类
class Aggregate {
    int x;
    Empty e;
};
```
`Empty`是个空类，按理来说不需占用内存。但C++规定凡是独立（非附属）对象必须具有非零大小（大部分编译器会插入一个char进空对象，让`sizeof(Empty)=1`），然后内存对齐又让`Aggregate`为`Empty`补位到int大小。如果是private继承：
```c
class PrivateInheritance: private Empty {
    int x;
};
```
几乎可以确定`sizeof(PrivateInheritance) == sizeof(int)`。这是EBO的效果（一般在单一继承下才可行）。

> private继承意味着implemented-in-terms-of。它通常比聚合的级别低。但是当派生类需要访问protected基类成员时，或需要重新定义继承而来的虚函数时，这么设计是合理的。

> 和聚合不同，private继承可以造成空白基类最优化。这对致力于“对象尺寸最小化”的程序库开发者而言，可能很重要。

### 条款40：明智而审慎的使用多重继承

多重继承常常会面对的一个问题，就是“钻石型多重继承”：

```c++
class A {};
class B : public A {};
class C : public A {};
class D : public B, public C {};
```

为了避免D包含多个A的子对象，应该让B和C虚继承A。以这种行为为正确行为的话，我们应该在任何使用public继承的时候，都使用virtual public继承。然而，虚继承的类产生的对象体积往往更大，访问成员变量也更慢。同时，由于是最低层（most derived）类对虚基类进行初始化，初始化会更为复杂且不直观。因此，我们应该：

- 非必要不使用虚继承。
- 如果必须使用虚基类，尽可能避免在其中放置数据。

> 多重继承比单一继承复杂。它可能导致新的歧义性，以及引入虚继承。

> 虚继承会增加大小、速度、初始化（及赋值）复杂度等等成本。如果虚基类不带任何数据，将是最具实用价值的情况。

> 多重继承的确有正当用途。其中一个情节涉及“public继承某个接口类”和“private继承摸个协助实现的类”的两相组合（例子在P195）。



## 模板与泛型编程

### 条款41：了解隐式接口和编译期多态

在面向对象编程中，显式接口（在类定义中声明的接口）和运行期多态很重要。但在泛型编程中，隐式接口（模板函数/类的表达式中对象调用的接口，比如`w.get()`说明`w`的类型`T`支持`get`接口）和编译期多态（以不同的模板参数实例化，会导致调用不同的类型成员函数）更重要。隐式接口返回类型模糊，比显式接口更难判断，但是它们都在编译期完成检查。

> 类和模板都支持接口（interface）和多态（polymorphism）。

> 对类而言接口是显式的，以函数签名为中心。多态则是通过虚函数发生在运行期。

> 对模板参数而言，接口是隐式的，基于有效表达式。多态则是通过模板实例化和函数重载解析（function overloading resolution）发生于编译期。

### 条款42：了解typename的双重意义

模板中的嵌套从属名称（nested dependent name, 嵌套的依赖于某个模板参数的名称，比如`T::iterator`）可能是一个变量名也可能是一个类型名，具体是哪个依赖于提供的模板实参。编译器在解析模板时，需要知道它是什么，一般会假设它是变量名而不是类型名。为了告诉编译器它是类型，需要在名称前加关键字`typename`：

```c
typename T::iterator* iter;
// 表示一个指向T的迭代器的指针，而非一个叫iterator的变量乘以一个叫iter的变量
```

不过在类派生列表和初始化列表中是不必出现的（也不允许出现），毕竟在那里肯定是类型（然而在形参列表中需要，不明白为什么）。

> 声明模板参数时，前缀关键字class和typename可以互换。

> 请使用关键字typename标识嵌套从属类型名称；但不得在类派生列表和初始化列表内以它作为修饰符。

### 条款43：学习处理模板化基类内的名称

考虑下面这个模板：

```c
template <typename T>
class Base {
public:
    void funcA();
    ...
};
```

在这个模板类里有一个函数funcA.然后该模板有一个全特化：

```c
template <>
class Base<myType> {
public:
    void funcB();
    ...
};
```

全特化删去了funcA,增加了funcB.此时如果有一个类想继承模板类，并也作为模板，那么：

```c
template <typename T>
class Derived: public Base<T> {
public:
    void funcC() {
        funcA();
        ...
    }
    ...
};
```

这里会抛出一个编译错误。事实上，这里的funcA是通过继承而来的，但编译器无法确认父类是否有funcA,不到具现化不知道类里究竟有什么，严谨的编译器就会视其为一个错误。正确的做法是要告诉编译器假设funcA就在父类里。

```c
template <typename T>
class Derived: public Base<T> {
public:
    using Base<T>::funcA;       //方法一
    void funcC() {
        this->funcA();          //方法二
        Base<T>::funcA();       //方法三
        // 最不好的一个做法。如果funcA是个虚函数，这样会阻止动态绑定
        ...
    }
    ...
};
```

这些“承诺”只是编译器在解析（parse）模板时需要的，如果承诺了却没有实践，在后续的编译中仍会报错（`error: ‘funcA’ is not a member of ‘Base<myType>’`）。

> 可在派生类的模板中通过"this->"指涉基类模板内的成员名称，或通过一个显式写出的"基类资格修饰符"完成。


### 条款44：将与参数无关的代码抽离templates

```c
template <typename T, int n>
void func() {cout << n;}
// 不好的模板代码。会根据每个非类型参数n的值实例化，得到多份代码
// 下面的更好
template <typename T>
void func(int n) {cout << n;}
```

冗余的模板实例化会造成代码膨胀。在和性能权衡后要尽可能减少代码膨胀的可能性，比如把模板中一些可有可无的代码移到外面去。实例化时，也可能会实例化出冗余代码（比如用int和long分别实例化，在实际中它们可能没有区别；`void*`可以代替各种指针来实例化）。

> 模板生成多个类和多个函数，所以任何模板代码都不该与某个造成膨胀的模板参数产生相依关系。

> 因非类型模板参数而造成的代码膨胀，往往可消除，做法是以函数参数或类成员变量替换模板参数。

> 因类型参数而造成的代码膨胀，往往可降低，做法是让带有完全相同二进制表述（binary representation）的实例类型（instantiation type，也许是模板实参）共享实现码。

### 条款45：运用成员函数模板接受所有兼容类型

通过声明一个非explicit模板拷贝构造函数，可实现从一个模板实例化的对象向另一个实例化的对象的隐式转换：

```c
template <typename T>
class Base {
public:
    template<typename U>
    Base(const Base<U>& other);
}
```

有时我们需要在类型T和U之间有联系时（比如可以从U隐式转换成T）才能发生隐式转换，应该如同以下例子一样控制拷贝：

```c
template <typename T>
class SmartPtr {
public:
    template<typename U>
    SmartPtr(const SmartPtr<U>& other): heldPtr(other.get()) {...}
    // 一个智能指针隐式转换的例子。如果从U*不能隐式转换为T*指针，那么此处无法通过编译
    T* get() const { return heldPtr; }
private:
    T* heldPtr;
}
```

在T和U类型一样时，泛化的拷贝构造函数会被实例化为普通的非模板的拷贝构造函数（默认拷贝构造函数、拷贝赋值运算符在实例化类中依然会自动生成）。如果需要控制这些普通的默认成员函数，需要在模板定义中声明它们：

```c
template <typename T>
class SmartPtr {
public:
    template<typename U>
    SmartPtr(const SmartPtr<U>& other);
    // 普通的拷贝构造函数。省略了<T>
    SmartPtr(const SmartPtr& other);
    // 普通的拷贝赋值运算符
    SmartPtr& operator=(SmartPtr const& other);
    T* get() const { return heldPtr; }
private:
    T* heldPtr;
}
```

> 请使用成员函数模板生成“可接受所有兼容类型”（隐式转换）的函数。

> 如果你声明成员模板用于“泛化拷贝构造”或“泛化赋值操作”，你还是需要声明正常的拷贝构造函数和拷贝赋值操作符（来操作它们）。

### 条款46：需要类型转换时请为模板定义非成员函数

考虑以下代码：

```c
template <typename T>
class Rational {
public:
    Rational(const T& numerator = 0, const T& denominator = 1);
    const T numerator() const;
    const T denominator() const;
    ...
};
template <typename T>
const Rational<T> operator* (const Rational<T> &lhs, const Rational<T> &rhs) {
    ...
}

int main() {
    Rational<int> oneHalf(1, 2);
    Rational<int> result = oneHalf * 2;     //错误！无法通过编译
}
```

之所以上面的代码无法通过编译，是因为编译器无法推导出operator*的实例化是什么。在template进行实参推导时，不会将隐式转换考虑进去。因此，即使2能隐式转换成与前一个操作数同类型的对象，也无法通过编译。更好的方法是把operator作为Rational类的friend函数。

```c
template <typename T>
class Rational {
public:
    Rational(const T& numerator = 0, const T& denominator = 1);
    const T numerator() const;
    const T denominator() const;
    friend const Rational operator* (const Rational &lhs, const Rational &rhs);
    //或者写成
    //friend const Rational<T> operator* (const Rational<T> &lhs, const Rational<T> &rhs);
    ...
};
template <typename T>
const Rational<T> operator* (const Rational<T> &lhs, const Rational<T> &rhs) {
    ...
}
```

这样，在`Rational<int> oneHalf(1, 2);`这里，`Rational<int>`类便被实例化出来，它的friend函数也被自动声明出来。这个friend函数是实例而非模板，所以能够进行隐式转换（`int -> Rational<int>`）。

但是，虽然这段代码能通过编译，但却不能连接。（事实上，在我用gcc编译器编译时，它是不能通过编译的。报错信息为"warning: friend declaration ‘const Rational<T> operator*(const Rational<T>&, const Rational<T>&)’ declares a non-template function [-Wnon-template-friend]"。在转换为下面的写法后问题得到了解决。如果泛型类要声明一个友元函数，在我的编译器下它必须也是模板函数。它有一个`template <typename U>`（U是为了和T不同）。然而即使这样它也是无法进行隐式转换。这个问题表明泛化友元的问题在书本这里尚没有说清楚）。解决方法是在声明friend函数时一起把它定义了。
注意，在这个例子中，friend函数并没有发挥到它传统的访问类non-public成员的功能，而是让operator*成功具现化。由于定义在类内的函数都会暗自成为inline函数，所以如果operator实现了更多行代码时，为了防止代码量膨胀，可以使用一个辅助函数：

```c
template <typename T> class Rational;       //声明Rational template
template <typename T>
const Rational<T> doMultiply(const Rational<T> &lhs, const Rational<T> &rhs);     //辅助函数
template <typename T>
class Rational {
public:
    ...
    friend const Rational operator* (const Rational &lhs, const Rational &rhs) {
        return doMultiply(lhs, rhs);
    }
    ...
};
```

如此便能让inline函数代码量只剩一行。

> 当我们编写一个类模板，而它所提供的与此模板相关的函数支持“所有形参之隐式类型转换”时，请将那些函数定义为类模板内部的友元函数。

### 条款47：请使用traits class表现类型信息

STL将迭代器分为了5种（input——每次只能向前移动一步且只读一次、output——每次只能向前移动一步且只写一次、forward——可读可写多次、bidirectional——双向移动、random access——随机访问）。C++标准库为这5种迭代器分别提供所谓卷标结构（tag struct）加以确认。在设计支持迭代器的泛型函数时，首先应该判断迭代器属于哪类，才能进行合法的操作。所谓traits允许用户在编译期间获得某些类型信息。由于traits对我很陌生，下面会详细介绍之。

traits是一种技术，也是一个C++程序员共同遵守的协议。这个技术的要求之一，是它对内置类型和用户自定义类型的表现必须一样好（例如对迭代器和普通指针）。以下以指针/迭代器的traits为例：

```c
template <...>
class deque {
public:
    class iterator {
    public:
        typedef random_access_iterator_tag iterator_category;
        // 用户自定义的迭代器类型嵌套一个typedef以确认恰当的tag struct
        // 这个deque类的迭代器是随机访问类型的
    };
};

template <typename IterT>
struct iterator_traits {
    typedef typename IterT::iterator_category iterator_category;
    // 用户自定义的iterator_category是traits的category
    ...
};
template <typename IterT>
struct iterator_traits<IterT*> {
    typedef random_access_iterator_tag iterator_category;
    // 偏特化版本，指定普通指针的类型为随机访问
    ...
};

// 一个应用traits判断迭代器类型的模板函数。
// 另外，这个函数的目的是将迭代器iter向前移动d步
template <typename IterT, typename DistT>
void advance(IterT &iter, DistT d) {
    if (typeid(typename iterator_traits<IterT>::iterator_category) == (random_access_iterator_tag)) {...}
    else if (...) {...}
};
```

使用`iterator_traits`对迭代器实例化后的`iterator_category`来判断迭代器的类型。上面这个`advance`函数描述出了traits技术的大概面目，但是伴随着另一个问题。请看下面的例子：

```c
// avdance模板的一个实例化
void advance(list<int>::iterator &iter, int d) {
    if (typeid(typename iterator_traits<list<int>::iterator>::iterator_category) == (random_access_iterator_tag)) {
        iter += d;
        // 这里无法通过编译
    }
    else {
        if (d >= 0) { while (d--) ++iter; }
        else        { while (d++) --iter; }
    }
};
```

链表的迭代器是不支持+=的。if判断总是会失败，但编译器必须确保所有源码都有效，即使是不会执行的代码。解决问题的一个思路借鉴了模板元编程（template metaprogramming, TMP）将运行期才确定的东西提前到编译器决定。这不仅节省了时间，还能防止可执行文件膨胀。在编译器执行判断的一个方法是使用重载。

```c
// 对各种迭代器类型进行重载
template <typename IterT, typename DistT>
void doAdvance(IterT &iter, DistT d, random_access_iterator_tag) {
    iter+= d;
};
template <typename IterT, typename DistT>
void doAdvance(IterT &iter, DistT d, bidirectional_iterator_tag) {
    if (d >= 0) { while (d--) ++iter; }
    else        { while (d++) --iter; }
};
template <typename IterT, typename DistT>
void doAdvance(IterT &iter, DistT d, input_iterator_tag) {
    if (d < 0)
        throw out_of_range("Negative distance");
    while (d--) ++iter;
};

// 修改后的函数。给重载子函数提供traits信息
template <typename IterT, typename DistT>
void advance(IterT &iter, DistT d) {
    doAdvance(
    iter, d,
    typename iterator_traits<IterT>::iterator_category
    );
};
```

为每个迭代器类型进行函数重载，让编译器在编译期进行函数匹配，决定使用哪一个版本的函数。如此便可以在编译期进行类型判断，将运行期的工作提前到编译期。

对traits类进行总结，它的使用方法大致有以下两点：

 - 建立一组重载函数或函数模板（如doAdvance），彼此间的差异只在于各自的traits参数。令每个函数实现码与其接受的traits信息相对应。
 - 建立一个控制函数或函数模板（如advance），它调用上述函数并传递traits类所提供的信息。

traits广泛应用于标准库，除了`iterator_traits`，还有`char_traits`用来保存字符类型的相关信息，以及`numeric_limits`用来保存数值类型的相关信息（没有遵循以字符`traits`结尾的风格）。

> traits类使得“类型相关信息”在编译期可用。它们以模板和模板特例化实现。

> 整合重载技术后，traits类有可能在编译期对类型执行if...else测试。

### 条款48：认识template元编程

模板元编程（template metaprogramming，TMP）是编写template-based C++程序并执行于编译期的过程。TMP是被发现而非发明出来的。TMP可将工作从运行期转移到编译期，使得某些错误原本通过在运行期才能侦测到，现在可在编译期找出来。TMP可能能让程序更高效：较小的可执行文件、较短的运行期、较少的内存需求。但相对地，编译时间也变长了。一个典型的TMP例子是递归阶乘模板函数。

> TMP可将工作由运行期移往编译期，因而得以实现早期错误侦测和更高的执行效率。

> TMP可被用来生成“基于政策选择组合”（based on combinations of policy choices）的客户定制代码，也可用来避免生成对某些特殊类型并不适合的代码。

## 定制new和delete

### 条款49：了解new-handler的行为

在operator new无法满足某一内存分配需求时，如果客户不指定一个new-handler函数来处理，将会抛出异常。客户调用`<new>`中`set_new_handler`函数来指定new-handler。当new无法满足内存申请时，它会不断调用new-handler函数，直到找到足够内存。一个设计良好的new-handler函数必须做以下事情：

 - 让更多内存可被使用。例如，将某些内存释放，以便下一次内存分配可以成功。
 - 安装另一个new-handler。如果当前new-handler无法取得更多可用内存，就让另一个new-handler替代自己处理。或者修改自己的行为（例如更改一个static参数）来使得下次被调用时行为不一样。
 - 卸除new-handler，即将null指针传给`set_new_handler`。一旦没有安装任何new-handler，new会在内存分配不成功时抛出异常。
 - 抛出`bad_alloc`异常。这样的异常不会被new捕捉，new会在内存分配不成功时抛出异常。
 - 不返回，通常调用abort或exit。

在书上提到为类提供`set_new_handler`和`operator new`便可在动态创建类对象时使用专属new-handlers，我试了一下书上的写法，似乎有些问题（毕竟类的`set_new_handler`不能“重载`std::set_new_handler`）。不过，书中提到的一些复用的设计手段，便值得一提。所谓“怪异的循环模板模式”（curiously recurring template pattern, CRTP），使用一个带有static成员变量的模板函数，实现某一种功能，每个继承它的类都能获得互相不同的static变量：

```c
template <typename T>
class CRTP {
    static int static_val;
};

class Derived: public CRTP<Derived> {...};
class Derived2: public CRTP<Derived2> {...};
// Derived 和 Derived2 都能使用CRTP中的接口，但不尽相同，比如static_val
```

在书中，CRTP被用来构造拥有自定义new-handler的类的基类。派生自此模板类的类都共用一套`set_new_handler`和`operator new`接口，但是`new_handler`函数指针被设为静态成员，每个类在继承实例时获得自己的专属版本。

> `set_new_handler`允许客户指定一个函数，在内存分配无法获得满足时使用。

> nothrow new是一个颇为局限的工具（分配失败时不抛出异常，而是返回空指针），因为它只适用于内存分配；后继的构造函数调用还是可能抛出异常。

### 条款50：了解new和delete的合理替换时机

重载new和delete是为了：

 - 用来检测运用上的错误。在分配销毁动态内存时，有可能会多次delete同一块内存，还可能因编程错误导致数据overrun（写入点在分配区块尾端之后）或underrun（写入点在分配区块起点之前）。通过自定义一个operator new，便可以超额分配内存，以额外空间（位于客户所得区块之前或之后）放置特定的byte patterns（即签名，signatures）。operator delete检查上述签名是否原封不动，若否则表示在分配区的某个生命时间点发生了overrun或underrun。另外，在设计包含signature的内存分配器时要注意内存对齐问题（alignment）。许多计算机体系结构要求特定类型必须放在特定的内存地址上。比如它可能要求指针的地址必须是4倍数（four-byte aligned），double的地址必须是8倍数（eight-byte aligned）。违反这个约定的程序可能会导致运行期硬件异常，或者降低运行效率（比如Inter X86体系）。比如令签名为int（4字节），分配8字节给一个double变量，得到的指针就是没有对齐的。这会令程序崩溃，或者运行速度慢。X86的缺省`operator new[]`分配一个double数组，也是不保证内存对齐的，必须通过自定义operator new来提高其效率。
 - 为了强化效能。用户既需要分配大块内存，也需要大量小块内存。分配内存必须考虑破碎问题（fragmentation），这最终会导致程序无法满足大区块内存需求，即使有总量足够但分散为许多小区块的自由内存。因此，缺省的new和delete会取折中的做法，适用范围广，但并不具备最佳性能。缺省new delete存在的问题包含：1）分配和归还内存的速度慢；2）空间额外开销高；3）非最佳齐位；4）不能将相关对象成簇集中。为特定内存分配需求定制的new和delete能大幅减少分配时间和所需内存。
 - 为了收集使用上的统计数据。自定义new和delete更易收集分配内存的形态（比如倾向于FIFO或LIFO、区块的大小和寿命分布等）。

> 有许多理由需要写个自定的new和delete，包括改善效能、对堆运用错误进行调试、收集堆使用信息。

### 条款51：编写new和delete时需固守常规

在编写自定义new时，为了保持一致性，需要遵守常规：new必须返回正确的值（可以满足内存需求时返回指针，否则抛出`bad_alloc`异常），内存不足时必须调用new-handler函数（如果有的话），必须有对付零内存需求的准备（要求分配0字节），还需要避免不慎掩盖正常形式的new。C++规定，即使客户要求0字节内存，有需要返回一个合法指针，也许是通过分配1字节给用户。一个new的典型行为如下：

```c
void* operator new(std::size_t size) throw(std::bad_alloc) {
    // 处理零内存需求
    if (size == 0) size = 1;
    while (true) {
        ...;   // 尝试分配size字节
        if (success) return pointer;
        // 分配失败；找出目前的new-handler函数
        // 只为了取得当前的new-handler，将其设置为0并无意义
        new_handler globalHandler = set_new_handler(0);
        set_new_handler(globalHandler);
        if (globalHandler) (*globalHandler)();
        else throw std::bad_alloc();
    }
}
```

为自定义类型设计自定义的new和delete，是为了提高创建动态对象的效率。但是，在类被继承后，由于派生类对象的大小大概率会大于基类对象的大小，基类的自定义new和delete或许将不再适用于派生类上。一种处理的方法就是在检查到当前要分配的内存大小和基类大小不再相同时，便改用缺省new处理。

`operator new[]`的函数声明类似`void* operator new[](size_t);`。在`new int[10]`时，函数获得参数值`size = sizeof(int) * 10 + extra_space`（`extra_space`可能存在，用来记录数组元素个数）。在重载`operator new[]`时，我们无法得知即将创建的数组包含多少元素（因为不知道类对象大小）。唯一需要做的事，就是分配一块未加工内存。

operator delete要做的事唯一需要记住的就是“删除空指针永远安全”。在派生类调用基类重载delete函数时，也需要注意和new一样的问题。另外，如果派生类没有虚析构函数，可能会导致delete接收的`size`参数不准确。

> operator new应该内含一个无穷循环，并在其中尝试分配内存，如果它无法满足内存需求，就该调用new-handler。它也应该有能力处理零内存申请。类专属版本则还应该处理”比正确大小更大的（错误）申请”。

> operator delete应该在收到空指针时不做任何事。类专属版本则还应该处理”比正确大小更大的（错误）申请”。

### 条款52：写了placement new也要写placement delete

接受除了`size`以外的参数的new版本是所谓的placement new。一个典型的placement new是`<new>`头文件中的`void* operator new(size_t, void*);`，它在第二个参数指定的位置创建对象。当我们为类自定义placement new时，必须也要提供相应的placement delete，否则在创建对象过程中发生异常（比如在new分配内存成功后，构造函数抛出异常），这时需要把分配的内存销毁，而编译器无法得知使用哪一个delete。一般来说，placement new的第一个参数是`size_t`类型，而placement delete第一个参数是`void*`类型，对应的placement new和placement delete的其他参数个数和类型都应该相同。如果为类定义了placement new，却没有定义相应的placement delete，在上述异常情况中，不会有任何operator delete被调用。

然而，placement delete就只有这个作用了。在主动`delete p`时，调用的是正常的operator delete（它可以是`void operator delete(void*)`，也可以是`void operator delete(void*, size_t)`）。一个只有placement delete却没有正常delete的类无法成功`delete p`（因为正常delete被隐藏了。placement new也会有这个问题！）。只有为类提供了正常的operator new和operator delete，才能正常使用这些功能。

> 当你写一个placement new，请确定也写出了对应的placement delete。如果没有这样做，你的程序可能会发生隐微而时断时续的内存泄漏。

> 当你声明placement new和placement delete，请确定不要无意识地遮掩了它们的正常版本。

### 条款53：不要轻忽编译器的警告

> 严肃对待编译期发出的警告信息。努力在你的编译器的最高（最严苛）警告级别下争取“无任何警告”的荣誉。

> 不要过度依赖编译器的报警能力，因为不同的编译器对待事情的态度并不相同。一旦移植另一个编译器上，你原本依赖的警告信息可能消失。

### 条款54：让自己熟悉包括TR1在内的标准程序库

C++0x的一些新机能被叙述于一份称为TR1的文档内，象征当时的新标准（如今已经是C++1x的年代了）。TR1加入了一些影响广泛的组件，例如智能指针、函数签名、哈希表、正则表达式等，如今已成为C++程序员日常使用的东西。

> TR1自身只是一份规范。为获得TR1提供的好处，你需要一份实物（现在2020年+早就不需要了）。一个好的实物来源是Boost。

### 条款55：让自己熟悉Boost

Boost是一个C++开发者集结的社群，也是一个高质量的开源C++程序库群。Boost有个目标：作为一个“可被加入标准C++之各种功能”的测试场。一些C++新标准中的新功能，就是起源于Boost。Boost以public peer review为基础，审核过程严格，保证了代码的高质量。它涵盖了多个编程主题。

> Boost是一个社群，也是一个网站。致力于免费、源码开放、同僚复审的C++程序库开发。Boost在C++标准化中扮演深具影响力的角色。

> Boost提供许多TR1组件实现品，以及其他许多程序库。