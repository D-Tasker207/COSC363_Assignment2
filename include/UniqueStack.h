#ifndef UNIQUESTACK_H
#define UNIQUESTACK_H

#include <vector>
#include <set>

template <typename T>
class UniqueStack {
    public:
        void push(T item) {
            if (set.find(item) == set.end()) {
                stack.push_back(item);
                set.insert(item);
            }
        }

        T pop() {
            T item = stack.back();
            stack.pop_back();
            set.erase(item);
            return item;
        }

        T top() {
            return stack.back();
        }

        bool empty() {
            return stack.empty();
        }

    private:
        std::vector<T> stack;
        std::set<T> set;
};

#endif