// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/ds/persistent/stack.hpp"

namespace nono {

void solve() {
    //  class Stack<T>
    {
        using persistent::Stack;
        Stack<int> s0;
        assert(s0.empty());
        assert(s0.size() == 0);

        Stack<int> s1 = s0.push(1);
        Stack<int> s2 = s1.push(2);
        Stack<int> s3 = s1.push(3);

        //  s1から分岐したs2, s3は互いに独立
        assert(s1.size() == 1 && s1.top() == 1);
        assert(s2.size() == 2 && s2.top() == 2);
        assert(s3.size() == 2 && s3.top() == 3);

        Stack<int> s2popped = s2.pop();
        assert(s2popped.size() == 1 && s2popped.top() == 1);
        //  popしてもs2自体は不変
        assert(s2.size() == 2 && s2.top() == 2);

        //  元のs0, s1は分岐後も不変
        assert(s0.empty());
        assert(s1.size() == 1 && s1.top() == 1);
    }

    //  persistent::stack:: の生ノードAPI
    {
        using namespace persistent::stack;
        NodePtr<int> n0 = nullptr;
        assert(empty(n0));
        assert(size(n0) == 0);

        NodePtr<int> n1 = push(n0, 10);
        NodePtr<int> n2 = push(n1, 20);
        assert(!empty(n2));
        assert(size(n2) == 2);
        assert(top(n2) == 20);
        assert(top(pop(n2)) == 10);
        //  popしてもn2自体は不変
        assert(size(n2) == 2);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
