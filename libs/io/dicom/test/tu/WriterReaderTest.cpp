/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/os/temp_path.hpp>

#include <data/boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <io/dicom/reader/series_set.hpp>
#include <io/dicom/writer/series.hpp>
#include <io/dicom/writer/series_set.hpp>

#include <utest/Filter.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/object.hpp>
#include <utest_data/generator/series_set.hpp>

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

void roundSpacing(data::image::sptr image)
{
    data::image::Spacing spacing = image->getSpacing();
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

    auto imgSeries = utest_data::generator::series_set::createImageSeries();
    core::os::temp_dir tmpDir;

    auto writer = std::make_shared<io::dicom::writer::series>();
    writer->set_object(imgSeries);
    writer->set_folder(tmpDir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    auto series_set = std::make_shared<data::series_set>();
    auto reader     = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(tmpDir);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // check series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    auto series    = series_set->front();
    auto imgseries = std::dynamic_pointer_cast<data::image_series>(series);
    roundSpacing(imgseries);
}

//------------------------------------------------------------------------------

data::series_set::sptr WriterReaderTest::createSeriesSet()
{
    //create series_set
    auto series_set  = std::make_shared<data::series_set>();
    auto imgSeries   = utest_data::generator::series_set::createImageSeries();
    auto modelSeries = utest_data::generator::series_set::createModelSeries(1);

    series_set->push_back(imgSeries);
    series_set->push_back(modelSeries);

    // Add landmarks
    data::point_list::sptr landmarks   = data::helper::MedicalImage::getLandmarks(*imgSeries);
    const data::image::Spacing spacing = imgSeries->getSpacing();
    const data::image::Origin origin   = imgSeries->getOrigin();
    const data::point::sptr point      = std::make_shared<data::point>(
        2.6 + origin[0],
        1.2 + origin[1],
        4.5 + origin[2]
    );
    point->setLabel("Label1");
    landmarks->getPoints().push_back(point);
    data::point::sptr point2 = std::make_shared<data::point>(
        1.2 + origin[0],
        2.4 + origin[1],
        0.3 + origin[2]
    );
    point2->setLabel("Label2");
    landmarks->getPoints().push_back(point2);
    const data::image::Size size   = imgSeries->size();
    const data::point::sptr point3 = std::make_shared<data::point>(
        1.2 + origin[0],
        2.4 + origin[1],
        static_cast<double>(size[2] - 1) * spacing[2] + origin[2]
    );
    point3->setLabel("Label3");
    landmarks->getPoints().push_back(point3);

    // Add distance
    data::point_list::sptr pl   = std::make_shared<data::point_list>();
    const data::point::sptr pt1 = std::make_shared<data::point>(0., 0., 0.);
    const data::point::sptr pt2 = std::make_shared<data::point>(
        static_cast<double>(size[0] - 1) * spacing[0],
        static_cast<double>(size[1] - 1) * spacing[1],
        static_cast<double>(size[2] - 1) * spacing[2]
    );
    pl->getPoints().push_back(pt1);
    pl->getPoints().push_back(pt2);

    data::vector::sptr vectDist = data::helper::MedicalImage::getDistances(*imgSeries);

    if(!vectDist)
    {
        vectDist = std::make_shared<data::vector>();
        data::helper::MedicalImage::setDistances(*imgSeries, vectDist);
    }

    vectDist->push_back(pl);

    data::helper::MedicalImage::setDistanceVisibility(*imgSeries, true);
    data::helper::MedicalImage::setLandmarksVisibility(*imgSeries, true);
    // Update Reconstruction
    data::reconstruction::sptr rec = modelSeries->getReconstructionDB().front();
    data::mesh::sptr mesh          = rec->getMesh();
    mesh->clear<data::mesh::Attributes::CELL_COLORS>();
    mesh->clear<data::mesh::Attributes::POINT_COLORS>();
    mesh->clear<data::mesh::Attributes::CELL_NORMALS>();

    // gdcm only manage ambient color in reconstruction
    data::material::sptr material = std::make_shared<data::material>();
    data::color::sptr color       = utest_data::generator::object::randomizeColor();
    material->setDiffuse(color);
    rec->setMaterial(material);
    rec->setImage(data::image::sptr()); // not managed

    modelSeries->setField("ShowReconstructions", std::make_shared<data::boolean>(true));

    return series_set;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
