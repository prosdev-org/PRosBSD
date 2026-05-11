#ifndef LIBKXX_LINKED_LIST_HXX
#define LIBKXX_LINKED_LIST_HXX

#include <libkxx/move.hxx>
#include <libkxx/unique_ptr.hxx>
#include <libkxx/vector.hxx>
#include <unique/assert.h>

namespace kxx {
    template<typename T>
    class LinkedList {
    public:
        class Node {
            friend class LinkedList;

        public:
            ~Node() = default;
            Node(Node &&other);
            Node &operator=(Node &&other);
            [[nodiscard]] const T &get() const;
            void set(const T &new_obj);
            void set(const T &&new_obj);
            Node *get_next();
            void insert_after(const T &new_obj);
            void insert_after(T &&new_obj);

        private:
            Node(const T &obj, UniquePtr<Node> &&next);
            Node(T &&obj, UniquePtr<Node> &&next);

            T obj;
            UniquePtr<Node> next;
        };

        LinkedList() = default;
        template<size_t src_size>
        explicit LinkedList(T (&src)[src_size]);
        template<size_t src_size>
        explicit LinkedList(T (&&src)[src_size]);
        explicit LinkedList(const T *src, size_t src_size);
        explicit LinkedList(const Vector<T> &src);
        explicit LinkedList(Vector<T> &&src);
        ~LinkedList() = default;
        LinkedList(const LinkedList &other);
        LinkedList(LinkedList &&other);
        LinkedList &operator=(const LinkedList &other);
        LinkedList &operator=(LinkedList &&other);
        [[nodiscard]] bool is_empty() const;
        [[nodiscard]] size_t get_size() const;
        [[nodiscard]] const T &get(size_t idx) const;
        const T &get_last() const;
        [[nodiscard]] Node &get_node(size_t idx);
        [[nodiscard]] Node &get_last_node();
        void set(const T &obj, size_t idx);
        void insert(const T &obj, size_t idx);
        void insert(T &&obj, size_t idx);
        void insert_back(const T &obj);
        void insert_back(T &&obj);
        void remove(size_t idx);

    private:
        void copy_init(const T *src, size_t count);
        void copy_init(const LinkedList &src);
        void move_init(T *src, size_t count);
        void move_init(LinkedList &&src);

        UniquePtr<Node> first = UniquePtr<Node>(nullptr);
    };

    template<typename T>
    LinkedList<T>::Node::Node(Node &&other) {
        obj = move(other.obj);
        next = move(other.next);
    }

    template<typename T>
    typename LinkedList<T>::Node &LinkedList<T>::Node::operator=(Node &&other) {
        if (&other == this) {
            return *this;
        }

        obj = move(other.obj);
        next = move(other.next);

        return *this;
    }

    template<typename T>
    const T &LinkedList<T>::Node::get() const {
        return obj;
    }

    template<typename T>
    void LinkedList<T>::Node::set(const T &new_obj) {
        obj = new_obj;
    }

    template<typename T>
    void LinkedList<T>::Node::set(const T &&new_obj) {
        obj = move(new_obj);
    }

    template<typename T>
    typename LinkedList<T>::Node *LinkedList<T>::Node::get_next() {
        return next.get_raw();
    }

    template<typename T>
    void LinkedList<T>::Node::insert_after(const T &new_obj) {
        UniquePtr<Node> new_node =
                UniquePtr<Node>(new Node(new_obj, move(next)));

        next = move(new_node);
    }

    template<typename T>
    void LinkedList<T>::Node::insert_after(T &&new_obj) {
        UniquePtr<Node> new_node =
                UniquePtr<Node>(new Node(move(new_obj), move(next)));

        next = move(new_node);
    }

    template<typename T>
    LinkedList<T>::Node::Node(const T &obj, UniquePtr<Node> &&next) {
        this->obj = obj;
        this->next = move(next);
    }

    template<typename T>
    LinkedList<T>::Node::Node(T &&obj, UniquePtr<Node> &&next) {
        this->obj = move(obj);
        this->next = move(next);
    }

    template<typename T>
    template<size_t src_size>
    LinkedList<T>::LinkedList(T (&src)[src_size]) {
        copy_init(src, src_size);
    }

    template<typename T>
    template<size_t src_size>
    LinkedList<T>::LinkedList(T (&&src)[src_size]) {
        move_init(src, src_size);
    }

    template<typename T>
    LinkedList<T>::LinkedList(const T *src, size_t src_size) {
        copy_init(src, src_size);
    }

    template<typename T>
    LinkedList<T>::LinkedList(const Vector<T> &src) {
        copy_init(src.back, src.size);
    }

    template<typename T>
    LinkedList<T>::LinkedList(Vector<T> &&src) {
        Vector<T> local_src = move(src);
        move_init(local_src.back, local_src.size);
    }

    template<typename T>
    LinkedList<T>::LinkedList(const LinkedList &other) {
        copy_init(other);
    }

    template<typename T>
    LinkedList<T>::LinkedList(LinkedList &&other) {
        copy_init(move(other));
    }

    template<typename T>
    LinkedList<T> &LinkedList<T>::operator=(const LinkedList &other) {
        if (&other == this) {
            return *this;
        }

        copy_init(other);

        return *this;
    }

    template<typename T>
    LinkedList<T> &LinkedList<T>::operator=(LinkedList &&other) {
        if (&other == this) {
            return *this;
        }

        move_init(move(other));

        return *this;
    }

    template<typename T>
    bool LinkedList<T>::is_empty() const {
        return first.is_nullptr();
    }

    template<typename T>
    size_t LinkedList<T>::get_size() const {
        size_t size = 0;
        const Node *node = first.get_const_raw();
        for (; node != nullptr; size++) {
            node = node->next.get_const_raw();
        }

        return size;
    }

    template<typename T>
    const T &LinkedList<T>::get(const size_t idx) const {
        const Node *node = first.get_const_raw();
        ASSERT(node != nullptr);

        for (size_t i = 0; i < idx; i++) {
            node = node->next.get_const_raw();
            ASSERT(node != nullptr);
        }

        return node->obj;
    }

    template<typename T>
    const T &LinkedList<T>::get_last() const {
        return get_last_node()->get();
    }

    template<typename T>
    typename LinkedList<T>::Node &LinkedList<T>::get_node(const size_t idx) {
        Node *node = first.get_raw();
        ASSERT(node != nullptr);

        for (size_t i = 0; i < idx; i++) {
            node = node->next.get_raw();
            ASSERT(node != nullptr);
        }

        return *node;
    }

    template<typename T>
    typename LinkedList<T>::Node &LinkedList<T>::get_last_node() {
        Node *node = first.get_raw();
        ASSERT(node != nullptr);

        while (!node->next.is_nullptr()) {
            node = node->next.get_raw();
        }

        return *node;
    }

    template<typename T>
    void LinkedList<T>::set(const T &obj, const size_t idx) {
        Node *node = first.get_raw();
        ASSERT(node != nullptr);

        for (size_t i = 0; i < idx; i++) {
            node = node->next;
            ASSERT(node != nullptr);
        }

        node->obj = obj;
    }

    template<typename T>
    void LinkedList<T>::insert(const T &obj, const size_t idx) {
        if (idx == 0) {
            UniquePtr<Node> new_first = kxx::UniquePtr<Node>(
                    new Node(obj, move(first)));
            first = move(new_first);
            return;
        }

        Node &prev_node = get_node(idx - 1);
        prev_node.insert_after(obj);
    }

    template<typename T>
    void LinkedList<T>::insert(T &&obj, const size_t idx) {
        if (idx == 0) {
            UniquePtr<Node> new_first = kxx::UniquePtr<Node>(
                    new Node(move(obj), move(first)));
            first = move(new_first);
            return;
        }

        Node &prev_node = get_node(idx - 1);
        prev_node.insert_after(move(obj));
    }

    template<typename T>
    void LinkedList<T>::insert_back(const T &obj) {
        if (is_empty()) {
            insert(obj, 0);
            return;
        }

        Node *node = &get_last_node();
        node->insert_after(obj);
    }

    template<typename T>
    void LinkedList<T>::insert_back(T &&obj) {
        if (is_empty()) {
            insert(move(obj), 0);
            return;
        }

        Node *node = &get_last_node();
        node->insert_after(move(obj));
    }

    template<typename T>
    void LinkedList<T>::remove(const size_t idx) {
        if (idx == 0) {
            UniquePtr<Node> temp = move(first->next);
            first = move(temp);
            return;
        }

        Node *prev_node = &get_node(idx - 1);
        ASSERT(!prev_node->next.is_nullptr());

        UniquePtr<Node> &node = prev_node->next;
        UniquePtr<Node> temp = move(node->next);
        node = move(temp);
    }

    template<typename T>
    void LinkedList<T>::copy_init(const T *src, const size_t count) {
        ASSERT(src != nullptr);

        if (count == 0) {
            return;
        }

        first = UniquePtr<Node>(
                new Node(src[0], UniquePtr<Node>(nullptr)));

        Node *node = first.get_raw();
        for (size_t i = 1; i < count; i++) {
            node->next = UniquePtr<Node>(
                    new Node(src[i], UniquePtr<Node>(nullptr)));
            node = node->next.get_raw();
        }
    }

    template<typename T>
    void LinkedList<T>::copy_init(const LinkedList &src) {
        const Node *src_node = src.first.get_const_raw();

        if (src_node == nullptr) {
            return;
        }

        first = UniquePtr<Node>(
                new Node(src_node->obj, UniquePtr<Node>(nullptr)));

        Node *node = first.get_raw();

        for (; src_node != nullptr; src_node = src_node->next) {
            node->next = UniquePtr<Node>(
                    new Node(src_node->obj, UniquePtr<Node>(nullptr)));
            node = node->next.get_raw();
        }
    }

    template<typename T>
    void LinkedList<T>::move_init(T *src, const size_t count) {
        ASSERT(src != nullptr);

        if (count == 0) {
            return;
        }

        first = UniquePtr<Node>(
                new Node(move(src[0]), UniquePtr<Node>(nullptr)));

        Node *node = first.get_raw();
        for (size_t i = 1; i < count; i++) {
            node->next = UniquePtr<Node>(
                    new Node(move(src[i]), UniquePtr<Node>(nullptr)));
            node = node->next.get_raw();
        }
    }

    template<typename T>
    void LinkedList<T>::move_init(LinkedList &&src) {
        Node *src_node = src.first;

        if (src_node == nullptr) {
            return;
        }

        first = UniquePtr<Node>(
                new Node(move(src_node->obj), UniquePtr<Node>(nullptr)));

        Node *node = first.get_raw();

        for (; src_node != nullptr; src_node = src_node->next) {
            node->next = UniquePtr<Node>(
                    new Node(move(src_node->obj), UniquePtr<Node>(nullptr)));
            node = node->next.get_raw();
        }
    }
} // namespace kxx

#endif
