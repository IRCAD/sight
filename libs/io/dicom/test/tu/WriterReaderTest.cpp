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

#include "WriterReaderTest.hpp"

#include <core/tools/System.hpp>

#include <data/Boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <io/dicom/reader/SeriesSet.hpp>
#include <io/dicom/writer/Series.hpp>
#include <io/dicom/writer/SeriesSet.hpp>

#include <utest/Filter.hpp>

#include <utestData/generator/Image.hpp>
#include <utestData/generator/Object.hpp>
#include <utestData/generator/SeriesSet.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::WriterReaderTest);

namespace sight::io::dicom::ut
{

//-----------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 1000. + .5) / 1000;
}

//------------------------------------------------------------------------------

void roundSpacing(data::Image::sptr image)
{
    data::Image::Spacing spacing = image->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image->setSpacing(spacing);
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
    imgSeries = utestData::generator::SeriesSet::createImageSeries();

    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "dicomTest";

    std::filesystem::create_directories(PATH);

    io::dicom::writer::Series::sptr writer = io::dicom::writer::Series::New();
    writer->setObject(imgSeries);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    auto series_set = data::SeriesSet::New();
    auto reader     = io::dicom::reader::SeriesSet::New();
    reader->setObject(series_set);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    std::filesystem::remove_all(PATH);

    // check series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    auto series    = series_set->front();
    auto imgseries = data::ImageSeries::dynamicCast(series);
    roundSpacing(imgseries);
}

//------------------------------------------------------------------------------

data::SeriesSet::sptr WriterReaderTest::createSeriesSet()
{
    //create SeriesSet
    auto series_set  = data::SeriesSet::New();
    auto imgSeries   = utestData::generator::SeriesSet::createImageSeries();
    auto modelSeries = utestData::generator::SeriesSet::createModelSeries(1);

    series_set->push_back(imgSeries);
    series_set->push_back(modelSeries);

    // Add landmarks
    data::PointList::sptr landmarks    = data::helper::MedicalImage::getLandmarks(*imgSeries);
    const data::Image::Spacing spacing = imgSeries->getSpacing();
    const data::Image::Origin origin   = imgSeries->getOrigin();
    const data::Point::sptr point      = data::Point::New(
        2.6 + origin[0],
        1.2 + origin[1],
        4.5 + origin[2]
    );
    point->setLabel("Label1");
    landmarks->getPoints().push_back(point);
    data::Point::sptr point2 = data::Point::New(
        1.2 + origin[0],
        2.4 + origin[1],
        0.3 + origin[2]
    );
    point2->setLabel("Label2");
    landmarks->getPoints().push_back(point2);
    const data::Image::Size size   = imgSeries->getSize();
    const data::Point::sptr point3 = data::Point::New(
        1.2 + origin[0],
        2.4 + origin[1],
        static_cast<double>(size[2] - 1) * spacing[2] + origin[2]
    );
    point3->setLabel("Label3");
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

    data::Vector::sptr vectDist = data::helper::MedicalImage::getDistances(*imgSeries);

    if(!vectDist)
    {
        vectDist = data::Vector::New();
        data::helper::MedicalImage::setDistances(*imgSeries, vectDist);
    }

    vectDist->push_back(pl);

    data::helper::MedicalImage::setDistanceVisibility(*imgSeries, true);
    data::helper::MedicalImage::setLandmarksVisibility(*imgSeries, true);
    // Update Reconstruction
    data::Reconstruction::sptr rec = modelSeries->getReconstructionDB().front();
    data::Mesh::sptr mesh          = rec->getMesh();
    mesh->clear<data::Mesh::Attributes::CELL_COLORS>();
    mesh->clear<data::Mesh::Attributes::POINT_COLORS>();
    mesh->clear<data::Mesh::Attributes::CELL_NORMALS>();

    // gdcm only manage ambient color in reconstruction
    data::Material::sptr material = data::Material::New();
    data::Color::sptr color       = utestData::generator::Object::randomizeColor();
    material->setDiffuse(color);
    rec->setMaterial(material);
    rec->setImage(data::Image::sptr()); // not managed

    modelSeries->setField("ShowReconstructions", data::Boolean::New(true));

    return series_set;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
