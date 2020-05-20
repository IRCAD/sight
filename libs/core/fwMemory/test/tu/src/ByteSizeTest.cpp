/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <limits>

#include <fwMemory/ByteSize.hpp>

#include "ByteSizeTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMemory::ut::ByteSizeTest );

namespace fwMemory
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
        ::fwMemory::ByteSize bsize;
        std::string refIEC("0 Bytes");
        std::string refSI("0 Bytes");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }

    {
        ::fwMemory::ByteSize::SizeType size = 1;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::KB);
        std::string refIEC("1000 Bytes");
        std::string refSI("1 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwMemory::ByteSize::SizeType size = 1;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::KiB);
        std::string refIEC("1 KiB");
        std::string refSI("1.024 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwMemory::ByteSize::SizeType size = 1;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::GB);
        std::string refIEC("953.674 MiB");
        std::string refSI("1 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        ::fwMemory::ByteSize::SizeType size = 1;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::GiB);
        std::string refIEC("1 GiB");
        std::string refSI("1.07374 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }



    {
        double size = 1.5;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::GiB);
        std::string refIEC("1.5 GiB");
        std::string refSI("1.61061 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }    {
        double size = 1.5;
        ::fwMemory::ByteSize bsize(size, ::fwMemory::ByteSize::TB);
        std::string refIEC("1.36424 TiB");
        std::string refSI("1.5 TB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }





    {
        std::string size("1.42 MiB");
        ::fwMemory::ByteSize bsize(size);
        std::string refIEC("1.42 MiB");
        std::string refSI("1.48898 MB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.32 GB");
        ::fwMemory::ByteSize bsize(size);
        std::string refIEC("1.22935 GiB");
        std::string refSI("1.32 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.123GB");
        ::fwMemory::ByteSize bsize(size);
        std::string refIEC("1.04588 GiB");
        std::string refSI("1.123 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(::fwMemory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("  1.42   MiB ");
        ::fwMemory::ByteSize bsize(size);
        ::fwMemory::ByteSize::SizeType refSize    = 1488977;
        ::fwMemory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("1.42 TB  ");
        ::fwMemory::ByteSize bsize(size);
        ::fwMemory::ByteSize::SizeType refSize    = 1420000000000LL;
        ::fwMemory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("  1.42 PiB");
        ::fwMemory::ByteSize bsize(size);
        ::fwMemory::ByteSize::SizeType refSize    = 1598777867716526LL;
        ::fwMemory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }

    {
        std::string size("1.66MiB");
        ::fwMemory::ByteSize bsize(size);
        ::fwMemory::ByteSize::SizeType refSize    = 1740636;
        ::fwMemory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
}

} // namespace ut
} // namespace fwMemory

