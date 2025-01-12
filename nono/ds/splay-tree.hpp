#pragma once

#include <cassert>
#include <iostream>
#include <utility>

#include "../structure/monoid.hpp"

///  DEPRECATED

namespace nono {

namespace splay_tree_node {

template <class M>
struct Node {
    using Rev = monoid::Rev<M>;
    using T = M::Value;

    bool rev = false;
    Rev::Value elem, tot;

    int size;
    Node* l = nullptr;
    Node* r = nullptr;
    Node* p = nullptr;

    Node(): elem(M::e()), tot(M::e()), size(1) {}
    Node(T elem): elem(elem), tot(elem), size(1) {}
    ~Node() {
        if (l) delete l;
        if (r) delete r;
    }

    void push() {
        if (rev) {
            std::swap(l, r);
            std::swap(tot.ord, tot.rev);
            if (l) l->rev ^= true;
            if (r) r->rev ^= true;
            rev = false;
        }
    }

    void update() {
        tot = M::e();
        size = 1;
        if (l) {
            tot = Rev::op(tot, l->tot);
            size += l->size;
        }
        tot = Rev::op(tot, elem);
        if (r) {
            tot = Rev::op(tot, r->tot);
            size += r->size;
        }
    }

    void splay() {
        while (state() != 0) {
            if (p->state() != 0) {
                if (state() == p->state()) {
                    p->rotate();
                    rotate();
                } else {
                    rotate();
                    rotate();
                }
            } else {
                rotate();
            }
        }
    }

  private:
    void right_rotate() {
        Node* above = p->p;
        Node* center = this;
        Node* below = p;

        assert(below->l == center);
        below->l = center->r;
        if (below->l) below->l->p = below;

        center->r = below;
        below->p = center;

        center->p = above;
        if (above) {
            if (above->l == below) {
                above->l = center;
            } else {
                above->r = center;
            }
        }
        below->update();
        center->update();
    }

    void left_rotate() {
        Node* above = p->p;
        Node* center = this;
        Node* below = p;

        assert(below->r == center);
        below->r = center->l;
        if (below->r) below->r->p = below;

        center->l = below;
        below->p = center;

        center->p = above;
        if (above) {
            if (above->l == below) {
                above->l = center;
            } else {
                above->r = center;
            }
        }
        below->update();
        center->update();
    }

    void rotate() {
        assert(p);
        if (p->l && p->l == this) {
            right_rotate();
        } else {
            left_rotate();
        }
    }

    int state() {
        if (p) {
            return (p->l == this ? 1 : -1);
        }
        return 0;
    }
};

template <class M>
using NodePtr = Node<M>*;

///  # split(node, k)
///  return {node [0, k), node [k, n)}
///  O(log n)
template <class M>
std::pair<NodePtr<M>, NodePtr<M>> split(NodePtr<M> node, int k) {
    if (node) node->push();
    if (k == 0) return {nullptr, node};
    if (node->size == k) return {node, nullptr};
    if (!node->l || node->l->size < k) {
        auto [lhs, rhs] = split(node->r, k - 1 - (node->l ? node->l->size : 0));
        node->r = lhs;
        if (node->l) node->l->push();
        node->update();
        return {node, rhs};
    } else {
        auto [lhs, rhs] = split(node->l, k);
        node->l = rhs;
        if (node->r) node->r->push();
        node->update();
        return {lhs, node};
    }
}

///  # merge(lhs, rhs)
///  return lhs + rhs
///  O(log n)
template <class M>
NodePtr<M> merge(NodePtr<M> lhs, NodePtr<M> rhs) {
    if (lhs) lhs->push();
    if (rhs) rhs->push();
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (lhs->priority < rhs->priority) {
        lhs->r = merge(lhs->r, rhs);
        if (lhs->l) lhs->l->push();
        lhs->update();
        return lhs;
    } else {
        rhs->l = merge(lhs, rhs->l);
        if (rhs->r) rhs->r->push();
        rhs->update();
        return rhs;
    }
}

///  # insert(node, k, v)
///  return node[0, k) + {v} + node[k, n)
///  O(n)
template <class M>
NodePtr<M> insert(NodePtr<M> node, int k, typename M::Value v) {
    auto [lhs, rhs] = split(node, k);
    return merge(merge(lhs, new Node<M>(v)), rhs);
}

///  # erase(node, k)
///  return node[0, k - 1) + node[k, n)
///  O(n)
template <class M>
NodePtr<M> erase(NodePtr<M> node, int k) {
    auto [lhs, t] = split(node, k);
    auto [mhs, rhs] = split(t, 1);
    delete mhs;
    return merge(lhs, rhs);
}

}  //  namespace splay_tree_node

///  brief : deprecated
template <class Key, class T>
class SplayTree {
    struct Node;
    using NodePtr = Node*;

    struct Node {
        Node() = default;
        Node(Key key, T value): key(key), value(value), size(1), left(nullptr), right(nullptr), parent(nullptr) {}

        void right_rotate() {
            NodePtr above = parent->parent;
            NodePtr center = this;
            NodePtr below = parent;

            assert(below->left == center);
            below->left = center->right;
            if (below->left) below->left->parent = below;

            center->right = below;
            below->parent = center;

            center->parent = above;
            if (above) {
                if (above->left == below) {
                    above->left = center;
                } else {
                    above->right = center;
                }
            }
            below->update();
            center->update();
        }

        void left_rotate() {
            NodePtr above = parent->parent;
            NodePtr center = this;
            NodePtr below = parent;

            assert(below->right == center);
            below->right = center->left;
            if (below->right) below->right->parent = below;

            center->left = below;
            below->parent = center;

            center->parent = above;
            if (above) {
                if (above->left == below) {
                    above->left = center;
                } else {
                    above->right = center;
                }
            }
            below->update();
            center->update();
        }

        void rotate() {
            assert(parent);
            if (parent->left && parent->left == this) {
                right_rotate();
            } else {
                left_rotate();
            }
        }

        int state() {
            if (parent) {
                return (parent->left == this ? 1 : -1);
            }
            return 0;
        }

        void splay() {
            while (state() != 0) {
                if (parent->state() != 0) {
                    if (state() == parent->state()) {
                        parent->rotate();
                        rotate();
                    } else {
                        rotate();
                        rotate();
                    }
                } else {
                    rotate();
                }
            }
        }

        void update() {
            size = 1;
            if (left) size += left->size;
            if (right) size += right->size;
        }

        Key key;
        T value;
        int size;
        NodePtr left = nullptr;
        NodePtr right = nullptr;
        NodePtr parent = nullptr;
    };

  public:
    SplayTree() = default;
    SplayTree(NodePtr root): root(root) {}
    ~SplayTree() {
        destract(root);
    }

    int size() {
        if (root) {
            return root->size;
        } else {
            return 0;
        }
    }

    bool empty() {
        return size() == 0;
    }

    void output() {
        output(root);
        std::cerr << std::endl;
    }

    void debug() {
        debug(root);
    }

    T& get(Key key) {
        assert(contains(key));
        root = find(key);
        return root->value;
    }

    //  0-indexed
    std::pair<Key, T> kth_element(int k) {
        root = kth_element(k, root);
        return {root->key, root->value};
    }

    void insert(Key key, T value) {
        assert(!contains(key));
        if (!root) {
            root = new Node(key, value);
            return;
        }
        auto [lhs, rhs] = split(key);
        assert(lhs || rhs);
        root = merge(merge(lhs, new Node(key, value)), rhs);
    }

    bool contains(Key key) {
        root = find(key);
        return root && root->key == key;
    }

    void erase(Key key) {
        assert(contains(key));
        root = find(key);
        if (root->left) root->left->parent = nullptr;
        if (root->right) root->right->parent = nullptr;
        NodePtr lhs = root->left;
        NodePtr rhs = root->right;
        root->left = nullptr;
        root->right = nullptr;
        delete root;
        root = merge(lhs, rhs);
    }

    void merge(NodePtr other) {
        this->root = merge(this->root, other);
    }

    void clear() {
        this->root = nullptr;
    }

    NodePtr& root_() {
        return root;
    }

    //  void query();

  private:
    static void output(NodePtr nodeptr) {
        if (nodeptr) {
            if (nodeptr->left) output(nodeptr->left);
            std::cerr << nodeptr->key << ' ';
            if (nodeptr->right) output(nodeptr->right);
        }
    }

    static void debug(NodePtr nodeptr) {
        if (!nodeptr) return;
        int size = 0;
        if (nodeptr->left) {
            assert(nodeptr->left->key < nodeptr->key);
            assert(nodeptr->left->parent == nodeptr);
            size += nodeptr->left->size;
            debug(nodeptr->left);
        }
        if (nodeptr->right) {
            assert(nodeptr->right->key > nodeptr->key);
            assert(nodeptr->right->parent == nodeptr);
            size += nodeptr->right->size;
            debug(nodeptr->right);
        }
        assert(nodeptr->size == size + 1);
    }

    static NodePtr kth_element(int k, NodePtr node) {
        assert(node);

        //  target = node
        if ((!node->left && k == 0) || (node->left && node->left->size == k)) {
            node->splay();
            return node;
        }
        //  target -> right
        if ((!node->left) || (node->left && node->left->size < k)) {
            if (node->right) {
                return kth_element(k - (node->left ? node->left->size : 0) - 1, node->right);
            } else {
                return nullptr;
            }
        }
        //  target -> left
        if ((!node->right) || (node->left && node->left->size > k)) {
            if (node->left) {
                return kth_element(k, node->left);
            } else {
                return nullptr;
            }
        }

        return nullptr;
    }

    //  [0, key), [key, INF)
    std::pair<NodePtr, NodePtr> split(Key key) {
        if (!root) return {nullptr, nullptr};
        root = find(key);
        assert(root);
        NodePtr lhs, rhs;
        if (root->key >= key) {
            lhs = root->left;
            rhs = root;
            if (lhs) lhs->parent = nullptr;
            if (rhs) rhs->left = nullptr;
        } else {
            lhs = root;
            rhs = root->right;
            if (lhs) lhs->right = nullptr;
            if (rhs) rhs->parent = nullptr;
        }
        root = nullptr;
        if (lhs) lhs->update();
        if (rhs) rhs->update();
        return {lhs, rhs};
    }

    static NodePtr merge(NodePtr lhs, NodePtr rhs) {
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        NodePtr root = lhs;
        while (root->right) root = root->right;
        root->splay();
        assert(root);
        assert(!root->right);
        root->right = rhs;
        rhs->parent = root;
        root->update();
        return root;
    }

    NodePtr find(Key key) {
        if (!root) return nullptr;
        NodePtr target = root;
        while (target && target->key != key) {
            if (target->key > key) {
                if (target->left) {
                    target = target->left;
                } else {
                    break;
                }
            } else {
                if (target->right) {
                    target = target->right;
                } else {
                    break;
                }
            }
        }
        target->splay();
        return target;
    }

    void destract(NodePtr node) {
        if (!node) return;
        destract(node->left);
        node->left = nullptr;
        destract(node->right);
        node->right = nullptr;
        node->parent = nullptr;
        //  std::cerr << "destract key: " << node->key << std::endl;
        delete node;
    }

    NodePtr root = nullptr;
};

}  //  namespace nono
