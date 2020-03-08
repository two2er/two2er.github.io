typedef int Rank;  //秩
#define DEFAULT_CAPACITY 3  //默认的初始容量（实际应用中可设置为更大）

template <typename T>
class Vector {  //向量模板类
   protected:
    Rank _size;
    int _capacity;
    T* _elem;                                     //规模、容量、数据区

    void copyFrom(T const* A, Rank lo, Rank hi) {
        _elem = new T[_capacity = 2 * (hi - lo)];
        _size = 0;                     //分配空间，规模清零
        while (lo < hi)                // A[lo, hi)内的元素逐一
            _elem[_size++] = A[lo++];  //复制至_elem[0, hi - lo)
    }  //复制数组区间A[lo, hi)

    void expand() {
        if ( _size < _capacity ) return; //尚未满员时，不必扩容
        if ( _capacity < DEFAULT_CAPACITY ) _capacity = DEFAULT_CAPACITY; //不低于最小容量
        T* oldElem = _elem;  _elem = new T[_capacity <<= 1]; //容量加倍
        for ( int i = 0; i < _size; i++ )
            _elem[i] = oldElem[i]; //复制原向量内容（T为基本类型，或已重载赋值操作符'='）
        /*DSA*/ //printf("\n_ELEM [%x]*%d/%d expanded and shift to [%x]*%d/%d\n", oldElem, _size, _capacity/2, _elem, _size, _capacity);
        delete [] oldElem; //释放原空间
    }   //空间不足时扩容

    void shrink() {
        if (_capacity < DEFAULT_CAPACITY << 1)
            return;  //不致收缩到DEFAULT_CAPACITY以下
        if (_size << 2 > _capacity) return;  //以25%为界
        T* oldElem = _elem;
        _elem = new T[_capacity >>= 1];                         //容量减半
        for (int i = 0; i < _size; i++) _elem[i] = oldElem[i];  //复制原向量内容
        delete[] oldElem;           //释放原空间
    }                               //装填因子过小时压缩

    bool bubble(Rank lo, Rank hi);                //扫描交换

    void bubbleSort(Rank lo, Rank hi);            //起泡排序算法

    Rank max(Rank lo, Rank hi);                   //选取最大元素

    void selectionSort(Rank lo, Rank hi);         //选择排序算法

    void merge(Rank lo, Rank mi, Rank hi);        //归并算法

    void mergeSort(Rank lo, Rank hi);             //归并排序算法

    void heapSort(Rank lo, Rank hi);   //堆排序

    Rank partition(Rank lo, Rank hi);  //轴点构造算法

    void quickSort(Rank lo, Rank hi);  //快速排序算法

    void shellSort(Rank lo, Rank hi);  //希尔排序算法

   public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0,
           T v = 0)  //容量为c、规模为s、所有元素初始为v
    {
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v)
            ;
    }                                                  // s<=c

    Vector(T const* A, Rank n) { copyFrom(A, 0, n); }  //数组整体复制

    Vector(T const* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }  //区间

    Vector(Vector<T> const& V) {
        copyFrom(V._elem, 0, V._size);
    }  //向量整体复制

    Vector(Vector<T> const& V, Rank lo, Rank hi) {
        copyFrom(V._elem, lo, hi);
    }                                      //区间

                                           // 析构函数
    ~Vector() { delete[] _elem; }          //释放内部空间

                                           // 只读访问接口
    Rank size() const { return _size; }    //规模

    bool empty() const { return !_size; }  //判空

    Rank find(T const& e) const {
        return find(e, 0, _size);
    }                                               //无序向量整体查找

    Rank find(T const& e, Rank lo, Rank hi) const {  // assert: 0 <= lo < hi <= _size
        while ((lo < hi--) && (e != _elem[hi])) ;       //从后向前，顺序查找
        return hi;  //若hi < lo，则意味着失败；否则hi即命中元素的秩
    }               //无序向量区间查找

    Rank search(T const& e) const {
        return (0 >= _size) ? -1 : search(e, 0, _size);
    }                                                 //有序向量整体查找

    Rank search(T const& e, Rank lo, Rank hi) const;  //有序向量区间查找
                                                      // 可写访问接口

    T& operator[](Rank r) {
        return _elem[r];
    }  //重载下标操作符，可以类似于数组形式引用各元素

    const T& operator[](Rank r) const {
        return _elem[r];
    }  //仅限于做右值的重载版本

    Vector<T>& operator=(Vector<T> const&) {
        if (_elem) delete[] _elem;       //释放原有内容
        copyFrom(V._elem, 0, V.size());  //整体复制
        return *this;  //返回当前对象的引用，以便链式赋值
    }  //重载赋值操作符，以便直接克隆向量

    T remove(Rank r) {   //删除向量中秩为r的元素，0 <= r < size
        T e = _elem[r];  //备份被删除元素
        remove(r, r + 1);  //调用区间删除算法，等效于对区间[r, r + 1)的删除
        return e;  //返回被删除元素
    }              //删除秩为r的元素

    int remove(Rank lo, Rank hi) {  //删除区间[lo, hi)
        if (lo == hi)
            return 0;  //出于效率考虑，单独处理退化情况，比如remove(0, 0)
        while (hi < _size)
            _elem[lo++] = _elem[hi++];  //[hi, _size)顺次前移hi - lo个单元
        _size = lo;  //更新规模，直接丢弃尾部[lo, _size = hi)区间
        shrink();    //若有必要，则缩容
        return hi - lo;  //返回被删除元素的数目
    }                    //删除秩在区间[lo, hi)之内的元素

    Rank insert(Rank r, T const& e) {  // assert: 0 <= r <= size
        expand();                      //若有必要，扩容
        for (int i = _size; i > r; i--)
            _elem[i] = _elem[i - 1];  //自后向前，后继元素顺次后移一个单元
        _elem[r] = e;
        _size++;   //置入新元素并更新容量
        return r;  //返回秩
    }              //插入元素

    Rank insert(T const& e) { return insert(_size, e); }  //默认作为末元素插入

    void sort(Rank lo, Rank hi) {  //向量区间[lo, hi)排序
        switch (rand() % 6) {
            case 1:
                bubbleSort(lo, hi);
                break;  //起泡排序
            case 2:
                selectionSort(lo, hi);
                break;  //选择排序（习题）
            case 3:
                mergeSort(lo, hi);
                break;  //归并排序
            case 4:
                heapSort(lo, hi);
                break;  //堆排序（第12章）
            case 5:
                quickSort(lo, hi);
                break;  //快速排序（第14章）
            default:
                shellSort(lo, hi);
                break;  //希尔排序（第14章）
        }  //随机选择算法以充分测试。实用时可视具体问题的特点灵活确定或扩充
    }      //对[lo, hi)排序

    void sort() { sort(0, _size); }                       //整体排序

    void unsort(Rank lo, Rank hi) {  //等概率随机置乱区间[lo, hi)
        T* V = _elem + lo;  //将子向量_elem[lo, hi)视作另一向量V[0, hi - lo)
        for (Rank i = hi - lo; i > 0; i--)  //自后向前
            swap(V[i - 1], V[rand() % i]);  //将V[i - 1]与V[0, i)中某一元素随机交换
    }                             //对[lo, hi)置乱

    void unsort() { unsort(0, _size); }                   //整体置乱

    int deduplicate() {       //删除无序向量中重复元素（高效版）
        int oldSize = _size;  //记录原规模
        Rank i = 1;           //从_elem[1]开始
        while (i < _size)     //自前向后逐一考查各元素_elem[i]
            if (find(_elem[i], 0, i) < 0)    //在前缀[0,i)中寻找与之雷同者（至多一个）
                i++;  //若无雷同则继续考查其后继
            else
                remove(i);       //否则删除当前元素
        return oldSize - _size;  //被删除元素总数
    }                            //无序去重

    int uniquify() {  //有序向量重复元素剔除算法（高效版）
        Rank i = 0, j = 0;             //各对互异“相邻”元素的秩
        while (++j < _size)            //逐一扫描，直至末元素
            if (_elem[i] != _elem[j])  //跳过雷同者
                _elem[++i] = _elem[j];  //发现不同元素时，向前移至紧邻于前者右侧
        _size = ++i;
        shrink();      //直接截除尾部多余元素
        return j - i;  //向量规模变化量，即被删除元素总数
    }                  //有序去重

    // 遍历
    void traverse(void (*)(T&)) {
        for (int i = 0; i < _size; i++) visit(_elem[i]);
    }  //遍历（使用函数指针，只读或局部性修改）

    template <typename VST>
    void traverse(VST&) {
        for (int i = 0; i < _size; i++) visit(_elem[i]);
    }  //遍历（使用函数对象，可全局性修改）

};                        // Vector