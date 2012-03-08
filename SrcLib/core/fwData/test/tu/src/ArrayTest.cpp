/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <fwData/Array.hpp>
#include "ArrayTest.hpp"


using namespace boost::assign;

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
    ::fwData::Array::NewSptr array;

    CPPUNIT_ASSERT(array->getSize().empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), array->getSizeInBytes());

    const size_t NB_COMPONENT = 2;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);

    CPPUNIT_ASSERT_EQUAL(size.size(), array->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(size[0], array->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(size[1], array->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(NB_COMPONENT, array->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(true, array->getIsBufferOwner());
    CPPUNIT_ASSERT(array->getStrides() == list_of(8)(80));
    CPPUNIT_ASSERT(array->getType().string() == "uint32");
    CPPUNIT_ASSERT(array->getType().sizeOf() == 4);

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getNumberOfComponents());
    CPPUNIT_ASSERT(  array->empty() );
    CPPUNIT_ASSERT(  array->getBuffer() == NULL );

    char *buffer = new char[1000];

    for (int i=0 ; i<1000; i++)
    {
        buffer[i] = i;
    }

    array->resize(::fwTools::Type::create("uint8"), size, 1);
    array->setBuffer(buffer);
    CPPUNIT_ASSERT_EQUAL(  (size_t)1, array->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)1, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)1*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == list_of(1)(10));
    CPPUNIT_ASSERT_EQUAL(  buffer[0], *(array->getItem< char >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  buffer[10], *(array->getItem< char >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  buffer[999], *(array->getItem< char >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  buffer[326], *(array->getItem< char >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  buffer[947], *(array->getItem< char >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  buffer[238], *(array->getItem< char >(list_of(8)(23))));
    CPPUNIT_ASSERT_EQUAL(false, array->getIsBufferOwner());

    array->clear();


    delete[] buffer;
}

//-----------------------------------------------------------------------------

void ArrayTest::resize()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);

    unsigned int count = 0;
    unsigned int *iter = array->begin<unsigned int>();
    for (; iter != array->end<unsigned int>() ; ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == list_of(4)(40));
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
    CPPUNIT_ASSERT(array->getStrides() == list_of(4)(400));
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
    CPPUNIT_ASSERT(array->getStrides() == list_of(4)(100));
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
    CPPUNIT_ASSERT(array->getStrides() == list_of(40));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0), 10)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(99), 9)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(32), 6)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(94), 7)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(23), 8)));

    newSize.clear();
    newSize += 10, 100;

    array->resize("uint16", newSize, 2, false);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*100*10*2, array->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == list_of(4)(40) );
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(8)(23))));
}

//-----------------------------------------------------------------------------

void ArrayTest::reallocate()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);

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
    array->setItem(list_of(35)(48), 0, &value3);
    CPPUNIT_ASSERT_EQUAL(  value3, *(array->getItem< unsigned int >(list_of(35)(48), 0)));

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

    array->setNumberOfComponents(2);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)8, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)4*10*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(5)(0),0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)999, *(array->getItem< unsigned int >(list_of(9)(49), 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(3)(16), 0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)947, *(array->getItem< unsigned int >(list_of(3)(47), 1)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(9)(11), 0)));

    array->setType(::fwTools::Type::create("uint16"));
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*2, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*10*100*2, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)0, *(array->getItem< ::boost::uint16_t >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)10, *(array->getItem< ::boost::uint16_t >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)999, *(array->getItem< ::boost::uint16_t >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)326, *(array->getItem< ::boost::uint16_t >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)947, *(array->getItem< ::boost::uint16_t >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)238, *(array->getItem< ::boost::uint16_t >(list_of(8)(23))));

    array->setNumberOfComponents(1);
    CPPUNIT_ASSERT(newSize == array->getSize());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2, array->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (size_t)2*10*100, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)0, *(array->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)10, *(array->getItem< unsigned int >(list_of(0)(2),0)));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)326, *(array->getItem< unsigned int >(list_of(2)(65))));
    CPPUNIT_ASSERT_EQUAL(  (unsigned int)238, *(array->getItem< unsigned int >(list_of(6)(47), 0)));

    ::boost::uint16_t val;
    array->getItem(list_of(2)(65), &val);
    CPPUNIT_ASSERT_EQUAL(  (::boost::uint16_t)326, val);

    char * charValue = array->getBufferPtr(list_of(6)(47), 0, array->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL( (unsigned int)238, *(reinterpret_cast<unsigned int*>(charValue)));

    array->clear();
    CPPUNIT_ASSERT_EQUAL(  (size_t)0, array->getSizeInBytes());
}

//-----------------------------------------------------------------------------

void ArrayTest::copy()
{
    ::fwData::Array::NewSptr array;
    ::fwData::Array::NewSptr deepCopyArray;
    ::fwData::Array::NewSptr shallowCopyArray;
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);

    unsigned int count = 0;
    unsigned int *iter = array->begin<unsigned int>();
    for (; iter != array->end<unsigned int>() ; ++iter)
    {
        *iter = count++;
    }

    // check deepCopy
    deepCopyArray->deepCopy(array);
    CPPUNIT_ASSERT_EQUAL(  array->getBufferOffset(list_of(1)(0), 0, 4), deepCopyArray->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  array->getElementSizeInBytes(), deepCopyArray->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  array->getSizeInBytes(), deepCopyArray->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == deepCopyArray->getStrides());
    CPPUNIT_ASSERT(array->getSize() == deepCopyArray->getSize());
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(0)(0))) , *(deepCopyArray->getItem< unsigned int >(list_of(0)(0))));
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(0)(1))) , *(deepCopyArray->getItem< unsigned int >(list_of(0)(1))));
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(9)(99))), *(deepCopyArray->getItem< unsigned int >(list_of(9)(99))));
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(6)(32))), *(deepCopyArray->getItem< unsigned int >(list_of(6)(32))));
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(7)(94))), *(deepCopyArray->getItem< unsigned int >(list_of(7)(94))));
    CPPUNIT_ASSERT_EQUAL( *(array->getItem< unsigned int >(list_of(8)(23))), *(deepCopyArray->getItem< unsigned int >(list_of(8)(23))));
    CPPUNIT_ASSERT_EQUAL( true , deepCopyArray->getIsBufferOwner());

    //check shallowCopy
    shallowCopyArray->shallowCopy(array);
    CPPUNIT_ASSERT_EQUAL(  array->getBufferOffset(list_of(1)(0), 0, 4), shallowCopyArray->getBufferOffset(list_of(1)(0), 0, 4));
    CPPUNIT_ASSERT_EQUAL(  array->getElementSizeInBytes(), shallowCopyArray->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(  array->getSizeInBytes(), shallowCopyArray->getSizeInBytes());
    CPPUNIT_ASSERT(array->getStrides() == shallowCopyArray->getStrides());
    CPPUNIT_ASSERT(array->getSize() == shallowCopyArray->getSize());
    CPPUNIT_ASSERT_EQUAL( array->getBuffer(), shallowCopyArray->getBuffer());
    CPPUNIT_ASSERT_EQUAL( false , shallowCopyArray->getIsBufferOwner());
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
