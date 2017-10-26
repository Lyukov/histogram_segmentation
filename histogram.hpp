#include <cstdint>
#include <iostream>
#include <vector>

#define BLOCKSIZE 65536

template <size_t N, typename T>
class Key {
    T body[N];

   public:
    T &operator[](size_t index) { return body[index]; }
    T operator[](size_t index) const { return body[index]; }
    bool operator==(const Key &other) const {
        for(size_t i = 0; i < N; ++i) {
            if(body[i] != other.body[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Key &other) const {
        for(size_t i = 0; i < N; ++i) {
            if(body[i] != other.body[i]) {
                return true;
            }
        }
        return false;
    }
    bool operator<(const Key &other) const {
        for(size_t i = 0; i < N; ++i) {
            if(body[i] != other.body[i]) {
                return body[i] < other.body[i];
            }
        }
        return false;
    }
    bool operator>(const Key &other) const {
        for(size_t i = 0; i < N; ++i) {
            if(body[i] != other.body[i]) {
                return body[i] > other.body[i];
            }
        }
        return false;
    }
};

template <size_t N, typename T>
struct Node {
    Key<N, T> key;
    double count;
    // Tree
    Node *parent;
    Node *left;
    Node *right;
    // List
    Node *next;
    Node() {
        count = 0.;
        parent = NULL;
        left = NULL;
        right = NULL;
        next = NULL;
    }
};

template <size_t N, typename T>
class Histogram {
    Node<N, T> *free;
    std::vector<Node<N, T> *> body;
    Node<N, T> *head;

    Histogram();
    Node<N, T> *allocate();
    Node<N, T> *get_new();
    Node<N, T> &as_tree_at(const Key<N, T> key);

   public:
    void add(double w, const Key<N, T> key);
    void remove(const Key<N, T> key);
    void sort();
    void rebuild_tree();
    Node<N, T> &operator[](size_t index);
};

template <size_t N, typename T>
Histogram<N, T>::Histogram() {
    free = allocate();
    head = NULL;
}

template <size_t N, typename T>
Node<N, T> *Histogram<N, T>::allocate() {
    std::vector<Node<N, T> > __p(BLOCKSIZE);
    body.push_back(&(__p[0]));
    for(size_t i = 0; i < BLOCKSIZE - 1; ++i) {
        __p[i].next = &(__p[i + 1]);
    }
    __p[BLOCKSIZE - 1].next = NULL;
    return &(__p[0]);
}

template <size_t N, typename T>
Node<N, T> *Histogram<N, T>::get_new() {
    if(free.next == NULL) {
        free.next = allocate();
    }
    Node<N, T> *result = free;
    free = free.next;
    result->next = NULL;
    return result;
}

template <size_t N, typename T>
Node<N, T> &Histogram<N, T>::as_tree_at(const Key<N, T> key) {
    if(head == NULL) {
        head = get_new();
        head->key = key;
        return *head;
    }
    Node<N, T> *p = head;
    do {
        if(key < p->key) {
            if(p->left != NULL) {
                p = p->left;
            } else {
                Node<N, T> *result = get_new();
                p->left = result;
                result->parent = p;
                result->key = key;
                return result;
            }
        } else if(key > p->key) {
            if(p->right != NULL) {
                p = p->right;
            } else {
                Node<N, T> *result = get_new();
                p->right = result;
                result->parent = p;
                result->key = key;
                return result;
            }
        } else {
            return p;
        }
    } while(true);
}

template <size_t N, typename T>
void Histogram<N, T>::add(double w, const Key<N, T> key) {
    as_tree_at(key).count += w;
}