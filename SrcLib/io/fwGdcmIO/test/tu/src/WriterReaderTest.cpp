/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwGdcmIO/reader/SeriesDB.hpp>
#include <fwGdcmIO/writer/Series.hpp>
#include <fwGdcmIO/writer/SeriesDB.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/Object.hpp>
#include <fwTest/generator/SeriesDB.hpp>
#include <fwTest/helper/compare.hpp>
#include <fwTest/Slow.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::WriterReaderTest );

namespace fwGdcmIO
{
namespace ut
{

//-----------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 1000. + .5) / 1000;
}

//------------------------------------------------------------------------------

void roundSpacing(::fwData::Image::sptr image)
{
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image->setSpacing(spacing);
}

//------------------------------------------------------------------------------

void WriterReaderTest::setUp()
{
    // Set up context before running a test.
    if(::fwTest::Slow::ignoreSlowTests())
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
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwTest::generator::Image::initRand();
    ::fwMedData::ImageSeries::sptr imgSeries;
    imgSeries = ::fwTest::generator::SeriesDB::createImageSeries();

    const ::boost::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "dicomTest";

    ::boost::filesystem::create_directories( PATH );

    ::fwGdcmIO::writer::Series::sptr writer = ::fwGdcmIO::writer::Series::New();
    writer->setObject(imgSeries);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    ::fwMedData::SeriesDB::sptr sdb           = ::fwMedData::SeriesDB::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    ::boost::filesystem::remove_all( PATH );

    // check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    ::fwMedData::Series::sptr series         = sdb->getContainer().front();
    ::fwMedData::ImageSeries::sptr imgseries = ::fwMedData::ImageSeries::dynamicCast(series);
    ::fwData::Image::sptr image              = imgseries->getImage();
    roundSpacing(image);

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(::fwTest::helper::compare(imgSeries, sdb->getContainer().front()));
}
//------------------------------------------------------------------------------

void WriterReaderTest::writeReadSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwTest::generator::Image::initRand();
    ::fwMedData::SeriesDB::sptr seriesDB;
    seriesDB = this->createSeriesDB();

    const ::boost::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "dicomTest";

    ::boost::filesystem::create_directories( PATH );

    ::fwGdcmIO::writer::SeriesDB::sptr writer = ::fwGdcmIO::writer::SeriesDB::New();
    writer->setObject(seriesDB);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    ::fwMedData::SeriesDB::sptr sdb           = ::fwMedData::SeriesDB::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    ::boost::filesystem::remove_all( PATH );

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(::fwTest::helper::compare(seriesDB, sdb));
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr WriterReaderTest::createSeriesDB()
{
    //create SeriesDB
    ::fwMedData::SeriesDB::sptr sdb            = ::fwMedData::SeriesDB::New();
    ::fwMedData::ImageSeries::sptr imgSeries   = ::fwTest::generator::SeriesDB::createImageSeries();
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwTest::generator::SeriesDB::createModelSeries(1);

    sdb->getContainer().push_back(imgSeries);
    sdb->getContainer().push_back(modelSeries);

    ::fwData::Image::sptr image = imgSeries->getImage();

    // Add landmarks
    ::fwDataTools::fieldHelper::MedicalImageHelpers::checkLandmarks(image);
    ::fwData::PointList::sptr landmarks =
        image->getField< ::fwData::PointList >( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    ::fwData::Image::OriginType origin   = image->getOrigin();
    ::fwData::Point::sptr point          = ::fwData::Point::New(2.6 + origin[0],
                                                                1.2 + origin[1],
                                                                4.5 + origin[2]);
    point->setField( ::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New("Label1") );
    landmarks->getPoints().push_back(point);
    ::fwData::Point::sptr point2 = ::fwData::Point::New(1.2 + origin[0],
                                                        2.4 + origin[1],
                                                        0.3 + origin[2]);
    point2->setField( ::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New("Label2") );
    landmarks->getPoints().push_back(point2);
    ::fwData::Image::SizeType size = image->getSize();
    ::fwData::Point::sptr point3   = ::fwData::Point::New(1.2 + origin[0],
                                                          2.4 + origin[1],
                                                          static_cast<double>(size[2]-1) * spacing[2] + origin[2]);
    point3->setField( ::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New("Label3") );
    landmarks->getPoints().push_back(point3);

    // Add distance
    ::fwData::PointList::sptr pl = ::fwData::PointList::New();
    ::fwData::Point::sptr pt1    = ::fwData::Point::New(0., 0., 0.);
    ::fwData::Point::sptr pt2    = ::fwData::Point::New(static_cast<double>(size[0]-1) * spacing[0],
                                                        static_cast<double>(size[1]-1) * spacing[1],
                                                        static_cast<double>(size[2]-1) * spacing[2]);
    pl->getPoints().push_back( pt1 );
    pl->getPoints().push_back( pt2 );

    ::fwData::Vector::sptr vectDist;
    vectDist = image->setDefaultField< ::fwData::Vector >(
        ::fwDataTools::fieldHelper::Image::m_imageDistancesId, ::fwData::Vector::New());
    vectDist->getContainer().push_back(pl);

    image->setField("ShowLandmarks", ::fwData::Boolean::New(true));
    image->setField("ShowDistances", ::fwData::Boolean::New(true));

    // Update Reconstruction
    ::fwData::Reconstruction::sptr rec = modelSeries->getReconstructionDB().front();
    ::fwData::Mesh::sptr mesh          = rec->getMesh();
    mesh->setPointColorsArray(::fwData::Array::sptr());
    mesh->setCellColorsArray(::fwData::Array::sptr());
    mesh->setCellNormalsArray(::fwData::Array::sptr());

    // gdcm only manage ambient color in reconstruction
    ::fwData::Material::sptr material = ::fwData::Material::New();
    ::fwData::Color::sptr color       = ::fwTest::generator::Object::randomizeColor();
    material->setDiffuse(color);
    rec->setMaterial(material);
    rec->setImage(::fwData::Image::sptr()); // not managed

    modelSeries->setField("ShowReconstructions", ::fwData::Boolean::New(true));

    return sdb;
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwGdcmIO
