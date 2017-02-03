/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DicomAnonymizerTest.hpp"

#include <fwGdcmIO/helper/DicomAnonymizer.hpp>
#include <fwGdcmIO/helper/DicomData.hpp>
#include <fwGdcmIO/helper/DicomSearch.hpp>
#include <fwGdcmIO/helper/DicomSeriesWriter.hpp>
#include <fwGdcmIO/reader/SeriesDB.hpp>
#include <fwGdcmIO/writer/Series.hpp>

#include <fwData/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwTools/System.hpp>

#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmReader.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::DicomAnonymizerTest );

namespace fwGdcmIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomAnonymizerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void DicomAnonymizerTest::anonymizeImageSeriesTest()
{
    ::fwTest::generator::Image::initRand();
    ::fwMedData::ImageSeries::sptr imgSeries;
    imgSeries = ::fwTest::generator::SeriesDB::createImageSeries();

    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "anonymizedDicomFolderTest";
    ::boost::filesystem::create_directories( path );

    // Write ImageSeries
    ::fwGdcmIO::writer::Series::sptr writer = ::fwGdcmIO::writer::Series::New();
    writer->setObject(imgSeries);
    writer->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize ImageSeries
    ::fwGdcmIO::helper::DicomAnonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(path));

    // Load ImageSeries
    ::fwMedData::SeriesDB::sptr sdb           = ::fwMedData::SeriesDB::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    ::fwMedData::ImageSeries::sptr anonymizedSeries =
        ::fwMedData::ImageSeries::dynamicCast(sdb->getContainer().front());

    CPPUNIT_ASSERT(anonymizedSeries);

    // Check values
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string("Description"), anonymizedSeries->getDescription()); // action code K
    CPPUNIT_ASSERT_EQUAL(size_t(0), anonymizedSeries->getPerformingPhysiciansName().size());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getEquipment()->getInstitutionName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED"), anonymizedSeries->getPatient()->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED"), anonymizedSeries->getPatient()->getPatientId());
    CPPUNIT_ASSERT_EQUAL(std::string("19700101"), anonymizedSeries->getPatient()->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(std::string("O"), anonymizedSeries->getPatient()->getSex());
    CPPUNIT_ASSERT_EQUAL(std::string("19700101"), anonymizedSeries->getStudy()->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string("000000.000000"), anonymizedSeries->getStudy()->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED"),
                         anonymizedSeries->getStudy()->getReferringPhysicianName());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getStudy()->getDescription());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymizedSeries->getStudy()->getPatientAge());
    CPPUNIT_ASSERT(imgSeries->getInstanceUID() != anonymizedSeries->getInstanceUID());
    CPPUNIT_ASSERT(imgSeries->getStudy()->getInstanceUID() != anonymizedSeries->getStudy()->getInstanceUID());

}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::anonymizeDICOMTest()
{
    this->testDICOMFolder(::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB/08-CT-PACS");
    //TODO Do we have to test more images ?
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::testDICOMFolder(const ::boost::filesystem::path& srcPath)
{
    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + srcPath.string() + "' does not exist",
                           ::boost::filesystem::exists(srcPath));

    // Save old UID
    m_uidContainer.clear();
    std::vector< std::string > oldFilenames;
    ::fwGdcmIO::helper::DicomSearch::searchRecursively(srcPath, oldFilenames, true);
    for(std::string filename: oldFilenames)
    {
        // Try to open the file
        ::gdcm::Reader reader;
        reader.SetFileName( filename.c_str() );
        CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \""+filename+"\"", reader.Read());
        ::gdcm::File& gdcmFile   = reader.GetFile();
        ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

        std::string data;
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x9161>(dataset);    //Concatenation UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x010D>(dataset);    //Context Group Extension Creator
                                                                                       // UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x9123>(dataset);    //Creator Version UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0018, 0x1002>(dataset);    //Device UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x9164>(dataset);    //Dimension Organization UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x300A, 0x0013>(dataset);    //Dose Reference UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x0058>(dataset);    //Failed SOP Instance UID List
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0070, 0x031A>(dataset);    //Fiducial UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x0052>(dataset);    //Frame of Reference UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x0014>(dataset);    //Instance Creator UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x3010>(dataset);    //Irradiation Event UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0028, 0x1214>(dataset);    //Large Palette Color Lookup
                                                                                       // Table UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0002, 0x0003>(dataset);    //Media Storage SOP Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0028, 0x1199>(dataset);    //Palette Color Lookup Table UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x3006, 0x0024>(dataset);    //Referenced Frame of Reference
                                                                                       // UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0040, 0x4023>(dataset);    //Referenced General Purpose
                                                                                       // Scheduled Procedure Step
                                                                                       // Transaction UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x1155>(dataset);    //Referenced SOP Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0004, 0x1511>(dataset);    //Referenced SOP Instance UID in
                                                                                       // File
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x3006, 0x00C2>(dataset);    //Related Frame of Reference UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0000, 0x1001>(dataset);    //Requested SOP Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x000E>(dataset);    //Series Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x0018>(dataset);    //SOP Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0088, 0x0140>(dataset);    //Storage Media File-set UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x000D>(dataset);    //Study Instance UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0020, 0x0200>(dataset);    //Synchronization Frame of
                                                                                       // Reference UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0040, 0xDB0D>(dataset);    //Template Extension Creator UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0040, 0xDB0C>(dataset);    //Template Extension Organization
                                                                                       // UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0008, 0x1195>(dataset);    //Transaction UID
        m_uidContainer.insert(data);
        data = ::fwGdcmIO::helper::DicomData::getTagValue<0x0040, 0xA124>(dataset);    //UID
        m_uidContainer.insert(data);
    }
    m_uidContainer.erase("");

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    // Read DicomSeries
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(srcPath);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Write DicomSeries
    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "anonymizedDicomFolderTest2";
    ::boost::filesystem::create_directories(path);
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();
    writer->setObject((*seriesDB)[0]);
    writer->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize folder
    ::fwGdcmIO::helper::DicomAnonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(path));

    // Read DICOM files
    std::vector< std::string > filenames;
    ::fwGdcmIO::helper::DicomSearch::searchRecursively(path, filenames, true);
    for(std::string filename: filenames)
    {
        this->testAnonymizedFile(filename);
    }
}

//------------------------------------------------------------------------------

std::string getDummyValue(const ::gdcm::Tag& tag)
{
    switch (::gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(tag).GetVR())
    {
        case ::gdcm::VR::AE:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::AS:
        {
            return "000Y";
        }
        case ::gdcm::VR::AT:
        {
            return "00H,00H,00H,00H";
        }
        case ::gdcm::VR::CS:
        {
            //Patient’s Sex
            if(tag == ::gdcm::Tag(0x0010, 0x0040))
            {
                return "O";
            }
            else
            {
                return "ANONYMIZED";
            }
        }
        case ::gdcm::VR::DA:
        {
            return "19700101";
        }
        case ::gdcm::VR::DS:
        {
            return "0";
        }
        case ::gdcm::VR::DT:
        {
            return "19700101000000.000000";
        }
        case ::gdcm::VR::FD:
        {
            return "0";
        }
        case ::gdcm::VR::FL:
        {
            return "0";
        }
        case ::gdcm::VR::IS:
        {
            return "0";
        }
        case ::gdcm::VR::LO:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::LT:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::OB:
        {
            return "00H00H";
        }
        case ::gdcm::VR::OF:
        {
            return "0";
        }
        case ::gdcm::VR::OW:
        {
            return "0";
        }
        case ::gdcm::VR::PN:
        {
            return "ANONYMIZED^ANONYMIZED";
        }
        case ::gdcm::VR::SH:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::SL:
        {
            return "0";
        }
        case ::gdcm::VR::SQ:
        {
            CPPUNIT_FAIL("We should not be there...");
            break;
        }
        case ::gdcm::VR::SS:
        {
            return "0";
        }
        case ::gdcm::VR::ST:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::TM:
        {
            return "000000.000000";
        }
        case ::gdcm::VR::UI:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::UL:
        {
            return "0";
        }
        case ::gdcm::VR::UN:
        {
            return "ANONYMIZED";
        }
        case ::gdcm::VR::US:
        {
            return "0";
        }
        case ::gdcm::VR::UT:
        {
            return "ANONYMIZED";
        }
        default:
        {
            CPPUNIT_FAIL("Unknown value representation.");
            break;
        }
    }

    return "";
}

//------------------------------------------------------------------------------

template< uint16_t GROUP, uint16_t ELEMENT >
void processTag(const ::gdcm::DataSet& dataset, const std::string& actionCode,
                const std::set< std::string >& uidContainer)
{
    // Retrieve tag value
    const std::string& dataStr = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue<GROUP, ELEMENT>(dataset);

    //X – remove
    //X/Z – X unless Z is required to maintain IOD conformance (Type 3 versus Type 2)
    //X/D – X unless D is required to maintain IOD conformance (Type 3 versus Type 1)
    //X/Z/D – X unless Z or D is required to maintain IOD conformance (Type 3 versus Type 2 versus Type 1)
    //X/Z/U* - X unless Z or replacement of contained instance UIDs (U) is required to maintain IOD conformance
    if(actionCode == "X" || actionCode == "X/Z" || actionCode == "X/D" || actionCode == "X/Z/D" ||
       actionCode == "X/Z/U*")
    {
        CPPUNIT_ASSERT(!dataset.FindDataElement(::gdcm::Tag(GROUP, ELEMENT)) || dataStr.empty());
    }
    //Z – replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with the
    // VR
    //D – replace with a non-zero length value that may be a dummy value and consistent with the VR
    //Z/D – Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
    else if(actionCode == "Z" || actionCode == "D" || actionCode == "Z/D")
    {
        if(::gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(::gdcm::Tag(GROUP,
                                                                                             ELEMENT)).GetVR() ==
           ::gdcm::VR::SQ)
        {
            CPPUNIT_ASSERT(!dataset.FindDataElement(::gdcm::Tag(GROUP, ELEMENT)) || dataStr.empty());
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(getDummyValue(::gdcm::Tag(GROUP, ELEMENT)), dataStr);
        }
    }
    //K – keep (unchanged for non-sequence attributes, cleaned for sequences)
    else if(actionCode == "K")
    {
        if(::gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(::gdcm::Tag(GROUP,
                                                                                             ELEMENT)).GetVR() ==
           ::gdcm::VR::SQ)
        {
            CPPUNIT_ASSERT(!dataset.FindDataElement(::gdcm::Tag(GROUP, ELEMENT)) || dataStr.empty());
        }
    }
    //C – clean, that is replace with values of similar meaning known not to contain identifying information and
    // consistent with the VR
    else if(actionCode == "C")
    {
        CPPUNIT_FAIL("We should not be there...");
    }
    //U – replace with a non-zero length UID that is internally consistent within a set of Instances
    else if(actionCode == "U")
    {
        // Check that the new UID is not in the old series
        CPPUNIT_ASSERT(uidContainer.find(dataStr) == uidContainer.end());
    }
}
//------------------------------------------------------------------------------

void DicomAnonymizerTest::testAnonymizedFile(const std::string& filename)
{
    // Try to open the file
    ::gdcm::Reader reader;
    reader.SetFileName( filename.c_str() );
    CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \""+filename+"\"", reader.Read());
    ::gdcm::File& gdcmFile   = reader.GetFile();
    ::gdcm::DataSet& dataset = gdcmFile.GetDataSet();

    processTag<0x0008, 0x0050>(dataset, "Z", m_uidContainer);        //Accession Number
    processTag<0x0018, 0x4000>(dataset, "X", m_uidContainer);        //Acquisition Comments
    processTag<0x0040, 0x0555>(dataset, "X", m_uidContainer);        //Acquisition Context Sequence
    processTag<0x0008, 0x0022>(dataset, "X/Z", m_uidContainer);      //Acquisition Date
    processTag<0x0008, 0x002A>(dataset, "X/D", m_uidContainer);      //Acquisition DateTime
    processTag<0x0018, 0x1400>(dataset, "X/D", m_uidContainer);      //Acquisition Device Processing Description
    processTag<0x0018, 0x9424>(dataset, "X", m_uidContainer);        //Acquisition Protocol Description
    processTag<0x0008, 0x0032>(dataset, "X/Z", m_uidContainer);      //Acquisition Time
    processTag<0x0040, 0x4035>(dataset, "X", m_uidContainer);        //Actual Human Performers Sequence
    processTag<0x0010, 0x21B0>(dataset, "X", m_uidContainer);        //Additional Patient’s History
    processTag<0x0038, 0x0010>(dataset, "X", m_uidContainer);        //Admission ID
    processTag<0x0038, 0x0020>(dataset, "X", m_uidContainer);        //Admitting Date
    processTag<0x0008, 0x1084>(dataset, "X", m_uidContainer);        //Admitting Diagnoses Code Sequence
    processTag<0x0008, 0x1080>(dataset, "X", m_uidContainer);        //Admitting Diagnoses Description
    processTag<0x0038, 0x0021>(dataset, "X", m_uidContainer);        //Admitting Time
    processTag<0x0000, 0x1000>(dataset, "X", m_uidContainer);        //Affected SOP Instance UID
    processTag<0x0010, 0x2110>(dataset, "X", m_uidContainer);        //Allergies
    processTag<0x4000, 0x0010>(dataset, "X", m_uidContainer);        //Arbitrary
    processTag<0x0040, 0xA078>(dataset, "X", m_uidContainer);        //Author Observer Sequence
    processTag<0x0010, 0x1081>(dataset, "X", m_uidContainer);        //Branch of Service
    processTag<0x0018, 0x1007>(dataset, "X", m_uidContainer);        //Cassette ID
    processTag<0x0040, 0x0280>(dataset, "X", m_uidContainer);        //Comments on Performed Procedure Step
    processTag<0x0020, 0x9161>(dataset, "U", m_uidContainer);        //Concatenation UID
    processTag<0x0040, 0x3001>(dataset, "X", m_uidContainer);        //Confidentiality Constraint on Patient Data
                                                                     // Description
    processTag<0x0070, 0x0084>(dataset, "Z", m_uidContainer);        //Content Creator’s Name
    processTag<0x0070, 0x0086>(dataset, "X", m_uidContainer);        //Content Creator’s Identification Code Sequence
    processTag<0x0008, 0x0023>(dataset, "Z/D", m_uidContainer);      //Content Date
    processTag<0x0040, 0xA730>(dataset, "X", m_uidContainer);        //Content Sequence
    processTag<0x0008, 0x0033>(dataset, "Z/D", m_uidContainer);      //Content Time
    processTag<0x0008, 0x010D>(dataset, "U", m_uidContainer);        //Context Group Extension Creator UID
    processTag<0x0018, 0x0010>(dataset, "Z/D", m_uidContainer);      //Contrast Bolus Agent
    processTag<0x0018, 0xA003>(dataset, "X", m_uidContainer);        //Contribution Description
    processTag<0x0010, 0x2150>(dataset, "X", m_uidContainer);        //Country of Residence
    processTag<0x0008, 0x9123>(dataset, "U", m_uidContainer);        //Creator Version UID
    processTag<0x0038, 0x0300>(dataset, "X", m_uidContainer);        //Current Patient Location
    processTag<0x0008, 0x0025>(dataset, "X", m_uidContainer);        //Curve Date
    processTag<0x0008, 0x0035>(dataset, "X", m_uidContainer);        //Curve Time
    processTag<0x0040, 0xA07C>(dataset, "X", m_uidContainer);        //Custodial Organization Sequence
    processTag<0xFFFC, 0xFFFC>(dataset, "X", m_uidContainer);        //Data Set Trailing Padding
    processTag<0x0008, 0x2111>(dataset, "X", m_uidContainer);        //Derivation Description
    processTag<0x0018, 0x700A>(dataset, "X", m_uidContainer);        //Detector ID
    processTag<0x0018, 0x1000>(dataset, "X/Z/D", m_uidContainer);    //Device Serial Number
    processTag<0x0018, 0x1002>(dataset, "U", m_uidContainer);        //Device UID
    processTag<0x0400, 0x0100>(dataset, "X", m_uidContainer);        //Digital Signature UID
    processTag<0xFFFA, 0xFFFA>(dataset, "X", m_uidContainer);        //Digital Signatures Sequence
    processTag<0x0020, 0x9164>(dataset, "U", m_uidContainer);        //Dimension Organization UID
    processTag<0x0038, 0x0040>(dataset, "X", m_uidContainer);        //Discharge Diagnosis Description
    processTag<0x4008, 0x011A>(dataset, "X", m_uidContainer);        //Distribution Address
    processTag<0x4008, 0x0119>(dataset, "X", m_uidContainer);        //Distribution Name
    processTag<0x300A, 0x0013>(dataset, "U", m_uidContainer);        //Dose Reference UID
    processTag<0x0010, 0x2160>(dataset, "X", m_uidContainer);        //Ethnic Group
    processTag<0x0008, 0x0058>(dataset, "U", m_uidContainer);        //Failed SOP Instance UID List
    processTag<0x0070, 0x031A>(dataset, "U", m_uidContainer);        //Fiducial UID
    processTag<0x0040, 0x2017>(dataset, "Z", m_uidContainer);        //Filler Order Number of Imaging Service Request
    processTag<0x0020, 0x9158>(dataset, "X", m_uidContainer);        //Frame Comments
    processTag<0x0020, 0x0052>(dataset, "U", m_uidContainer);        //Frame of Reference UID
    processTag<0x0018, 0x1008>(dataset, "X", m_uidContainer);        //Gantry ID
    processTag<0x0018, 0x1005>(dataset, "X", m_uidContainer);        //Generator ID
    processTag<0x0070, 0x0001>(dataset, "D", m_uidContainer);        //Graphic Annotation Sequence
    processTag<0x0040, 0x4037>(dataset, "X", m_uidContainer);        //Human Performers Name
    processTag<0x0040, 0x4036>(dataset, "X", m_uidContainer);        //Human Performers Organization
    processTag<0x0088, 0x0200>(dataset, "X", m_uidContainer);        //Icon Image Sequence(see Note 12)
    processTag<0x0008, 0x4000>(dataset, "X", m_uidContainer);        //Identifying Comments
    processTag<0x0020, 0x4000>(dataset, "X", m_uidContainer);        //Image Comments
    processTag<0x0028, 0x4000>(dataset, "X", m_uidContainer);        //Image Presentation Comments
    processTag<0x0040, 0x2400>(dataset, "X", m_uidContainer);        //Imaging Service Request Comments
    processTag<0x4008, 0x0300>(dataset, "X", m_uidContainer);        //Impressions
    processTag<0x0008, 0x0014>(dataset, "U", m_uidContainer);        //Instance Creator UID
    processTag<0x0008, 0x0081>(dataset, "X", m_uidContainer);        //Institution Address
    processTag<0x0008, 0x0082>(dataset, "X/Z/D", m_uidContainer);    //Institution Code Sequence
    processTag<0x0008, 0x0080>(dataset, "X/Z/D", m_uidContainer);    //Institution Name
    processTag<0x0008, 0x1040>(dataset, "X", m_uidContainer);        //Institutional Department Name
    processTag<0x0010, 0x1050>(dataset, "X", m_uidContainer);        //Insurance Plan Identification
    processTag<0x0040, 0x1011>(dataset, "X", m_uidContainer);        //Intended Recipients of Results Identification
                                                                     // Sequence
    processTag<0x4008, 0x0111>(dataset, "X", m_uidContainer);        //Interpretation Approver Sequence
    processTag<0x4008, 0x010C>(dataset, "X", m_uidContainer);        //Interpretation Author
    processTag<0x4008, 0x0115>(dataset, "X", m_uidContainer);        //Interpretation Diagnosis Description
    processTag<0x4008, 0x0202>(dataset, "X", m_uidContainer);        //Interpretation ID Issuer
    processTag<0x4008, 0x0102>(dataset, "X", m_uidContainer);        //Interpretation Recorder
    processTag<0x4008, 0x010B>(dataset, "X", m_uidContainer);        //Interpretation Text
    processTag<0x4008, 0x010A>(dataset, "X", m_uidContainer);        //Interpretation Transcriber
    processTag<0x0008, 0x3010>(dataset, "U", m_uidContainer);        //Irradiation Event UID
    processTag<0x0038, 0x0011>(dataset, "X", m_uidContainer);        //Issuer of Admission ID
    processTag<0x0010, 0x0021>(dataset, "X", m_uidContainer);        //Issuer of Patient ID
    processTag<0x0038, 0x0061>(dataset, "X", m_uidContainer);        //Issuer of Service Episode ID
    processTag<0x0028, 0x1214>(dataset, "U", m_uidContainer);        //Large Palette Color Lookup Table UID
    processTag<0x0010, 0x21D0>(dataset, "X", m_uidContainer);        //Last Menstrual Date
    processTag<0x0400, 0x0404>(dataset, "X", m_uidContainer);        //MAC
    processTag<0x0002, 0x0003>(dataset, "U", m_uidContainer);        //Media Storage SOP Instance UID
    processTag<0x0010, 0x2000>(dataset, "X", m_uidContainer);        //Medical Alerts
    processTag<0x0010, 0x1090>(dataset, "X", m_uidContainer);        //Medical Record Locator
    processTag<0x0010, 0x1080>(dataset, "X", m_uidContainer);        //Military Rank
    processTag<0x0400, 0x0550>(dataset, "X", m_uidContainer);        //Modified Attributes Sequence
    processTag<0x0020, 0x3406>(dataset, "X", m_uidContainer);        //Modified Image Description
    processTag<0x0020, 0x3401>(dataset, "X", m_uidContainer);        //Modifying Device ID
    processTag<0x0020, 0x3404>(dataset, "X", m_uidContainer);        //Modifying Device Manufacturer
    processTag<0x0008, 0x1060>(dataset, "X", m_uidContainer);        //Name of Physician(s) Reading Study
    processTag<0x0040, 0x1010>(dataset, "X", m_uidContainer);        //Names of Intended Recipient of Results
    processTag<0x0010, 0x2180>(dataset, "X", m_uidContainer);        //Occupation
    processTag<0x0008, 0x1072>(dataset, "X/D", m_uidContainer);      //Operators’ Identification Sequence
    processTag<0x0008, 0x1070>(dataset, "X/Z/D", m_uidContainer);    //Operators’ Name
    processTag<0x0400, 0x0561>(dataset, "X", m_uidContainer);        //Original Attributes Sequence
    processTag<0x0040, 0x2010>(dataset, "X", m_uidContainer);        //Order Callback Phone Number
    processTag<0x0040, 0x2008>(dataset, "X", m_uidContainer);        //Order Entered By
    processTag<0x0040, 0x2009>(dataset, "X", m_uidContainer);        //Order Enterer Location
    processTag<0x0010, 0x1000>(dataset, "X", m_uidContainer);        //Other Patient IDs
    processTag<0x0010, 0x1002>(dataset, "X", m_uidContainer);        //Other Patient IDs Sequence
    processTag<0x0010, 0x1001>(dataset, "X", m_uidContainer);        //Other Patient Names
    processTag<0x0008, 0x0024>(dataset, "X", m_uidContainer);        //Overlay Date
    processTag<0x0008, 0x0034>(dataset, "X", m_uidContainer);        //Overlay Time
    processTag<0x0028, 0x1199>(dataset, "U", m_uidContainer);        //Palette Color Lookup Table UID
    processTag<0x0040, 0xA07A>(dataset, "X", m_uidContainer);        //Participant Sequence
    processTag<0x0010, 0x1040>(dataset, "X", m_uidContainer);        //Patient Address
    processTag<0x0010, 0x4000>(dataset, "X", m_uidContainer);        //Patient Comments
    processTag<0x0010, 0x0020>(dataset, "Z", m_uidContainer);        //Patient ID
    processTag<0x0010, 0x2203>(dataset, "X/Z", m_uidContainer);      //Patient Sex Neutered
    processTag<0x0038, 0x0500>(dataset, "X", m_uidContainer);        //Patient State
    processTag<0x0040, 0x1004>(dataset, "X", m_uidContainer);        //Patient Transport Arrangements
    processTag<0x0010, 0x1010>(dataset, "X", m_uidContainer);        //Patient’s Age
    processTag<0x0010, 0x0030>(dataset, "Z", m_uidContainer);        //Patient’s Birth Date
    processTag<0x0010, 0x1005>(dataset, "X", m_uidContainer);        //Patient’s Birth Name
    processTag<0x0010, 0x0032>(dataset, "X", m_uidContainer);        //Patient’s Birth Time
    processTag<0x0038, 0x0400>(dataset, "X", m_uidContainer);        //Patient’s Institution Residence
    processTag<0x0010, 0x0050>(dataset, "X", m_uidContainer);        //Patient’s Insurance Plan Code Sequence
    processTag<0x0010, 0x1060>(dataset, "X", m_uidContainer);        //Patient’s Mother’s Birth Name
    processTag<0x0010, 0x0010>(dataset, "Z", m_uidContainer);        //Patient’s Name
    processTag<0x0010, 0x0101>(dataset, "X", m_uidContainer);        //Patient’s Primary Language Code Sequence
    processTag<0x0010, 0x0102>(dataset, "X", m_uidContainer);        //Patient’s Primary Language Modifier Code Sequence
    processTag<0x0010, 0x21F0>(dataset, "X", m_uidContainer);        //Patient’s Religious Preference
    processTag<0x0010, 0x0040>(dataset, "Z", m_uidContainer);        //Patient’s Sex
    processTag<0x0010, 0x1020>(dataset, "X", m_uidContainer);        //Patient’s Size
    processTag<0x0010, 0x2154>(dataset, "X", m_uidContainer);        //Patient’s Telephone Number
    processTag<0x0010, 0x1030>(dataset, "X", m_uidContainer);        //Patient’s Weight
    processTag<0x0040, 0x0243>(dataset, "X", m_uidContainer);        //Performed Location
    processTag<0x0040, 0x0254>(dataset, "X", m_uidContainer);        //Performed Procedure Step Description
    processTag<0x0040, 0x0253>(dataset, "X", m_uidContainer);        //Performed Procedure Step ID
    processTag<0x0040, 0x0244>(dataset, "X", m_uidContainer);        //Performed Procedure Step Start Date
    // processTag<0x0040,0x0245>(dataset, "X", m_uidContainer);        //Performed Procedure Step Start Time
    processTag<0x0040, 0x0241>(dataset, "X", m_uidContainer);        //Performed Station AE Title
    processTag<0x0040, 0x4030>(dataset, "X", m_uidContainer);        //Performed Station Geographic Location Code
                                                                     // Sequence
    processTag<0x0040, 0x0242>(dataset, "X", m_uidContainer);        //Performed Station Name
    // processTag<0x0040,0x0248>(dataset, "X", m_uidContainer);        //Performed Station Name Code Sequence
    processTag<0x0008, 0x1052>(dataset, "X", m_uidContainer);        //Performing Physicians’ Identification Sequence
    processTag<0x0008, 0x1050>(dataset, "X", m_uidContainer);        //Performing Physicians’ Name
    processTag<0x0040, 0x1102>(dataset, "X", m_uidContainer);        //Person Address
    processTag<0x0040, 0x1101>(dataset, "D", m_uidContainer);        //Person Identification Code Sequence
    processTag<0x0040, 0xA123>(dataset, "D", m_uidContainer);        //Person Name
    processTag<0x0040, 0x1103>(dataset, "X", m_uidContainer);        //Person Telephone Numbers
    processTag<0x4008, 0x0114>(dataset, "X", m_uidContainer);        //Physician Approving Interpretation
    processTag<0x0008, 0x1062>(dataset, "X", m_uidContainer);        //Physician Reading Study Identification Sequence
    processTag<0x0008, 0x1048>(dataset, "X", m_uidContainer);        //Physician(s) Record
    processTag<0x0008, 0x1049>(dataset, "X", m_uidContainer);        //Physician(s) of Record Identification Sequence
    processTag<0x0040, 0x2016>(dataset, "Z", m_uidContainer);        //Placer Order Number of Imaging Service Request
    processTag<0x0018, 0x1004>(dataset, "X", m_uidContainer);        //Plate ID
    processTag<0x0040, 0x0012>(dataset, "X", m_uidContainer);        //Pre-Medication
    processTag<0x0010, 0x21C0>(dataset, "X", m_uidContainer);        //Pregnancy Status
    processTag<0x0018, 0x1030>(dataset, "X/D", m_uidContainer);      //Protocol Name
    processTag<0x0040, 0x2001>(dataset, "X", m_uidContainer);        //Reason for Imaging Service Request
    processTag<0x0032, 0x1030>(dataset, "X", m_uidContainer);        //Reason for Study
    processTag<0x0400, 0x0402>(dataset, "X", m_uidContainer);        //Referenced Digital Signature Sequence
    processTag<0x3006, 0x0024>(dataset, "U", m_uidContainer);        //Referenced Frame of Reference UID
    processTag<0x0040, 0x4023>(dataset, "U", m_uidContainer);        //Referenced General Purpose Scheduled Procedure
                                                                     // Step Transaction UID
    processTag<0x0008, 0x1140>(dataset, "X/Z/U*", m_uidContainer);   //Referenced Image Sequence
    // processTag<0x0038,0x1234>(dataset, "X", m_uidContainer);        //Referenced Patient Alias Sequence
    processTag<0x0008, 0x1120>(dataset, "X", m_uidContainer);        //Referenced Patient Sequence
    processTag<0x0008, 0x1111>(dataset, "X/Z/D", m_uidContainer);    //Referenced Performed Procedure Step Sequence
    processTag<0x0400, 0x0403>(dataset, "X", m_uidContainer);        //Referenced SOP Instance MAC Sequence
    processTag<0x0008, 0x1155>(dataset, "U", m_uidContainer);        //Referenced SOP Instance UID
    processTag<0x0004, 0x1511>(dataset, "U", m_uidContainer);        //Referenced SOP Instance UID in File
    processTag<0x0008, 0x1110>(dataset, "X/Z", m_uidContainer);      //Referenced Study Sequence
    processTag<0x0008, 0x0092>(dataset, "X", m_uidContainer);        //Referring Physician’s Address
    processTag<0x0008, 0x0096>(dataset, "X", m_uidContainer);        //Referring Physician’s Identification Sequence
    processTag<0x0008, 0x0090>(dataset, "Z", m_uidContainer);        //Referring Physician’s Name
    processTag<0x0008, 0x0094>(dataset, "X", m_uidContainer);        //Referring Physician’s Telephone Numbers
    processTag<0x0010, 0x2152>(dataset, "X", m_uidContainer);        //Region of Residence
    processTag<0x3006, 0x00C2>(dataset, "U", m_uidContainer);        //Related Frame of Reference UID
    processTag<0x0040, 0x0275>(dataset, "X", m_uidContainer);        //Request Attributes Sequence
    processTag<0x0032, 0x1070>(dataset, "X", m_uidContainer);        //Requested Contrast Agent
    processTag<0x0040, 0x1400>(dataset, "X", m_uidContainer);        //Requested Procedure Comments
    processTag<0x0032, 0x1060>(dataset, "X/Z", m_uidContainer);      //Requested Procedure Description
    processTag<0x0040, 0x1001>(dataset, "X", m_uidContainer);        //Requested Procedure ID
    processTag<0x0040, 0x1005>(dataset, "X", m_uidContainer);        //Requested Procedure Location
    processTag<0x0000, 0x1001>(dataset, "U", m_uidContainer);        //Requested SOP Instance UID
    processTag<0x0032, 0x1032>(dataset, "X", m_uidContainer);        //Requesting Physician
    processTag<0x0032, 0x1033>(dataset, "X", m_uidContainer);        //Requesting Service
    processTag<0x0010, 0x2299>(dataset, "X", m_uidContainer);        //Responsible Organization
    processTag<0x0010, 0x2297>(dataset, "X", m_uidContainer);        //Responsible Person
    processTag<0x4008, 0x4000>(dataset, "X", m_uidContainer);        //Results Comments
    processTag<0x4008, 0x0118>(dataset, "X", m_uidContainer);        //Results Distribution List Sequence
    processTag<0x4008, 0x0042>(dataset, "X", m_uidContainer);        //Results ID Issuer
    processTag<0x300E, 0x0008>(dataset, "X/Z", m_uidContainer);      //Reviewer Name
    processTag<0x0040, 0x4034>(dataset, "X", m_uidContainer);        //Scheduled Human Performers Sequence
    processTag<0x0038, 0x001E>(dataset, "X", m_uidContainer);        //Scheduled Patient Institution Residence
    processTag<0x0040, 0x000B>(dataset, "X", m_uidContainer);        //Scheduled Performing Physician Identification
                                                                     // Sequence
    processTag<0x0040, 0x0006>(dataset, "X", m_uidContainer);        //Scheduled Performing Physician Name
    processTag<0x0040, 0x0004>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step End Date
    processTag<0x0040, 0x0005>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step End Time
    processTag<0x0040, 0x0007>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step Description
    processTag<0x0040, 0x0011>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step Location
    processTag<0x0040, 0x0002>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step Start Date
    processTag<0x0040, 0x0003>(dataset, "X", m_uidContainer);        //Scheduled Procedure Step Start Time
    processTag<0x0040, 0x0001>(dataset, "X", m_uidContainer);        //Scheduled Station AE Title
    processTag<0x0040, 0x4027>(dataset, "X", m_uidContainer);        //Scheduled Station Geographic Location Code
                                                                     // Sequence
    processTag<0x0040, 0x0010>(dataset, "X", m_uidContainer);        //Scheduled Station Name
    processTag<0x0040, 0x4025>(dataset, "X", m_uidContainer);        //Scheduled Station Name Code Sequence
    processTag<0x0032, 0x1020>(dataset, "X", m_uidContainer);        //Scheduled Study Location
    processTag<0x0032, 0x1021>(dataset, "X", m_uidContainer);        //Scheduled Study Location AE Title
    processTag<0x0008, 0x0021>(dataset, "X/D", m_uidContainer);      //Series Date
    processTag<0x0008, 0x103E>(dataset, "X", m_uidContainer);        //Series Description
    processTag<0x0020, 0x000E>(dataset, "U", m_uidContainer);        //Series Instance UID
    processTag<0x0008, 0x0031>(dataset, "X/D", m_uidContainer);      //Series Time
    processTag<0x0038, 0x0062>(dataset, "X", m_uidContainer);        //Service Episode Description
    processTag<0x0038, 0x0060>(dataset, "X", m_uidContainer);        //Service Episode ID
    processTag<0x0010, 0x21A0>(dataset, "X", m_uidContainer);        //Smoking Status
    processTag<0x0008, 0x0018>(dataset, "U", m_uidContainer);        //SOP Instance UID
    processTag<0x0008, 0x2112>(dataset, "X/Z/U*", m_uidContainer);   //Source Image Sequence
    processTag<0x0038, 0x0050>(dataset, "X", m_uidContainer);        //Special Needs
    processTag<0x0008, 0x1010>(dataset, "X/Z/D", m_uidContainer);    //Station Name
    processTag<0x0088, 0x0140>(dataset, "U", m_uidContainer);        //Storage Media File-set UID
    processTag<0x0032, 0x4000>(dataset, "X", m_uidContainer);        //Study Comments
    processTag<0x0008, 0x0020>(dataset, "Z", m_uidContainer);        //Study Date
    processTag<0x0008, 0x1030>(dataset, "X", m_uidContainer);        //Study Description
    processTag<0x0020, 0x0010>(dataset, "Z", m_uidContainer);        //Study ID
    processTag<0x0032, 0x0012>(dataset, "X", m_uidContainer);        //Study ID Issuer
    processTag<0x0020, 0x000D>(dataset, "U", m_uidContainer);        //Study Instance UID
    processTag<0x0008, 0x0030>(dataset, "Z", m_uidContainer);        //Study Time
    processTag<0x0020, 0x0200>(dataset, "U", m_uidContainer);        //Synchronization Frame of Reference UID
    processTag<0x0040, 0xDB0D>(dataset, "U", m_uidContainer);        //Template Extension Creator UID
    processTag<0x0040, 0xDB0C>(dataset, "U", m_uidContainer);        //Template Extension Organization UID
    processTag<0x4000, 0x4000>(dataset, "X", m_uidContainer);        //Text Comments
    processTag<0x2030, 0x0020>(dataset, "X", m_uidContainer);        //Text String
    processTag<0x0008, 0x0201>(dataset, "X", m_uidContainer);        //Timezone Offset From UTC
    processTag<0x0088, 0x0910>(dataset, "X", m_uidContainer);        //Topic Author
    // processTag<0x0088,0x0912>(dataset, "X", m_uidContainer);        //Topic Key Words
    processTag<0x0088, 0x0906>(dataset, "X", m_uidContainer);        //Topic Subject
    processTag<0x0088, 0x0904>(dataset, "X", m_uidContainer);        //Topic Title
    processTag<0x0008, 0x1195>(dataset, "U", m_uidContainer);        //Transaction UID
    processTag<0x0040, 0xA124>(dataset, "U", m_uidContainer);        //UID
    processTag<0x0040, 0xA088>(dataset, "Z", m_uidContainer);        //Verifying Observer Identification Code Sequence
    processTag<0x0040, 0xA075>(dataset, "D", m_uidContainer);        //Verifying Observer Name
    processTag<0x0040, 0xA073>(dataset, "D", m_uidContainer);        //Verifying Observer Sequence
    processTag<0x0040, 0xA027>(dataset, "X", m_uidContainer);        //Verifying Organization
    processTag<0x0038, 0x4000>(dataset, "X", m_uidContainer);        //Visit Comments

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwGdcmIO
