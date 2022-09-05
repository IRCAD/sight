/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ByteSizeTest.hpp"

#include <core/memory/ByteSize.hpp>
#include <core/memory/exception/BadCast.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::ByteSizeTest);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void ByteSizeTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ByteSizeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ByteSizeTest::byteSizeTest()
{
    {
        core::memory::ByteSize bsize;
        std::string refIEC("0 Bytes");
        std::string refSI("0 Bytes");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }

    {
        core::memory::ByteSize::SizeType size = 1;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::KB);
        std::string refIEC("1000 Bytes");
        std::string refSI("1 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        core::memory::ByteSize::SizeType size = 1;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::KiB);
        std::string refIEC("1 KiB");
        std::string refSI("1.024 KB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        core::memory::ByteSize::SizeType size = 1;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::GB);
        std::string refIEC("953.674 MiB");
        std::string refSI("1 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        core::memory::ByteSize::SizeType size = 1;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::GiB);
        std::string refIEC("1 GiB");
        std::string refSI("1.07374 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }

    {
        double size = 1.5;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::GiB);
        std::string refIEC("1.5 GiB");
        std::string refSI("1.61061 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }    {
        double size = 1.5;
        core::memory::ByteSize bsize(size, core::memory::ByteSize::TB);
        std::string refIEC("1.36424 TiB");
        std::string refSI("1.5 TB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }

    {
        std::string size("1.42 MiB");
        core::memory::ByteSize bsize(size);
        std::string refIEC("1.42 MiB");
        std::string refSI("1.48898 MB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.32 GB");
        core::memory::ByteSize bsize(size);
        std::string refIEC("1.22935 GiB");
        std::string refSI("1.32 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("1.123GB");
        core::memory::ByteSize bsize(size);
        std::string refIEC("1.04588 GiB");
        std::string refSI("1.123 GB");
        std::string resultIEC(bsize);
        std::string resultSI(bsize.getHumanReadableSize(core::memory::ByteSize::SI));
        CPPUNIT_ASSERT_EQUAL(refIEC, resultIEC);
        CPPUNIT_ASSERT_EQUAL(refSI, resultSI);
    }
    {
        std::string size("  1.42   MiB ");
        core::memory::ByteSize bsize(size);
        core::memory::ByteSize::SizeType refSize    = 1488977;
        core::memory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("1.42 TB  ");
        core::memory::ByteSize bsize(size);
        core::memory::ByteSize::SizeType refSize    = 1420000000000LL;
        core::memory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
    {
        std::string size("  1.42 PiB");
        core::memory::ByteSize bsize(size);
        core::memory::ByteSize::SizeType refSize    = 1598777867716526LL;
        core::memory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }

    {
        std::string size("1.66MiB");
        core::memory::ByteSize bsize(size);
        core::memory::ByteSize::SizeType refSize    = 1740636;
        core::memory::ByteSize::SizeType resultSize = bsize.getSize();
        CPPUNIT_ASSERT_EQUAL(refSize, resultSize);
    }
}

//------------------------------------------------------------------------------

void ByteSizeTest::invalidSizeTest()
{
    CPPUNIT_ASSERT_THROW(core::memory::ByteSize a(-1.), core::memory::exception::BadCast);
    CPPUNIT_ASSERT_THROW(core::memory::ByteSize a(std::string("-2B")), core::memory::exception::BadCast);
    core::memory::ByteSize a;
    CPPUNIT_ASSERT_THROW(a = -3., core::memory::exception::BadCast);
    CPPUNIT_ASSERT_THROW(a = std::string("-4B"), core::memory::exception::BadCast);
    CPPUNIT_ASSERT_THROW(a.setSize(-5.), core::memory::exception::BadCast);
    CPPUNIT_ASSERT_THROW(a.setSize(std::string("-6B")), core::memory::exception::BadCast);
    CPPUNIT_ASSERT_THROW(a = std::string("7A"), core::memory::exception::BadCast);
}

} // namespace sight::core::memory::ut
