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

#include "SeriesSetReaderTest.hpp"

#include <core/log/logger.hpp>
#include <core/memory/buffer_manager.hpp>

#include <data/Color.hpp>
#include <data/DicomSeries.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <io/dicom/reader/SeriesSet.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>
#include <utestData/DicomReaderTest.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/compute/detail/sha1.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cppunit/extensions/HelperMacros.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::SeriesSetReaderTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

std::string getValue(
    const boost::property_tree::ptree& node,
    const std::string& name,
    const std::filesystem::path& filePath
)
{
    std::string value;
    try
    {
        value = node.get<std::string>(name);

        // Remove leading and trailing spaces
        std::size_t first = value.find_first_not_of(' ');
        if(first != std::string::npos)
        {
            std::size_t last = value.find_last_not_of(' ');
            value = value.substr(first, last - first + 1);
        }
    }
    catch(boost::property_tree::ptree_bad_path&)
    {
        SIGHT_WARN(name + " information are missing in '" + filePath.string() + "'.");
        value = "";
    }

    return value;
}

//------------------------------------------------------------------------------

/**
 * @brief Verify tag values according to JSON files generated from DICOM dump
 * @param filename DICOM folder name
 * @param series_set SeriesSet object containing DICOM series
 */
void verifyTagValues(const std::string& filename, const data::SeriesSet::sptr& series_set)
{
    const double delta                    = 0.001;
    const std::filesystem::path dicomPath = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB";
    const std::filesystem::path metaPath  = dicomPath / "META";

    for(const auto& object : *series_set)
    {
        auto series = std::dynamic_pointer_cast<data::ImageSeries>(object);

        // Parse META File
        const std::string metaName           = filename + "/" + series->getSeriesInstanceUID() + ".json";
        const std::filesystem::path metaFile = metaPath / metaName;
        const std::string mf                 = metaFile.string();
        boost::property_tree::ptree root;
        boost::property_tree::json_parser::read_json(mf, root);

        // Series
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesInstanceUID", mf), series->getSeriesInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "Modality", mf), series->getModality());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDate", mf), series->getSeriesDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesTime", mf), series->getSeriesTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDescription", mf), series->getSeriesDescription());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PerformingPhysicianName", mf), series->getPerformingPhysicianName());

        // Patient
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientID", mf), series->getPatientID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientName", mf), series->getPatientName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientBirthDate", mf), series->getPatientBirthDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientSex", mf), series->getPatientSex());

        // Study
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyInstanceUID", mf), series->getStudyInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDate", mf), series->getStudyDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyTime", mf), series->getStudyTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "ReferringPhysicianName", mf), series->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDescription", mf), series->getStudyDescription());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientAge", mf), series->getPatientAge());

        // Equipment
        CPPUNIT_ASSERT_EQUAL(getValue(root, "InstitutionName", mf), series->getInstitutionName());

        // PixelSpacing - Not checked as the image could be rotated

        // SliceThickness - This value is recomputed using the SliceThicknessModifier filter.
//        std::string spacingZ = getValue(root, "SliceThickness", mf);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(boost::lexical_cast< double >(spacingZ), image->getSpacing()[2], 0.0001);

        // Origin - Not checked as the image could be rotated

        // Size - Not checked as the image could be rotated

        // Window Center
        const std::string windowCenter = getValue(root, "WindowCenter", mf);
        if(!windowCenter.empty())
        {
            std::vector<std::string> windowCenterValues;
            boost::split(windowCenterValues, windowCenter, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                boost::lexical_cast<double>(windowCenterValues[0]),
                series->getWindowCenter().front(),
                delta
            );
        }
        else
        {
            CPPUNIT_ASSERT(series->getWindowCenter().empty());
        }

        // Window Width
        const std::string windowWidth = getValue(root, "WindowWidth", mf);
        if(!windowWidth.empty())
        {
            std::vector<std::string> windowWidthValues;
            boost::split(windowWidthValues, windowWidth, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                boost::lexical_cast<double>(windowWidthValues[0]),
                series->getWindowWidth().front(),
                delta
            );
        }
        else
        {
            CPPUNIT_ASSERT(series->getWindowWidth().empty());
        }

        // Number of components
        const std::string photometricInterpretation = getValue(root, "PhotometricInterpretation", mf);
        std::size_t nbComponents                    = 0;
        if(photometricInterpretation == "MONOCHROME2")
        {
            nbComponents = 1;
        }
        else if(photometricInterpretation == "RGB" || photometricInterpretation == "YBR")
        {
            nbComponents = 3;
        }
        else if(photometricInterpretation == "ARGB" || photometricInterpretation == "CMYK")
        {
            nbComponents = 4;
        }

        CPPUNIT_ASSERT_EQUAL(nbComponents, series->numComponents());
    }
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::setUp()
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

void SeriesSetReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readJMSSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readJMSSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readCTSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readCTSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readMRSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readOTSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readOTSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSEGSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readSEGSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSFSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readSFSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSRSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readSRSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::read3DSRSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::read3DSRSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readDisabledSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readDisabledSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRSeriesWithDicomDirDBTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readMRSeriesWithDicomDir();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMultipleRescaleSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readMultipleRescaleSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readCTWithSurviewSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readCTWithSurviewSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRWithTemporalPositionSeriesSetTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readMRWithTemporalPositionSeries();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readCTSeriesSetIssue01Test()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    core::memory::buffer_manager::get()->set_loading_mode(core::memory::buffer_manager::DIRECT);
    SeriesSetReaderTest::readCTSeriesSetIssue01();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readJMSSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();
    // cspell: ignore Genou
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/JMSGenou";

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    data::ImageSeries::sptr series = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());

    // Check trimmed values
    CPPUNIT_ASSERT(utestData::DicomReaderTest::checkSeriesJMSGenouTrimmed(series));

    // Read image in lazy mode
    const auto dumpLock = series->dump_lock();
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readCTSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "01-CT-DICOM_LIVER";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.00001;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->numDimensions());

    // Check size
    const data::Image::Size size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(129), size[2]);

    // Check spacing
    const data::Image::Spacing spacing = series->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.57), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.6), spacing[2], delta);

    // Check origin
    const data::Image::Origin origin = series->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, series->getWindowCenter().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, series->getWindowWidth().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::INT16, series->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, series_set);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->numDimensions());

    // Check size
    const data::Image::Size size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1024), size[2]);

    // Check spacing
    const data::Image::Spacing spacing = series->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), spacing[2], delta);

    // Check origin
    const data::Image::Origin origin = series->getOrigin();
    SIGHT_WARN("ORIGIN : " << origin[0] << " " << origin[1] << " " << origin[2]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-112.828), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-180.058), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(97.1478), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1000), series->getWindowCenter().front(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2000), series->getWindowWidth().front(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::DOUBLE, series->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, series_set);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readOTSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "42-OT-BARRE-MONO2-8-colon";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Read image buffer
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series->numDimensions());

    // Check size
    data::Image::Size size = series->size();
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(512), size[1]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

    // Check spacing
    data::Image::Spacing spacing = series->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), spacing[2], delta);

    // Check origin
    data::Image::Origin origin = series->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT(series->getWindowCenter().empty());

    // Check window width
    CPPUNIT_ASSERT(series->getWindowWidth().empty());

    // Check image type
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, series->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, series_set);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSEGSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "71-CT-DICOM_SEG";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    //Retrieve ImageSeries
    data::ModelSeries::sptr series = std::dynamic_pointer_cast<data::ModelSeries>((*series_set)[1]);
    CPPUNIT_ASSERT(series);

    data::ModelSeries::ReconstructionVectorType reconstructionDB = series->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), reconstructionDB.size());

    // Check reconstruction
    data::Reconstruction::sptr reconstruction = reconstructionDB[0];
    CPPUNIT_ASSERT_EQUAL(std::string("Liver"), reconstruction->getOrganName());

    // Check mesh
    data::Mesh::sptr mesh = reconstruction->getMesh();
    CPPUNIT_ASSERT_EQUAL(data::Mesh::size_t(2498), mesh->numPoints());
    CPPUNIT_ASSERT_EQUAL(data::Mesh::size_t(5000), mesh->numCells());

    // Check material
    data::Material::sptr material = reconstruction->getMaterial();
    data::Color::sptr color       = material->diffuse();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.8), color->red(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), color->green(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.2), color->blue(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1), color->alpha(), delta);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSFSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "71-CT-DICOM_SF";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Retrieve landmarks
    data::PointList::sptr pointList = data::helper::MedicalImage::getLandmarks(*series);

    // Verify first landmark
    const data::Point::sptr& pointA = pointList->getPoints()[0];
    const std::string labelA        = pointA->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), pointA->getCoord()[2], delta);

    // Verify second landmark
    const data::Point::sptr& pointB = pointList->getPoints()[1];
    const std::string labelB        = pointB->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointB->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readSRSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "71-CT-DICOM_SR";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    CPPUNIT_ASSERT(series);
    const auto dumpLock = series->dump_lock();

    // Retrieve landmarks
    data::PointList::sptr landmarkPointList = data::helper::MedicalImage::getLandmarks(*series);

    // Verify first landmark
    const data::Point::sptr& pointA = landmarkPointList->getPoints()[0];
    const std::string labelA        = pointA->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), pointA->getCoord()[2], delta);

    // Verify second landmark
    const data::Point::sptr& pointB = landmarkPointList->getPoints()[1];
    const std::string labelB        = pointB->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointB->getCoord()[2], delta);

    // Retrieve distances
    data::Vector::sptr distanceVector = data::helper::MedicalImage::getDistances(*series);

    // Verify first distance
    auto distancePointList          = std::dynamic_pointer_cast<data::PointList>((*distanceVector)[0]);
    const data::Point::sptr& pointC = distancePointList->getPoints()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(38.34), pointC->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(116.67), pointC->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointC->getCoord()[2], delta);
    const data::Point::sptr& pointD = distancePointList->getPoints()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(329.41), pointD->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(302.33), pointD->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointD->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::read3DSRSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "71-CT-DICOM_3DSR";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve ImageSeries
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Retrieve landmarks
    data::PointList::sptr landmarkPointList = data::helper::MedicalImage::getLandmarks(*series);
    // Verify first landmark
    const data::Point::sptr& pointA = landmarkPointList->getPoints()[0];
    const std::string labelA        = pointA->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(80.89), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(102.16), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(153), pointA->getCoord()[2], delta);

    // Verify second landmark
    const data::Point::sptr& pointB = landmarkPointList->getPoints()[1];
    const std::string labelB        = pointB->getLabel();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(326.52), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointB->getCoord()[2], delta);

    // Retrieve distances
    data::Vector::sptr distanceVector = data::helper::MedicalImage::getDistances(*series);

    // Verify first distance
    auto distancePointList          = std::dynamic_pointer_cast<data::PointList>((*distanceVector)[0]);
    const data::Point::sptr& pointC = distancePointList->getPoints()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(281.63), pointC->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(99.30), pointC->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(87.26), pointC->getCoord()[2], delta);
    const data::Point::sptr& pointD = distancePointList->getPoints()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(329.41), pointD->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(302.33), pointD->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(276), pointD->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readDisabledSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<std::string> supportedSOPClassContainer;
    supportedSOPClassContainer.emplace_back("1.2.840.10008.5.1.4.1.1.2"); // CT Image Storage

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");
    reader->setSupportedSOPClassContainer(supportedSOPClassContainer);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), std::size_t(reader->getLogger()->count(core::log::log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRSeriesWithDicomDir()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");
    reader->setDicomdirActivated(true);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has succeed
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->getLogger()->count(core::log::log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMultipleRescaleSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "83-CT-MultipleRescale";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Retrieve ImageSeries
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    auto series         = std::dynamic_pointer_cast<data::ImageSeries>(series_set->front());
    const auto dumpLock = series->dump_lock();

    // Get internal buffer
    auto* buffer = series->buffer();
    CPPUNIT_ASSERT(buffer);

    // Compute sha1 digest
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(static_cast<char*>(buffer), series->getSizeInBytes());
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

void SeriesSetReaderTest::readCTWithSurviewSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "84-CT-Surview";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->getLogger()->count(core::log::log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readMRWithTemporalPositionSeries()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "85-MR-TemporalPosition";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->getLogger()->count(core::log::log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::readCTSeriesSetIssue01()
{
    auto series_set = std::make_shared<data::SeriesSet>();

    const std::string filename       = "86-CT-Skull";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    io::dicom::reader::SeriesSet::sptr reader = std::make_shared<io::dicom::reader::SeriesSet>();
    reader->setObject(series_set);
    reader->set_folder(path);
    reader->setDicomFilterType("sight::filter::dicom::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), std::size_t(reader->getLogger()->count(core::log::log::CRITICAL)));
}

} // namespace sight::io::dicom::ut
