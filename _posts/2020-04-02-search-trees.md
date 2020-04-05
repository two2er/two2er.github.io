---
layout: post
title: 搜索树：从二叉搜索树到红黑树
excerpt: "各种常见的搜索树。"
categories: [CS]
comments: true

---

---

## 搜索树

搜索树是一种非常常见的数据结构。搜索树的节点对象包含`key`和`value`两个成员变量。给定一棵搜索树和一个指定`key`，我们可以通过在这棵树种找到具有对应`key`的节点，取得其`value`，来完成一次`key->value`的映射操作。这个想法是C++`map`数据结构的根本，C++ `map`就是用红黑树搜索树实现的。当然，C++的`unordered_map`使用哈希函数作为映射方法，增删改查的效率都和以搜索树为基础的映射方法不同。由于搜索树概念的重要性，它在面试中常考，“手写红黑树”已经成为面试者口口相传的一个恐怖传说……

下面是搜索树树节点和树基类的代码实现。下文的各种搜索树都继承自该基类。

```c++
template <typename T, typename U>
struct TreeNode {
	T key;
	U value;
	int height;
	TreeNode *left, *right, *parent;
        TreeNode(T key, U value, int height = -1, TreeNode *parent = nullptr,
                 TreeNode *left = nullptr, TreeNode *right = nullptr)
            : key(key), value(value), height(height),
              parent(parent), left(left), right(right) {}
};

template <typename T, typename U>
class SearchTree {
   protected:
    TreeNode<T, U> *root;
   public:

    SearchTree() : root(nullptr) {}

	~SearchTree() {
		_delete(root);
	}
	void _delete(TreeNode<T, U> *node) {
		if (!node) return;
		_delete(node->left), _delete(node->right);
		delete node;
	}

	#define getHeight(node) ( (node) ? (node)->height : -1 )
	void updateHeight(TreeNode<T, U> *node) {
		int Lheight = getHeight(node->left), Rheight = getHeight(node->right);
		node->height = 1 + (Lheight > Rheight ? Lheight : Rheight);
	}
};
```



## 二叉搜索树

最基本的搜索树是**二叉搜索树（Binary Search Tree，BST）**。它具有以下性质：

> 任一节点r的左（右）子树中，所有节点的key值（若存在）均不大于（不小于）r。

对二叉搜索树进行中序遍历，遍历序列肯定是单调非降的，这个很容易证明。为了简便讨论，假设二叉树的节点没有重复的key值。在二叉搜索树上进行查找，思路也非常简单：对于当前节点，如果它的key值和给定key值一样，那么返回该节点的value值；如果小于当前节点key值，则进入其左孩子继续查找；否则进入其右孩子继续查找。下面代码中，返回搜索结果节点的引用。如果在二叉搜索树中没有key值为`key`的节点，则返回一个空节点的引用。另外，参考*《数据结构（C++语言版）》（丁俊晖，第三版）*，在搜索过程中更新`_hot`节点，表示当前到达节点的父节点。无论是查找成功还是查找失败，`_hot`都是一个有效节点。如下图，可以认为查找失败时，返回了一个所谓的“哨兵节点”。

![binary search tree search](https://two2er.github.io/img/search_tree/binary_search.png)

可以看到，二叉搜索树的搜索时间复杂度和结果节点所在的层数有关。层数在`1`到`n`（节点数）间变动。如果树是一个完全二叉树，层数至多为`O(logn)`，搜索效率较为理想。因此，提高二叉搜素树搜索效率的一种手段就是让树更平衡，也就是形似完全二叉树。**平衡二叉搜索树（Balanced Binary Search Tree，BBST）**有多种，包括AVL树、伸展树、红黑树等，区别在于平衡的方法。在提及平衡二叉搜索树之前，先了解二叉搜索树插入、删除节点的方法。

在一棵二叉搜索树种插入一个给定`key value`的节点，只需要对`key`值进行搜索。如果二叉搜索树中早已存在该`key`值，我们直接忽略这次操作，因为假定了树节点没有重复值。如果树中不存在该`key`值，返回的是一个空节点引用。可以证明，这个空节点所在位置，就是新节点的一个合理插入位置。因为利用它的`key`值一路搜索行成的搜索路径，是满足二叉搜索树定义的。此时`_hot`是它的父节点。不过，由于我们的`search`函数返回的是节点引用，直接修改节点值即可，无需为`_hot`添加左孩子或右孩子。

对二叉搜索树节点的删除比较麻烦。如果要删除的节点`m`是一个叶子节点那还好办，直接将`m`置为空；如果`m`上有老下有小，那么需要在它的孩子中挑选出一个节点来代替它。显然，如果它只有一个孩子，那么让那个孩子来代替它就行了，满足二叉搜索树定义；如果它有两个孩子，那么可以找出它左子树中最大的节点，或者右子树中最小的节点`n`来代替它。为了方便，我们使用`n`。这里替代的做法是交换`m`和`n`的数据，然后删除`n`。`n`显然最多只有一个孩子（而且只能是右孩子），删除它很容易，只要让它的父节点指向`n`的右孩子即可。

从代码中可以看到，插入、删除节点的时间复杂度和树的高度依然息息相关。为了提高查找、插入、删除的效率，优化树的高度是很有必要的。

```c++
template <typename T, typename U>
class BinarySearchTree : public SearchTree<T, U> {
   protected:
	using SearchTree<T, U>::root;
	TreeNode<T, U> *_hot;		// parent of the current returned node

	TreeNode<T, U>*& _search(TreeNode<T, U> *&node, T key) {
		if (!node || node->key == key) return node;
		_hot = node;
		return _search(node->key > key ? node->left : node->right, key);
	}

	void _remove(TreeNode<T, U> *&node) {
		TreeNode<T, U> *obj = node;		// node to remove
		// only one child
		if (!node->left) node = node->right;
		else if (!node->right) node = node->left;
		else {	// has both children
			// find succeed node
			TreeNode<T, U> *succ = node->right, *parent = node;
			while (succ->left) parent = succ, succ = succ->left;
			// swap data
			node->key = succ->key, node->value = succ->value;
			// delete succeed node
			if (parent == node) parent->right = succ->right;
			else parent->left = succ->right;
			obj = succ;
		}
		delete obj;
	}

   public:
    BinarySearchTree() : SearchTree<T, U>() {}

    TreeNode<T, U>*& search(T key) {
		_hot = nullptr;
		return _search(root, key);
	}
    
	void insert(T key, U value) {
		TreeNode<T, U>* &node = search(key);
		if (node) return;		// key already in tree
		// don't care heights and parents
		node = new TreeNode<T, U>(key, value);
	}

	void remove(T key) {
		TreeNode<T, U>* &node = search(key);
		if (!node) return;		// no such node
		_remove(node);
	}
};
```



## AVL树

**AVL树（AVL tree）**是最早被发明的平衡二叉搜索树，其中的AVL来源于发明者的姓名。在渐进意义下，AVL数可始终将树高度控制在`O(logn)`以内，从而保证查找、插入、删除操作的时间复杂度也为`O(logn)`。AVL树有一套“平衡”的标准：任一节点都有一个**平衡因子（balance factor）**，定义为其左右子树的高度差。在本文中，空树的高度为-1，单节点树的高度为0。所谓AVL树，即平衡因子受限的二叉搜索树，其每个节点平衡因子的绝对值均不超过1。可以想象，一棵AVL树具有类似完全二叉树的姿态。它的子树也是AVL树，形如一个锥形。在插入删除节点后，如果打破了AVL树原有的平衡，会对某个子树进行不超过两次的旋转操作，使其恢复平衡。旋转操作有以下两种，分别是zig和zag：

![zig zag](https://two2er.github.io/img/search_tree/zigzag.png)

zig zag操作虽然简单，但它们能保证旋转后的二叉搜索树是**等价变换**的，即中序遍历序列相等。zig zag旋转后，原二叉搜索树的性质不会改变。显然，插入节点`x`的父节点不可能失衡，但是父节点高度可能会加一，导致`x`的祖父节点可能会失衡。因此，那些因插入`x`的节点，最深的不深过`x`的祖父节点。记这个最深的平衡因子绝对值不小于1的节点为`g`，在`g`通往`x`的路上，依次经过的前三个节点为`g->p->v`。`g p v`可能的场景有四种：

- `g->right == p, p->right == v`
- `g->right == p, p->left == v`
- `g->left == p, p->left == v`
- `g->left == p, p->right == v`

显然，第3、4种是第1、2种的镜面对称，我们只需讨论如何处理第1、2种场景即可。对于第一种场景，旋转操作如下：

![unbalancedAVL2](https://two2er.github.io/img/search_tree/unbalancedAVL2.png)

第二种场景需要一次zig操作加一次zag操作，才能让AVL树恢复平衡。对于第3、4种场景，只需要镜面对称操作即可。

如果是因为删除节点导致的失衡，处理方式会稍微不同。删除的节点可能是叶子节点，也可能是一个至多仅有右孩子的中间节点。因此后果仅是可能让包含这个节点的子树高度减一。可以证明，因此导致的失衡节点（平衡因子绝对值超过1）至多只有1个。同时，这个失衡节点`g`可能是被删除节点`x`的父节点。在确定了`g`的位置后，在它不包含`x`的一侧，必有一个非空孩子`p`（因为这一侧比另一侧高2单位导致了失衡），且`p`也必有孩子（至少一个）。从`p`的孩子中选出更高者`v`；如果等高则优先选和`p`同向者（即，`v`与`p`同为左孩子或右孩子）。下图示意`g->left == p, p->left == v`、`x`属于`T3`的场景，通过一次zig操作即可使AVL树重新平衡：

![unbalancedAVL3](https://two2er.github.io/img/search_tree/unbalancedAVL3.png)

图中连接的灰色方框表示不会同时为空的两个节点；另一个灰色方框表示可有可无的节点。另一种场景`g->left == p, p->right == v`、`x`属于`T3`如下：

![unbalancedAVL4](https://two2er.github.io/img/search_tree/unbalancedAVL4.png)

在至多两次旋转操作后也恢复了平衡。然而，这棵以`g`为根的子树虽然恢复平衡了，但它的高度可能会减1，导致它的祖先节点继续失衡！因此，在平衡该子树后，还要继续向上检查节点，是否保持平衡；对于失衡的节点要再进行旋转操作。

从上面4张图中，我们可以总结出它们的模式：在恢复平衡后，它们都展现出一种“3+4”结构：

![3+4](https://two2er.github.io/img/search_tree/3p4.png)

其中，`a b c`是`g p v`的某一个。我们可以通过识别旋转前`a b c`的相对位置，来确定旋转后它们应该在的位置。因此，我们只需要在旋转前的子树上提取出`a b c T0 T1 T2 T3`，然后组装成“3+4”结构，就能完全重新平衡操作，无须真的去旋转。这种思路能让代码变得异常简洁。

```c++
template <typename T, typename U>
class AVLTree : public BinarySearchTree<T, U> {
   protected:

    using SearchTree<T, U>::root;
    using BinarySearchTree<T, U>::_hot;
	using SearchTree<T, U>::updateHeight;
	using BinarySearchTree<T, U>::_remove;
	
	#define balanceFactor(node) ( getHeight((node)->left) - getHeight((node)->right) )
	#define isLChild(node) ( (node)->parent && (node)->parent->left == (node) )
	#define fromParentTo(node)                                                   \
		((node)->parent                                                          \
			? (isLChild(node) ? (node)->parent->left : (node)->parent->right) \
			: root)

    bool AvlUnbalanced(TreeNode<T, U> *node) {
        int bf = balanceFactor(node);
        return -1 > bf || bf > 1;
    }
    TreeNode<T, U> *tallerChild(TreeNode<T, U> *node) {
        // return the taller child. if left.height == right.height, return the
        // one of same side
        int Lheight = getHeight(node->left), Rheight = getHeight(node->right);
        if (Lheight > Rheight) return node->left;
        else if (Rheight > Lheight) return node->right;
        else if (isLChild(node)) return node->left;
        else return node->right;
    }

	TreeNode<T, U>* rotate(TreeNode<T, U> *v) {
		TreeNode<T, U> *p = v->parent, *g = p->parent;
		// connect 3+4 structure based on relation between g, p, v
		if (isLChild(p)) { // zig
			if (isLChild(v)) { // zig-zig
				p->parent = g->parent;
				return connect34(v, p, g, v->left, v->right, p->right, g->right);
			} else { // zig-zag
				v->parent = g->parent;
				return connect34(p, v, g, p->left, v->left, v->right, g->right);
			}
		} else { // zag
			if (isLChild(v)) { // zag-zig
				v->parent = g->parent;
				return connect34(g, v, p, g->left, v->left, v->right, p->right);
			} else { // zag-zag
				p->parent = g->parent;
				return connect34(g, p, v, g->left, p->left, v->left, v->right);
			}
		}
	}

	TreeNode<T, U>* connect34(TreeNode<T, U> *a, TreeNode<T, U> *b, TreeNode<T, U> *c,
		TreeNode<T, U> *T0, TreeNode<T, U> *T1, TreeNode<T, U> *T2, TreeNode<T, U> *T3) {
			a->left = T0; if (T0) T0->parent = a;
			a->right = T1; if (T1) T1->parent = a; updateHeight(a);
			c->left = T2; if (T2) T2->parent = c;
			c->right = T3; if (T3) T3->parent = c; updateHeight(c);
			b->left = a; a->parent = b;
			b->right = c; c->parent = b; updateHeight(b);
			return b;
	}

   public:

    using BinarySearchTree<T, U>::search;

    AVLTree() : BinarySearchTree<T, U>() {}

	void insert(T key, U value) {
		TreeNode<T, U>* &node = search(key);
		if (node) return;		// key already in tree
		// height = 0, parent = _hot
		node = new TreeNode<T, U>(key, value, 0, _hot);
		// update heights & check balance factors for ancestors
		for (TreeNode<T, U> *g = _hot; g; g = g->parent) {
			if (AvlUnbalanced(g)) { // rotate based on g, p, v
			    TreeNode<T, U>* &parent = fromParentTo(g);
				parent = rotate(tallerChild(tallerChild(g)));
				break;
			} else { // insert node increases heights of ancestors
				updateHeight(g);
			}
		}
	}
	void remove(T key) {
		TreeNode<T, U>* &node = search(key);
		if (!node) return;		// no such node
		_remove(node);
		// update heights & check balance factors for ancestors
		for (TreeNode<T, U> *g = _hot; g; g = g->parent) {
			if (AvlUnbalanced(g)) { // rotate & get root of the subtree
				TreeNode<T, U>* &parent = fromParentTo(g);
				g = parent = rotate(tallerChild(tallerChild(g)));
			}
			updateHeight(g);
		}
	}
};
```



## 伸展树

**伸展树（splay tree）**是平衡二叉搜索树的另一种形式。相比AVL树，伸展树无须时刻都严格保持全树的平衡，但却能够在任何足够长的真实操作序列中，保持分摊意义上的高效率。伸展树的核心思路是局部性。对于二叉搜索树，数据局部性具体体现在：

- 刚刚被访问过的节点，极有可能在不久以后再次被访问到
- 将被访问的下一节点，极有可能就处于不久之前被访问过的某个节点的附近

因此，只需将刚被访问的节点，及时转移到树根（附近），即可加速后续的操作。最直接的一种转移方法是**逐层伸展**，即不断旋转它的父节点，每次让它向上移动一层：

![splay 1](https://two2er.github.io/img/search_tree/splay1.png)

这种伸展方法并不好，在极端情况下，它的单次访问分摊时间复杂度可能为`O(n)`（参考*《数据结构（C++语言版）》（丁俊晖，第三版）* P205页的例子）。一种较好的伸展方式是**双层伸展**，即每次都从当前节点`v`向上追溯两层，并根据其父节点`p`以及祖父节点`g`的相对位置，进行相应的旋转。旋转方式有zig和zag，经过至多两次旋转，就能将`v`转移到以`g`为根的子树的根节点处。双层伸展的移动轨迹和逐层伸展很相似，但是它有zig-zig或zag-zag这种操作：

![splay 3](https://two2er.github.io/img/search_tree/splay3.png)

就是说，它会先把`v`上面的节点先旋转了，然后再将`v`向上提。每次将`v`向上移动两层，当`v`离根节点只有一层时，再通过一次zig或zag旋转，即可将它转移到整棵树的根节点处。可以证明，在进行双层伸展将节点转移到根处后，树的结构会比逐层伸展调整得更加平衡。就树的形态而言，双层伸展策略能“折叠”被访问的子树分支，从而有效地避免对长分支的连续访问。可以证明，双层伸展的伸展树的单次操作可在分摊的`O(logn)`时间内完成。

![splay 2](https://two2er.github.io/img/search_tree/splay2.png)

伸展树的查找、插入、删除操作基于二叉搜索树，只是加上了伸展的操作。在查找到节点后，需要将其转移到根节点处。插入之前，会先查找插入位置，将插入位置的父节点提到根节点处。然后，根据插入节点和根节点的值大小关系，将插入节点插入到根节点的位置：

![splay 4](https://two2er.github.io/img/search_tree/splay4.png)

删除操作和插入操作类似；在搜索到删除节点后，删除节点会被提升到根节点处，然后将其删除。这样，伸展树就变成了两棵子树。然后，我们可以以删除的`key`值再搜索一次右子树。由于右子树上所有的键值都会大于`key`，所以这次搜索后，`_hot`是右子树中键值最小的节点。将其提升到右子树根节点处，它不可能有左孩子。因此，只需要将左子树设为该节点的左孩子即可。其实，也可以在左子树中搜索`key`，这样根节点处是比`key`小的最大节点。

![splay 5](https://two2er.github.io/img/search_tree/splay5.png)

```c++
template <typename T, typename U>
class SplayTree : public BinarySearchTree<T, U> {
   protected:

    using SearchTree<T, U>::root;
    using BinarySearchTree<T, U>::_hot;
    using BinarySearchTree<T, U>::_remove;
	using BinarySearchTree<T, U>::_search;

	void attachAsLChild(TreeNode<T, U> *node, TreeNode<T, U> *lc) {
		node->left = lc; if (lc) lc->parent = node;
	}
	void attachAsRChild(TreeNode<T, U> *node, TreeNode<T, U> *rc) {
		node->right = rc; if (rc) rc->parent = node;
	}

	TreeNode<T, U>* splay(TreeNode<T, U> *v) {
		// move the node to root
		if (!v) return nullptr;
		TreeNode<T, U> *p, *g, *gg;
		while ((p = v->parent) && (g = p->parent)) {
			gg = g->parent;		// parent of the subtree
			if (isLChild(v)) {
				if (isLChild(p)) { 	// zig-zig
					attachAsLChild(g, p->right); attachAsLChild(p, v->right);
					attachAsRChild(p, g); attachAsRChild(v, p);
				} else { // zig-zag
					attachAsLChild(p, v->right); attachAsRChild(g, v->left);
					attachAsLChild(v, g); attachAsRChild(v, p);
				}
			} else if (!isLChild(p)) { // zag-zag
				attachAsRChild(g, p->left); attachAsRChild(p, v->left);
				attachAsLChild(p, g); attachAsLChild(v, p);
			} else { // zag-zig
				attachAsRChild(p, v->left); attachAsLChild(g, v->right);
				attachAsRChild(v, g); attachAsLChild(v, p);
			}
			if (!gg) { // v is root now
				v->parent = nullptr;
			} else {
				(g == gg->left) ? attachAsLChild(gg, v) : attachAsRChild(gg, v);
			}
		}
		if (p = v->parent) { // g is null, need one more zig/zag
			if (isLChild(v)) { // zig
				attachAsLChild(p, v->right); attachAsRChild(v, p);
			} else { // zag
				attachAsRChild(p, v->left); attachAsLChild(v, p);
			}
		}
		v->parent = nullptr;	// set as root
		return v;
	}

   public:
    TreeNode<T, U> *&search(T key) {
        _hot = nullptr;
        TreeNode<T, U> *v = _search(root, key);
		root = splay((v ? v : _hot));	// set the last visited node as root
		return root;
    }

	void insert(T key, U value) {
		if (!root) { root = new TreeNode<T, U>(key, value); return; }
		TreeNode<T, U>* node = search(key);
		if (key == node->key) return;		// key already in tree
		root = new TreeNode<T, U>(key, value);
		node->parent = root;
		if (node->key < key) {
			root->left = node; root->right = node->right;
			if (node->right) {
				node->right->parent = root; node->right = nullptr;
			}
		} else {
			root->right = node; root->left = node->left;
			if (node->left) {
				node->left->parent = root; node->left = nullptr;
			}
		}
	}
	void remove(T key) {
		if (!root || search(key)->key != key) return;	// no such node
		TreeNode<T, U> *to_delete = root;
		if (!root->left) { // delete root directly
			root = root->right;
			if (root) root->parent = nullptr;
		} else if (!root->right) {
			root = root->left;
			if (root) root->parent = nullptr;
		} else {
			root = root->right; root->parent = nullptr;
			search(key); // search right subtree
			root->left = to_delete->left; to_delete->left->parent = root;
		}
		delete(to_delete);
	}
};
```

















