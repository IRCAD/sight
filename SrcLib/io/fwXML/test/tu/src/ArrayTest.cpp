/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>
#include <fwData/Array.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>

#include "ArrayTest.hpp"

using namespace boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ArrayTest );

//------------------------------------------------------------------------------

void ArrayTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ArrayTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ArrayTest::testArray()
{
    ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() / "ArrayTest" / "Array.xml";

    ::fwData::Array::sptr array1 = this->generateArray();

    // save Array in fwXML
    ::boost::filesystem::create_directories( testFile.parent_path() );

    ::fwXML::Serializer serializer;
    ::fwXML::NeverSplitPolicy::sptr spolicy( new ::fwXML::NeverSplitPolicy() );
    serializer.setSplitPolicy( spolicy );

#if BOOST_FILESYSTEM_VERSION > 2
    ::fwXML::UniquePathPolicy::sptr pPathPolicy ( new ::fwXML::UniquePathPolicy( testFile.filename().string() ) );
#else
    ::fwXML::UniquePathPolicy::sptr pPathPolicy ( new ::fwXML::UniquePathPolicy( testFile.leaf() ) );
#endif
    serializer.setPathPolicy( pPathPolicy );

    serializer.rootFolder() = testFile.parent_path().string();
    bool doSaveSchema = false;
    serializer.serialize(array1, doSaveSchema);
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    // load Array
    ::fwData::Array::sptr array2 = ::fwData::Array::dynamicCast(serializer.deSerialize(testFile, true));

    // check Array
    CPPUNIT_ASSERT(array2);

    CPPUNIT_ASSERT_EQUAL(array1->getSizeInBytes(), array2->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(array1->getElementSizeInBytes(), array2->getElementSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(array1->getNumberOfDimensions(), array2->getNumberOfDimensions());
    CPPUNIT_ASSERT(array1->getSize() == array2->getSize());
    CPPUNIT_ASSERT_EQUAL(array1->getNumberOfComponents(), array2->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(array1->getIsBufferOwner(), array2->getIsBufferOwner());
    CPPUNIT_ASSERT(array1->getStrides() == array2->getStrides());
    CPPUNIT_ASSERT(array1->getType().string() == array2->getType().string());
    CPPUNIT_ASSERT(array1->getType().sizeOf() == array2->getType().sizeOf());

    this->compareBuffer(array1, array2);

    ::boost::filesystem::remove_all( testFile.parent_path().string() );
}

//------------------------------------------------------------------------------

void ArrayTest::compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2)
{
    CPPUNIT_ASSERT( (!buff1 && !buff2) || (buff1 && buff2));
    if(buff1)
    {
        CPPUNIT_ASSERT(buff1->getSize() == buff2->getSize());

        char *iter1 = buff1->begin<char>();
        char *iter2 = buff2->begin<char>();

        for (; iter1 != buff1->end<char>() ; ++iter1, ++iter2)
        {
            CPPUNIT_ASSERT_EQUAL(*iter1, *iter2);
        }
    }
}

//------------------------------------------------------------------------------

::fwData::Array::sptr ArrayTest::generateArray()
{
    ::fwData::Array::NewSptr array;
    const size_t NB_COMPONENT = 2;
    ::fwData::Array::SizeType size;
    size += 10,100;

    array->resize("uint32", size, NB_COMPONENT, true);
    char *iter = array->begin<char>();

    unsigned int count = 0;
    for (; iter != array->end<char>() ; ++iter)
    {
        *iter = count++;
    }
    return array;
}

//------------------------------------------------------------------------------
