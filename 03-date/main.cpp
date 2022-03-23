#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
public:
    InvalidDateException()
        : invalid_argument("invalid date or format")
    {
    }
};
//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base &(*date_format(const char *fmt))(ios_base &x)
{
    return [](ios_base &ios) -> ios_base &
    { return ios; };
}
//=================================================================================================
class CDate
{
private:
    int m_year;
    int m_month;
    int m_day;

public:
    static bool isLeapYear(int year)
    {
        return (year % 4 == 0 && ((year % 100 != 0 || year % 400 == 0) && year % 4000 != 0));
    }

    static int getDaysInMonth(int year, int month)
    {
        if (month == 2 && isLeapYear(year))
            return 29;

        int months[] = {31,
                        28,
                        31,
                        30,
                        31,
                        30,
                        31,
                        31,
                        30,
                        31,
                        30,
                        31};

        return months[month - 1];
    }

    static bool isValidDate(int year, int month, int day)
    {
        if (month < 1 || month > 12)
            return false;

        if (day < 0 || day > getDaysInMonth(year, month))
            return false;

        return true;
    }

    static int countDaysDifference(const CDate &dateA, const CDate &dateB)
    {
        long long daysDiff = 0;

        if (dateA.m_year != dateB.m_year)
        {
            // Remaining days to the end of the year
            daysDiff = (isLeapYear(dateA.m_year) ? 366 : 365) - dateA.countDaysFromBeggining();

            for (int i = dateA.m_year + 1; i < dateB.m_year; i++)
                daysDiff += isLeapYear(i) ? 366 : 365;

            daysDiff += dateB.countDaysFromBeggining();
        }
        else
        {
            daysDiff = dateB.countDaysFromBeggining() - dateA.countDaysFromBeggining();
        }

        return daysDiff;
    }

    int countDaysFromBeggining() const
    {
        int result = 0;

        result += m_day;

        for (int i = 1; i < m_month; i++)
        {
            result += getDaysInMonth(m_year, i);
        }

        return result;
    }

    void addYears(int years)
    {
        m_year += years;
    }

    void addMonths(int months)
    {
        // For adding positive value, possible overflow to next year
        if (m_month + months > 12)
        {
            int years = (months > 12) ? months / 12 : 1;
            addYears(years);
            m_month = (m_month + months - 12 * (years));
            return;
        }

        // For adding negative value, possible underflow to previous year
        if (m_month + months < 1)
        {
            int years = (abs(months) > 12) ? (abs(months) / 12) : 1;
            addYears(-years);
            m_month = (m_month + months + 12 * (years));
            return;
        }

        m_month += months;
    }

    void addDays(int days)
    {
        while (days > 0)
        {

            int daysInCurrentYear = isLeapYear(m_year) ? 366 : 365;
            if (days >= daysInCurrentYear)
            {
                addYears(1);
                days -= daysInCurrentYear;
                continue;
            }

            int daysInCurrentMonth = getDaysInMonth(m_year, m_month);
            if (days >= daysInCurrentMonth)
            {
                addMonths(1);
                days -= daysInCurrentMonth;
                continue;
            }

            m_day += days;

            if (m_day > getDaysInMonth(m_year, m_month))
            {
                m_day -= getDaysInMonth(m_year, m_month);
                addMonths(1);
            }

            days = 0;
        }

        while (days < 0)
        {

            int daysInCurrentYear = isLeapYear(m_year) ? 366 : 365;
            if (abs(days) >= daysInCurrentYear)
            {
                addYears(-1);
                days += daysInCurrentYear;
                continue;
            }

            if (abs(days) >= getDaysInMonth(m_year, m_month))
            {
                addMonths(-1);
                days += getDaysInMonth(m_year, m_month);
                continue;
            }

            m_day += days;

            if (m_day < 1)
            {
                addMonths(-1);
                m_day += getDaysInMonth(m_year, m_month);
            }

            days = 0;
        }
    }

    CDate() = default;

    CDate(int year, int month, int day)
    {
        if (!isValidDate(year, month, day))
            throw InvalidDateException();

        m_year = year;
        m_month = month;
        m_day = day;
    }

    // Output date as formatted string
    friend ostream &operator<<(ostream &output, const CDate &date)
    {
        output << std::setfill('0') << std::setw(4) << date.m_year << '-';
        output << std::setfill('0') << std::setw(2) << date.m_month << '-';
        output << std::setfill('0') << std::setw(2) << date.m_day;
        return output;
    }

    // Read input and parse to date
    friend istream &operator>>(istream &input, CDate &date)
    {
        char d0 = 0;
        char d1 = 0;
        int year = 0;
        int month = 0;
        int day = 0;
        CDate tmp;

        if (!(input >> year >> d0 >> month >> d1 >> day))
        {
            input.setstate(ios::failbit);
            cerr << "Bad input" << endl;
            return input;
        }

        if (d0 != '-' ||
            d1 != '-')
        {
            input.setstate(ios::failbit);
            cerr << "Bad delimiter" << endl;
            return input;
        }

        try
        {
            tmp = CDate(year, month, day);
        }
        catch (const exception &e)
        {
            cerr << date << " | " << e.what() << endl;
            input.setstate(ios::failbit);
            return input;
        }

        date = tmp;
        return input;
    }

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

    bool operator>(const CDate &rhs) const
    {
        return countDaysDifference(rhs, *this) > 0;
    }

    bool operator<(const CDate &rhs) const
    {
        return countDaysDifference(rhs, *this) < 0;
    }

    bool operator>=(const CDate &rhs) const
    {
        return countDaysDifference(rhs, *this) >= 0;
    }

    bool operator<=(const CDate &rhs) const
    {
        return countDaysDifference(rhs, *this) <= 0;
    }

    // date + num
    CDate operator+(const int days) const
    {
        CDate ret(m_year, m_month, m_day);
        ret.addDays(days);
        return ret;
    }

    // date += num
    CDate &operator+=(const int days)
    {
        *this = *this + days;
        return *this;
    }

    // date - num
    CDate operator-(const int days) const
    {
        CDate ret(m_year, m_month, m_day);
        ret.addDays(-days);
        return ret;
    }

    // date -= num
    CDate &operator-=(const int days)
    {
        *this = *this - days;
        return *this;
    }

    // Prefix ++
    CDate &operator++()
    {
        addDays(1);
        return *this;
    }

    // Postfix ++
    CDate operator++(int)
    {
        CDate temp = *this;
        addDays(1);
        return temp;
    }

    // Prefix --
    CDate &operator--()
    {
        addDays(-1);
        return *this;
    }

    // Postfix --
    CDate operator--(int)
    {
        CDate temp = *this;
        addDays(-1);
        return temp;
    }

    // Difference in days between two days
    int operator-(const CDate &rhs) const
    {
        return abs(countDaysDifference(*this, rhs));
    }
};

#ifndef __PROGTEST__
int main(void)
{
    ostringstream oss;
    istringstream iss;

    CDate months(2000, 1, 1);

    months.addMonths(12);
    oss.str("");
    oss << months;
    cout << months << endl;
    assert(oss.str() == "2001-01-01");

    months.addMonths(24);
    oss.str("");
    oss << months;
    cout << months << endl;
    assert(oss.str() == "2003-01-01");

    months.addMonths(-48);
    oss.str("");
    oss << months;
    cout << months << endl;
    assert(oss.str() == "1999-01-01");

    CDate a(2000, 1, 2);
    CDate b(2010, 2, 3);
    CDate c(2004, 2, 10);
    oss.str("");
    oss << a;
    assert(oss.str() == "2000-01-02");
    oss.str("");
    oss << b;
    assert(oss.str() == "2010-02-03");
    oss.str("");
    oss << c;
    a = a + 1500;
    oss.str("");
    oss << a;
    assert(oss.str() == "2004-02-10");

    // přičtení kladného přes rok
    CDate presrok(2022, 12, 24);
    // cout << presrok << endl;

    oss.str("");
    oss << presrok;
    assert(oss.str() == "2022-12-24");

    presrok += 11;

    cout << presrok << endl;

    oss.str("");
    oss << presrok;
    assert(oss.str() == "2023-01-04");

    // přičtení záporného měsíce
    CDate zaporne(2022, 3, 1);
    // cout << zaporne << endl;

    oss.str("");
    oss << zaporne;
    assert(oss.str() == "2022-03-01");

    zaporne += -(28 + 31 + 31 + 30);

    // cout << zaporne << endl;

    oss.str("");
    oss << zaporne;
    assert(oss.str() == "2021-11-01");

    cout << b << endl;
    b = b - 2000;
    cout << b << endl;
    oss.str("");
    oss << b;
    assert(oss.str() == "2004-08-13");

    assert(b - a == 185);

    assert((b == a) == false);
    assert((b != a) == true);
    assert((b <= a) == false);
    assert((b < a) == false);
    assert((b >= a) == true);
    assert((b > a) == true);
    assert((c == a) == true);
    assert((c != a) == false);
    assert((c <= a) == true);
    assert((c < a) == false);
    assert((c >= a) == true);
    assert((c > a) == false);

    a = ++c;
    oss.str("");
    oss << a << " " << c;
    assert(oss.str() == "2004-02-11 2004-02-11");
    a = --c;
    oss.str("");
    oss << a << " " << c;
    assert(oss.str() == "2004-02-10 2004-02-10");
    a = c++;
    oss.str("");
    oss << a << " " << c;
    assert(oss.str() == "2004-02-10 2004-02-11");
    a = c--;
    oss.str("");
    oss << a << " " << c;
    assert(oss.str() == "2004-02-11 2004-02-10");

    iss.clear();
    iss.str("2015-09-03");
    assert((iss >> a));
    oss.str("");
    oss << a;
    assert(oss.str() == "2015-09-03");
    a = a + 70;
    oss.str("");
    oss << a;
    assert(oss.str() == "2015-11-12");

    CDate d(2000, 1, 1);
    try
    {
        CDate e(2000, 32, 1);
        assert("No exception thrown!" == nullptr);
    }
    catch (...)
    {
    }
    iss.clear();
    iss.str("2000-12-33");
    assert(!(iss >> d));
    oss.str("");
    oss << d;
    assert(oss.str() == "2000-01-01");
    iss.clear();
    iss.str("2000-11-31");
    assert(!(iss >> d));
    oss.str("");
    oss << d;
    assert(oss.str() == "2000-01-01");
    iss.clear();
    iss.str("2000-02-29");
    assert((iss >> d));
    oss.str("");
    oss << d;
    assert(oss.str() == "2000-02-29");
    iss.clear();
    iss.str("2001-02-29");
    assert(!(iss >> d));
    oss.str("");
    oss << d;
    assert(oss.str() == "2000-02-29");
    /*
                //-----------------------------------------------------------------------------
                // bonus test examples
                //-----------------------------------------------------------------------------
                CDate f(2000, 5, 12);
                oss.str("");
                oss << f;
                assert(oss.str() == "2000-05-12");
                oss.str("");
                oss << date_format("%Y/%m/%d") << f;
                assert(oss.str() == "2000/05/12");
                oss.str("");
                oss << date_format("%d.%m.%Y") << f;
                assert(oss.str() == "12.05.2000");
                oss.str("");
                oss << date_format("%m/%d/%Y") << f;
                assert(oss.str() == "05/12/2000");
                oss.str("");
                oss << date_format("%Y%m%d") << f;
                assert(oss.str() == "20000512");
                oss.str("");
                oss << date_format("hello kitty") << f;
                assert(oss.str() == "hello kitty");
                oss.str("");
                oss << date_format("%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%") << f;
                assert(oss.str() == "121212121212050505200020002000%%%%%");
                oss.str("");
                oss << date_format("%Y-%m-%d") << f;
                assert(oss.str() == "2000-05-12");
                iss.clear();
                iss.str("2001-01-1");
                assert(!(iss >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2000-05-12");
                iss.clear();
                iss.str("2001-1-01");
                assert(!(iss >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2000-05-12");
                iss.clear();
                iss.str("2001-001-01");
                assert(!(iss >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2000-05-12");
                iss.clear();
                iss.str("2001-01-02");
                assert((iss >> date_format("%Y-%m-%d") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2001-01-02");
                iss.clear();
                iss.str("05.06.2003");
                assert((iss >> date_format("%d.%m.%Y") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2003-06-05");
                iss.clear();
                iss.str("07/08/2004");
                assert((iss >> date_format("%m/%d/%Y") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2004-07-08");
                iss.clear();
                iss.str("2002*03*04");
                assert((iss >> date_format("%Y*%m*%d") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2002-03-04");
                iss.clear();
                iss.str("C++09format10PA22006rulez");
                assert((iss >> date_format("C++%mformat%dPA2%Yrulez") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2006-09-10");
                iss.clear();
                iss.str("%12%13%2010%");
                assert((iss >> date_format("%%%m%%%d%%%Y%%") >> f));
                oss.str("");
                oss << f;
                assert(oss.str() == "2010-12-13");

                CDate g(2000, 6, 8);
                iss.clear();
                iss.str("2001-11-33");
                assert(!(iss >> date_format("%Y-%m-%d") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("29.02.2003");
                assert(!(iss >> date_format("%d.%m.%Y") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("14/02/2004");
                assert(!(iss >> date_format("%m/%d/%Y") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("2002-03");
                assert(!(iss >> date_format("%Y-%m") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("hello kitty");
                assert(!(iss >> date_format("hello kitty") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("2005-07-12-07");
                assert(!(iss >> date_format("%Y-%m-%d-%m") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-06-08");
                iss.clear();
                iss.str("20000101");
                assert((iss >> date_format("%Y%m%d") >> g));
                oss.str("");
                oss << g;
                assert(oss.str() == "2000-01-01");
            */
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */