/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ArrayTest.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ArrayTest );

namespace fwData
{
namespace ut
{

//-----------------------------------------------------------------------------

void ArrayTest::setUp()
{
    // Set up context before running a test.

}

//-----------------------------------------------------------------------------

void ArrayTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ArrayTest::allocation()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwDataTools::helper::Array arrayHelper(array);

    CPPUNIT_ASSERT(array->empty());
    CPPUNIT_ASSERT(arrayHelper.getBuffer() == NULL);
    CPPUNIT_ASSERT(array->getSize().empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), array->getSizeInBytes());

    const size_t NB_COMPONENT = 2;
    ::fwData::Array::SizeType size = {10, 100};

    array->resize("uint32", size, NB_COMPONENT, true);
    CPPUNIT_ASSERT(arrayHelper.getBuffer() != NULL);
    CPPUNIT_ASSERT(!array->empty());

    CPPUNIT_ASSERT_EQUAL(size.size(), array->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(size[0], array->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(size[1], array->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(NB_COMPONENT, array->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(true, array->getIsBufferOwner());
    {
        ::fwData::Array::OffsetType stride = {8, 80};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT(array->getType().string() == "uint32");
    CPPUNIT_ASSERT(array->getType().sizeOf() == 4);

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getNumberOfComponents());
    CPPUNIT_ASSERT(  array->empty() );
    CPPUNIT_ASSERT(  arrayHelper.getBuffer() == NULL );

    char* buffer = new char[1000];

    for (int i = 0; i < 1000; i++)
    {
        buffer[i] = i;
    }

    // array->resize(::fwTools::Type::create("uint8"), size, 1);
    // array->setBuffer(buffer);

    arrayHelper.setBuffer(buffer, false, ::fwTools::Type::create("uint8"), size, 1);

    CPPUNIT_ASSERT_EQUAL(  (size_t)1, array->getBufferOffset({1, 0}, 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)1, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)1*10*100, array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {1, 10};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  buffer[0], *(arrayHelper.getItem< char >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  buffer[10], *(arrayHelper.getItem< char >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  buffer[999], *(arrayHelper.getItem< char >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  buffer[326], *(arrayHelper.getItem< char >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  buffer[947], *(arrayHelper.getItem< char >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  buffer[238], *(arrayHelper.getItem< char >({8, 23})));
    CPPUNIT_ASSERT_EQUAL(false, array->getIsBufferOwner());

    array->clear();

    delete[] buffer;
}

//-----------------------------------------------------------------------------

void ArrayTest::resize()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwDataTools::helper::Array arrayHelper(array);

    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size {10, 100};

    array->resize("uint32", size, NB_COMPONENT, true);

    // CPPUNIT_ASSERT(array->getBuffer() != NULL);
    // CPPUNIT_ASSERT(array->begin<unsigned int>() != NULL);

    unsigned int count = 0;
    unsigned int* iter = arrayHelper.begin<unsigned int>();

    // CPPUNIT_ASSERT(iter != NULL);
    for (; iter != arrayHelper.end<unsigned int>(); ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getBufferOffset({1, 0}, 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({8, 23})));

    ::fwData::Array::SizeType newSize = {100, 10};

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getBufferOffset({1, 0}, 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 400};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({10, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({99, 9})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({26, 3})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({47, 9})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({38, 2})));

    newSize.clear();
    newSize = {25, 40};

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 100};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({10, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({24, 39})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({1, 13})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({22, 37})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({13, 9})));

    newSize.clear();
    newSize = {100};

    const size_t nbComponant = 10;
    array->resize(newSize, nbComponant, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)40, array->getElementSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {40};
        CPPUNIT_ASSERT(array->getStrides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0}, 10)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({99}, 9)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({32}, 6)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({94}, 7)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({23}, 8)));

    newSize.clear();
    newSize = {10, 100};

    array->resize("uint16", newSize, 2, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*100*10*2, array->getSizeInBytes());
    {
        ::fwData::Array::OffsetType stride = {4, 40};
        CPPUNIT_ASSERT(array->getStrides() == stride );
    }
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({8, 23})));
}

//-----------------------------------------------------------------------------

void ArrayTest::reallocate()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwDataTools::helper::Array arrayHelper(array);

    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size = {10, 100};

    array->resize("uint32", size, NB_COMPONENT, true);
    // CPPUNIT_ASSERT(array->getBuffer() != NULL);
    // CPPUNIT_ASSERT(array->begin<unsigned int>() != NULL);

    unsigned int count = 0;
    unsigned int* iter = arrayHelper.begin<unsigned int>();
    for (; iter != arrayHelper.end<unsigned int>(); ++iter)
    {
        *iter = count++;
    }
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({8, 23})));

    ::fwData::Array::SizeType newSize = {100, 100};

    array->resize(newSize, NB_COMPONENT, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*100*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({10, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({99, 9})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({26, 3})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({47, 9})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({38, 2})));

    unsigned int value = 1859;
    arrayHelper.setItem({50, 90}, &value);
    CPPUNIT_ASSERT_EQUAL(  value, *(arrayHelper.getItem< unsigned int >({50, 90})));

    unsigned int value2 = 25464;
    arrayHelper.setItem({99, 99}, &value2);
    CPPUNIT_ASSERT_EQUAL(  value2, *(arrayHelper.getItem< unsigned int >({99, 99})));

    array->resize("uint32", newSize, 2, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*100*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({5, 0}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({99, 4}, 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({63, 1}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({73, 4}, 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({19, 1}, 0)));

    CPPUNIT_ASSERT_EQUAL(  value, *(arrayHelper.getItem< unsigned int >({25, 45}, 0)));

    CPPUNIT_ASSERT_EQUAL(  value2, *(arrayHelper.getItem< unsigned int >({99, 49}, 1)));

    unsigned int value3 = 45643;
    arrayHelper.setItem({35, 48}, 0, &value3);
    CPPUNIT_ASSERT_EQUAL(  value3, *(arrayHelper.getItem< unsigned int >({35, 48}, 0)));

    unsigned int value4 = 16165;
    arrayHelper.setItem({99, 99}, 1, &value4);
    CPPUNIT_ASSERT_EQUAL(  value4, *(arrayHelper.getItem< unsigned int >({99, 99}, 1)));

    newSize.clear();
    newSize = {10, 100};

    array->resize(newSize, NB_COMPONENT, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({8, 23})));

    array->setNumberOfComponents(2);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({5, 0}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(arrayHelper.getItem< unsigned int >({9, 49}, 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({3, 16}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(arrayHelper.getItem< unsigned int >({3, 47}, 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({9, 11}, 0)));

    array->setType(::fwTools::Type::create("uint16"));
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*2, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*10*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)0, *(arrayHelper.getItem< std::uint16_t >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)10, *(arrayHelper.getItem< std::uint16_t >({0, 1})));
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)999, *(arrayHelper.getItem< std::uint16_t >({9, 99})));
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)326, *(arrayHelper.getItem< std::uint16_t >({6, 32})));
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)947, *(arrayHelper.getItem< std::uint16_t >({7, 94})));
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)238, *(arrayHelper.getItem< std::uint16_t >({8, 23})));

    array->setNumberOfComponents(1);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(arrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(arrayHelper.getItem< unsigned int >({0, 2}, 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(arrayHelper.getItem< unsigned int >({2, 65})));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(arrayHelper.getItem< unsigned int >({6, 47}, 0)));

    std::uint16_t val;
    arrayHelper.getItem({2, 65}, &val);
    CPPUNIT_ASSERT_EQUAL(  (std::uint16_t)326, val);

    char* charValue = arrayHelper.getBufferPtr({6, 47}, 0, array->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL( (unsigned int)238, *(reinterpret_cast<unsigned int*>(charValue)));

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

void ArrayTest::copy()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwDataTools::helper::Array arrayHelper(array);

    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size = {10, 100};

    array->resize("uint32", size, NB_COMPONENT, true);
    CPPUNIT_ASSERT(arrayHelper.getBuffer() != NULL);
    CPPUNIT_ASSERT(arrayHelper.begin<unsigned int>() != NULL);

    unsigned int count = 0;
    unsigned int* iter = arrayHelper.begin<unsigned int>();
    for (; iter != arrayHelper.end<unsigned int>(); ++iter)
    {
        *iter = count++;
    }

    ::fwData::Array::sptr deepCopyArray;
    deepCopyArray = ::fwData::Object::copy(array);

    ::fwDataTools::helper::Array deepCopyArrayHelper(deepCopyArray);

    // check deepCopy
    CPPUNIT_ASSERT_EQUAL(  array->getBufferOffset({1, 0}, 0, 4),
                           deepCopyArray->getBufferOffset({1, 0}, 0, 4));
    CPPUNIT_ASSERT_EQUAL(  array->getElementSizeInBytes(), deepCopyArray->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  array->getSizeInBytes(), deepCopyArray->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == deepCopyArray->getStrides());
    CPPUNIT_ASSERT(array->getSize() == deepCopyArray->getSize());
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({0, 0})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({0, 0})));
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({0, 1})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({0, 1})));
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({9, 99})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({9, 99})));
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({6, 32})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({6, 32})));
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({7, 94})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({7, 94})));
    CPPUNIT_ASSERT_EQUAL( *(arrayHelper.getItem< unsigned int >({8, 23})),
                          *(deepCopyArrayHelper.getItem< unsigned int >({8, 23})));
    CPPUNIT_ASSERT_EQUAL( true, deepCopyArray->getIsBufferOwner());
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
