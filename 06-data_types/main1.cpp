#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDataType
{

protected:
public:
    string m_typeName;
    string m_name;
    size_t m_typeSize = 0;

    CDataType(const string &typeName, const string &name)
        : m_typeName(typeName), m_name(name){};

    CDataType(const string &typeName, const string &name, size_t size)
        : m_typeName(typeName), m_name(name), m_typeSize(size){};

    virtual shared_ptr<CDataType> clone() const = 0;

    virtual size_t getSize() const
    {
        return m_typeSize;
    }

    string getTypeName() const
    {
        return m_typeName;
    }

    virtual bool operator==(const CDataType &rhs) const
    {
        return m_typeName == rhs.m_typeName;
    };

    bool operator!=(const CDataType &rhs) const
    {
        return !(*this == rhs);
    }

    virtual void print(ostream &out) const
    {
        if (m_name == "")
            out << m_typeName << flush;
        else
            out << m_typeName << " " << m_name << ";" << endl;
    }

    friend ostream &operator<<(ostream &out, const CDataType &rhs)
    {
        rhs.print(out);
        return out;
    }

    virtual ~CDataType() = default;
};

class CDataTypeInt : public CDataType
{
public:
    CDataTypeInt()
        : CDataType("int", "", 4){};

    CDataTypeInt(const string &name)
        : CDataType("int", name, 4){};

    virtual shared_ptr<CDataType> clone() const override
    {
        return make_shared<CDataTypeInt>(*this);
    }
};

class CDataTypeDouble : public CDataType
{
public:
    CDataTypeDouble()
        : CDataType("double", "", 8){};

    CDataTypeDouble(const string &name)
        : CDataType("double", name, 8){};

    virtual shared_ptr<CDataType> clone() const override
    {
        return make_shared<CDataTypeDouble>(*this);
    }
};
class CDataTypeEnum : public CDataType
{
private:
    vector<string> enumNames;

public:
    CDataTypeEnum()
        : CDataType("enum", "", 4){};

    CDataTypeEnum(const string &name)
        : CDataType("enum", name, 4){};

    virtual shared_ptr<CDataType> clone() const override
    {
        return make_shared<CDataTypeEnum>(*this);
    }

    CDataTypeEnum &add(const string &name)
    {
        if (count(enumNames.begin(), enumNames.end(), name) != 0)
            throw invalid_argument(string("Duplicate enum value: ") + name);

        enumNames.push_back(name);

        return *this;
    }

    virtual bool operator==(const CDataType &rhs) const override
    {
        if (m_typeName != rhs.getTypeName())
            return false;

        const auto *rhsEnum = dynamic_cast<const CDataTypeEnum *>(&rhs);

        // ? Is neccessary to compare sizes or does std::equal() compare it too?
        if (enumNames.size() != rhsEnum->enumNames.size())
            return false;

        return equal(enumNames.begin(), enumNames.end(), rhsEnum->enumNames.begin());
    }

    virtual void print(ostream &out) const override
    {
        out << "enum" << endl;
        out << "{" << endl;

        for (const auto &enumName : enumNames)
        {
            out << "    " << enumName;

            if (enumName != enumNames.back())
                out << ",";

            out << endl;
        }

        if (m_name == "")
            out << "}" << flush;
        else
            out << "} " << m_name << ";" << endl;
    }
};
class CDataTypeStruct : public CDataType
{
private:
    struct TStructItem
    {
        string m_itemName;
        shared_ptr<CDataType> m_itemPtr;

        TStructItem(const string &name, const shared_ptr<CDataType> &ptr)
        {
            m_itemName = name;
            m_itemPtr = ptr;
        }

        bool operator==(const TStructItem &rhs) const
        {
            return m_itemName.compare(rhs.m_itemName);
        }

        bool operator!=(const TStructItem &rhs) const
        {
            return !(*this == rhs);
        }
    };

    vector<TStructItem> items;

public:
    CDataTypeStruct()
        : CDataType("struct", ""){};

    CDataTypeStruct(const string &name)
        : CDataType("struct", name){};

    virtual shared_ptr<CDataType> clone() const override
    {
        return make_shared<CDataTypeStruct>(*this);
    }

    virtual size_t getSize() const override
    {
        size_t size = 0;

        for (const auto &item : items)
        {
            size += item.m_itemPtr->getSize();
        }

        return size;
    }

    CDataTypeStruct &addField(const string &name, const CDataType &newItem)
    {
        for (const auto &item : items)
        {
            if (item.m_itemName == name)
                throw invalid_argument(string("Duplicate field: ") + name);
        }

        auto newItemClone = newItem.clone();
        newItemClone->m_name = "";

        items.emplace_back(name, newItemClone);

        return *this;
    }

    CDataType &field(const string &name) const
    {

        for (const auto &item : items)
        {
            if (item.m_itemName == name)
            {
                return *(item.m_itemPtr);
            }
        }

        throw invalid_argument(string("Unknown field: ") + name);
    }

    virtual void print(ostream &out) const override
    {
        out << "struct" << endl;
        out << "{" << endl;

        for (const auto &item : items)
        {
            item.m_itemPtr->print(out);
            out << " " << item.m_itemName << ";" << endl;
        }

        out << "}" << endl;
    }

    virtual bool operator==(const CDataType &rhs) const override
    {
        if (getTypeName() != rhs.getTypeName())
            return false;

        if (getSize() != rhs.getSize())
            return false;

        const auto *rhsStruct = dynamic_cast<const CDataTypeStruct *>(&rhs);

        for (size_t i = 0; i < items.size(); i++)
        {
            if (*(items[i].m_itemPtr) != *(rhsStruct->items[i].m_itemPtr))
                return false;
        }

        return true;
    }
};

#ifndef __PROGTEST__

static bool isSpace_unsigned(unsigned char x)
{
    return isspace(x);
}

static bool whitespaceMatch(const string &a, const string &b)
{
    string aNoSpace = a;
    string bNoSpace = b;

    // Inspired from https://stackoverflow.com/a/83468
    // First we "remove" spaces by shuffling the characters using remove_if(),
    // then we need to call erase() to actually modify the string container
    aNoSpace.erase(remove_if(aNoSpace.begin(), aNoSpace.end(), isSpace_unsigned), aNoSpace.end());
    bNoSpace.erase(remove_if(bNoSpace.begin(), bNoSpace.end(), isSpace_unsigned), bNoSpace.end());

    if (aNoSpace.size() != bNoSpace.size())
        return false;

    return (aNoSpace.compare(bNoSpace) == 0);
}

template <typename T_>
static bool whitespaceMatch(const T_ &x, const string &ref)
{
    ostringstream oss;
    oss << x;
    return whitespaceMatch(oss.str(), ref);
}

int main(void)
{
    // Check whitespaceMatch
    assert(whitespaceMatch("hello", "hello"));
    assert(whitespaceMatch("hel Lo world   ", "helLo world"));
    assert(whitespaceMatch("hel Lo world   ", "helLo	world")); // Tab character
    assert(whitespaceMatch("", "   "));
    assert(whitespaceMatch("", ""));
    assert(whitespaceMatch("+ěščřžýáíé", "   +ě    ščřž ý  áí é"));
    assert(!whitespaceMatch("  hel  lo  ", "  HeL  Lo  "));

    ostringstream oss;

    // Check CDataType printing
    CDataTypeInt intA("mujIntA");
    oss.str("");
    oss << intA;
    assert(oss.str() == "int mujIntA;\n");

    CDataTypeInt intB("mujIntB");
    oss.str("");
    oss << intB;
    assert(oss.str() == "int mujIntB;\n");

    CDataTypeDouble doubleA("mujDoubleA");
    oss.str("");
    oss << doubleA;
    assert(oss.str() == "double mujDoubleA;\n");

    CDataTypeEnum enumA("mujEnumA");
    enumA
        .add("type1")
        .add("type2")
        .add("type4")
        .add("type3");

    oss.str("");
    oss << enumA;
    assert(oss.str() == "enum\n{\n    type1,\n    type2,\n    type4,\n    type3\n} mujEnumA;\n");

    // Check CDataType size
    assert(intA.getSize() == 4);
    assert(doubleA.getSize() == 8);
    assert(enumA.getSize() == 4);

    // Check CDataType comparison
    assert(intA == intB);
    assert(doubleA != intB);
    assert(!(doubleA == intB));
    assert(intA != enumA);
    assert(enumA != intA);

    CDataTypeStruct structA = CDataTypeStruct().addField("m_Length", CDataTypeInt());

    CDataTypeStruct a = CDataTypeStruct()
                            .addField("m_Length", CDataTypeInt())
                            .addField("m_Status", CDataTypeEnum()
                                                      .add("NEW")
                                                      .add("FIXED")
                                                      .add("BROKEN")
                                                      .add("DEAD"))
                            .addField("m_Ratio", CDataTypeDouble());

    CDataTypeStruct b = CDataTypeStruct()
                            .addField("m_Length", CDataTypeInt())
                            .addField("m_Status", CDataTypeEnum()
                                                      .add("NEW")
                                                      .add("FIXED")
                                                      .add("BROKEN")
                                                      .add("READY"))
                            .addField("m_Ratio", CDataTypeDouble());

    CDataTypeStruct c = CDataTypeStruct()
                            .addField("m_First", CDataTypeInt())
                            .addField("m_Second", CDataTypeEnum()
                                                      .add("NEW")
                                                      .add("FIXED")
                                                      .add("BROKEN")
                                                      .add("DEAD"))
                            .addField("m_Third", CDataTypeDouble());

    CDataTypeStruct d = CDataTypeStruct()
                            .addField("m_Length", CDataTypeInt())
                            .addField("m_Status", CDataTypeEnum()
                                                      .add("NEW")
                                                      .add("FIXED")
                                                      .add("BROKEN")
                                                      .add("DEAD"))
                            .addField("m_Ratio", CDataTypeInt());

    assert(whitespaceMatch(a, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Status;\n"
                              "  double m_Ratio;\n"
                              "}"));

    assert(whitespaceMatch(b, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    READY\n"
                              "  } m_Status;\n"
                              "  double m_Ratio;\n"
                              "}"));

    assert(whitespaceMatch(c, "struct\n"
                              "{\n"
                              "  int m_First;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Second;\n"
                              "  double m_Third;\n"
                              "}"));

    assert(whitespaceMatch(d, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Status;\n"
                              "  int m_Ratio;\n"
                              "}"));

    assert(a != b);
    assert(a == c);
    assert(a != d);

    assert(a.field("m_Status") == CDataTypeEnum()
                                      .add("NEW")
                                      .add("FIXED")
                                      .add("BROKEN")
                                      .add("DEAD"));

    assert(a.field("m_Status") != CDataTypeEnum()
                                      .add("NEW")
                                      .add("BROKEN")
                                      .add("FIXED")
                                      .add("DEAD"));

    assert(a != CDataTypeInt());
    assert(whitespaceMatch(a.field("m_Status"), "enum\n"
                                                "{\n"
                                                "  NEW,\n"
                                                "  FIXED,\n"
                                                "  BROKEN,\n"
                                                "  DEAD\n"
                                                "}"));

    CDataTypeStruct aOld = a;
    b.addField("m_Other", CDataTypeDouble());

    a.addField("m_Sum", CDataTypeInt());

    assert(a != aOld);
    assert(a != c);
    assert(aOld == c);
    assert(whitespaceMatch(a, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Status;\n"
                              "  double m_Ratio;\n"
                              "  int m_Sum;\n"
                              "}"));

    assert(whitespaceMatch(b, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    READY\n"
                              "  } m_Status;\n"
                              "  double m_Ratio;\n"
                              "  double m_Other;\n"
                              "}"));

    c.addField("m_Another", a.field("m_Status"));

    assert(whitespaceMatch(c, "struct\n"
                              "{\n"
                              "  int m_First;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Second;\n"
                              "  double m_Third;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Another;\n"
                              "}"));

    d.addField("m_Another", a.field("m_Ratio"));

    assert(whitespaceMatch(d, "struct\n"
                              "{\n"
                              "  int m_Length;\n"
                              "  enum\n"
                              "  {\n"
                              "    NEW,\n"
                              "    FIXED,\n"
                              "    BROKEN,\n"
                              "    DEAD\n"
                              "  } m_Status;\n"
                              "  int m_Ratio;\n"
                              "  double m_Another;\n"
                              "}"));

    assert(a.getSize() == 20);
    assert(b.getSize() == 24);
    try
    {
        a.addField("m_Status", CDataTypeInt());
        assert("addField: missing exception!" == nullptr);
    }
    catch (const invalid_argument &e)
    {
        assert(e.what() == "Duplicate field: m_Status"sv);
    }

    try
    {
        cout << a.field("m_Fail") << endl;
        assert("field: missing exception!" == nullptr);
    }
    catch (const invalid_argument &e)
    {
        assert(e.what() == "Unknown field: m_Fail"sv);
    }

    try
    {
        CDataTypeEnum en;
        en.add("FIRST").add("SECOND").add("FIRST");
        assert("add: missing exception!" == nullptr);
    }
    catch (const invalid_argument &e)
    {
        assert(e.what() == "Duplicate enum value: FIRST"sv);
    }

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */