#include <stdio.h>
#include <queue>
#include <vector>

template <typename T, typename U>
struct MultiNode {
    std::vector<T> keys;
    std::vector<U> values;
    int height;
    MultiNode *parent;
    std::vector<MultiNode*> children;
    MultiNode() : parent(nullptr), height(0), children(1, nullptr) {}
};

template <typename T, typename U>
class MultiTree {
   protected:
    MultiNode<T, U> *root;

   public:
    MultiTree() : root(nullptr) {}

    ~MultiTree() { _delete(root); }
    void _delete(MultiNode<T, U> *node) {
        if (!node) return;
        for (MultiNode<T, U>* child : node->children)
            _delete(child);
        delete node;
    }

    #define getHeight(node) ((node) ? (node)->height : -1)
    void updateHeight(MultiNode<T, U> *node) {
        int max_height = -1, child_height;
        for (MultiNode<T, U>* child : node->children) {
            child_height = getHeight(child);
            if (child_height > max_height) max_height = child_height; 
        }
        node->height = 1 + max_height;
    }
};

template <typename T, typename U>
class BTree : public MultiTree<T, U> {
   protected:
    using MultiTree<T, U>::root;
    
    int order;  // m-order B-Tree
    MultiNode<T, U> *_hot;

    #define move_range(from,to,start) (to).assign((from).begin()+(start), (from).end()); (from).erase((from).begin()+(start), (from).end());

    void solveOverflow(MultiNode<T, U> *node) {
        if (node->children.size() <= order) return;
        // move s-th key to parent node
        int s = order / 2;
        T key = node->keys[s];
        U value = node->values[s];
        // split to left and right child
        MultiNode<T, U> *right = new MultiNode<T, U>();
        move_range(node->keys, right->keys, s + 1); node->keys.pop_back();
        move_range(node->values, right->values, s + 1); node->values.pop_back();
        move_range(node->children, right->children, s + 1);
        int i;
        for (i = 0; i < right->children.size(); ++i)
            if (right->children[i]) right->children[i]->parent = right;
        // insert s-th key to parent
        MultiNode<T, U> *parent = node->parent;
        if (!parent) { // new root
            parent = new MultiNode<T, U>();
            root = parent;
            root->children[0] = node;
        }
        for (i = 0; i < parent->keys.size(); ++i)
            if (parent->keys[i] > key) break;
        parent->keys.insert(parent->keys.begin()+i, key);
        parent->values.insert(parent->values.begin()+i, value);
        parent->children.insert(parent->children.begin()+i+1, right);
        node->parent = parent; right->parent = parent;
        solveOverflow(parent);
    }

    void solveUnderflow(MultiNode<T, U> *v) {
        if ((order + 1) / 2 <= v->children.size()) return;
        MultiNode<T, U> *p = v->parent;
        if (!p) { // v is the root
            if (!v->keys.size() && v->children[0]) {
                // when merging the two children of root, root has only one key
                root = v->children[0]; root->parent = nullptr;
                delete(v);
            }
            return;
        }
        int i = 0;  // v is the i-th child of p
        while (p->children[i] != v) ++i;
        if (0 < i) { // has left siblings
            MultiNode<T, U> *L = p->children[i-1];
            if ((order + 1) / 2 < L->children.size()) { // case 1
                // move y and x
                v->keys.insert(v->keys.begin(), p->keys[i-1]);
                v->values.insert(v->values.begin(), p->values[i-1]);
                p->keys[i-1] = L->keys.back(); p->values[i-1] = L->values.back();
                L->keys.pop_back(); L->values.pop_back();
                // and the last child of L
                v->children.insert(v->children.begin(), L->children.back());
                L->children.pop_back();
                if (v->children[0]) v->children[0]->parent = v;
                return;
            }
        } else if (p->children.size() - 1 > i) { // has right siblings
            MultiNode<T, U> *R = p->children[i+1];
            if ((order + 1) / 2 < R->children.size()) { // case 2
                v->keys.insert(v->keys.end(), p->keys[i]);
                v->values.insert(v->values.end(), p->values[i]);
                p->keys[i] = R->keys[0]; p->values[i] = R->values[0];
                R->keys.erase(R->keys.begin()); R->values.erase(R->values.begin());
                v->children.push_back(R->children[0]);
                R->children.erase(R->children.begin());
                if (v->children.back()) v->children.back()->parent = v;
                return;
            }
        }
        // case 3
        if (0 < i) { // merge with left sibling
            MultiNode<T, U> *L = p->children[i-1];
            // merge them to L. delete v
            L->keys.push_back(p->keys[i-1]); L->values.push_back(p->values[i-1]);
            p->keys.erase(p->keys.begin()+i-1); p->values.erase(p->values.begin()+i-1);
            p->children.erase(p->children.begin() + i);   // delete a branch
            // move keys and children from v to L
            L->keys.insert(L->keys.end(), v->keys.begin(), v->keys.end());
            L->values.insert(L->values.end(), v->values.begin(), v->values.end());
            L->children.insert(L->children.end(), v->children.begin(), v->children.end());
            for (MultiNode<T, U> *child : L->children)
                if (child) child->parent = L;
            delete(v);
        } else { // merge with right sibling
            MultiNode<T, U> *R = p->children[i + 1];
            R->keys.insert(R->keys.begin(), p->keys[i]);
            R->values.insert(R->values.begin(), p->values[i]);
            p->keys.erase(p->keys.begin() + i); p->values.erase(p->values.begin() + i);
            p->children.erase(p->children.begin() + i);
            // move keys and children from v to R
            R->keys.insert(R->keys.begin(), v->keys.begin(), v->keys.end());
            R->values.insert(R->values.begin(), v->values.begin(), v->values.end());
            R->children.insert(R->children.begin(), v->children.begin(), v->children.end());
            for (MultiNode<T, U> *child : R->children)
                if (child) child->parent = R;
            delete (v);
        }
        solveUnderflow(p);
    }

   public:
    BTree(int order = 3) : MultiTree<T, U>(), order(order) {}

    MultiNode<T, U>* search(T key) {
        MultiNode<T, U> *curr = root;
        _hot = nullptr;
        int i;
        while (curr) {
            // search key in current node
            for (i = 0; i < curr->keys.size(); ++i) {
                if (curr->keys[i] == key) return curr;
                else if (curr->keys[i] > key) break;
            }
            _hot = curr;
            curr = curr->children[i];   // go to next node
        }
        return nullptr; // no such key
    }

    void insert(T key, U value) {
        MultiNode<T, U> *node = search(key);
        if (node) return;   // has existed
        if (!_hot) root = _hot = new MultiNode<T, U>();
        int i;
        for (i = 0; i < _hot->keys.size(); ++i) {
            if (_hot->keys[i] > key) break;
        }
        // insert key at _hot->key[i]
        _hot->keys.insert(_hot->keys.begin()+i, key);
        _hot->values.insert(_hot->values.begin()+i, value);
        // add a new child
        _hot->children.insert(_hot->children.begin()+i+1, nullptr);
        solveOverflow(_hot);
    }

    void remove(T key) {
        MultiNode<T, U> *node = search(key);
        if (!node) return;  // no such node
        // find key in node
        int i;
        for (i = 0; i < node->keys.size(); ++i) {
            if (node->keys[i] == key) break; 
        }
        if (node->children[0]) { // internal node
            MultiNode<T, U> *u = node->children[i+1];   // right subtree
            while (u->children[0]) u = u->children[0];  // direct succeed
            node->keys[i] = u->keys[0]; node->values[i] = u->values[0];
            node = u; i = 0;    // go to the leaf node
        }
        // delete a key of leaf node
        node->keys.erase(node->keys.begin()+i); node->values.erase(node->values.begin()+i);
        node->children.erase(node->children.begin()+i+1);
        solveUnderflow(node);
    }
};

void test_btree() {
    BTree<int, int> bt;
    std::vector<int> keys = {3, 2, 1, 4, 6, 5, 8, 7, 10, 20, 15, 21, 39, -2};
    for (auto each : keys) bt.insert(each, each);
    printf("success insert");
    std::vector<int> ops = {1, 3, 2, 10, 20};
    for (auto each : ops) {
        bt.remove(each);
        auto rtn = bt.search(7);
        bool flag = true;
        for (int value : rtn->values) {
            if (value == 7) flag = false;
        }
        if (flag) printf("error!");
    }
    printf("finished");
}

int main() { test_btree(); }
