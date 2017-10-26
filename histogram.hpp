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
    bool operator==(const Key& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (body[i] != other.body[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Key& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (body[i] != other.body[i]) {
                return true;
            }
        }
        return false;
    }
    bool operator<(const Key& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (body[i] != other.body[i]) {
                return body[i] < other.body[i];
            }
        }
        return false;
    }
    bool operator>(const Key& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (body[i] != other.body[i]) {
                return body[i] > other.body[i];
            }
        }
        return false;
    }
};

template <size_t N, typename T>
struct Node {
    T key[N];
    double count;
    // Tree
    Node *parent;
    Node *left;
    Node *right;
    // List
    Node *next;
    Node(Node<N, T> *parent = NULL) {
        count = 0.;
        parent = parent;
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
    Node<N, T> &as_tree_at(const T *key);

   public:
    void add(double w, const T *key);
    void remove(const T *key);
    void sort();
    void rebuild_tree();
    Node<N, T> &operator[](size_t key);
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
Node<N, T> &as_tree_at(const T *key) {
    if(head == NULL) {
        //
    }
}
