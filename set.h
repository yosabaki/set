//
// Created by artem on 17.06.18.
//
#ifndef SET_SET_H
#define SET_SET_H

#include <iterator>
#include <cassert>

template<typename T>
class set {
private:
    class node {
    public:
        node *left;
        node *right;
        node *parent;

        node() { left = right = parent = nullptr; };

        virtual ~node() = default;

    };

    struct node_t : node {
        T value;

        explicit node_t(T const &x) : value(x) {};

        friend void swap(node_t *a, node_t *b) {
            if (b->parent != nullptr) {
                if (b->parent->left == b)
                    b->parent->left = a;
                else b->parent->right = a;
            }
            if (a->parent != nullptr) {
                if (a->parent->left == a)
                    a->parent->left = b;
                else a->parent->right = b;
            }
            if (a->right != nullptr)
                a->right->parent = b;
            if (b->right != nullptr)
                b->right->parent = a;
            if (a->left != nullptr)
                a->left->parent = b;
            if (b->left != nullptr)
                b->left->parent = a;
            std::swap(b->parent, a->parent);
            std::swap(b->right, a->right);
            std::swap(b->left, a->left);
        }
    };

    size_t _size;
    node root;
    template<typename U>
    struct set_iterator;
public:
    using iterator = set_iterator<T const>;
    using const_iterator = iterator;
    using reverse_iterator =std::reverse_iterator<set_iterator<T const>>;
    using const_reverse_iterator = reverse_iterator;
private:
    template<typename U>
    class set_iterator : public std::iterator<std::bidirectional_iterator_tag, U> {
    public:
        friend class set;

        set_iterator() = default;

        set_iterator(set_iterator const &other) = default;

        set_iterator &operator++() {
            if (p->right != nullptr) {
                p = p->right;
                while (p->left != nullptr) {
                    p = p->left;
                }
                return *this;
            } else {
                while (p->parent->right == p) {
                    p = p->parent;
                }
                p = p->parent;
                return *this;
            }
        }

        set_iterator operator++(int) {
            set_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        set_iterator &operator--() {
            if (p->left != nullptr) {
                p = p->left;
                while (p->right != nullptr) {
                    p = p->right;
                }
                return *this;
            } else {
                while (p->parent->left == p) {
                    p = p->parent;
                }
                p = p->parent;
                return *this;
            }
        }

        set_iterator operator--(int) {
            set_iterator tmp = *this;
            --*this;
            return tmp;
        }

        friend bool operator==(set_iterator const &a, set_iterator const &b) {
            return a.p == b.p;
        }

        friend bool operator!=(set_iterator const &a, set_iterator const &b) {
            return a.p != b.p;
        }

        U &operator*() {
            return static_cast<const node_t *>(p)->value;
        }

    private:
        set_iterator(const node *ptr) : p(ptr) {}

        const node *p;
    };

    std::pair<iterator, bool> insert_impl(T const &x, node *curr) {
        T &value = static_cast<node_t *> (curr)->value;
        if (!(value < x) && !(x < value)) {
            return {curr, false};
        }
        if (value < x) {
            if (curr->right == nullptr) {
                curr->right = new node_t(x);
                curr->right->parent = curr;
                return {curr->right, true};
            }
            return insert_impl(x, curr->right);
        }
        if (x < value) {
            if (curr->left == nullptr) {
                curr->left = new node_t(x);
                curr->left->parent = curr;
                return {curr->right, true};
            }
            return insert_impl(x, curr->left);
        }
    };

    const_iterator find_impl(T const &x, node *curr) const {
        if (curr == nullptr) {
            return end();
        }
        T const &value = static_cast<node_t *>(curr)->value;
        if (value < x) {
            return find_impl(x, curr->right);
        }
        if (x < value) {
            return find_impl(x, curr->left);
        }
        return const_iterator(curr);
    };

    const_iterator lower_bound_impl(T const &x, node *curr) const {
        T const &value = static_cast<node_t *>(curr)->value;
        if (value < x) {
            if (curr->right == nullptr) {
                auto tmp = const_iterator(curr);
                return ++tmp;
            }
            return lower_bound_impl(x, curr->right);
        } else {
            if (curr->left == nullptr) {
                return curr;
            }
            return lower_bound_impl(x, curr->left);
        }
    };

    const_iterator upper_bound_impl(T const &x, node *curr) const {
        T const &value = static_cast<node_t *>(curr)->value;
        if (x < value) {
            if (curr->left == nullptr) {
                return curr;
            }
            return upper_bound_impl(x, curr->left);
        } else {
            if (curr->right == nullptr) {
                auto tmp = const_iterator(curr);
                return ++tmp;
            }
            return upper_bound_impl(x, curr->right);
        }
    };

    iterator erase_impl(node *curr) {
        iterator tmp_it = curr;
        tmp_it++;
        if (curr->right == nullptr) {
            if (curr->parent != nullptr) {
                if (curr->parent->right == curr) {
                    curr->parent->right = curr->left;
                } else {
                    curr->parent->left = curr->left;
                }
            }
            if (curr->left != nullptr) {
                curr->left->parent = curr->parent;
            }
            delete curr;
        } else if (curr->left == nullptr) {
            if (curr->parent != nullptr) {
                if (curr->parent->right == curr) {
                    curr->parent->right = curr->right;
                } else {
                    curr->parent->left = curr->right;
                }
            }
            curr->right->parent = curr->parent;
            delete curr;
        } else {
            node *tmp = curr->left;
            while (tmp->right != nullptr) {
                tmp = tmp->right;
            }
            swap(static_cast<node_t *>(tmp), static_cast<node_t *>(curr));
            erase_impl(curr);
        }
        return tmp_it;
    }


public:

    set() : _size(0), root() {}

    set(set const &other) : _size(0) {
        for (iterator it = other.begin(); it != other.end(); it++) {
            insert(*it);
        }
    };

    ~set() {
        clear();
    }

    set &operator=(set const &other) {
        set tmp(other);
        swap(tmp, *this);
        return *this;
    }

    iterator begin() {
        node *curr = &root;
        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return curr;
    }

    const_iterator cbegin() const {
        const node *curr = &root;
        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return const_cast<node *const>(curr);
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        return iterator(&root);
    }

    const_iterator end() const {
        return const_iterator(&root);
    }

    const_iterator cend() const {
        return const_iterator(&root);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }

    std::pair<iterator, bool> insert(T const &x) {
        if (empty()) {
            root.left = new node_t(x);
            root.left->parent = &root;
            _size++;
            return {root.left, true};
        }
        auto tmp = insert_impl(x, root.left);
        _size += tmp.second;
        return tmp;
    }

    const_iterator find(T const &x) const {
        return find_impl(x, root.left);
    }

    iterator erase(const_iterator it) {
        assert(it != end());
        _size--;
        return erase_impl(const_cast<node *>(it.p));
    }

    const_iterator lower_bound(T const &x) const {
        if (empty()) return end();
        return lower_bound_impl(x, root.left);
    }

    const_iterator upper_bound(T const &x) const {
        if (empty()) return end();
        return upper_bound_impl(x, root.left);
    }

    bool empty() const {
        return root.left == nullptr;
    }

    size_t size() const {
        return _size;
    }

    void clear() {
        iterator it = begin();
        while (it != end()) {
            it = erase(it);
        }
    }

    friend void swap(set<T> &a, set<T> &b) {
        std::swap(a.root, b.root);
        if (b.root.left != nullptr) {
            b.root.left->parent = &b.root;
        }
        if (a.root.left != nullptr) {
            a.root.left->parent = &a.root;
        }
        std::swap(a._size, b._size);
    }
};


#endif //SET_SET_H
