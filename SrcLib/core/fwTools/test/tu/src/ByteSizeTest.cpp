/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <limits>

#include <fwTools/ByteSize.hpp>

#include "ByteSizeTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::ByteSizeTest );

namespace fwTools
{
namespace ut
{

void ByteSizeTest::setUp()
{
    // Set up context before running a test.
}

void ByteSizeTest::tearDown()
{
    // Clean up after the test run.
}

void ByteSizeTest::byteSizeTest()
{
    {
        ::fwTools::ByteSize bsize;
        std::string refIEC("0 Bytes");
        std::string refSI("0 Bytes");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }

    {
        ::fwTools::ByteSize::SizeType size = 1;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::KB);
        std::string refIEC("1000 Bytes");
        std::string refSI("1 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwTools::ByteSize::SizeType size = 1;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::KiB);
        std::string refIEC("1 KiB");
        std::string refSI("1.024 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwTools::ByteSize::SizeType size = 1;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::GB);
        std::string refIEC("953.674 MiB");
        std::string refSI("1 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwTools::ByteSize::SizeType size = 1;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::GiB);
        std::string refIEC("1 GiB");
        std::string refSI("1.07374 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }



    {
        double size = 1.5;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::GiB);
        std::string refIEC("1.5 GiB");
        std::string refSI("1.61061 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }    {
        double size = 1.5;
        ::fwTools::ByteSize bsize(size, ::fwTools::ByteSize::TB);
        std::string refIEC("1.36424 TiB");
        std::string refSI("1.5 TB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }





    {
        std::string size("1.42 MiB");
        ::fwTools::ByteSize bsize(size);
        std::string refIEC("1.42 MiB");
        std::string refSI("1.48898 MB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.32 GB");
        ::fwTools::ByteSize bsize(size);
        std::string refIEC("1.22935 GiB");
        std::string refSI("1.32 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.123GB");
        ::fwTools::ByteSize bsize(size);
        std::string refIEC("1.04588 GiB");
        std::string refSI("1.123 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwTools::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("  1.42   MiB ");
        ::fwTools::ByteSize bsize(size);
        ::fwTools::ByteSize::SizeType refSize = 1488977 ;
        ::fwTools::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("1.42 TB  ");
        ::fwTools::ByteSize bsize(size);
        ::fwTools::ByteSize::SizeType refSize = 1420000000000LL ;
        ::fwTools::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("  1.42 PiB");
        ::fwTools::ByteSize bsize(size);
        ::fwTools::ByteSize::SizeType refSize = 1598777867716526LL ;
        ::fwTools::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }

    {
        std::string size("1.66MiB");
        ::fwTools::ByteSize bsize(size);
        ::fwTools::ByteSize::SizeType refSize = 1740636 ;
        ::fwTools::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
}

} // namespace ut
} // namespace fwTools

