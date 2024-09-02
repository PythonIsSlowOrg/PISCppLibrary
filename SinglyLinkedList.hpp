#ifndef SINGLYLINKEDLIST_HPP
#define SINGLYLINKEDLIST_HPP

#include <iostream>
#include <stdexcept>
#include <memory>
#include <utility>
#include <iterator>
#include <algorithm>
#include <vector>
#include <array>
#include <list>
#include <cassert>

/**
 * @brief A singly linked list implementation.
 * 
 * This class provides a basic implementation of a singly linked list with support for standard
 * operations such as push_back, push_front, pop_back, and pop_front. It also includes methods 
 * to convert the list to various standard containers.
 * 
 * @tparam T Type of elements stored in the list.
 */
template<typename T>
class SinglyLinkedList {
private:
    /**
     * @brief Node structure for the singly linked list.
     * 
     * Each node contains data and a pointer to the next node in the list.
     */
    struct Node {
        T data; //!< Data stored in the node.
        std::shared_ptr<Node> next; //!< Pointer to the next node.

        /**
         * @brief Constructs a Node with given value.
         * @param value The value to initialize the node with.
         */
        Node(T value) : data(std::move(value)), next(nullptr) {}

        /**
         * @brief Copy constructor for Node.
         * @param other The Node to copy.
         */
        Node(const Node& other) : data(other.data), next(nullptr) {}

        /**
         * @brief Move constructor for Node.
         * @param other The Node to move from.
         */
        Node(Node&& other) noexcept : data(std::move(other.data)), next(std::move(other.next)) {}

        /**
         * @brief Copy assignment operator for Node.
         * @param other The Node to copy.
         * @return Reference to this Node.
         */
        Node& operator=(const Node& other) {
            if (this != &other) {
                data = other.data;
                next.reset(other.next ? new Node(*other.next) : nullptr);
            }
            return *this;
        }

        /**
         * @brief Move assignment operator for Node.
         * @param other The Node to move from.
         * @return Reference to this Node.
         */
        Node& operator=(Node&& other) noexcept {
            if (this != &other) {
                data = std::move(other.data);
                next = std::move(other.next);
            }
            return *this;
        }
    };

    std::shared_ptr<Node> head; //!< Pointer to the first node in the list.
    Node* tail; //!< Pointer to the last node in the list.
    std::size_t list_size; //!< Number of elements in the list.

public:
    /**
     * @brief Default constructor for SinglyLinkedList.
     */
    SinglyLinkedList() : head(nullptr), tail(nullptr), list_size(0) {}

    /**
     * @brief Constructs a SinglyLinkedList from a range of iterators.
     * @param first The start iterator of the range.
     * @param last The end iterator of the range.
     */
    template<typename InputIt>
    SinglyLinkedList(InputIt first, InputIt last) : head(nullptr), tail(nullptr), list_size(0) {
        std::for_each(first, last, [this](const T& value) { push_back(value); });
    }

    /**
     * @brief Constructs a SinglyLinkedList from an initializer list.
     * @param initList The initializer list.
     */
    SinglyLinkedList(std::initializer_list<T> initList) : SinglyLinkedList(initList.begin(), initList.end()) {}

    /**
     * @brief Destructor for SinglyLinkedList.
     */
    ~SinglyLinkedList() = default;

    /**
     * @brief Copy constructor for SinglyLinkedList.
     * @param other The SinglyLinkedList to copy.
     */
    SinglyLinkedList(const SinglyLinkedList& other) : head(nullptr), tail(nullptr), list_size(0) {
        if (this != &other) {
            clear();
            Node* current = other.head.get();
            while (current != nullptr) {
                push_back(current->data);
                current = current->next.get();
            }
        }
    }

/*
     **
     * @brief Copy assignment operator for SinglyLinkedList.
     * @param other The SinglyLinkedList to copy.
     * @return Reference to this SinglyLinkedList.
     *
    SinglyLinkedList& operator=(const SinglyLinkedList other) {
        if (this != &other) {
            clear();
            Node* current = other.head.get();
            while (current != nullptr) {
                push_back(current->data);
                current = current->next.get();
            }
        }
        return *this;
    }
*/

    // Copy assignment operator (deleted to avoid ambiguity)
    SinglyLinkedList& operator=(const SinglyLinkedList& other) = delete;

    /**
     * @brief Move constructor for SinglyLinkedList.
     * @param other The SinglyLinkedList to move from.
     */
    SinglyLinkedList(SinglyLinkedList&& other) noexcept : head(std::move(other.head)), tail(other.tail), list_size(other.list_size) {
        other.tail = nullptr;
        other.list_size = 0;
    }

    /**
     * @brief Move assignment operator for SinglyLinkedList.
     * @param other The SinglyLinkedList to move from.
     * @return Reference to this SinglyLinkedList.
     */
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            head = std::move(other.head);
            tail = other.tail;
            list_size = other.list_size;
            other.tail = nullptr;
            other.list_size = 0;
        }
        return *this;
    }

    /**
     * @brief Adds a new element to the end of the list.
     * @param val The value to add.
     */
    void push_back(T val) {
        auto newNode = std::make_unique<Node>(std::move(val));
        Node* newNodePtr = newNode.get();
        if (!head) {
            head = std::move(newNode);
            tail = newNodePtr;
        } else {
            tail->next = std::move(newNode);
            tail = newNodePtr;
        }
        ++list_size;
    }

    /**
     * @brief Adds a new element to the front of the list.
     * @param val The value to add.
     */
    void push_front(T val) {
        auto newNode = std::make_unique<Node>(std::move(val));
        if (!head) {
            head = std::move(newNode);
            tail = head.get();
        } else {
            newNode->next = std::move(head);
            head = std::move(newNode);
        }
        ++list_size;
    }

    /**
     * @brief Removes the last element of the list.
     * @throws std::runtime_error if the list is empty.
     */
    void pop_back() {
        if (!head) {
            throw std::runtime_error("List is empty: cannot pop back.");
        }

        if (head.get() == tail) {
            head.reset();
            tail = nullptr;
        } else {
            Node* current = head.get();
            while (current->next.get() != tail) {
                current = current->next.get();
            }
            current->next.reset();
            tail = current;
        }
        --list_size;
    }

    /**
     * @brief Removes the first element of the list.
     * @throws std::runtime_error if the list is empty.
     */
    void pop_front() {
        if (!head) {
            throw std::runtime_error("List is empty: cannot pop front.");
        }
        head = std::move(head->next);
        if (!head) {
            tail = nullptr;
        }
        --list_size;
    }

    /**
     * @brief Inserts a new element before the specified node.
     * @param pos The node before which to insert.
     * @param val The value to insert.
     * @throws std::runtime_error if the position is not found.
     */
    void insert_before(Node* pos, T val) {
        if (pos == head.get()) {
            push_front(std::move(val));
            return;
        }
        Node* prev = nullptr;
        Node* current = head.get();
        while (current && current->next.get() != pos) {
            prev = current;
            current = current->next.get();
        }
        if (!current) {
            throw std::runtime_error("Position not found.");
        }
        auto newNode = std::make_unique<Node>(std::move(val));
        newNode->next = std::move(current->next);
        current->next = std::move(newNode);
        if (current->next.get() == tail) {
            tail = current->next.get();
        }
        ++list_size;
    }

    /**
     * @brief Erases the node before the specified node.
     * @param pos The node after which to erase.
     * @throws std::runtime_error if the position is not found or is the first element.
     */
    void erase_before(Node* pos) {
        if (pos == head.get() || !head) {
            throw std::runtime_error("Cannot erase before the first element.");
        }
        Node* prev = nullptr;
        Node* current = head.get();
        while (current->next.get() != pos) {
            prev = current;
            current = current->next.get();
            if (!current->next) {
                throw std::runtime_error("Position not found.");
            }
        }
        if (prev) {
            prev->next = std::move(current->next);
            if (!prev->next) {
                tail = prev;
            }
            --list_size;
        }
    }

    /**
     * @brief Clears the list.
     */
    void clear() {
        head.reset();
        tail = nullptr;
        list_size = 0;
    }

    /**
     * @brief Retrieves the data at the head of the list.
     * @return The data at the head.
     * @throws std::runtime_error if the list is empty.
     */
    T getHead() const {
        if (!head) {
            throw std::runtime_error("List is empty: cannot access head.");
        }
        return head->data;
    }

    /**
     * @brief Retrieves the data at the tail of the list.
     * @return The data at the tail.
     * @throws std::runtime_error if the list is empty.
     */
    T getTail() const {
        if (!tail) {
            throw std::runtime_error("List is empty: cannot access tail.");
        }
        return tail->data;
    }

    /**
     * @brief Gets the number of elements in the list.
     * @return The number of elements.
     */
    std::size_t size() const { return list_size; }

    /**
     * @brief Swaps the contents of two SinglyLinkedLists.
     * @param first The first list.
     * @param second The second list.
     */
    friend void swap(SinglyLinkedList& first, SinglyLinkedList& second) noexcept {
        using std::swap;
        swap(first.head, second.head);
        swap(first.tail, second.tail);
        swap(first.list_size, second.list_size);
    }

    /**
     * @brief Assigns elements from an initializer list to the list.
     * @param initList The initializer list.
     * @return Reference to this SinglyLinkedList.
     */
    SinglyLinkedList& operator=(std::initializer_list<T> initList) {
        clear();
        for (const auto& item : initList) {
            push_back(item);
        }
        return *this;
    }

    /**
     * @brief Assigns elements from a std::vector to the list.
     * @param vec The std::vector.
     * @return Reference to this SinglyLinkedList.
     */
    SinglyLinkedList& operator=(const std::vector<T>& vec) {
        clear();
        for (const auto& item : vec) {
            push_back(item);
        }
        return *this;
    }

    /**
     * @brief Assigns elements from a std::array to the list.
     * @param arr The std::array.
     * @tparam N The size of the array.
     * @return Reference to this SinglyLinkedList.
     */
    template<std::size_t N>
    SinglyLinkedList& operator=(const std::array<T, N>& arr) {
        clear();
        for (const auto& item : arr) {
            push_back(item);
        }
        return *this;
    }

    /**
     * @brief Assigns elements from a std::list to the list.
     * @param stdList The std::list.
     * @return Reference to this SinglyLinkedList.
     */
    SinglyLinkedList& operator=(const std::list<T>& stdList) {
        clear();
        for (const auto& item : stdList) {
            push_back(item);
        }
        return *this;
    }

    /**
     * @brief Converts the list to a std::vector.
     * @return A std::vector containing the list elements.
     */
    std::vector<T> to_vector() const {
        std::vector<T> vec;
        vec.reserve(list_size);
        for (const auto& item : *this) {
            vec.push_back(item);
        }
        return vec;
    }

    /**
     * @brief Converts the list to a std::array, padding with default values if necessary.
     * @tparam N The size of the array.
     * @return A std::array containing the list elements, padded with default values if needed.
     * @throws std::runtime_error if the array size is less than 1 or if the list size exceeds the array size.
     */
    template<std::size_t N>
    std::array<T, N> to_array_pad() const {
        if (N < 1) throw std::runtime_error("Array size must be a positive integer.");
        std::array<T, N> arr;
        std::size_t i = 0;
        for (const auto& item : *this) {
            if (i >= N) throw std::runtime_error("List size exceeds array size.");
            arr[i++] = item;
        }
        if (i < N) arr.fill(T());
        return arr;
    }

    /**
     * @brief Converts the list to a std::array, cutting off excess elements if necessary.
     * @tparam N The size of the array.
     * @return A std::array containing the list elements, with excess elements cut off.
     * @throws std::runtime_error if the array size is less than 1 or if the list size is less than the array size.
     */
    template<std::size_t N>
    std::array<T, N> to_array_cut() const {
        if (N < 1) throw std::runtime_error("Array size must be a positive integer.");
        std::array<T, N> arr;
        std::size_t i = 0;
        for (const auto& item : *this) {
            if (i >= N) return arr;
            arr[i++] = item;
        }
        if (i < N) throw std::runtime_error("Array size exceeds list size.");
        return arr;
    }

    /**
     * @brief Converts the list to a std::array, automatically padding with default values or cutting off excess elements.
     * @tparam N The size of the array.
     * @return A std::array containing the list elements, padded with default values or cut off as needed.
     * @throws std::runtime_error if the array size is less than 1.
     */
    template<std::size_t N>
    std::array<T, N> to_array_auto() const {
        if (N < 1) throw std::runtime_error("Array size must be a positive integer.");
        std::array<T, N> arr;
        std::size_t i = 0;
        for (const auto& item : *this) {
            if (i >= N) return arr;
            arr[i++] = item;
        }
        if (i < N) arr.fill(T());
        return arr;
    }

    /**
     * @brief Converts the list to a std::list.
     * @return A std::list containing the list elements.
     */
    std::list<T> to_list() const {
        std::list<T> stdList;
        for (const auto& item : *this) {
            stdList.push_back(item);
        }
        return stdList;
    }

    /**
     * @brief Iterator for the SinglyLinkedList.
     * 
     * Provides forward iteration over the list elements.
     */
    class Iterator {
    public:
        Node* current; //!< Current node in the iteration.

        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        /**
         * @brief Constructs an Iterator starting at the given node.
         * @param start The starting node.
         */
        explicit Iterator(Node* start) : current(start) {}

        /**
         * @brief Dereferences the iterator to access the current element.
         * @return Reference to the current element.
         */
        T& operator*() const { return current->data; }

        /**
         * @brief Accesses the current element through the iterator.
         * @return Pointer to the current element.
         */
        T* operator->() const { return &current->data; }

        /**
         * @brief Advances the iterator to the next element.
         * @return Reference to this iterator.
         */
        Iterator& operator++() {
            current = current->next.get();
            return *this;
        }

        /**
         * @brief Advances the iterator to the next element (postfix).
         * @return The previous state of the iterator.
         */
        Iterator operator++(int) {
            Iterator temp = *this;
            current = current->next.get();
            return temp;
        }

        /**
         * @brief Checks if two iterators are equal.
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         */
        bool operator==(const Iterator& other) const { return current == other.current; }

        /**
         * @brief Checks if two iterators are not equal.
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         */
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    /**
     * @brief ConstIterator for the SinglyLinkedList.
     * 
     * Provides forward iteration over the list elements, with const access.
     */
    class ConstIterator : public Iterator {
    public:
        using Iterator::Iterator;

        /**
         * @brief Dereferences the iterator to access the current element (const version).
         * @return Const reference to the current element.
         */
        const T& operator*() const { return Iterator::operator*(); }

        /**
         * @brief Accesses the current element through the iterator (const version).
         * @return Const pointer to the current element.
         */
        const T* operator->() const { return Iterator::operator->(); }
    };

    /**
     * @brief Gets an iterator to the beginning of the list.
     * @return An Iterator pointing to the first element.
     */
    Iterator begin() { return Iterator(head.get()); }

    /**
     * @brief Gets an iterator to the end of the list.
     * @return An Iterator pointing to one past the last element.
     */
    Iterator end() { return Iterator(nullptr); }

    /**
     * @brief Gets a const iterator to the beginning of the list.
     * @return A ConstIterator pointing to the first element.
     */
    ConstIterator begin() const { return ConstIterator(head.get()); }

    /**
     * @brief Gets a const iterator to the end of the list.
     * @return A ConstIterator pointing to one past the last element.
     */
    ConstIterator end() const { return ConstIterator(nullptr); }

};

#endif // SINGLYLINKEDLIST_HPP