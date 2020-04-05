#include <vector>
#include <queue>
#include <stdio.h>

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

	void inorder() {
		_inorder(root);
	}
	void _inorder(TreeNode<T, U> *node) {
		if (!node) return;
		_inorder(node->left);
		printf("%d ", node->value);
		_inorder(node->right);
	}
};

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


void test_bst() {
    BinarySearchTree<int, int> bst;
    std::vector<int> keys = {3, 2, 1, 4, 6, 5, 8, 7, 10, 20, 15, 21, 39, -2};
    for (auto each : keys) bst.insert(each, each);
    std::vector<int> ops = {1, 3, 2, 10, 20};
	for (auto each : ops) {
		bst.remove(each);
		auto rtn = bst.search(7);
		if (rtn->value != 7) printf("error");
	}
	printf("finished");
}

void test_avl() {
    AVLTree<int, int> avl;
    std::vector<int> keys = {3, 2, 1, 4, 6, 5, 8, 7, 10, 20, 15, 21, 39, -2};
    for (auto each : keys) avl.insert(each, each);
	printf("success insert");
    std::vector<int> ops = {1, 3, 2, 10, 20};
    for (auto each : ops) {
        avl.remove(each);
        auto rtn = avl.search(7);
        if (rtn->value != 7) printf("error");
    }
    printf("finished");
}

void test_splay() {
    SplayTree<int, int> spl;
    std::vector<int> keys = {3, 2, 1, 4, 6, 5, 8, 7, 10, 20, 15, 21, 39, -2};
    for (auto each : keys) spl.insert(each, each);
    printf("success insert");
    std::vector<int> ops = {1, 3, 2, 10, 20};
    for (auto each : ops) {
        spl.remove(each);
        auto rtn = spl.search(7);
        if (rtn->value != 7) printf("error");
    }
    printf("finished");
}

int main() {
	test_splay();
}
