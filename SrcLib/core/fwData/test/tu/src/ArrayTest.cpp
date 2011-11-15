/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include "fwData/Array.hpp"
#include "ArrayTest.hpp"


using namespace boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ArrayTest );

void ArrayTest::setUp()
{
    // Set up context before running a test.

}
void ArrayTest::tearDown()
{
    // Clean up after the test run.
}

void ArrayTest::allocation()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 2;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->allocate("uint32", size, NB_COMPONENT);

    CPPUNIT_ASSERT_EQUAL(size.size(), array->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(size[0], array->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(size[1], array->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(NB_COMPONENT, array->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(true, array->getIsBufferOwner());
    CPPUNIT_ASSERT(array->getStrides() == list_of(8)(80));
}

void ArrayTest::resize()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->allocate("uint32", size, NB_COMPONENT);

    unsigned int count = 0;
    unsigned int *iter = array->begin<unsigned int>();
    for (; iter != array->end<unsigned int>() ; ++iter)
    {
        *iter = count++;
    }

    unsigned int v = *( array->getItem< unsigned int >(list_of(0)(0)));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(8)(23))));

    ::fwData::Array::SizeType newSize;
    newSize += 100,10;

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(10)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(99)(9))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(26)(3))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(47)(9))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(38)(2))));

    newSize.clear();
    newSize += 25,40;

    array->resize(newSize);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(10)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(24)(39))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(1)(13))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(22)(37))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(13)(9))));

    newSize.clear();
    newSize += 100;

    const size_t nbComponant = 10;
    array->resize(newSize, nbComponant, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)40, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0), 10)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(99), 9)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(32), 6)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(94), 7)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(23), 8)));
}

void ArrayTest::reallocate()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->allocate("uint32", size, NB_COMPONENT);

    unsigned int count = 0;
    unsigned int *iter = array->begin<unsigned int>();
    for (; iter != array->end<unsigned int>() ; ++iter)
    {
        *iter = count++;
    }
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(8)(23))));

    ::fwData::Array::SizeType newSize;
    newSize += 100,100;

    array->resize(newSize, NB_COMPONENT, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*100*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(10)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(99)(9))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(26)(3))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(47)(9))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(38)(2))));

    unsigned int value = 1859;
    array->setItem(list_of(50)(90), &value);
    CPPUNIT_ASSERT_EQUAL(  value, *(array->getItem< unsigned int >(list_of(50)(90))));

    unsigned int value2 = 25464;
    array->setItem(list_of(99)(99), &value2);
    CPPUNIT_ASSERT_EQUAL(  value2, *(array->getItem< unsigned int >(list_of(99)(99))));

    array->resize("uint32", newSize, 2, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*100*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(5)(0), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(99)(4), 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(63)(1), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(73)(4), 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(19)(1), 0)));

    CPPUNIT_ASSERT_EQUAL(  value, *(array->getItem< unsigned int >(list_of(25)(45), 0)));

    CPPUNIT_ASSERT_EQUAL(  value2, *(array->getItem< unsigned int >(list_of(99)(49), 1)));

    unsigned int value3 = 45643;
    array->setItem(list_of(35)(48), 1, &value3);
    CPPUNIT_ASSERT_EQUAL(  value3, *(array->getItem< unsigned int >(list_of(35)(48), 1)));

    unsigned int value4 = 16165;
    array->setItem(list_of(99)(99), 1, &value4);
    CPPUNIT_ASSERT_EQUAL(  value4, *(array->getItem< unsigned int >(list_of(99)(99), 1)));


    newSize.clear();
    newSize += 10, 100;

    array->resize(newSize, NB_COMPONENT, true);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(8)(23))));
}


