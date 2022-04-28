#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <variant>
#include <any>
using namespace std;
#endif /* __PROGTEST__ */

template <typename ContainerT, typename CompareT = std::less<typename ContainerT::value_type>>
class CIndex
{
private:
    ContainerT m_container;
    CompareT m_compare;

    /**
     * @brief Compare if two values are equals using the < or std::less operator
     *
     * @param lhs
     * @param rhs
     * @return true
     * @return false
     */
    bool isMatching(const typename ContainerT::value_type &lhs, const typename ContainerT::value_type &rhs) const
    {
        // If value is neither smaller nor bigger, it must be equals
        if (!(m_compare(lhs, rhs)) && !(m_compare(rhs, lhs)))
            return true;

        return false;
    }

public:
    /**
     * @brief Construct a new CIndex object
     *
     * @param container
     */
    CIndex(ContainerT container)
        : m_container(container){};

    /**
     * @brief Construct a new CIndex object with compare
     *
     * @param container
     * @param compare
     */
    CIndex(ContainerT container, CompareT compare)
        : m_container(container), m_compare(compare){};

    /**
     * @brief Return value of indexes where pattern matches values in container
     *
     * @param pattern
     * @return set<size_t>
     */
    set<size_t> search(const ContainerT &pattern) const
    {
        set<size_t> foundIndexes;
        auto patternIt = pattern.begin();

        size_t index = 0;

        for (auto it = m_container.begin(); it != m_container.end(); it++, index++)
        {
            // if (!isMatching(*it, *patternIt))
            //     continue;

            // Save original iterator to restore it later
            auto itBefore = it;

            bool isMatch = true;

            while (it != m_container.end() && patternIt != pattern.end())
            {
                // If some of the values are not maching, don't bother checking further values
                if (!isMatching(*it, *patternIt))
                {
                    isMatch = false;
                    break;
                }

                // Move through the pattern and container simultaneously
                it++;
                patternIt++;
            }

            // If we kept matching and moved through the whole pattern, It's a match! (*wink* swipe right)
            if (patternIt == pattern.end() && isMatch)
                foundIndexes.emplace(index);

            // Restore iterators for next value
            it = itBefore;
            patternIt = pattern.begin();
        }

        return foundIndexes;
    }
};

#ifndef __PROGTEST__
class CStrComparator
{
public:
    CStrComparator(bool caseSensitive)
        : m_CaseSensitive(caseSensitive)
    {
    }
    bool operator()(const string &a, const string &b) const
    {
        return m_CaseSensitive ? strcasecmp(a.c_str(), b.c_str()) < 0 : a < b;
    }
    bool m_CaseSensitive;
};

bool upperCaseCompare(const char &a, const char &b)
{
    return toupper(a) < toupper(b);
}

int main(void)
{
    CIndex<string> t0("abcabcabc");
    set<size_t> r0 = t0.search("a");

    assert(r0 == (set<size_t>{0, 3, 6}));
    set<size_t> r1 = t0.search("abc");
    assert(r1 == (set<size_t>{0, 3, 6}));
    set<size_t> r2 = t0.search("abcabc");
    assert(r2 == (set<size_t>{0, 3}));

    CIndex<string> t1("abcababc");
    set<size_t> r3 = t1.search("a");
    assert(r3 == (set<size_t>{0, 3, 5}));
    set<size_t> r4 = t1.search("abc");
    assert(r4 == (set<size_t>{0, 5}));
    set<size_t> r5 = t1.search("abcabc");
    assert(r5 == (set<size_t>{}));

    CIndex<string> t2("kokokokoskokosokos");
    set<size_t> r6 = t2.search("kos");
    assert(r6 == (set<size_t>{6, 11, 15}));
    set<size_t> r7 = t2.search("kokos");
    assert(r7 == (set<size_t>{4, 9}));

    CIndex<string> t3("aaaaaaau aaauaaaau");
    set<size_t> r8 = t3.search("aa");
    assert(r8 == (set<size_t>{0, 1, 2, 3, 4, 5, 9, 10, 13, 14, 15}));
    set<size_t> r9 = t3.search("aaa");
    assert(r9 == (set<size_t>{0, 1, 2, 3, 4, 9, 13, 14}));
    set<size_t> r10 = t3.search("aaaa");
    assert(r10 == (set<size_t>{0, 1, 2, 3, 13}));

    CIndex<string> t4("automatIc authentication automotive auTOmation raut");
    set<size_t> r11 = t4.search("auto");
    assert(r11 == (set<size_t>{0, 25}));
    set<size_t> r12 = t4.search("aut");
    assert(r12 == (set<size_t>{0, 10, 25, 48}));
    set<size_t> r13 = t4.search("tic");
    assert(r13 == (set<size_t>{16}));
    set<size_t> r14 = t4.search("trunk");
    assert(r14 == (set<size_t>{}));
    set<size_t> r15 = t4.search("a");
    assert(r15 == (set<size_t>{0, 5, 10, 19, 25, 36, 41, 48}));
    set<size_t> r16 = t4.search("");
    assert(r16 == (set<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}));

    CIndex<string, bool (*)(const char &, const char &)> t5("automatIc authentication automotive auTOmation raut", upperCaseCompare);
    set<size_t> r17 = t5.search("auto");
    assert(r17 == (set<size_t>{0, 25, 36}));
    set<size_t> r18 = t5.search("aut");
    assert(r18 == (set<size_t>{0, 10, 25, 36, 48}));
    set<size_t> r19 = t5.search("tic");
    assert(r19 == (set<size_t>{6, 16}));
    set<size_t> r20 = t5.search("trunk");
    assert(r20 == (set<size_t>{}));
    set<size_t> r21 = t5.search("a");
    assert(r21 == (set<size_t>{0, 5, 10, 19, 25, 36, 41, 48}));
    set<size_t> r22 = t5.search("");
    assert(r22 == (set<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}));

    CIndex<list<string>, CStrComparator> t6(list<string>{"Hello", "world", "test", "this", "foo", "TEsT", "this", "done"}, CStrComparator(false));
    set<size_t> r23 = t6.search(list<string>{"test", "this", "foo"});
    assert(r23 == (set<size_t>{2}));
    set<size_t> r24 = t6.search(list<string>{"test", "this"});
    assert(r24 == (set<size_t>{2}));

    CIndex<list<string>, CStrComparator> t7(list<string>{"Hello", "world", "test", "this", "foo", "TEsT", "this", "done"}, CStrComparator(true));
    set<size_t> r25 = t7.search(list<string>{"test", "this", "foo"});
    assert(r25 == (set<size_t>{2}));
    set<size_t> r26 = t7.search(list<string>{"test", "this"});
    assert(r26 == (set<size_t>{2, 5}));

    return 0;
}
#endif /* __PROGTEST__ */