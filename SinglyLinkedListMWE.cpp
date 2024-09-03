#include "SinglyLinkedList.hpp"
#include <iostream>
#include <cassert>
#include <queue>

int main() {
    std::cout << "MWE test starts!\n";
    
    // Test constructor and push operations
    SinglyLinkedList<int> list;
    assert(list.empty());
    list.push_back(1);
    list.push_back(2);
    list.push_front(0);
    assert(list.size() == 3);
    std::cout << "0\n";

    // Test access operations
    assert(list.front() == 0);
    assert(list.back() == 2);
    assert(list.get(1) == 1);
    std::cout << "1\n";

    // Test pop operations
    list.pop_front();
    list.pop_back();
    assert(list.size() == 1);
    assert(list.front() == 1);
    std::cout << "2\n";

    // Test clear operation
    list.clear();
    assert(list.empty());
    std::cout << "3\n";

    // Test copy constructor
    SinglyLinkedList<int> list2 = {1, 2, 3, 4, 5};
    SinglyLinkedList<int> list3(list2);
    assert(list2 == list3);
    std::cout << "4\n";

    // Test assignment operator
    list = list2;
    assert(list == list2);
    std::cout << "5\n";

    // Test conversion to/from std::vector
    std::vector<int> vec = list.to_vector();
    SinglyLinkedList<int> list4;
    list4 = vec;
    assert(list == list4);
    std::cout << "6\n";

    // Test conversion to/from std::array
    auto arr = list.to_array_auto<5>();
    SinglyLinkedList<int> list5;
    list5 = arr;
    assert(list == list5);
    std::cout << "7\n";

    // Test conversion to/from std::list
    std::list<int> stdList = list.to_list();
    SinglyLinkedList<int> list6;
    list6 = stdList;
    assert(list == list6);
    std::cout << "8\n";

    // Test iterator
    int sum = 0;
    for (const auto& item : list) {
        sum += item;
    }
    assert(sum == 15);
    std::cout << "9\n";

    // Test compatibility with std::queue
    std::queue<int, SinglyLinkedList<int>> myQueue;
    myQueue.push(10);
    myQueue.push(20);
    myQueue.push(30);
    assert(myQueue.front() == 10);
    assert(myQueue.back() == 30);
    myQueue.pop();
    assert(myQueue.size() == 2);
    std::cout << "10\n";

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}