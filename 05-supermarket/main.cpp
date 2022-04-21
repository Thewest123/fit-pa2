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

    // Also used by std::map for std::less comparator
    bool operator<(const CDate &rhs) const
    {
        return tie(m_year, m_month, m_day) < tie(rhs.m_year, rhs.m_month, rhs.m_day);
    }

    bool operator>(const CDate &rhs) const
    {
        return tie(m_year, m_month, m_day) > tie(rhs.m_year, rhs.m_month, rhs.m_day);
    }

    bool operator>=(const CDate &rhs) const
    {
        return tie(m_year, m_month, m_day) >= tie(rhs.m_year, rhs.m_month, rhs.m_day);
    }
};

class CSupermarket
{
private:
    struct TProductType
    {
        map<CDate, int> m_dates;

        void addDate(const CDate &expiryDate, int count)
        {
            if (m_dates.count(expiryDate))
            {
                m_dates.at(expiryDate) += count;
            }
            else
            {
                m_dates.emplace(expiryDate, count);
            }
        }

        int sellCount(int desiredCount)
        {
            int soldCount = 0;

            while (m_dates.size() > 0)
            {
                auto itemCount = m_dates.begin()->second;

                if (itemCount <= (desiredCount - soldCount))
                {
                    soldCount += itemCount;
                    m_dates.erase(m_dates.begin());
                    continue;
                }

                m_dates.begin()->second -= (desiredCount - soldCount);
                soldCount += (desiredCount - soldCount);
                break;
            }

            return soldCount;
        }
    };

    unordered_map<string, TProductType> m_products;

public:
    // default constructor
    // store   ( name, expiryDate, count )
    CSupermarket &store(const string &productName, const CDate &expiryDate, int count)
    {
        // If product already exists
        if (m_products.count(productName))
        {
            // Add new expiry date to it
            m_products.at(productName).addDate(expiryDate, count);
        }
        else
        {
            // Create a new product and add it to products
            TProductType newProduct;
            newProduct.addDate(expiryDate, count);

            m_products.emplace(productName, newProduct);
        }

        // Return reference to itself, so we can daisy-chain this method
        return *this;
    }

    static bool countCompare(const pair<string, int> &lhs, const pair<string, int> &rhs)
    {
        return lhs.second > rhs.second;
    }

    void sell(list<pair<string, int>> &shoppingList)
    {
        auto nonSellale = getAllSimilar(shoppingList);
        string alternative = "";

        auto it = shoppingList.begin();
        while (it != shoppingList.end())
        {
            // it->first = product name
            // it->second = desired count

            if (nonSellale.count(it->first) || !isUnique(it->first, alternative))
            {
                it++;
                continue;
            }

            int soldCount = m_products.at(alternative).sellCount(it->second);

            if (m_products.at(alternative).m_dates.size() == 0)
                m_products.erase(alternative);

            if (it->second - soldCount == 0)
            {
                it = shoppingList.erase(it);
                continue;
            }
            else
            {
                it->second -= soldCount;
            }

            it++;
        }
    }

    list<pair<string, int>> expired(const CDate &expiryDate) const
    {
        // List containing expired products, key is productName
        list<pair<string, int>> expiredList;

        // Loop through all products
        for (auto const &[productName, productType] : m_products)
        {
            int expiredCount = 0;

            // Loop through all expiryDates for product type and check if it's expired
            for (auto const &[dateDate, dateCount] : productType.m_dates)
            {
                // Because m_dates are sorted, we can break once we find date that is higher or equal than expiryDate
                // (rest of them will be only higher, no need to check them)
                if (dateDate >= expiryDate)
                    break;

                expiredCount += dateCount;
            }

            if (expiredCount > 0)
            {
                // Create a new product pair
                auto product = make_pair(productName, expiredCount);

                // Find the right position, sorted by count
                auto it = lower_bound(expiredList.begin(), expiredList.end(), product, countCompare);

                // Add to the list
                expiredList.insert(it, product);
            }
        }

        return expiredList;
    }

    static bool isSimilar(const string &stringA, const string &stringB)
    {
        if (stringA.length() != stringB.length())
            return false;

        int diffCount = 0;
        for (size_t i = 0; i < stringA.length(); i++)
        {
            if (stringA.at(i) != stringB.at(i))
                diffCount++;

            if (diffCount > 1)
                return false;
        }

        return true;
    }

    bool isUnique(const string &search, string &result)
    {
        // If there is exact match, everything is fine
        if (m_products.count(search))
        {
            result = search;
            return true;
        }

        // Else we need to find all items, that have similar name
        int similarCount = 0;
        for (const auto &product : m_products)
        {
            if (similarCount > 1)
                return false;

            if (isSimilar(product.first, search))
            {
                similarCount++;
                result = product.first;
            }
        }

        if (similarCount != 1)
            return false;

        return true;
    }

    set<string> getAllSimilar(const list<pair<string, int>> &shoppingList)
    {
        set<string> allSimilar;
        string result;

        for (const auto &[name, count] : shoppingList)
        {
            if (!isUnique(name, result))
                allSimilar.insert(name);
        }

        return allSimilar;
    }
};
#ifndef __PROGTEST__
int main(void)
{
    // Check dates comparisons work correctly
    assert(CDate(2020, 6, 1) > CDate(2020, 5, 30));
    assert(CDate(2019, 6, 1) < CDate(2020, 5, 30));

    assert(CSupermarket::isSimilar("lorem", "lorem"));
    assert(CSupermarket::isSimilar("lorem", "Lorem"));
    assert(CSupermarket::isSimilar("lOrem", "lorem"));
    assert(!CSupermarket::isSimilar("lorem", "lorem ipsum"));
    assert(!CSupermarket::isSimilar("lorem", "lorEM"));
    assert(!CSupermarket::isSimilar("Lorem", "lOrem"));

    CSupermarket x;
    x.store("debugging cookie", CDate(2022, 06, 10), 10);
    x.store("debugging cookie", CDate(2022, 06, 9), 9);
    x.store("debugging cookie", CDate(2022, 06, 13), 13);
    x.store("debugging cookie", CDate(2022, 06, 12), 12);

    auto lx = x.expired(CDate(2022, 06, 13));
    assert(lx.size() == 1);
    assert((lx == list<pair<string, int>>{{"debugging cookie", 9 + 10 + 12}}));

    auto ly = x.expired(CDate(1969, 4, 20));
    assert(ly.size() == 0);
    assert((ly == list<pair<string, int>>{}));

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

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */