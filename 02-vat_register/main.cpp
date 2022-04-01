/**
 * @file main.cpp
 * @author Jan Cerny (cernyj87@fit.cvut.cz)
 * @version 1.0
 * @date 2022-04-01
 */

#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CVATRegister
{

private:
    // Alias for shared_ptr iterator in vector of T
    template <typename T>
    using ptrIterator = typename vector<shared_ptr<T>>::const_iterator;

    struct TCompany
    {
        unsigned int m_invoicesSum = 0u;
        string m_name;
        string m_address;
        string m_taxId;

        TCompany(const string &name,
                 const string &address,
                 const string &taxId)
            : m_name(name),
              m_address(address),
              m_taxId(taxId){};
    };

    vector<shared_ptr<TCompany>> m_companiesByName;
    vector<shared_ptr<TCompany>> m_companiesById;
    vector<unsigned int> m_invoices;

    /**
     * @brief Searche for company by name and adress
     *
     * @param name
     * @param address
     * @param[out] result ptrIterator to the found company
     * @return true If company was found
     * @return false If company was NOT found
     */
    bool searchCompanyByName(const string &name, const string &address, ptrIterator<TCompany> &result) const
    {
        // Companies vector is empty, can't search
        if (m_companiesByName.empty())
            return false;

        // Create dummy company for compare function
        auto searchCompany = make_shared<TCompany>(name, address, "");

        // Search for the existing company
        auto iter = lower_bound(m_companiesByName.begin(), m_companiesByName.end(), searchCompany, nameComp);

        // If company doesn't exist
        if (iter == m_companiesByName.end())
        {
            // Decrement the shared_ptr usage counter
            searchCompany.reset();
            return false;
        }

        // Check values are same
        if (strcasecmp(name.c_str(), (*iter)->m_name.c_str()) != 0 ||
            strcasecmp(address.c_str(), (*iter)->m_address.c_str()) != 0)
        {
            // Decrement the shared_ptr usage counter
            searchCompany.reset();
            return false;
        }

        result = iter;

        // Decrement the shared_ptr usage counter
        searchCompany.reset();

        return true;
    }

    /**
     * @brief Search for company by taxId
     *
     * @param[in] taxId
     * @param[out] result ptrIterator to the found company
     * @return true If company was found
     * @return false If company was NOT found
     */
    bool searchCompanyById(const string &taxId, ptrIterator<TCompany> &result) const
    {
        // Companies vector is empty, can't search
        if (m_companiesById.empty())
            return false;

        // Create dummy company for compare function
        auto searchCompany = make_shared<TCompany>("", "", taxId);

        // Search for the existing company
        auto iter = lower_bound(m_companiesById.begin(), m_companiesById.end(), searchCompany, idComp);

        // If company doesn't exist
        if (iter == m_companiesById.end())
        {
            // Decrement the shared_ptr usage counter
            searchCompany.reset();
            return false;
        }

        // Check values are same
        if (taxId != (*iter)->m_taxId)
        {
            // Decrement the shared_ptr usage counter
            searchCompany.reset();
            return false;
        }

        result = iter;

        // Decrement the shared_ptr usage counter
        searchCompany.reset();

        return true;
    }

public:
    CVATRegister(void) = default;

    ~CVATRegister(void) = default;

    /**
     * @brief Compare function for lower_bound, case-insensitive, compares lexicographically by name and then by address
     *
     * @param a
     * @param b
     * @return true
     * @return false
     */
    static bool nameComp(const shared_ptr<TCompany> &a, const shared_ptr<TCompany> &b)
    {
        // Compare names case-insensitively
        int namesCompare = strcasecmp(a->m_name.c_str(), b->m_name.c_str());

        // If they are the same, compare addresses
        if (namesCompare == 0)
        {
            int addrComapre = strcasecmp(a->m_address.c_str(), b->m_address.c_str());
            return (addrComapre < 0) ? true : false;
        }

        return (namesCompare < 0) ? true : false;
    };

    /**
     * @brief Compare function for lower_bound, compares lexicographically by taxId
     *
     * @param a
     * @param b
     * @return true
     * @return false
     */
    static bool idComp(const shared_ptr<TCompany> &a, const shared_ptr<TCompany> &b)
    {
        int idsCompare = strcmp(a->m_taxId.c_str(), b->m_taxId.c_str());

        return (idsCompare < 0) ? true : false;
    };

    /**
     * @brief Create a new company if it doesn't exist yet
     *
     * @param name Name for the company, case-insensitive
     * @param addr Address for the company, case-insensitive
     * @param taxID TaxID for the company
     * @return true If new company was added
     * @return false If company already exists
     */
    bool newCompany(const string &name,
                    const string &addr,
                    const string &taxID)
    {
        auto company = make_shared<TCompany>(name, addr, taxID);

        // If vectors are empty, push_back new company
        if (m_companiesByName.empty() && m_companiesById.empty())
        {

            m_companiesByName.push_back(company);
            m_companiesById.push_back(company);

            return true;
        }

        // Else find out the correct position where to insert
        auto nameIter = lower_bound(m_companiesByName.begin(), m_companiesByName.end(), company, nameComp);
        auto idIter = lower_bound(m_companiesById.begin(), m_companiesById.end(), company, idComp);

        // If we found a position, check the company doesn't exist yet
        if (nameIter != m_companiesByName.end() || idIter != m_companiesById.end())
        {

            // If we found by name+addr, check that the taxId matches
            if (idIter != m_companiesById.end() && (*idIter)->m_taxId == company->m_taxId)
            {
                // Decrement the usage counter, we're not be using this pointer anymore
                company.reset();
                return false;
            }

            // If we found by ID, check the name+addr match
            if (nameIter != m_companiesByName.end() &&
                strcasecmp((*nameIter)->m_name.c_str(), company->m_name.c_str()) == 0 &&
                strcasecmp((*nameIter)->m_address.c_str(), company->m_address.c_str()) == 0)

            {
                // Decrement the usage counter, we're not be using this pointer anymore
                company.reset();
                return false;
            }
        }

        // Else insert the newly made company
        m_companiesByName.insert(nameIter, company);
        m_companiesById.insert(idIter, company);

        return true;
    };

    /**
     * @brief Delete company from database, by name and address (case-insensitive)
     *
     * @param name
     * @param addr
     * @return true If company was successfully deleted
     * @return false If company wasn't found
     */
    bool cancelCompany(const string &name,
                       const string &addr)
    {
        ptrIterator<TCompany> iterName;
        ptrIterator<TCompany> iterId;

        // Search for company in both vectors
        if (!searchCompanyByName(name, addr, iterName))
            return false;

        if (!searchCompanyById((*iterName)->m_taxId, iterId))
            return false;

        // Erase from vectors
        m_companiesByName.erase(iterName);
        m_companiesById.erase(iterId);

        return true;
    }

    /**
     * @brief Delete company from database, by taxID
     *
     * @param taxID
     * @param addr
     * @return true If company was successfully deleted
     * @return false If company wasn't found
     */
    bool cancelCompany(const string &taxID)
    {
        ptrIterator<TCompany> iterId;
        ptrIterator<TCompany> iterName;

        // Search for company in both vectors
        if (!searchCompanyById(taxID, iterId))
            return false;

        if (!searchCompanyByName((*iterId)->m_name, (*iterId)->m_address, iterName))
            return false;

        // Erase from vectors
        m_companiesByName.erase(iterName);
        m_companiesById.erase(iterId);

        return true;
    }

    /**
     * @brief Add a new invoice for company, by taxID
     *
     * @param taxID
     * @param amount Amount of money to add
     * @return true If amount was successfully added
     * @return false If company wasn't found
     */
    bool invoice(const string &taxID,
                 unsigned int amount)
    {
        ptrIterator<TCompany> iter;

        // Search for company
        if (!searchCompanyById(taxID, iter))
            return false;

        // Add amount to found company
        (*iter)->m_invoicesSum += amount;

        // Add amount to invocies vector
        auto invoiceIter = lower_bound(m_invoices.begin(), m_invoices.end(), amount);
        m_invoices.insert(invoiceIter, amount);

        return true;
    }

    /**
     * @brief Add a new invoice for company, by name and address (case-insensitive)
     *
     * @param name
     * @param addr
     * @param amount Amount of money to add
     * @return true If amount was successfully added
     * @return false If company wasn't found
     */
    bool invoice(const string &name,
                 const string &addr,
                 unsigned int amount)
    {
        ptrIterator<TCompany> iter;

        // Search for company
        if (!searchCompanyByName(name, addr, iter))
            return false;

        // Add amount to found company
        (*iter)->m_invoicesSum += amount;

        // Add amount to invocies vector
        auto invoiceIter = lower_bound(m_invoices.begin(), m_invoices.end(), amount);
        m_invoices.insert(invoiceIter, amount);

        return true;
    }

    /**
     * @brief Get sum of invoices amounts from company, by name and address (case-insensitive)
     *
     * @param name
     * @param addr
     * @param[out] sumIncome Sum of all company's invoices
     * @return true If company was found
     * @return false If company was NOT found
     */
    bool audit(const string &name,
               const string &addr,
               unsigned int &sumIncome) const
    {
        ptrIterator<TCompany> iter;

        // Search for company
        if (!searchCompanyByName(name, addr, iter))
            return false;

        sumIncome = (*iter)->m_invoicesSum;

        return true;
    }

    /**
     * @brief Get sum of invoices amounts from company, by taxId
     *
     * @param taxID
     * @param[out] sumIncome Sum of all company's invoices
     * @return true If company was found
     * @return false If company was NOT found
     */
    bool audit(const string &taxID,
               unsigned int &sumIncome) const
    {
        ptrIterator<TCompany> iter;

        // Search for company
        if (!searchCompanyById(taxID, iter))
            return false;

        sumIncome = (*iter)->m_invoicesSum;

        return true;
    }

    /**
     * @brief Get info about first added company
     *
     * @param[out] name Name of the first company
     * @param[out] addr Address of the first company
     * @return true If at least 1 company exists
     * @return false If there are no companis yet
     */
    bool firstCompany(string &name,
                      string &addr) const
    {
        if (m_companiesByName.empty())
            return false;

        name = m_companiesByName[0]->m_name;
        addr = m_companiesByName[0]->m_address;

        return true;
    }

    /**
     * @brief Get next following company after company searched by name and address (case-insensitive)
     *
     * @param[in,out] name Name of the company to search for, is set to next company's name afterwards
     * @param[in,out] addr Address of the company to search for, is set to next company's address afterwards
     * @return true If company was found and next company exists
     * @return false If company was NOT found, or there is NO next company present
     */
    bool nextCompany(string &name,
                     string &addr) const
    {
        ptrIterator<TCompany> iter;

        // Search for company
        if (!searchCompanyByName(name, addr, iter))
            return false;

        // If next company doesn't exist
        if ((iter + 1) == m_companiesByName.end())
            return false;

        name = (*(iter + 1))->m_name;
        addr = (*(iter + 1))->m_address;

        return true;
    }

    /**
     * @brief Get median of all added invoices
     *
     * @return unsigned int Median of all added invoices, return 0 if there are no invoices yet
     */
    unsigned int medianInvoice(void) const
    {
        size_t size = m_invoices.size();

        // Can't find median if there are no invoices
        if (size == 0)
            return 0u;

        // No point in finding the middle if thres only 1 element
        if (size == 1)
            return m_invoices[0];

        // Get the middle from vector
        size_t middle = size / (size_t)2;

        // If the number is odd, get the upper element from two middles
        if (size % 2 == 0)
            return m_invoices[middle];

        // Else get the lower element (decrement by 1 because index counts from 0, vector::size() not)
        return m_invoices[middle];
    }
};

#ifndef __PROGTEST__
int main(void)
{
    string name, addr;
    unsigned int sumIncome;
    CVATRegister b0;
    assert(b0.newCompany("AAA", "ABC", "100")); // 1 / 2 (names position / ids position)
    assert(!b0.newCompany("AAA", "ABC", "100"));
    assert(!b0.newCompany("XXX", "XXX", "100"));
    assert(!b0.newCompany("AAA", "ABC", "999"));
    assert(b0.newCompany("AAB", "AAB", "111")); // 2 / 3
    assert(b0.newCompany("DEF", "AAA", "001")); // 3 / 1

    assert(b0.invoice("100", 20));
    assert(b0.invoice("100", 60));
    assert(b0.invoice("100", 100));

    CVATRegister b1;
    assert(b1.newCompany("ACME", "Thakurova", "666/666"));
    assert(b1.newCompany("ACME", "Kolejni", "666/666/666"));
    assert(b1.newCompany("Dummy", "Thakurova", "123456"));
    assert(b1.invoice("666/666", 2000));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("666/666/666", 3000));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 4000));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("aCmE", "Kolejni", 5000));
    assert(b1.medianInvoice() == 4000);

    assert(b1.audit("ACME", "Kolejni", sumIncome) && sumIncome == 8000);
    assert(b1.audit("123456", sumIncome) && sumIncome == 4000);
    assert(b1.firstCompany(name, addr) && name == "ACME" && addr == "Kolejni");
    assert(b1.nextCompany(name, addr) && name == "ACME" && addr == "Thakurova");
    assert(b1.nextCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert(!b1.nextCompany(name, addr));

    assert(b1.cancelCompany("ACME", "KoLeJnI"));
    assert(b1.medianInvoice() == 4000);
    assert(b1.cancelCompany("666/666"));
    assert(b1.medianInvoice() == 4000);
    assert(b1.invoice("123456", 100));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 300));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 200));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 230));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 830));
    assert(b1.medianInvoice() == 830);
    assert(b1.invoice("123456", 1830));
    assert(b1.medianInvoice() == 1830);
    assert(b1.invoice("123456", 2830));
    assert(b1.medianInvoice() == 1830);
    assert(b1.invoice("123456", 2830));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 3200));
    assert(b1.medianInvoice() == 2000);
    assert(b1.firstCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert(!b1.nextCompany(name, addr));
    assert(b1.cancelCompany("123456"));
    assert(!b1.firstCompany(name, addr));

    CVATRegister b2;
    assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert(b2.newCompany("Dummy", "Kolejni", "123456"));
    assert(!b2.newCompany("AcMe", "kOlEjNi", "1234"));
    assert(b2.newCompany("Dummy", "Thakurova", "ABCDEF"));
    assert(b2.medianInvoice() == 0);
    assert(b2.invoice("ABCDEF", 1000));
    assert(b2.medianInvoice() == 1000);
    assert(b2.invoice("abcdef", 2000));
    assert(b2.medianInvoice() == 2000);
    assert(b2.invoice("aCMe", "kOlEjNi", 3000));
    assert(b2.medianInvoice() == 2000);
    assert(!b2.invoice("1234567", 100));
    assert(!b2.invoice("ACE", "Kolejni", 100));
    assert(!b2.invoice("ACME", "Thakurova", 100));
    assert(!b2.audit("1234567", sumIncome));
    assert(!b2.audit("ACE", "Kolejni", sumIncome));
    assert(!b2.audit("ACME", "Thakurova", sumIncome));
    assert(!b2.cancelCompany("1234567"));
    assert(!b2.cancelCompany("ACE", "Kolejni"));
    assert(!b2.cancelCompany("ACME", "Thakurova"));
    assert(b2.cancelCompany("abcdef"));
    assert(b2.medianInvoice() == 2000);
    assert(!b2.cancelCompany("abcdef"));
    assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert(b2.cancelCompany("ACME", "Kolejni"));
    assert(!b2.cancelCompany("ACME", "Kolejni"));

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
