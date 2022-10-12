/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ReaderTest.hpp"

#include <core/memory/BufferManager.hpp>

#include <data/ImageSeries.hpp>

#include <io/dicom/Reader.hpp>
#include <io/dicom/reader/SeriesSet.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>

#include <boost/compute/detail/sha1.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <filesystem>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::ReaderTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

inline static sight::data::SeriesSet::sptr read(const std::filesystem::path path)
{
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    auto seriesSet = data::SeriesSet::New();

    auto reader = io::dicom::Reader::New();
    reader->setObject(seriesSet);
    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    return seriesSet;
}

//------------------------------------------------------------------------------

void ReaderTest::setUp()
{
    // Set up context before running a test.
    if(utest::Filter::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }

    core::memory::BufferManager::getDefault()->setLoadingMode(core::memory::BufferManager::DIRECT);
}

//------------------------------------------------------------------------------

void ReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReaderTest::readJMSSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    // cspell: ignore Genou
    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/JMSGenou");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    const auto& size = imageSeries->getSize();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(404), size[2]);
}

//------------------------------------------------------------------------------

void ReaderTest::readCTSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    const double delta = 0.00001;

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

    // Check size
    const auto& size = imageSeries->getSize();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(129), size[2]);

    // Check spacing
    const auto& spacing = imageSeries->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.6), spacing[2], delta);

    // Check origin
    const auto& origin = imageSeries->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, imageSeries->getWindowCenter().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, imageSeries->getWindowWidth().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::Type::INT16, imageSeries->getType());
}

//------------------------------------------------------------------------------

void ReaderTest::readMRSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/46-MR-BARRE-MONO2-12-shoulder");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    const double delta = 0.01;

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

    // Check size
    const auto& size = imageSeries->getSize();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

    // Check spacing
    const auto& spacing = imageSeries->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6.6), spacing[2], delta);

    // Check origin
    const auto& origin = imageSeries->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-180.058), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-97.1478), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(112.828), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1000), imageSeries->getWindowCenter().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2000), imageSeries->getWindowWidth().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::Type::DOUBLE, imageSeries->getType());
}

//------------------------------------------------------------------------------

void ReaderTest::readOTSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/42-OT-BARRE-MONO2-8-colon");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    const double delta = 0.01;

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

    // Check size
    const auto& size = imageSeries->getSize();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

    // Check spacing
    const auto& spacing = imageSeries->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[2], delta);

    // Check origin
    const auto& origin = imageSeries->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

    // Check window width
    CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
}

//------------------------------------------------------------------------------

void ReaderTest::readMultipleRescaleSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/83-CT-MultipleRescale");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    // Get internal buffer
    const auto dumpLock = imageSeries->dump_lock();
    auto* buffer        = imageSeries->getBuffer();
    CPPUNIT_ASSERT(buffer);

    // Compute sha1 digest
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(static_cast<char*>(buffer), imageSeries->getSizeInBytes());
    boost::uuids::detail::sha1::digest_type digest = {0};
    sha1.get_digest(digest);

    // Check digests
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 808070165U, digest[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 1419762457U, digest[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 664759744U, digest[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 4220766428U, digest[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 2226307254U, digest[4]);
}

//------------------------------------------------------------------------------

void ReaderTest::readCTWithSurviewSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/84-CT-Surview");
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), seriesSet->size());

    for(const auto& series : *seriesSet)
    {
        const auto& imageSeries = data::ImageSeries::dynamicCast(series);
        CPPUNIT_ASSERT(imageSeries);
    }
}

//------------------------------------------------------------------------------

void ReaderTest::readMRWithTemporalPositionSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/85-MR-TemporalPosition");
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), seriesSet->size());

    for(const auto& series : *seriesSet)
    {
        const auto& imageSeries = data::ImageSeries::dynamicCast(series);
        CPPUNIT_ASSERT(imageSeries);
    }
}

//------------------------------------------------------------------------------

void ReaderTest::readCTSeriesSetIssue01Test()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "sight/Patient/Dicom/DicomDB/86-CT-Skull");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);
}

//------------------------------------------------------------------------------

void ReaderTest::readEnhancedUSVolumeTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const auto& seriesSet = read(utestData::Data::dir() / "us/Enhanced US Volume Storage/GE, 3D+t, lossy JPEG");
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

    const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->front());
    CPPUNIT_ASSERT(imageSeries);

    const double delta = 0.01;

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

    // Check size
    const auto& size = imageSeries->getSize();
    CPPUNIT_ASSERT_EQUAL(std::size_t(187), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(157), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3600), size[2]);

    // Check spacing
    const auto& spacing = imageSeries->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, spacing[2], delta);

    // Check origin
    const auto& origin = imageSeries->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

    // Check window width
    CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
}

//------------------------------------------------------------------------------

void ReaderTest::readUltrasoundImageTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    // cspell:ignore Aixplorer
    // Aixplorer
    {
        const auto& seriesSet = read(utestData::Data::dir() / "us/Ultrasound Image Storage/Aixplorer");
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), seriesSet->size());

        for(const auto& series : *seriesSet)
        {
            const auto& imageSeries = data::ImageSeries::dynamicCast(series);
            CPPUNIT_ASSERT(imageSeries);

            if(imageSeries->getFile().filename() == "us_monochrome2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(1440), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1080), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

                // Check window width
                CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
            else if(imageSeries->getFile().filename() == "us_rgb.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(1440), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1080), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

                // Check window width
                CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
        }
    }

    // GE, lossy JPEG
    {
        const auto& seriesSet = read(utestData::Data::dir() / "us/Ultrasound Image Storage/GE, lossy JPEG");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

        const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
        CPPUNIT_ASSERT(imageSeries);

        const double delta = 0.01;

        // Check number of dimensions
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

        // Check size
        const auto& size = imageSeries->getSize();
        CPPUNIT_ASSERT_EQUAL(std::size_t(636), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(434), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = imageSeries->getSpacing();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.05, spacing[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.05, spacing[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

        // Check origin
        const auto& origin = imageSeries->getOrigin();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

        // Check window center
        CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

        // Check window width
        CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

        // Check image type
        CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
    }

    // cspell:ignore Kretztechnik
    // GE, pixel spacing, Kretztechnik non-cartesian volume
    {
        const auto& seriesSet = read(
            utestData::Data::dir()
            / "us/Ultrasound Image Storage/GE, pixel spacing, Kretztechnik non-cartesian volume"
        );
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), seriesSet->size());

        for(const auto& series : *seriesSet)
        {
            const auto& imageSeries = data::ImageSeries::dynamicCast(series);
            CPPUNIT_ASSERT(imageSeries);

            // cspell:ignore kretz
            if(imageSeries->getFile().filename() == "us_kretz.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

                // Check window width
                CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
            else if(imageSeries->getFile().filename() == "us_kretz2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(1136), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(852), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

                // Check window width
                CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
        }
    }

    // Philips, 16 bit palette color, aspect ratio
    {
        const auto& seriesSet = read(
            utestData::Data::dir() / "us/Ultrasound Image Storage/Philips, 16 bit palette color, aspect ratio"
        );
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

        const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
        CPPUNIT_ASSERT(imageSeries);

        const double delta = 0.01;

        // Check number of dimensions
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

        // Check size
        const auto& size = imageSeries->getSize();
        CPPUNIT_ASSERT_EQUAL(std::size_t(480), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(430), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = imageSeries->getSpacing();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(3.6, spacing[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

        // Check origin
        const auto& origin = imageSeries->getOrigin();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

        // Check window center
        CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

        // Check window width
        CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

        // Check image type
        CPPUNIT_ASSERT_EQUAL(core::Type::UINT16, imageSeries->getType());
    }

    // Philips, RLE, palette color
    {
        const auto& seriesSet =
            read(utestData::Data::dir() / "us/Ultrasound Image Storage/Philips, RLE, palette color");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

        const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
        CPPUNIT_ASSERT(imageSeries);

        const double delta = 0.01;

        // Check number of dimensions
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

        // Check size
        const auto& size = imageSeries->getSize();
        CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(13), size[2]);

        // Check spacing
        const auto& spacing = imageSeries->getSpacing();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

        // Check origin
        const auto& origin = imageSeries->getOrigin();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

        // Check window center
        CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

        // Check window width
        CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

        // Check image type
        CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
    }
}

//------------------------------------------------------------------------------

void ReaderTest::readUltrasoundMultiframeImageTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    // cspell:ignore Acuson
    // Acuson, 2D+t, lossy JPEG
    {
        const auto& seriesSet = read(
            utestData::Data::dir() / "us/Ultrasound Multi-frame Image Storage/Acuson, 2D+t, lossy JPEG"
        );
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), seriesSet->size());

        for(const auto& series : *seriesSet)
        {
            const auto& imageSeries = data::ImageSeries::dynamicCast(series);
            CPPUNIT_ASSERT(imageSeries);

            if(imageSeries->getFile().filename() == "us_acuson.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(576), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(456), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(28), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT_DOUBLES_EQUAL(128.0, imageSeries->getWindowCenter().front(), delta);

                // Check window width
                CPPUNIT_ASSERT_DOUBLES_EQUAL(255.0, imageSeries->getWindowWidth().front(), delta);

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
            else if(imageSeries->getFile().filename() == "us_acuson2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

                // Check size
                const auto& size = imageSeries->getSize();
                CPPUNIT_ASSERT_EQUAL(std::size_t(576), size[0]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(456), size[1]);
                CPPUNIT_ASSERT_EQUAL(std::size_t(55), size[2]);

                // Check spacing
                const auto& spacing = imageSeries->getSpacing();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

                // Check origin
                const auto& origin = imageSeries->getOrigin();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

                // Check window center
                CPPUNIT_ASSERT_DOUBLES_EQUAL(128.0, imageSeries->getWindowCenter().front(), delta);

                // Check window width
                CPPUNIT_ASSERT_DOUBLES_EQUAL(255.0, imageSeries->getWindowWidth().front(), delta);

                // Check image type
                CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
            }
        }
    }

    // GE, 2D+t, RLE
    {
        const auto& seriesSet = read(utestData::Data::dir() / "us/Ultrasound Multi-frame Image Storage/GE, 2D+t, RLE");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

        const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
        CPPUNIT_ASSERT(imageSeries);

        const double delta = 0.01;

        // Check number of dimensions
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

        // Check size
        const auto& size = imageSeries->getSize();
        CPPUNIT_ASSERT_EQUAL(std::size_t(636), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(434), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(7), size[2]);

        // Check spacing
        const auto& spacing = imageSeries->getSpacing();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.03, spacing[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

        // Check origin
        const auto& origin = imageSeries->getOrigin();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

        // Check window center
        CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

        // Check window width
        CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

        // Check image type
        CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
    }

    // Philips, 2D+t,  lossy JPEG
    {
        const auto& seriesSet = read(
            utestData::Data::dir() / "us/Ultrasound Multi-frame Image Storage/Philips, 2D+t,  lossy JPEG"
        );
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

        const auto& imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
        CPPUNIT_ASSERT(imageSeries);

        const double delta = 0.01;

        // Check number of dimensions
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), imageSeries->numDimensions());

        // Check size
        const auto& size = imageSeries->getSize();
        CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
        CPPUNIT_ASSERT_EQUAL(std::size_t(292), size[2]);

        // Check spacing
        const auto& spacing = imageSeries->getSpacing();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.01, spacing[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, spacing[2], delta);

        // Check origin
        const auto& origin = imageSeries->getOrigin();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[1], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, origin[2], delta);

        // Check window center
        CPPUNIT_ASSERT(imageSeries->getWindowCenter().empty());

        // Check window width
        CPPUNIT_ASSERT(imageSeries->getWindowWidth().empty());

        // Check image type
        CPPUNIT_ASSERT_EQUAL(core::Type::UINT8, imageSeries->getType());
    }
}

} // namespace sight::io::dicom::ut
