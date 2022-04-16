#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
using namespace std;
#endif /* __PROGTEST__ */

class CFile
{

private:
    typedef unsigned char byte;

    template <typename T>
    struct TArray
    {

        T *m_data;
        size_t m_size;

        TArray()
            : m_data(nullptr), m_size(0) {}

        TArray(size_t size)
            : m_size(size)
        {
            m_data = new T[m_size];
        }

        TArray(T *data, size_t size)
            : m_data(data), m_size(size) {}

        ~TArray()
        {
            if (m_data != nullptr)
                delete[] m_data;
        }

        /**
         * @brief Resize current array or allocate new if nullptr
         *
         * @param[in] newSize
         */
        void resize(size_t newSize)
        {
            // Allocate array with new size
            auto newData = new T[newSize];

            // Check if current array already exists
            if (m_data != nullptr)
            {
                if (newSize < m_size)
                    // If newSize is smaller, shrink the array (do not copy remaining values of m_data)
                    memcpy(newData, m_data, newSize);
                else
                    // If newSize is bigger, grow the array (do not copy values past m_data)
                    memcpy(newData, m_data, m_size);

                // Delete previously allocated m_data
                delete[] m_data;
            }

            m_data = newData;

            // Set new size
            m_size = newSize;
        }

        T &operator[](size_t index)
        {
            if (index >= m_size)
                throw out_of_range("Index is out of range of TArray");

            return m_data[index];
        }
    };

    /**
     * @brief Simillar to shared_ptr, keeps reference counter to heap allocated bytes array
     *
     */
    struct TDataHandler
    {
        unsigned int *m_refCount;
        TArray<byte> *m_tarray;

        /**
         * @brief Construct a new TDataHandler object
         *
         */
        TDataHandler()
            : m_refCount(new unsigned int(1)),
              m_tarray(new TArray<byte>()) {}

        /**
         * @brief Copy constructor
         *
         * Create a new shallow copy of 'rhs', increment reference counter
         *
         * @param rhs
         */
        TDataHandler(const TDataHandler &rhs)
            : m_refCount(rhs.m_refCount),
              m_tarray(rhs.m_tarray)
        {
            // Increment refCount
            (*rhs.m_refCount)++;
        }

        /**
         * @brief Destroy the TDataHandler object
         *
         * Decrement the reference counter, free/delete heap allocated variables when no ones is using this object anymore
         *
         */
        ~TDataHandler()
        {
            // Decrement refCount
            (*m_refCount)--;

            if (*m_refCount <= 0)
            {
                delete m_refCount;
                delete m_tarray;
            }
        }

        /**
         * @brief Create a deep copy by allocating new bytes array on heap, copy original values, reset reference counter
         *
         * @return TDataHandler
         */
        TDataHandler deepCopy()
        {
            // Create a dummy object
            TDataHandler copy;

            // Allocate a new bytes array with same size as original
            copy.m_tarray->m_data = new byte[m_tarray->m_size];

            // Copy all values to new array
            memcpy(copy.m_tarray->m_data, m_tarray->m_data, m_tarray->m_size);

            // Copy array size from original object
            copy.m_tarray->m_size = m_tarray->m_size;

            return copy;
        }

        /**
         * @brief Copy assignement operator
         *
         * Copy values from 'rhs 'to this object, free/delete previous values, increment reference counter
         *
         * @param rhs
         * @return TDataHandler&
         */
        TDataHandler &operator=(const TDataHandler &rhs)
        {
            // If we're assigning the same object to itself, do nothing
            if (&rhs == this)
                return *this;

            // Increment the reference counter
            (*rhs.m_refCount)++;

            // Delete previous values
            this->~TDataHandler();

            // Copy new values from 'rhs'
            m_refCount = rhs.m_refCount;
            m_tarray = rhs.m_tarray;

            return *this;
        }
    };

    TDataHandler m_data;
    uint32_t m_position;
    CFile *m_previous;

public:
    /**
     * @brief Construct a new CFile object
     *
     */
    CFile()
        : m_position(0), m_previous(nullptr) {}

    /**
     * @brief Destroy the CFile object
     *
     */
    ~CFile()
    {
        delete m_previous;
    }

    /**
     * @brief Copy constructor
     *
     * Create a new copy of 'rhs', keeping previous versions separately
     * (new copy won't affect previous versions of 'rhs')
     *
     * @param rhs
     */
    CFile(const CFile &rhs)
    {
        m_data = rhs.m_data;
        m_position = rhs.m_position;

        // Set to nullptr, so we don't call CFile's operator= with next assignement
        m_previous = nullptr;

        if (rhs.m_previous != nullptr)
            m_previous = new CFile(*rhs.m_previous);
    }

    /**
     * @brief Copy assignement operator
     *
     * Copy values from 'rhs 'to this object, free/delete all previous versions,
     * we're making separate copies of them recursively
     *
     * @param rhs
     * @return CFile&
     */
    CFile &operator=(const CFile &rhs)
    {
        if (&rhs == this)
            return *this;

        CFile *toDelete = m_previous;
        // delete m_previous;

        m_data = rhs.m_data;
        m_position = rhs.m_position;
        m_previous = nullptr;

        if (rhs.m_previous != nullptr)
            m_previous = new CFile(*rhs.m_previous);

        delete toDelete;

        return *this;
    }

    /**
     * @brief Move cursor by 'offset' positions if possible
     *
     * @param[in] offset
     * @return true If cursor was moves succesfully
     * @return false If offset was too big, cursor can't move over file size
     */
    bool seek(uint32_t offset)
    {
        if (offset > m_data.m_tarray->m_size)
            return false;

        m_position = offset;
        return true;
    }

    /**
     * @brief Read 'bytes' number of bytes from CFile and copy them into 'dst'
     *
     * @param[out] dst Pointer to byte array
     * @param[in] bytes Number of bytes to read
     * @return uint32_t Count of read bytes
     */
    uint32_t read(uint8_t *dst, uint32_t bytes)
    {
        // If desired bytes count is larger then dataSize, set to max available size
        if (m_position + bytes >= m_data.m_tarray->m_size)
            bytes = m_data.m_tarray->m_size - m_position;

        // Iterate over data and copy to dst
        for (size_t i = 0; i < bytes; i++)
            dst[i] = (*m_data.m_tarray)[m_position++];

        return bytes;
    }

    /**
     * @brief Write 'bytes' number of bytes from 'src' into CFile
     *
     * @param[in] src Pointer to byte array
     * @param bytes Number of bytes to write
     * @return uint32_t Count of written bytes
     */
    uint32_t write(const uint8_t *src, uint32_t bytes)
    {
        if (bytes <= 0)
            return 0u;

        // If reference counter is bigger than one, we shouldn't modify the shared file, instead create a new deep copy
        if (*(m_data.m_refCount) > 1)
            m_data = m_data.deepCopy();

        // If we're writing past file size, resize the file
        if (m_position + bytes >= m_data.m_tarray->m_size)
            m_data.m_tarray->resize(m_position + bytes);

        // Iterate over data and copy to the bytes array
        for (size_t i = 0; i < bytes; i++)
            (*m_data.m_tarray)[m_position++] = src[i];

        return bytes;
    }

    /**
     * @brief Shrink the file size to current cursor's position
     *
     */
    void truncate(void)
    {
        // If reference counter is bigger than one, we shouldn't modify the shared file, instead create a new deep copy
        if (*(m_data.m_refCount) > 1)
            m_data = m_data.deepCopy();

        m_data.m_tarray->resize(m_position);
    }

    /**
     * @brief Return current file size
     *
     * @return uint32_t
     */
    uint32_t fileSize(void) const
    {
        return m_data.m_tarray->m_size;
    }

    /**
     * @brief Add new version of the file
     *
     * Adds current file as its own copy, so when we modify it, we have the previous version before modification
     *
     */
    void addVersion(void)
    {
        // Delete previous versions, we're gonna copy them as separate with CFile()'s copy constructor
        CFile *toDelete = m_previous;

        m_previous = new CFile(*this);

        delete toDelete;
    }

    /**
     * @brief Make previous version as the current version of the file
     *
     * @return true If succesfull
     * @return false If there's no previous version present
     */
    bool undoVersion(void)
    {
        if (m_previous == nullptr)
            return false;

        *this = *m_previous;
        return true;
    }
};

#ifndef __PROGTEST__
bool writeTest(CFile &x,
               const initializer_list<uint8_t> &data,
               uint32_t wrLen)
{
    return x.write(data.begin(), data.size()) == wrLen;
}

bool readTest(CFile &x,
              const initializer_list<uint8_t> &data,
              uint32_t rdLen)
{
    uint8_t tmp[100];
    uint32_t idx = 0;

    if (x.read(tmp, rdLen) != data.size())
        return false;
    for (auto v : data)
        if (tmp[idx++] != v)
            return false;
    return true;
}

int main(void)
{
    CFile f0;

    assert(writeTest(f0, {10, 20, 30}, 3));
    assert(f0.fileSize() == 3);
    assert(writeTest(f0, {60, 70, 80}, 3));
    assert(f0.fileSize() == 6);
    assert(f0.seek(2));
    assert(writeTest(f0, {5, 4}, 2));
    assert(f0.fileSize() == 6);
    assert(f0.seek(1));
    assert(readTest(f0, {20, 5, 4, 70, 80}, 7));
    assert(f0.seek(3));
    f0.addVersion();
    assert(f0.seek(6));
    assert(writeTest(f0, {100, 101, 102, 103}, 4));
    f0.addVersion();
    assert(f0.seek(5));
    CFile f1(f0);
    f0.truncate();
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70}, 20));
    assert(f0.undoVersion());
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70, 80, 100, 101, 102, 103}, 20));
    assert(f0.undoVersion());
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70, 80}, 20));
    assert(!f0.seek(100));
    assert(writeTest(f1, {200, 210, 220}, 3));
    assert(f1.seek(0));
    assert(readTest(f1, {10, 20, 5, 4, 70, 200, 210, 220, 102, 103}, 20));
    assert(f1.undoVersion());
    assert(f1.undoVersion());
    assert(readTest(f1, {4, 70, 80}, 20));
    assert(!f1.undoVersion());
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */