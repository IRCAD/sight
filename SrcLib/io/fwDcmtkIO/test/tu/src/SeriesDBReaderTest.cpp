/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SeriesDBReaderTest.hpp"

#include <fwDcmtkIO/SeriesDBReader.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(::fwDcmtkIO::ut::SeriesDBReaderTest);

namespace fwDcmtkIO
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

std::string getValue(const ::boost::property_tree::ptree& node, const std::string& name,
                     const ::boost::filesystem::path& filePath )
{
    std::string value;
    try
    {
        value = node.get< std::string >(name);

        // Remove leading and trailing spaces
        size_t first = value.find_first_not_of(" ");
        if (first != std::string::npos)
        {
            size_t last = value.find_last_not_of(" ");
            value = value.substr(first, last - first + 1);
        }
    }
    catch( ::boost::property_tree::ptree_bad_path& )
    {
        SLM_WARN(name + " information are missing in '"+ filePath.string() +"'.");
        value = "";
    }

    return value;
}

//------------------------------------------------------------------------------

/**
 * @brief Verify tag values according to JSON files generated from DICOM dump
 * @param filename DICOM folder name
 * @param seriesDB SeriesDB object containing DICOM series
 */
void verifyTagValues(const std::string& filename, const ::fwMedData::SeriesDB::sptr& seriesDB)
{
    const double delta                        = 0.001;
    const ::boost::filesystem::path dicomPath = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB";
    const ::boost::filesystem::path metaPath  = dicomPath / "META";

    for(::fwMedData::SeriesDB::iterator sIt = seriesDB->begin(); sIt != seriesDB->end(); ++sIt)
    {
        ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(*sIt);

        // Parse META File
        const std::string metaName               = filename + "/" + series->getInstanceUID() + ".json";
        const ::boost::filesystem::path metaFile = metaPath / metaName;
        const std::string mf                     = metaFile.string();
        ::boost::property_tree::ptree root;
        ::boost::property_tree::json_parser::read_json(mf, root);

        // Series
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesInstanceUID", mf), series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "Modality", mf), series->getModality());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDate", mf), series->getDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesTime", mf), series->getTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDescription", mf), series->getDescription());

        ::fwMedData::DicomValuesType performingPhysiciansName = series->getPerformingPhysiciansName();
        std::string performingPhysiciansNameStr = "";
        for(::fwMedData::DicomValuesType::iterator i = performingPhysiciansName.begin();
            i != performingPhysiciansName.end(); ++i)
        {
            performingPhysiciansNameStr += *i;
            if((i+1) != performingPhysiciansName.end())
            {
                performingPhysiciansNameStr += "\\";
            }
        }
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PerformingPhysiciansName", mf), performingPhysiciansNameStr);

        // Patient
        ::fwMedData::Patient::sptr patient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientID", mf), patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientName", mf), patient->getName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientBirthDate", mf), patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientSex", mf), patient->getSex());

        // Study
        ::fwMedData::Study::sptr study = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyInstanceUID", mf), study->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDate", mf), study->getDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyTime", mf), study->getTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "ReferringPhysicianName", mf), study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDescription", mf), study->getDescription());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientAge", mf), study->getPatientAge());

        // Equipment
        ::fwMedData::Equipment::sptr equipment = series->getEquipment();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "InstitutionName", mf), equipment->getInstitutionName());

        // Image
        ::fwData::Image::sptr image = series->getImage();

        // PixelSpacing
        std::vector< std::string > spacingValues;
        const std::string spacingXY = getValue(root, "PixelSpacing", mf);
        ::boost::split(spacingValues, spacingXY, boost::is_any_of("\\"));
        if(!spacingValues[0].empty() && !spacingValues[1].empty())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingValues[0]),
                                         image->getSpacing()[0], delta);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingValues[1]),
                                         image->getSpacing()[1], delta);
        }

        // SliceThickness - This value is recomputed using the SliceThicknessModifier filter.
//        std::string spacingZ = getValue(root, "SliceThickness", mf);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingZ), image->getSpacing()[2], 0.0001);

        // Origin
        std::string origin = getValue(root, "ImagePositionPatient", mf);
        ::boost::algorithm::replace_all(origin, " ", "");
        std::vector< std::string > originValues;
        ::boost::split(originValues, origin, boost::is_any_of("\\"));
        if(originValues.size() == 3)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[0]), image->getOrigin()[0],
                                         delta);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[1]), image->getOrigin()[1],
                                         delta);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[2]), image->getOrigin()[2],
                                         delta);
        }

        // Size
        CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(getValue(root, "Rows", mf)),
                                     image->getSize()[0], delta);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(getValue(root, "Columns", mf)),
                                     image->getSize()[1], delta);
        //TODO: Try to find a way to test depth size.

        // Window Center
        const std::string windowCenter = getValue(root, "WindowCenter", mf);
        if(!windowCenter.empty())
        {
            std::vector< std::string > windowCenterValues;
            ::boost::split(windowCenterValues, windowCenter, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowCenterValues[0]),
                                         image->getWindowCenter(), delta);
        }
        else
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0, image->getWindowCenter(), delta);
        }

        // Window Width
        const std::string windowWidth = getValue(root, "WindowWidth", mf);
        if(!windowWidth.empty())
        {
            std::vector< std::string > windowWidthValues;
            ::boost::split(windowWidthValues, windowWidth, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowWidthValues[0]),
                                         image->getWindowWidth(), delta);
        }
        else
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0, image->getWindowWidth(), delta);
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
        CPPUNIT_ASSERT_EQUAL(nbComponents, image->getNumberOfComponents());

    }

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::tearDown()
{
    // Clean up after the test run.
}
//------------------------------------------------------------------------------

void SeriesDBReaderTest::readACHSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readACHSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readACHLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readACHSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readCTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readCTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readMRSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readMRSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readOTSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readOTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readOTLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readOTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readUnsupportedSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readUnsupportedSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readUnsupportedLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readUnsupportedSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readDisabledSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readDisabledSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readDisabledLazySeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);
    this->readDisabledSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readACHSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());

    // Check trimmed values
    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkSeriesACHGenouTrimmed( series ) );

    // Read image in lazy mode
    ::fwDataTools::helper::Image locker( series->getImage() );

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "01-CT-DICOM_LIVER";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.00001;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    ::fwDataTools::helper::Image locker( image );

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    ::fwData::Image::SizeType size = image->getSize();
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 129 ), size[2]);

    // Check spacing
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.57 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.57 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1.6 ), spacing[2], delta);

    // Check origin
    ::fwData::Image::OriginType origin = image->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_INT16, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "46-MR-BARRE-MONO2-12-shoulder";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    ::fwDataTools::helper::Image locker( image );

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    ::fwData::Image::SizeType size = image->getSize();
    CPPUNIT_ASSERT_EQUAL( size_t( 1024 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1024 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size[2]);

    // Check spacing
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 6 ), spacing[2], delta);

    // Check origin
    ::fwData::Image::OriginType origin = image->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( -180.058 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( -97.1478 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 112.828 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1000 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 2000 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_DOUBLE, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readOTSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "42-OT-BARRE-MONO2-8-colon";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    ::fwDataTools::helper::Image locker( image );

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    ::fwData::Image::SizeType size = image->getSize();
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size[2]);

    // Check spacing
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[2], delta);

    // Check origin
    ::fwData::Image::OriginType origin = image->getOrigin();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readUnsupportedSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "71-CT-DICOM_SEG";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_THROW(reader->read(), ::fwCore::Exception);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readDisabledSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "46-MR-BARRE-MONO2-12-shoulder";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    std::vector< std::string > supportedSOPClassContainer;
    supportedSOPClassContainer.push_back("1.2.840.10008.5.1.4.1.1.2");   // CT Image Storage

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");
    reader->setSupportedSOPClassContainer(supportedSOPClassContainer);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), seriesDB->size());
}

//------------------------------------------------------------------------------

}// namespace ut

} // namespace fwDcmtkIO
