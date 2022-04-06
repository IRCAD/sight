/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include <core/runtime/EConfigurationElement.hpp>

#include <data/Image.hpp>

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::itk
{

namespace ut
{

class ImageReaderWriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ImageReaderWriterTest);
CPPUNIT_TEST(inrReadWriteTest);
CPPUNIT_TEST(inrStressTest);
CPPUNIT_TEST(niftiReadTest);
CPPUNIT_TEST(niftiWriteTest);
CPPUNIT_TEST(jpegWriteTest);
CPPUNIT_TEST(inrReadJpegWriteTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void inrReadWriteTest();
    void inrStressTest();
    void niftiReadTest();
    void niftiWriteTest();
    void jpegWriteTest();
    void inrReadJpegWriteTest();

private:

    void inrStressTestWithType(core::Type type, int nbTest);
    void inrReadWriteCheck(data::Image::sptr image);
    void niftiReadCheck(
        const data::Image::sptr& imageToTest,
        const core::Type& expectedType,
        const std::size_t expectedDim,
        const data::Image::Spacing& expectedSpacing,
        const data::Image::Origin& expectedOrigin,
        const data::Image::Size& expectedSize
    );
};

} //namespace ut

} //namespace sight::io::itk
