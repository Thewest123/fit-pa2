#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDate
{
private:
    int m_year;
    int m_month;
    int m_day;

public:
    CDate(int year, int month, int day)
        : m_year(year), m_month(month), m_day(day) {}

    bool operator==(const CDate &rhs) const
    {
        return (m_year == rhs.m_year &&
                m_month == rhs.m_month &&
                m_day == rhs.m_day);
    }

    bool operator!=(const CDate &rhs) const
    {
        return (m_year != rhs.m_year ||
                m_month != rhs.m_month ||
                m_day != rhs.m_day);
    }
};

class CSupermarket
{
private:
    struct cmpDates
    {
        bool operator()(const CDate &lhs, CDate &rhs) const
        {
            return lhs < rhs;
        }
    };

    struct TProductType
    {
        string m_name;
        int m_count;
        map<CDate, int, cmpDates> m_products;
    };

public:
    // default constructor
    // store   ( name, expiryDate, count )
    // sell    ( shoppingList )
    // expired ( date ) const
};
#ifndef __PROGTEST__
int main(void)
{
    cout << "xd" << endl;
    /*
    CSupermarket s;
    s.store("bread", CDate(2016, 4, 30), 100)
        .store("butter", CDate(2016, 5, 10), 10)
        .store("beer", CDate(2016, 8, 10), 50)
        .store("bread", CDate(2016, 4, 25), 100)
        .store("okey", CDate(2016, 7, 18), 5);



    list<pair<string, int>> l0 = s.expired(CDate(2018, 4, 30));
    assert(l0.size() == 4);
    assert((l0 == list<pair<string, int>>{{"bread", 200}, {"beer", 50}, {"butter", 10}, {"okey", 5}}));

    list<pair<string, int>> l1{{"bread", 2}, {"Coke", 5}, {"butter", 20}};
    s.sell(l1);
    assert(l1.size() == 2);
    assert((l1 == list<pair<string, int>>{{"Coke", 5}, {"butter", 10}}));

    list<pair<string, int>> l2 = s.expired(CDate(2016, 4, 30));
    assert(l2.size() == 1);
    assert((l2 == list<pair<string, int>>{{"bread", 98}}));

    list<pair<string, int>> l3 = s.expired(CDate(2016, 5, 20));
    assert(l3.size() == 1);
    assert((l3 == list<pair<string, int>>{{"bread", 198}}));

    list<pair<string, int>> l4{{"bread", 105}};
    s.sell(l4);
    assert(l4.size() == 0);
    assert((l4 == list<pair<string, int>>{}));

    list<pair<string, int>> l5 = s.expired(CDate(2017, 1, 1));
    assert(l5.size() == 3);
    assert((l5 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"okey", 5}}));

    s.store("Coke", CDate(2016, 12, 31), 10);

    list<pair<string, int>> l6{{"Cake", 1}, {"Coke", 1}, {"cake", 1}, {"coke", 1}, {"cuke", 1}, {"Cokes", 1}};
    s.sell(l6);
    assert(l6.size() == 3);
    assert((l6 == list<pair<string, int>>{{"cake", 1}, {"cuke", 1}, {"Cokes", 1}}));

    list<pair<string, int>> l7 = s.expired(CDate(2017, 1, 1));
    assert(l7.size() == 4);
    assert((l7 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 7}, {"okey", 5}}));

    s.store("cake", CDate(2016, 11, 1), 5);

    list<pair<string, int>> l8{{"Cake", 1}, {"Coke", 1}, {"cake", 1}, {"coke", 1}, {"cuke", 1}};
    s.sell(l8);
    assert(l8.size() == 2);
    assert((l8 == list<pair<string, int>>{{"Cake", 1}, {"coke", 1}}));

    list<pair<string, int>> l9 = s.expired(CDate(2017, 1, 1));
    assert(l9.size() == 5);
    assert((l9 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 6}, {"okey", 5}, {"cake", 3}}));

    list<pair<string, int>> l10{{"cake", 15}, {"Cake", 2}};
    s.sell(l10);
    assert(l10.size() == 2);
    assert((l10 == list<pair<string, int>>{{"cake", 12}, {"Cake", 2}}));

    list<pair<string, int>> l11 = s.expired(CDate(2017, 1, 1));
    assert(l11.size() == 4);
    assert((l11 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"Coke", 6}, {"okey", 5}}));

    list<pair<string, int>> l12{{"Cake", 4}};
    s.sell(l12);
    assert(l12.size() == 0);
    assert((l12 == list<pair<string, int>>{}));

    list<pair<string, int>> l13 = s.expired(CDate(2017, 1, 1));
    assert(l13.size() == 4);
    assert((l13 == list<pair<string, int>>{{"bread", 93}, {"beer", 50}, {"okey", 5}, {"Coke", 2}}));

    list<pair<string, int>> l14{{"Beer", 20}, {"Coke", 1}, {"bear", 25}, {"beer", 10}};
    s.sell(l14);
    assert(l14.size() == 1);
    assert((l14 == list<pair<string, int>>{{"beer", 5}}));

    s.store("ccccb", CDate(2019, 3, 11), 100)
        .store("ccccd", CDate(2019, 6, 9), 100)
        .store("dcccc", CDate(2019, 2, 14), 100);

    list<pair<string, int>> l15{{"ccccc", 10}};
    s.sell(l15);
    assert(l15.size() == 1);
    assert((l15 == list<pair<string, int>>{{"ccccc", 10}}));

    */

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */