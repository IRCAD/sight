/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "DicomDataToolsTest.hpp"

#include <core/type.hpp>

#include <io/dicom/helper/DicomDataTools.hpp>

#include <gdcmPixelFormat.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::DicomDataToolsTest);

namespace sight::io::dicom::helper::ut
{

using helper = helper::DicomDataTools;

//------------------------------------------------------------------------------

void DicomDataToolsTest::getPixelTypeTest()
{
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT8), helper::getPixelType(core::type::UINT8));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT8), helper::getPixelType(core::type::INT8));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT16), helper::getPixelType(core::type::UINT16));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT16), helper::getPixelType(core::type::INT16));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT32), helper::getPixelType(core::type::UINT32));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT32), helper::getPixelType(core::type::INT32));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::FLOAT32), helper::getPixelType(core::type::FLOAT));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::FLOAT64), helper::getPixelType(core::type::DOUBLE));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UNKNOWN), helper::getPixelType(core::type::NONE));
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::getPhotometricInterpretationTest()
{
    using gdcmPI = gdcm::PhotometricInterpretation;

    auto image = std::make_shared<data::Image>();

    image->resize({1}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(gdcmPI(gdcmPI::MONOCHROME2), helper::getPhotometricInterpretation(image));

    image->resize({1}, core::type::UINT8, data::Image::PixelFormat::RG);
    CPPUNIT_ASSERT_EQUAL(gdcmPI(gdcmPI::UNKNOWN), helper::getPhotometricInterpretation(image));

    image->resize({1}, core::type::UINT8, data::Image::PixelFormat::RGB);
    CPPUNIT_ASSERT_EQUAL(gdcmPI(gdcmPI::RGB), helper::getPhotometricInterpretation(image));

    image->resize({1}, core::type::UINT8, data::Image::PixelFormat::RGBA);
    CPPUNIT_ASSERT_EQUAL(gdcmPI(gdcmPI::ARGB), helper::getPhotometricInterpretation(image));
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertToPresentationTypeTest()
{
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::SURFACE, helper::convertToPresentationType(data::Material::SURFACE));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::POINTS, helper::convertToPresentationType(data::Material::POINT));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::WIREFRAME, helper::convertToPresentationType(data::Material::WIREFRAME));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::SURFACE, helper::convertToPresentationType(data::Material::EDGE));
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertToRepresentationModeTest()
{
    CPPUNIT_ASSERT_EQUAL(data::Material::SURFACE, helper::convertToRepresentationMode(gdcm::Surface::SURFACE));
    CPPUNIT_ASSERT_EQUAL(data::Material::WIREFRAME, helper::convertToRepresentationMode(gdcm::Surface::WIREFRAME));
    CPPUNIT_ASSERT_EQUAL(data::Material::POINT, helper::convertToRepresentationMode(gdcm::Surface::POINTS));
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertPointToFrameNumberNominalTest()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    image->setSpacing({1, 1, 1});
    CPPUNIT_ASSERT_EQUAL(
        std::size_t(3),
        helper::convertPointToFrameNumber(image, std::make_shared<data::Point>(0., 1., 2.))
    );
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertPointToFrameNumberFailureTest()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    image->setSpacing({1, 1, 1});
    CPPUNIT_ASSERT_THROW(
        helper::convertPointToFrameNumber(image, std::make_shared<data::Point>(1., 2., 3.)),
        io::dicom::exception::Failed
    );
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertFrameNumberToZCoordinateNominalTest()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    image->setSpacing({1, 1, 1});
    CPPUNIT_ASSERT_EQUAL(2., helper::convertFrameNumberToZCoordinate(image, 3));
}

//------------------------------------------------------------------------------

void DicomDataToolsTest::convertFrameNumberToZCoordinateFailureTest()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    image->setSpacing({1, 1, 1});
    CPPUNIT_ASSERT_THROW(helper::convertFrameNumberToZCoordinate(image, 0), io::dicom::exception::Failed);
}

} // namespace sight::io::dicom::helper::ut
