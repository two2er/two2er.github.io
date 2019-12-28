---
layout: post
title: Effective C++ Note
excerpt: "Effective C++笔记"
categories: [book]
comments: true
---

## 

## 让自己习惯C++

### 条款1：视C++为一个语言联邦

C++是多重范型编程语言（multiparadigm programming language），最好视其为一个相关语言组成的联邦，而非单一语言。它主要包含四种次语言：C, Object-Oriented C++, Template C++, STL。每个次语言都有自己的规约，切换次语言时需改变策略。

> C++高效编程守则视状况而变化，取决于你使用C++的哪一部分。

### 条款2：尽量以const、enum、inline替换#define

“宁可以编译器替换预处理器”。
```C
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

程序库代码：
```
class FileSystem {
public:
    ...
    //一个成员函数
    std::size_t numDisks() const;
    ...
};
//预备给客户用的对象
extern FileSystem tfs; 
```
客户建立的代码：
```
class Directory {
public:
    Directory( params );
    ...
};
Directory::Directory( params ) {
    ...
    std::size_t disks = tfs.numDisks();
    ...
}
...
Directory tempDir( params );
```
是`tfs`先被初始化，还是`tempDir`先被初始化，无法确定。要用reference-returning模式：
```
class FileSystem {
public:
    ...
    //一个成员函数
    std::size_t numDisks() const;
    ...
};
//预备给客户用的对象
FileSystem& tfs() {
    static FileSystem fs;
    return fs;
}
```
客户使用`tfs()`而非`tfs`。（non-local static -> local static）

> 为内置型对象进行手工初始化，因为C++不保证初始化它们。

> 构造函数最好使用初始化列表而不是在构造函数体中赋值。

> 用local-static代替non-local static。

## 构造/析构/赋值运算

### 条款5：了解C++默认编写并调用哪些函数

> 编译器可以暗自为类创建默认构造函数、拷贝构造函数、拷贝赋值运算符和析构函数。

### 条款6：若不想使用编译器自动生成的函数，就该明确拒绝

C++11允许我们定义成员函数为`=delete`来阻止编译器自动生成它们。

> 为驳回编译器自动生成默认成员函数，可以将相应成员函数声明为private并不予实现。

### 条款7：为多态基类声明virtual析构函数

如果你想要一个abstract class, 但实现的成员函数没有可以设成pure virtual的，不妨把析构函数设成pure virtual。不过这样就必须为析构函数提供一个定义：
```
AWOV::~AWOV() {}
```

> 多态的基类应该声明一个virtual析构函数。如果类带有任何虚函数，它就应该拥有一个虚析构函数。

> 类的设计目的如果不是作为基类使用，或者不具备多态性，就不该声明虚析构函数。

### 条款8：别让异常逃离析构函数

为用户提供显式析构/关闭对象的接口（非析构函数），如果关闭过程中抛出异常，由用户来处理。但是在隐式析构过程中（析构函数），如果抛出异常，最好要捕获之并适当处理，防止内存泄漏。

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

使用智能指针来确保动态内存在离开作用域时正确释放。不过智能指针还有环状引用（cycles of references，例如两个没有被使用的对象彼此互指）等问题，所以要小心处理。

> 为防止资源泄漏，请使用RAII对象（Resource Acquisition Is Initialization）：它们在构造函数中获得资源并在析构函数中释放资源。

### 条款14：在资源管理类中小心coping行为

RAII算是资源管理类。在复制资源管理类对象时要小心。如果复制是不合理的，要禁止用户复制；或者用引用计数法等提高复制的安全性。有时根据资源的特性，在复制资源管理类对象时，一并深复制它的资源。

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

> 类的设计就是类型的设计。在定义一个类型之前，有诸多问题要考虑，比如是否必要、如何创建和销毁、如何初始化和赋值、合法值有哪些、继承关系如何、如何类型转换和开放接口，等等。

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

根据“越少函数可以访问数据，数据的封装性就越低”的标准，第二种封装性更强，因为它不增加可访问private数据成员的函数数量。另一点，像这样的“便利函数”，可以和类放在同一个namespace下，但分在多个头文件中，实现更好的组织结构。（不过个人认为这个不是很有必要，有种流于形式的感觉）

> 宁可拿non-member、non-friend函数替换member函数。这样做可以增加封装性、包裹弹性（packaging flexibility）和机能扩充性。

### 条款24：若所有参数皆需要类型转换，请为此采用non-member函数

比如说字符串相加。如果重载了string的+成员操作符，那么`"char" + str`就无法调用该操作符。最好是将该操作符定义为`operator+(const string &lhs, const string &rhs)`，这样允许`const char*`隐式转换为string以相加。

> 如果你需要为某个函数的所有参数（包括被this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是个non-member。

### 条款25：考虑写出一个不抛出异常的swap函数

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
```
class Base {
public:
    void func() {cout << v << endl;}
    int v = 10;
};

class Derived: public Base {
   public:
    void f2() {static_cast<Base>(*this).func();}
    int v = 20;
};

Derived d;
d.func();
// 10. 如果Derived类也有一个void func() {cout << v << endl;}，那么会输出20。派生类向基类的转换是很容易出错的。
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

在异常被抛出时，异常安全函数提供以下三个保证之一：1）基本承诺（对象仍处于有效状态，但不确定是哪种状态）；2）强烈保证（回复到调用函数前状态，“全有或全无”）；3）nothrow状态（承诺不抛出异常）。

copy and swap策略能提供强烈保证。首先为打算修改的对象做一个副本，在副本上修改，最后再和原对象交换。修改过程中的异常不会影响原对象。不过copy and swap会面临效率问题，在一些情况下不是很实际。

时代在进步，如今我们致力于写出异常安全的代码。不过，系统总是要么全局安全，要么不安全的，没有“局部异常安全系统”。在并入过去不安全的代码后，如今的系统仍然无法是异常安全的。

> 异常安全函数即使发生异常也不会泄漏资源或允许任何数据结构败坏。这样的函数区分为三种可能的保证：基本型、强烈型、不抛异常型。

> “强烈保证”往往能够以copy and swap实现出来，但“强烈保证”并非对所有函数都可实现或具备现实意义。

> 函数提供的“异常安全保证”通常最高只等于其调用之各个函数的“异常安全保证”中的最弱者。

### 条款30：透彻了解inlining的里里外外

过多的inline函数会造成程序体积过大，造成的代码膨胀会导致额外的换页（paging）行为，降低指令高速缓存装置的击中率（instruction cache hit rate），以及伴随这些而来的效率损失。

编译器有时会拒绝函数inline。比如，要取inline函数地址，它就必须是个outline函数；虚函数也不能是inline，因为它是否调用在运行期才被决定。而有些函数（比如构造/析构函数），即使函数体内代码很少，不代表编译器不会为其生成代码，同样不适合inline。

在inline函数被修改以后，程序需要重新编译。如果是outline函数被修改，只需要重新连接即可；如果程序库采取动态连接，修改后的outline函数甚至可以不知不觉就被程序吸纳。再者，inline函数内无法设置debug断点，难以调试。综上，设置inline时需要考虑的问题是很多的。

> 将大多数inline限制在小型、被频繁调用的函数身上。这可使日后的调试过程和二进制升级（binary upgradability，指重新连接）更容易，也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。

> 不要只因为函数模板出现在头文件，就将它们声明为inline。