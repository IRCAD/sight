/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/helper/Image.hpp>
#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <fwDicomIOExt/dcmtk/SeriesDBReader.hpp>

#include "SeriesDBReaderTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (::fwDicomIOExt::ut::SeriesDBReaderTest);

namespace fwDicomIOExt
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

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
    catch( ::boost::property_tree::ptree_bad_path & )
    {
        SLM_WARN(name + " information are missing in '"+ filePath.string() +"'.");
        value = "";
    }

    return value;
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSeriesDBTest()
{
    const ::boost::filesystem::path dicomPath = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB";
    const ::boost::filesystem::path metaPath = dicomPath / "META";

    // Create a map to store Dicom files and meta
    typedef std::vector< std::string > DicomVectorType;
    DicomVectorType dicomVector;
    dicomVector.push_back("01-CT-DICOM_LIVER");

    // TODO: Push other image names to improve test

    for(DicomVectorType::iterator it = dicomVector.begin(); it != dicomVector.end(); ++it)
    {
        // Read Series
        ::fwDicomIOExt::dcmtk::SeriesDBReader::sptr myLoader = ::fwDicomIOExt::dcmtk::SeriesDBReader::New();
        ::fwMedData::SeriesDB::sptr dummy = ::fwMedData::SeriesDB::New();
        myLoader->setObject(dummy);
        myLoader->setFolder(dicomPath / *it);
        myLoader->read();

        for(::fwMedData::SeriesDB::iterator sIt = dummy->begin(); sIt != dummy->end(); ++sIt)
        {
            ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(*sIt);

            // Parse META File
            const std::string metaName = *it + "/" + series->getInstanceUID() + ".json";
            const ::boost::filesystem::path metaFile = metaPath / metaName;
            const std::string mf = metaFile.string();
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
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingValues[0]), image->getSpacing()[0], 0.0001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingValues[1]), image->getSpacing()[1], 0.0001);

            // SliceThickness - This value is recomputed using the SliceThicknessModifier filter.
//            std::string spacingZ = getValue(root, "SliceThickness", mf);
//            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingZ), image->getSpacing()[2], 0.0001);

            // Origin
            std::string origin = getValue(root, "ImagePositionPatient", mf);
            ::boost::algorithm::replace_all(origin, " ", "");
            std::vector< std::string > originValues;
            ::boost::split(originValues, origin, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[0]), image->getOrigin()[0], 0.0001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[1]), image->getOrigin()[1], 0.0001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(originValues[2]), image->getOrigin()[2], 0.0001);

            // Size
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(getValue(root, "Rows", mf)),
                    image->getSize()[0], 0.0001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(getValue(root, "Columns", mf)),
                            image->getSize()[1], 0.0001);
            //TODO: CTR Find a way to test depth size.

            // Window Center
            const std::string windowCenter = getValue(root, "WindowCenter", mf);
            if(!windowCenter.empty())
            {
                std::vector< std::string > windowCenterValues;
                ::boost::split(windowCenterValues, windowCenter, boost::is_any_of("\\"));
                CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowCenterValues[0]),
                                image->getWindowCenter(), 0.0001);
            }
            else
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0,image->getWindowCenter(), 0.0001);
            }

            // Window Width
            const std::string windowWidth = getValue(root, "WindowWidth", mf);
            if(!windowWidth.empty())
            {
                std::vector< std::string > windowWidthValues;
                ::boost::split(windowWidthValues, windowWidth, boost::is_any_of("\\"));
                CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowWidthValues[0]),
                                image->getWindowWidth(), 0.0001);
            }
            else
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0,image->getWindowWidth(), 0.0001);
            }

            // Number of components
            const std::string photometricInterpretation = getValue(root, "PhotometricInterpretation", mf);
            unsigned int nbComponents = 0;
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
            CPPUNIT_ASSERT_DOUBLES_EQUAL(nbComponents, image->getNumberOfComponents(), 0);

        }


    }


}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::lazyReadSeriesDBTest()
{
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::LAZY);

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::fwDicomIOExt::dcmtk::SeriesDBReader::sptr reader = ::fwDicomIOExt::dcmtk::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());

    //TODO: Correct the test function because GDCM reading doesn't give the same result as DCMTK reading
//    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkSeriesACHGenou( series ) );

#if (SPYLOG_LEVEL >= 4 ) // Log level info
        ::fwCore::HiResTimer timer;
        timer.start();
#endif

    ::fwComEd::helper::Image locker ( series->getImage() );

#if (SPYLOG_LEVEL >= 4 )
        timer.stop();
        OSLM_INFO( "Time in to read data : " << timer.getElapsedTimeInMilliSec() );
#endif
}

//------------------------------------------------------------------------------

}// namespace ut

} // namespace fwDicomIOExt

