/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "DicomAnonymizerTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <io/dicom/helper/DicomAnonymizer.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>
#include <io/dicom/helper/DicomSearch.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>
#include <io/dicom/reader/series_set.hpp>
#include <io/dicom/writer/series.hpp>

#include <utest/Filter.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>
#include <utest_data/generator/series_set.hpp>

#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmReader.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::DicomAnonymizerTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void DicomAnonymizerTest::setUp()
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

void DicomAnonymizerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void DicomAnonymizerTest::anonymizeImageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    data::image_series::sptr img_series;
    img_series = utest_data::generator::series_set::createImageSeries();

    core::os::temp_dir tmp_dir;

    // Write ImageSeries
    io::dicom::writer::series::sptr writer = std::make_shared<io::dicom::writer::series>();
    writer->set_object(img_series);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize ImageSeries
    io::dicom::helper::DicomAnonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(tmp_dir));

    // Load ImageSeries
    auto series_set = std::make_shared<data::series_set>();
    auto reader     = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Check series
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    auto anonymized_series =
        std::dynamic_pointer_cast<data::image_series>(series_set->front());

    CPPUNIT_ASSERT(anonymized_series);

    // Check values
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->getSeriesDate());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->getSeriesTime());
    CPPUNIT_ASSERT_EQUAL(std::string("Description"), anonymized_series->getSeriesDescription()); // action code K
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), anonymized_series->getPerformingPhysicianName().size());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->getInstitutionName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED"), anonymized_series->getPatientName());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED"), anonymized_series->getPatientID());
    CPPUNIT_ASSERT_EQUAL(std::string("19000101"), anonymized_series->getPatientBirthDate());
    CPPUNIT_ASSERT_EQUAL(std::string("O"), anonymized_series->getPatientSex());
    CPPUNIT_ASSERT_EQUAL(std::string("19000101"), anonymized_series->getStudyDate());
    CPPUNIT_ASSERT_EQUAL(std::string("000000.000000"), anonymized_series->getStudyTime());
    CPPUNIT_ASSERT_EQUAL(
        std::string("ANONYMIZED^ANONYMIZED"),
        anonymized_series->getReferringPhysicianName()
    );
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->getStudyDescription());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->getPatientAge());
    CPPUNIT_ASSERT(img_series->getSeriesInstanceUID() != anonymized_series->getSeriesInstanceUID());
    CPPUNIT_ASSERT(img_series->getStudyInstanceUID() != anonymized_series->getStudyInstanceUID());
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::anonymizeDICOMTest()
{
    if(!utest::Filter::ignoreSlowTests())
    {
        //TODO Do we have to test more images ?
        this->testDICOMFolder(utest_data::Data::dir() / "sight/Patient/Dicom/DicomDB/08-CT-PACS");
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::testDICOMFolder(const std::filesystem::path& _src_path)
{
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + _src_path.string() + "' does not exist",
        std::filesystem::exists(_src_path)
    );

    // Save old UID
    m_uidContainer.clear();
    std::vector<std::filesystem::path> old_filenames;
    io::dicom::helper::DicomSearch::searchRecursively(_src_path, old_filenames, true);
    for(const std::filesystem::path& filename : old_filenames)
    {
        // Try to open the file
        gdcm::Reader reader;
        reader.SetFileName(filename.string().c_str());
        CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \"" + filename.string() + "\"", reader.Read());
        gdcm::File& gdcm_file  = reader.GetFile();
        gdcm::DataSet& dataset = gdcm_file.GetDataSet();

        std::string data;
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x9161>(dataset); //Concatenation UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x010D>(dataset); //Context Group Extension
                                                                                         // Creator
        // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x9123>(dataset); //Creator Version UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1002>(dataset); //Device UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x9164>(dataset); //Dimension Organization
                                                                                         // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x300A, 0x0013>(dataset); //Dose Reference UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0058>(dataset); //Failed SOP Instance UID
                                                                                         // List
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x031A>(dataset); //Fiducial UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0052>(dataset); //Frame of Reference UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0014>(dataset); //Instance Creator UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x3010>(dataset); //Irradiation Event UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0028, 0x1214>(dataset); //Large Palette Color
                                                                                         // Lookup
        // Table UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0002, 0x0003>(dataset); //Media Storage SOP
                                                                                         // Instance UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0028, 0x1199>(dataset); //Palette Color Lookup
                                                                                         // Table UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x3006, 0x0024>(dataset); //Referenced Frame of
                                                                                         // Reference
        // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x4023>(dataset); //Referenced General
                                                                                         // Purpose
        // Scheduled Procedure Step
        // Transaction UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1155>(dataset); //Referenced SOP Instance
                                                                                         // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0004, 0x1511>(dataset); //Referenced SOP Instance
                                                                                         // UID in
        // File
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x3006, 0x00C2>(dataset); //Related Frame of
                                                                                         // Reference UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0000, 0x1001>(dataset); //Requested SOP Instance
                                                                                         // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000E>(dataset); //Series Instance UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0018>(dataset); //SOP Instance UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0088, 0x0140>(dataset); //Storage Media File-set
                                                                                         // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000D>(dataset); //Study Instance UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0200>(dataset); //Synchronization Frame of
        // Reference UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xDB0D>(dataset); //Template Extension
                                                                                         // Creator UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xDB0C>(dataset); //Template Extension
                                                                                         // Organization
        // UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1195>(dataset); //Transaction UID
        m_uidContainer.insert(data);
        data = io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0xA124>(dataset); //UID
        m_uidContainer.insert(data);
    }

    m_uidContainer.erase("");

    auto series_set = std::make_shared<data::series_set>();

    // Read DicomSeries
    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(_src_path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Write DicomSeries
    core::os::temp_dir tmp_dir;
    auto writer = std::make_shared<io::dicom::helper::DicomSeriesWriter>();
    writer->set_object((*series_set)[0]);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize folder
    io::dicom::helper::DicomAnonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(tmp_dir));

    // Read DICOM files
    std::vector<std::filesystem::path> filenames;
    io::dicom::helper::DicomSearch::searchRecursively(tmp_dir, filenames, true);
    for(const std::filesystem::path& filename : filenames)
    {
        this->testAnonymizedFile(filename);
    }
}

//------------------------------------------------------------------------------

std::string get_dummy_value(const gdcm::Tag& _tag)
{
    switch(gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(_tag).GetVR())
    {
        case gdcm::VR::AE:
            return "ANONYMIZED";

        case gdcm::VR::AS:
            return "000Y";

        case gdcm::VR::AT:
            return "00H,00H,00H,00H";

        case gdcm::VR::CS:
            //Patient's Sex
            if(_tag == gdcm::Tag(0x0010, 0x0040))
            {
                return "O";
            }
            else
            {
                return "ANONYMIZED";
            }

        case gdcm::VR::DA:
            return "19000101";

        case gdcm::VR::DS:
            return "0";

        case gdcm::VR::DT:
            return "19000101000000.000000";

        case gdcm::VR::FD:
        case gdcm::VR::FL:
        case gdcm::VR::IS:
            return "0";

        case gdcm::VR::LO:
        case gdcm::VR::LT:
            return "ANONYMIZED";

        case gdcm::VR::OB:
            return "00H00H";

        case gdcm::VR::OF:
        case gdcm::VR::OW:
            return "0";

        case gdcm::VR::PN:
            return "ANONYMIZED^ANONYMIZED";

        case gdcm::VR::SH:
            return "ANONYMIZED";

        case gdcm::VR::SL:
            return "0";

        case gdcm::VR::SQ:
            CPPUNIT_FAIL("We should not be there...");
            break;

        case gdcm::VR::SS:
            return "0";

        case gdcm::VR::ST:
            return "ANONYMIZED";

        case gdcm::VR::TM:
            return "000000.000000";

        case gdcm::VR::UI:
            return "ANONYMIZED";

        case gdcm::VR::UL:
            return "0";

        case gdcm::VR::UN:
            return "ANONYMIZED";

        case gdcm::VR::US:
            return "0";

        case gdcm::VR::UT:
            return "ANONYMIZED";

        default:
            CPPUNIT_FAIL("Unknown value representation.");
            break;
    }
}

//------------------------------------------------------------------------------

template<uint16_t GROUP, uint16_t ELEMENT>
void process_tag(
    const gdcm::DataSet& _dataset,
    const std::string& _action_code,
    const std::set<std::string>& _uid_container
)
{
    // Retrieve tag value
    const std::string& data_str = io::dicom::helper::DicomDataReader::getTagValue<GROUP, ELEMENT>(_dataset);

    //X - remove
    //X/Z - X unless Z is required to maintain IOD conformance (Type 3 versus Type 2)
    //X/D - X unless D is required to maintain IOD conformance (Type 3 versus Type 1)
    //X/Z/D - X unless Z or D is required to maintain IOD conformance (Type 3 versus Type 2 versus Type 1)
    //X/Z/U* - X unless Z or replacement of contained instance UIDs (U) is required to maintain IOD conformance
    if(_action_code == "X" || _action_code == "X/Z" || _action_code == "X/D" || _action_code == "X/Z/D"
       || _action_code == "X/Z/U*")
    {
        CPPUNIT_ASSERT(!_dataset.FindDataElement(gdcm::Tag(GROUP, ELEMENT)) || data_str.empty());
    }
    //Z - replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with the
    // VR
    //D - replace with a non-zero length value that may be a dummy value and consistent with the VR
    //Z/D - Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
    else if(_action_code == "Z" || _action_code == "D" || _action_code == "Z/D")
    {
        if(gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(
               gdcm::Tag(
                   GROUP,
                   ELEMENT
               )
        ).GetVR()
           == gdcm::VR::SQ)
        {
            CPPUNIT_ASSERT(!_dataset.FindDataElement(gdcm::Tag(GROUP, ELEMENT)) || data_str.empty());
        }
        else
        {
            CPPUNIT_ASSERT_EQUAL(get_dummy_value(gdcm::Tag(GROUP, ELEMENT)), data_str);
        }
    }
    //K - keep (unchanged for non-sequence attributes, cleaned for sequences)
    else if(_action_code == "K")
    {
        if(gdcm::Global::GetInstance().GetDicts().GetPublicDict().GetDictEntry(
               gdcm::Tag(
                   GROUP,
                   ELEMENT
               )
        ).GetVR()
           == gdcm::VR::SQ)
        {
            CPPUNIT_ASSERT(!_dataset.FindDataElement(gdcm::Tag(GROUP, ELEMENT)) || data_str.empty());
        }
    }
    //C - clean, that is replace with values of similar meaning known not to contain identifying information and
    // consistent with the VR
    else if(_action_code == "C")
    {
        CPPUNIT_FAIL("We should not be there...");
    }
    //U - replace with a non-zero length UID that is internally consistent within a set of Instances
    else if(_action_code == "U")
    {
        // Check that the new UID is not in the old series
        CPPUNIT_ASSERT(_uid_container.find(data_str) == _uid_container.end());
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizerTest::testAnonymizedFile(const std::filesystem::path& _filename)
{
    // Try to open the file
    gdcm::Reader reader;
    reader.SetFileName(_filename.string().c_str());
    CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \"" + _filename.string() + "\"", reader.Read());
    gdcm::File& gdcm_file  = reader.GetFile();
    gdcm::DataSet& dataset = gdcm_file.GetDataSet();

    process_tag<0x0008, 0x0050>(dataset, "Z", m_uidContainer);     //Accession Number
    process_tag<0x0018, 0x4000>(dataset, "X", m_uidContainer);     //Acquisition Comments
    process_tag<0x0040, 0x0555>(dataset, "X", m_uidContainer);     //Acquisition Context Sequence
    process_tag<0x0008, 0x0022>(dataset, "X/Z", m_uidContainer);   //Acquisition Date
    process_tag<0x0008, 0x002A>(dataset, "X/D", m_uidContainer);   //Acquisition DateTime
    process_tag<0x0018, 0x1400>(dataset, "X/D", m_uidContainer);   //Acquisition Device Processing Description
    process_tag<0x0018, 0x9424>(dataset, "X", m_uidContainer);     //Acquisition Protocol Description
    process_tag<0x0008, 0x0032>(dataset, "X/Z", m_uidContainer);   //Acquisition Time
    process_tag<0x0040, 0x4035>(dataset, "X", m_uidContainer);     //Actual Human Performers Sequence
    process_tag<0x0010, 0x21B0>(dataset, "X", m_uidContainer);     //Additional Patient's History
    process_tag<0x0038, 0x0010>(dataset, "X", m_uidContainer);     //Admission ID
    process_tag<0x0038, 0x0020>(dataset, "X", m_uidContainer);     //Admitting Date
    process_tag<0x0008, 0x1084>(dataset, "X", m_uidContainer);     //Admitting Diagnoses Code Sequence
    process_tag<0x0008, 0x1080>(dataset, "X", m_uidContainer);     //Admitting Diagnoses Description
    process_tag<0x0038, 0x0021>(dataset, "X", m_uidContainer);     //Admitting Time
    process_tag<0x0000, 0x1000>(dataset, "X", m_uidContainer);     //Affected SOP Instance UID
    process_tag<0x0010, 0x2110>(dataset, "X", m_uidContainer);     //Allergies
    process_tag<0x4000, 0x0010>(dataset, "X", m_uidContainer);     //Arbitrary
    process_tag<0x0040, 0xA078>(dataset, "X", m_uidContainer);     //Author Observer Sequence
    process_tag<0x0010, 0x1081>(dataset, "X", m_uidContainer);     //Branch of Service
    process_tag<0x0018, 0x1007>(dataset, "X", m_uidContainer);     //Cassette ID
    process_tag<0x0040, 0x0280>(dataset, "X", m_uidContainer);     //Comments on Performed Procedure Step
    process_tag<0x0020, 0x9161>(dataset, "U", m_uidContainer);     //Concatenation UID
    process_tag<0x0040, 0x3001>(dataset, "X", m_uidContainer);     //Confidentiality Constraint on Patient Data
                                                                   // Description
    process_tag<0x0070, 0x0084>(dataset, "Z", m_uidContainer);     //Content Creator's Name
    process_tag<0x0070, 0x0086>(dataset, "X", m_uidContainer);     //Content Creator's Identification Code Sequence
    process_tag<0x0008, 0x0023>(dataset, "Z/D", m_uidContainer);   //Content Date
    process_tag<0x0040, 0xA730>(dataset, "X", m_uidContainer);     //Content Sequence
    process_tag<0x0008, 0x0033>(dataset, "Z/D", m_uidContainer);   //Content Time
    process_tag<0x0008, 0x010D>(dataset, "U", m_uidContainer);     //Context Group Extension Creator UID
    process_tag<0x0018, 0x0010>(dataset, "Z/D", m_uidContainer);   //Contrast Bolus Agent
    process_tag<0x0018, 0xA003>(dataset, "X", m_uidContainer);     //Contribution Description
    process_tag<0x0010, 0x2150>(dataset, "X", m_uidContainer);     //Country of Residence
    process_tag<0x0008, 0x9123>(dataset, "U", m_uidContainer);     //Creator Version UID
    process_tag<0x0038, 0x0300>(dataset, "X", m_uidContainer);     //Current Patient Location
    process_tag<0x0008, 0x0025>(dataset, "X", m_uidContainer);     //Curve Date
    process_tag<0x0008, 0x0035>(dataset, "X", m_uidContainer);     //Curve Time
    process_tag<0x0040, 0xA07C>(dataset, "X", m_uidContainer);     //Custodial Organization Sequence
    process_tag<0xFFFC, 0xFFFC>(dataset, "X", m_uidContainer);     //Data Set Trailing Padding
    process_tag<0x0008, 0x2111>(dataset, "X", m_uidContainer);     //Derivation Description
    process_tag<0x0018, 0x700A>(dataset, "X", m_uidContainer);     //Detector ID
    process_tag<0x0018, 0x1000>(dataset, "X/Z/D", m_uidContainer); //Device Serial Number
    process_tag<0x0018, 0x1002>(dataset, "U", m_uidContainer);     //Device UID
    process_tag<0x0400, 0x0100>(dataset, "X", m_uidContainer);     //Digital Signature UID
    process_tag<0xFFFA, 0xFFFA>(dataset, "X", m_uidContainer);     //Digital Signatures Sequence
    process_tag<0x0020, 0x9164>(dataset, "U", m_uidContainer);     //Dimension Organization UID
    process_tag<0x0038, 0x0040>(dataset, "X", m_uidContainer);     //Discharge Diagnosis Description
    process_tag<0x4008, 0x011A>(dataset, "X", m_uidContainer);     //Distribution Address
    process_tag<0x4008, 0x0119>(dataset, "X", m_uidContainer);     //Distribution Name
    process_tag<0x300A, 0x0013>(dataset, "U", m_uidContainer);     //Dose Reference UID
    process_tag<0x0010, 0x2160>(dataset, "X", m_uidContainer);     //Ethnic Group
    process_tag<0x0008, 0x0058>(dataset, "U", m_uidContainer);     //Failed SOP Instance UID List
    process_tag<0x0070, 0x031A>(dataset, "U", m_uidContainer);     //Fiducial UID
    process_tag<0x0040, 0x2017>(dataset, "Z", m_uidContainer);     //Filler Order Number of Imaging Service Request
    process_tag<0x0020, 0x9158>(dataset, "X", m_uidContainer);     //Frame Comments
    process_tag<0x0020, 0x0052>(dataset, "U", m_uidContainer);     //Frame of Reference UID
    process_tag<0x0018, 0x1008>(dataset, "X", m_uidContainer);     //Gantry ID
    process_tag<0x0018, 0x1005>(dataset, "X", m_uidContainer);     //Generator ID
    process_tag<0x0070, 0x0001>(dataset, "D", m_uidContainer);     //Graphic Annotation Sequence
    process_tag<0x0040, 0x4037>(dataset, "X", m_uidContainer);     //Human Performers Name
    process_tag<0x0040, 0x4036>(dataset, "X", m_uidContainer);     //Human Performers Organization
    process_tag<0x0088, 0x0200>(dataset, "X", m_uidContainer);     //Icon Image Sequence(see Note 12)
    process_tag<0x0008, 0x4000>(dataset, "X", m_uidContainer);     //Identifying Comments
    process_tag<0x0020, 0x4000>(dataset, "X", m_uidContainer);     //Image Comments
    process_tag<0x0028, 0x4000>(dataset, "X", m_uidContainer);     //Image Presentation Comments
    process_tag<0x0040, 0x2400>(dataset, "X", m_uidContainer);     //Imaging Service Request Comments
    process_tag<0x4008, 0x0300>(dataset, "X", m_uidContainer);     //Impressions
    process_tag<0x0008, 0x0014>(dataset, "U", m_uidContainer);     //Instance Creator UID
    process_tag<0x0008, 0x0081>(dataset, "X", m_uidContainer);     //Institution Address
    process_tag<0x0008, 0x0082>(dataset, "X/Z/D", m_uidContainer); //Institution Code Sequence
    process_tag<0x0008, 0x0080>(dataset, "X/Z/D", m_uidContainer); //Institution Name
    process_tag<0x0008, 0x1040>(dataset, "X", m_uidContainer);     //Institutional Department Name
    process_tag<0x0010, 0x1050>(dataset, "X", m_uidContainer);     //Insurance Plan Identification
    process_tag<0x0040, 0x1011>(dataset, "X", m_uidContainer);     //Intended Recipients of Results Identification
                                                                   // Sequence
    process_tag<0x4008, 0x0111>(dataset, "X", m_uidContainer);     //Interpretation Approver Sequence
    process_tag<0x4008, 0x010C>(dataset, "X", m_uidContainer);     //Interpretation Author
    process_tag<0x4008, 0x0115>(dataset, "X", m_uidContainer);     //Interpretation Diagnosis Description
    process_tag<0x4008, 0x0202>(dataset, "X", m_uidContainer);     //Interpretation ID Issuer
    process_tag<0x4008, 0x0102>(dataset, "X", m_uidContainer);     //Interpretation Recorder
    process_tag<0x4008, 0x010B>(dataset, "X", m_uidContainer);     //Interpretation Text
    process_tag<0x4008, 0x010A>(dataset, "X", m_uidContainer);     //Interpretation Transcriber
    process_tag<0x0008, 0x3010>(dataset, "U", m_uidContainer);     //Irradiation Event UID
    process_tag<0x0038, 0x0011>(dataset, "X", m_uidContainer);     //Issuer of Admission ID
    process_tag<0x0010, 0x0021>(dataset, "X", m_uidContainer);     //Issuer of Patient ID
    process_tag<0x0038, 0x0061>(dataset, "X", m_uidContainer);     //Issuer of Service Episode ID
    process_tag<0x0028, 0x1214>(dataset, "U", m_uidContainer);     //Large Palette Color Lookup Table UID
    process_tag<0x0010, 0x21D0>(dataset, "X", m_uidContainer);     //Last Menstrual Date
    process_tag<0x0400, 0x0404>(dataset, "X", m_uidContainer);     //MAC
    process_tag<0x0002, 0x0003>(dataset, "U", m_uidContainer);     //Media Storage SOP Instance UID
    process_tag<0x0010, 0x2000>(dataset, "X", m_uidContainer);     //Medical Alerts
    process_tag<0x0010, 0x1090>(dataset, "X", m_uidContainer);     //Medical Record Locator
    process_tag<0x0010, 0x1080>(dataset, "X", m_uidContainer);     //Military Rank
    process_tag<0x0400, 0x0550>(dataset, "X", m_uidContainer);     //Modified Attributes Sequence
    process_tag<0x0020, 0x3406>(dataset, "X", m_uidContainer);     //Modified Image Description
    process_tag<0x0020, 0x3401>(dataset, "X", m_uidContainer);     //Modifying Device ID
    process_tag<0x0020, 0x3404>(dataset, "X", m_uidContainer);     //Modifying Device Manufacturer
    process_tag<0x0008, 0x1060>(dataset, "X", m_uidContainer);     //Name of Physician(s) Reading Study
    process_tag<0x0040, 0x1010>(dataset, "X", m_uidContainer);     //Names of Intended Recipient of Results
    process_tag<0x0010, 0x2180>(dataset, "X", m_uidContainer);     //Occupation
    process_tag<0x0008, 0x1072>(dataset, "X/D", m_uidContainer);   //Operators' Identification Sequence
    process_tag<0x0008, 0x1070>(dataset, "X/Z/D", m_uidContainer); //Operators' Name
    process_tag<0x0400, 0x0561>(dataset, "X", m_uidContainer);     //Original Attributes Sequence
    process_tag<0x0040, 0x2010>(dataset, "X", m_uidContainer);     //Order Callback Phone Number
    process_tag<0x0040, 0x2008>(dataset, "X", m_uidContainer);     //Order Entered By
    process_tag<0x0040, 0x2009>(dataset, "X", m_uidContainer);     //Order Enterer Location
    process_tag<0x0010, 0x1000>(dataset, "X", m_uidContainer);     //Other Patient IDs
    process_tag<0x0010, 0x1002>(dataset, "X", m_uidContainer);     //Other Patient IDs Sequence
    process_tag<0x0010, 0x1001>(dataset, "X", m_uidContainer);     //Other Patient Names
    process_tag<0x0008, 0x0024>(dataset, "X", m_uidContainer);     //Overlay Date
    process_tag<0x0008, 0x0034>(dataset, "X", m_uidContainer);     //Overlay Time
    process_tag<0x0028, 0x1199>(dataset, "U", m_uidContainer);     //Palette Color Lookup Table UID
    process_tag<0x0040, 0xA07A>(dataset, "X", m_uidContainer);     //Participant Sequence
    process_tag<0x0010, 0x1040>(dataset, "X", m_uidContainer);     //Patient Address
    process_tag<0x0010, 0x4000>(dataset, "X", m_uidContainer);     //Patient Comments
    process_tag<0x0010, 0x0020>(dataset, "Z", m_uidContainer);     //Patient ID
    process_tag<0x0010, 0x2203>(dataset, "X/Z", m_uidContainer);   //Patient Sex Neutered
    process_tag<0x0038, 0x0500>(dataset, "X", m_uidContainer);     //Patient State
    process_tag<0x0040, 0x1004>(dataset, "X", m_uidContainer);     //Patient Transport Arrangements
    process_tag<0x0010, 0x1010>(dataset, "X", m_uidContainer);     //Patient's Age
    process_tag<0x0010, 0x0030>(dataset, "Z", m_uidContainer);     //Patient's Birth Date
    process_tag<0x0010, 0x1005>(dataset, "X", m_uidContainer);     //Patient's Birth Name
    process_tag<0x0010, 0x0032>(dataset, "X", m_uidContainer);     //Patient's Birth Time
    process_tag<0x0038, 0x0400>(dataset, "X", m_uidContainer);     //Patient's Institution Residence
    process_tag<0x0010, 0x0050>(dataset, "X", m_uidContainer);     //Patient's Insurance Plan Code Sequence
    process_tag<0x0010, 0x1060>(dataset, "X", m_uidContainer);     //Patient's Mother's Birth Name
    process_tag<0x0010, 0x0010>(dataset, "Z", m_uidContainer);     //Patient's Name
    process_tag<0x0010, 0x0101>(dataset, "X", m_uidContainer);     //Patient's Primary Language Code Sequence
    process_tag<0x0010, 0x0102>(dataset, "X", m_uidContainer);     //Patient's Primary Language Modifier Code Sequence
    process_tag<0x0010, 0x21F0>(dataset, "X", m_uidContainer);     //Patient's Religious Preference
    process_tag<0x0010, 0x0040>(dataset, "Z", m_uidContainer);     //Patient's Sex
    process_tag<0x0010, 0x1020>(dataset, "X", m_uidContainer);     //Patient's Size
    process_tag<0x0010, 0x2154>(dataset, "X", m_uidContainer);     //Patient's Telephone Number
    process_tag<0x0010, 0x1030>(dataset, "X", m_uidContainer);     //Patient's Weight
    process_tag<0x0040, 0x0243>(dataset, "X", m_uidContainer);     //Performed Location
    process_tag<0x0040, 0x0254>(dataset, "X", m_uidContainer);     //Performed Procedure Step Description
    process_tag<0x0040, 0x0253>(dataset, "X", m_uidContainer);     //Performed Procedure Step ID
    process_tag<0x0040, 0x0244>(dataset, "X", m_uidContainer);     //Performed Procedure Step Start Date
    // processTag<0x0040,0x0245>(dataset, "X", m_uidContainer);        //Performed Procedure Step Start Time
    process_tag<0x0040, 0x0241>(dataset, "X", m_uidContainer); //Performed Station AE Title
    process_tag<0x0040, 0x4030>(dataset, "X", m_uidContainer); //Performed Station Geographic Location Code
                                                               // Sequence
    process_tag<0x0040, 0x0242>(dataset, "X", m_uidContainer); //Performed Station Name
    // processTag<0x0040,0x0248>(dataset, "X", m_uidContainer);        //Performed Station Name Code Sequence
    process_tag<0x0008, 0x1052>(dataset, "X", m_uidContainer);      //Performing Physicians' Identification Sequence
    process_tag<0x0008, 0x1050>(dataset, "X", m_uidContainer);      //Performing Physicians' Name
    process_tag<0x0040, 0x1102>(dataset, "X", m_uidContainer);      //Person Address
    process_tag<0x0040, 0x1101>(dataset, "D", m_uidContainer);      //Person Identification Code Sequence
    process_tag<0x0040, 0xA123>(dataset, "D", m_uidContainer);      //Person Name
    process_tag<0x0040, 0x1103>(dataset, "X", m_uidContainer);      //Person Telephone Numbers
    process_tag<0x4008, 0x0114>(dataset, "X", m_uidContainer);      //Physician Approving Interpretation
    process_tag<0x0008, 0x1062>(dataset, "X", m_uidContainer);      //Physician Reading Study Identification Sequence
    process_tag<0x0008, 0x1048>(dataset, "X", m_uidContainer);      //Physician(s) Record
    process_tag<0x0008, 0x1049>(dataset, "X", m_uidContainer);      //Physician(s) of Record Identification Sequence
    process_tag<0x0040, 0x2016>(dataset, "Z", m_uidContainer);      //Placer Order Number of Imaging Service Request
    process_tag<0x0018, 0x1004>(dataset, "X", m_uidContainer);      //Plate ID
    process_tag<0x0040, 0x0012>(dataset, "X", m_uidContainer);      //Pre-Medication
    process_tag<0x0010, 0x21C0>(dataset, "X", m_uidContainer);      //Pregnancy Status
    process_tag<0x0018, 0x1030>(dataset, "X/D", m_uidContainer);    //Protocol Name
    process_tag<0x0040, 0x2001>(dataset, "X", m_uidContainer);      //Reason for Imaging Service Request
    process_tag<0x0032, 0x1030>(dataset, "X", m_uidContainer);      //Reason for Study
    process_tag<0x0400, 0x0402>(dataset, "X", m_uidContainer);      //Referenced Digital Signature Sequence
    process_tag<0x3006, 0x0024>(dataset, "U", m_uidContainer);      //Referenced Frame of Reference UID
    process_tag<0x0040, 0x4023>(dataset, "U", m_uidContainer);      //Referenced General Purpose Scheduled Procedure
                                                                    // Step Transaction UID
    process_tag<0x0008, 0x1140>(dataset, "X/Z/U*", m_uidContainer); //Referenced Image Sequence
    // processTag<0x0038,0x1234>(dataset, "X", m_uidContainer);        //Referenced Patient Alias Sequence
    process_tag<0x0008, 0x1120>(dataset, "X", m_uidContainer);      //Referenced Patient Sequence
    process_tag<0x0008, 0x1111>(dataset, "X/Z/D", m_uidContainer);  //Referenced Performed Procedure Step Sequence
    process_tag<0x0400, 0x0403>(dataset, "X", m_uidContainer);      //Referenced SOP Instance MAC Sequence
    process_tag<0x0008, 0x1155>(dataset, "U", m_uidContainer);      //Referenced SOP Instance UID
    process_tag<0x0004, 0x1511>(dataset, "U", m_uidContainer);      //Referenced SOP Instance UID in File
    process_tag<0x0008, 0x1110>(dataset, "X/Z", m_uidContainer);    //Referenced Study Sequence
    process_tag<0x0008, 0x0092>(dataset, "X", m_uidContainer);      //Referring Physician's Address
    process_tag<0x0008, 0x0096>(dataset, "X", m_uidContainer);      //Referring Physician's Identification Sequence
    process_tag<0x0008, 0x0090>(dataset, "Z", m_uidContainer);      //Referring Physician's Name
    process_tag<0x0008, 0x0094>(dataset, "X", m_uidContainer);      //Referring Physician's Telephone Numbers
    process_tag<0x0010, 0x2152>(dataset, "X", m_uidContainer);      //Region of Residence
    process_tag<0x3006, 0x00C2>(dataset, "U", m_uidContainer);      //Related Frame of Reference UID
    process_tag<0x0040, 0x0275>(dataset, "X", m_uidContainer);      //Request Attributes Sequence
    process_tag<0x0032, 0x1070>(dataset, "X", m_uidContainer);      //Requested Contrast Agent
    process_tag<0x0040, 0x1400>(dataset, "X", m_uidContainer);      //Requested Procedure Comments
    process_tag<0x0032, 0x1060>(dataset, "X/Z", m_uidContainer);    //Requested Procedure Description
    process_tag<0x0040, 0x1001>(dataset, "X", m_uidContainer);      //Requested Procedure ID
    process_tag<0x0040, 0x1005>(dataset, "X", m_uidContainer);      //Requested Procedure Location
    process_tag<0x0000, 0x1001>(dataset, "U", m_uidContainer);      //Requested SOP Instance UID
    process_tag<0x0032, 0x1032>(dataset, "X", m_uidContainer);      //Requesting Physician
    process_tag<0x0032, 0x1033>(dataset, "X", m_uidContainer);      //Requesting Service
    process_tag<0x0010, 0x2299>(dataset, "X", m_uidContainer);      //Responsible Organization
    process_tag<0x0010, 0x2297>(dataset, "X", m_uidContainer);      //Responsible Person
    process_tag<0x4008, 0x4000>(dataset, "X", m_uidContainer);      //Results Comments
    process_tag<0x4008, 0x0118>(dataset, "X", m_uidContainer);      //Results Distribution List Sequence
    process_tag<0x4008, 0x0042>(dataset, "X", m_uidContainer);      //Results ID Issuer
    process_tag<0x300E, 0x0008>(dataset, "X/Z", m_uidContainer);    //Reviewer Name
    process_tag<0x0040, 0x4034>(dataset, "X", m_uidContainer);      //Scheduled Human Performers Sequence
    process_tag<0x0038, 0x001E>(dataset, "X", m_uidContainer);      //Scheduled Patient Institution Residence
    process_tag<0x0040, 0x000B>(dataset, "X", m_uidContainer);      //Scheduled Performing Physician Identification
                                                                    // Sequence
    process_tag<0x0040, 0x0006>(dataset, "X", m_uidContainer);      //Scheduled Performing Physician Name
    process_tag<0x0040, 0x0004>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step End Date
    process_tag<0x0040, 0x0005>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step End Time
    process_tag<0x0040, 0x0007>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step Description
    process_tag<0x0040, 0x0011>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step Location
    process_tag<0x0040, 0x0002>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step Start Date
    process_tag<0x0040, 0x0003>(dataset, "X", m_uidContainer);      //Scheduled Procedure Step Start Time
    process_tag<0x0040, 0x0001>(dataset, "X", m_uidContainer);      //Scheduled Station AE Title
    process_tag<0x0040, 0x4027>(dataset, "X", m_uidContainer);      //Scheduled Station Geographic Location Code
                                                                    // Sequence
    process_tag<0x0040, 0x0010>(dataset, "X", m_uidContainer);      //Scheduled Station Name
    process_tag<0x0040, 0x4025>(dataset, "X", m_uidContainer);      //Scheduled Station Name Code Sequence
    process_tag<0x0032, 0x1020>(dataset, "X", m_uidContainer);      //Scheduled Study Location
    process_tag<0x0032, 0x1021>(dataset, "X", m_uidContainer);      //Scheduled Study Location AE Title
    process_tag<0x0008, 0x0021>(dataset, "X/D", m_uidContainer);    //Series Date
    process_tag<0x0008, 0x103E>(dataset, "X", m_uidContainer);      //Series Description
    process_tag<0x0020, 0x000E>(dataset, "U", m_uidContainer);      //Series Instance UID
    process_tag<0x0008, 0x0031>(dataset, "X/D", m_uidContainer);    //Series Time
    process_tag<0x0038, 0x0062>(dataset, "X", m_uidContainer);      //Service Episode Description
    process_tag<0x0038, 0x0060>(dataset, "X", m_uidContainer);      //Service Episode ID
    process_tag<0x0010, 0x21A0>(dataset, "X", m_uidContainer);      //Smoking Status
    process_tag<0x0008, 0x0018>(dataset, "U", m_uidContainer);      //SOP Instance UID
    process_tag<0x0008, 0x2112>(dataset, "X/Z/U*", m_uidContainer); //Source Image Sequence
    process_tag<0x0038, 0x0050>(dataset, "X", m_uidContainer);      //Special Needs
    process_tag<0x0008, 0x1010>(dataset, "X/Z/D", m_uidContainer);  //Station Name
    process_tag<0x0088, 0x0140>(dataset, "U", m_uidContainer);      //Storage Media File-set UID
    process_tag<0x0032, 0x4000>(dataset, "X", m_uidContainer);      //Study Comments
    process_tag<0x0008, 0x0020>(dataset, "Z", m_uidContainer);      //Study Date
    process_tag<0x0008, 0x1030>(dataset, "X", m_uidContainer);      //Study Description
    process_tag<0x0020, 0x0010>(dataset, "Z", m_uidContainer);      //Study ID
    process_tag<0x0032, 0x0012>(dataset, "X", m_uidContainer);      //Study ID Issuer
    process_tag<0x0020, 0x000D>(dataset, "U", m_uidContainer);      //Study Instance UID
    process_tag<0x0008, 0x0030>(dataset, "Z", m_uidContainer);      //Study Time
    process_tag<0x0020, 0x0200>(dataset, "U", m_uidContainer);      //Synchronization Frame of Reference UID
    process_tag<0x0040, 0xDB0D>(dataset, "U", m_uidContainer);      //Template Extension Creator UID
    process_tag<0x0040, 0xDB0C>(dataset, "U", m_uidContainer);      //Template Extension Organization UID
    process_tag<0x4000, 0x4000>(dataset, "X", m_uidContainer);      //Text Comments
    process_tag<0x2030, 0x0020>(dataset, "X", m_uidContainer);      //Text String
    process_tag<0x0008, 0x0201>(dataset, "X", m_uidContainer);      //Timezone Offset From UTC
    process_tag<0x0088, 0x0910>(dataset, "X", m_uidContainer);      //Topic Author
    // processTag<0x0088,0x0912>(dataset, "X", m_uidContainer);        //Topic Key Words
    process_tag<0x0088, 0x0906>(dataset, "X", m_uidContainer); //Topic Subject
    process_tag<0x0088, 0x0904>(dataset, "X", m_uidContainer); //Topic Title
    process_tag<0x0008, 0x1195>(dataset, "U", m_uidContainer); //Transaction UID
    process_tag<0x0040, 0xA124>(dataset, "U", m_uidContainer); //UID
    process_tag<0x0040, 0xA088>(dataset, "Z", m_uidContainer); //Verifying Observer Identification Code Sequence
    process_tag<0x0040, 0xA075>(dataset, "D", m_uidContainer); //Verifying Observer Name
    process_tag<0x0040, 0xA073>(dataset, "D", m_uidContainer); //Verifying Observer Sequence
    process_tag<0x0040, 0xA027>(dataset, "X", m_uidContainer); //Verifying Organization
    process_tag<0x0038, 0x4000>(dataset, "X", m_uidContainer); //Visit Comments
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
