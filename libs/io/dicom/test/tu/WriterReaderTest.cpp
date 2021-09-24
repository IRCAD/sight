/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "WriterReaderTest.hpp"

#include <core/tools/System.hpp>

#include <data/Boolean.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <io/dicom/reader/SeriesDB.hpp>
#include <io/dicom/writer/Series.hpp>
#include <io/dicom/writer/SeriesDB.hpp>

#include <utest/Filter.hpp>

#include <utestData/generator/Image.hpp>
#include <utestData/generator/Object.hpp>
#include <utestData/generator/SeriesDB.hpp>
#include <utestData/helper/compare.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::WriterReaderTest);

namespace sight::io::dicom
{

namespace ut
{

//-----------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 1000. + .5) / 1000;
}

//------------------------------------------------------------------------------

void roundSpacing(data::Image::sptr image)
{
    data::Image::Spacing spacing = image->getSpacing2();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image->setSpacing2(spacing);
}

//------------------------------------------------------------------------------

void WriterReaderTest::setUp()
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
}

//------------------------------------------------------------------------------

void WriterReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void WriterReaderTest::writeReadImageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    data::ImageSeries::sptr imgSeries;
    imgSeries = utestData::generator::SeriesDB::createImageSeries();

    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "dicomTest";

    std::filesystem::create_directories(PATH);

    io::dicom::writer::Series::sptr writer = io::dicom::writer::Series::New();
    writer->setObject(imgSeries);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    data::SeriesDB::sptr sdb                 = data::SeriesDB::New();
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    std::filesystem::remove_all(PATH);

    // check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    data::Series::sptr series         = sdb->getContainer().front();
    data::ImageSeries::sptr imgseries = data::ImageSeries::dynamicCast(series);
    data::Image::sptr image           = imgseries->getImage();
    roundSpacing(image);

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(utestData::helper::compare(imgSeries, sdb->getContainer().front()));
}

//------------------------------------------------------------------------------

void WriterReaderTest::writeReadSeriesDBTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    data::SeriesDB::sptr seriesDB;
    seriesDB = this->createSeriesDB();

    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "dicomTest";

    std::filesystem::create_directories(PATH);

    io::dicom::writer::SeriesDB::sptr writer = io::dicom::writer::SeriesDB::New();
    writer->setObject(seriesDB);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    data::SeriesDB::sptr sdb                 = data::SeriesDB::New();
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    std::filesystem::remove_all(PATH);

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(utestData::helper::compare(seriesDB, sdb));
}

//------------------------------------------------------------------------------

data::SeriesDB::sptr WriterReaderTest::createSeriesDB()
{
    //create SeriesDB
    data::SeriesDB::sptr sdb            = data::SeriesDB::New();
    data::ImageSeries::sptr imgSeries   = utestData::generator::SeriesDB::createImageSeries();
    data::ModelSeries::sptr modelSeries = utestData::generator::SeriesDB::createModelSeries(1);

    sdb->getContainer().push_back(imgSeries);
    sdb->getContainer().push_back(modelSeries);

    data::Image::sptr image = imgSeries->getImage();

    // Add landmarks
    data::fieldHelper::MedicalImageHelpers::checkLandmarks(image);
    data::PointList::sptr landmarks =
        image->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);
    const data::Image::Spacing spacing = image->getSpacing2();
    const data::Image::Origin origin   = image->getOrigin2();
    const data::Point::sptr point      = data::Point::New(
        2.6 + origin[0],
        1.2 + origin[1],
        4.5 + origin[2]
    );
    point->setField(data::fieldHelper::Image::m_labelId, data::String::New("Label1"));
    landmarks->getPoints().push_back(point);
    data::Point::sptr point2 = data::Point::New(
        1.2 + origin[0],
        2.4 + origin[1],
        0.3 + origin[2]
    );
    point2->setField(data::fieldHelper::Image::m_labelId, data::String::New("Label2"));
    landmarks->getPoints().push_back(point2);
    const data::Image::Size size   = image->getSize2();
    const data::Point::sptr point3 = data::Point::New(
        1.2 + origin[0],
        2.4 + origin[1],
        static_cast<double>(size[2] - 1) * spacing[2] + origin[2]
    );
    point3->setField(data::fieldHelper::Image::m_labelId, data::String::New("Label3"));
    landmarks->getPoints().push_back(point3);

    // Add distance
    data::PointList::sptr pl    = data::PointList::New();
    const data::Point::sptr pt1 = data::Point::New(0., 0., 0.);
    const data::Point::sptr pt2 = data::Point::New(
        static_cast<double>(size[0] - 1) * spacing[0],
        static_cast<double>(size[1] - 1) * spacing[1],
        static_cast<double>(size[2] - 1) * spacing[2]
    );
    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);

    data::Vector::sptr vectDist;
    vectDist = image->setDefaultField<data::Vector>(
        data::fieldHelper::Image::m_imageDistancesId,
        data::Vector::New()
    );
    vectDist->getContainer().push_back(pl);

    image->setField("ShowLandmarks", data::Boolean::New(true));
    image->setField("ShowDistances", data::Boolean::New(true));

    // Update Reconstruction
    data::Reconstruction::sptr rec = modelSeries->getReconstructionDB().front();
    data::Mesh::sptr mesh          = rec->getMesh();
    mesh->clearCellColors();
    mesh->clearPointColors();
    mesh->clearCellNormals();

    // gdcm only manage ambient color in reconstruction
    data::Material::sptr material = data::Material::New();
    data::Color::sptr color       = utestData::generator::Object::randomizeColor();
    material->setDiffuse(color);
    rec->setMaterial(material);
    rec->setImage(data::Image::sptr()); // not managed

    modelSeries->setField("ShowReconstructions", data::Boolean::New(true));

    return sdb;
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::dicom
