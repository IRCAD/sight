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

#include <utest/filter.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>
#include <utest_data/generator/series_set.hpp>

#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmReader.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::dicom_anonymizer_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void dicom_anonymizer_test::setUp()
{
    // Set up context before running a test.
    if(utest::filter::ignore_slow_tests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void dicom_anonymizer_test::anonymize_image_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    data::image_series::sptr img_series;
    img_series = utest_data::generator::series_set::create_image_series();

    core::os::temp_dir tmp_dir;

    // Write ImageSeries
    io::dicom::writer::series::sptr writer = std::make_shared<io::dicom::writer::series>();
    writer->set_object(img_series);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize ImageSeries
    io::dicom::helper::dicom_anonymizer anonymizer;
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
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->get_series_date());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->get_series_time());
    CPPUNIT_ASSERT_EQUAL(std::string("Description"), anonymized_series->get_series_description()); // action code K
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), anonymized_series->get_performing_physician_name().size());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->get_institution_name());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED^ANONYMIZED"), anonymized_series->get_patient_name());
    CPPUNIT_ASSERT_EQUAL(std::string("ANONYMIZED"), anonymized_series->get_patient_id());
    CPPUNIT_ASSERT_EQUAL(std::string("19000101"), anonymized_series->get_patient_birth_date());
    CPPUNIT_ASSERT_EQUAL(std::string("O"), anonymized_series->get_patient_sex());
    CPPUNIT_ASSERT_EQUAL(std::string("19000101"), anonymized_series->get_study_date());
    CPPUNIT_ASSERT_EQUAL(std::string("000000.000000"), anonymized_series->get_study_time());
    CPPUNIT_ASSERT_EQUAL(
        std::string("ANONYMIZED^ANONYMIZED"),
        anonymized_series->get_referring_physician_name()
    );
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->get_study_description());
    CPPUNIT_ASSERT_EQUAL(std::string(""), anonymized_series->get_patient_age());
    CPPUNIT_ASSERT(img_series->get_series_instance_uid() != anonymized_series->get_series_instance_uid());
    CPPUNIT_ASSERT(img_series->get_study_instance_uid() != anonymized_series->get_study_instance_uid());
}

//------------------------------------------------------------------------------

void dicom_anonymizer_test::anonymize_dicom_test()
{
    if(!utest::filter::ignore_slow_tests())
    {
        //TODO Do we have to test more images ?
        this->test_dicom_folder(utest_data::dir() / "sight/Patient/Dicom/DicomDB/08-CT-PACS");
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer_test::test_dicom_folder(const std::filesystem::path& _src_path)
{
    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + _src_path.string() + "' does not exist",
        std::filesystem::exists(_src_path)
    );

    // Save old UID
    m_uid_container.clear();
    std::vector<std::filesystem::path> old_filenames;
    io::dicom::helper::dicom_search::search_recursively(_src_path, old_filenames, true);
    for(const std::filesystem::path& filename : old_filenames)
    {
        // Try to open the file
        gdcm::Reader reader;
        reader.SetFileName(filename.string().c_str());
        CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \"" + filename.string() + "\"", reader.Read());
        gdcm::File& gdcm_file  = reader.GetFile();
        gdcm::DataSet& dataset = gdcm_file.GetDataSet();

        std::string data;
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x9161>(dataset); //Concatenation UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x010D>(dataset); //Context Group Extension
        // Creator
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x9123>(dataset); //Creator Version UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0018, 0x1002>(dataset); //Device UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x9164>(dataset); //Dimension Organization
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x300A, 0x0013>(dataset); //Dose Reference UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0058>(dataset); //Failed SOP Instance UID
        // List
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0070, 0x031A>(dataset); //Fiducial UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x0052>(dataset); //Frame of Reference UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0014>(dataset); //Instance Creator UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x3010>(dataset); //Irradiation Event UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1214>(dataset); //Large Palette Color
        // Lookup
        // Table UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0002, 0x0003>(dataset); //Media Storage SOP
        // Instance UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0028, 0x1199>(dataset); //Palette Color Lookup
        // Table UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x3006, 0x0024>(dataset); //Referenced Frame of
        // Reference
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0x4023>(dataset); //Referenced General
        // Purpose
        // Scheduled Procedure Step
        // Transaction UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x1155>(dataset); //Referenced SOP Instance
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0004, 0x1511>(dataset); //Referenced SOP Instance
        // UID in
        // File
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x3006, 0x00C2>(dataset); //Related Frame of
        // Reference UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0000, 0x1001>(dataset); //Requested SOP Instance
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000E>(dataset); //Series Instance UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x0018>(dataset); //SOP Instance UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0088, 0x0140>(dataset); //Storage Media File-set
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x000D>(dataset); //Study Instance UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0020, 0x0200>(dataset); //Synchronization Frame of
        // Reference UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0xDB0D>(dataset); //Template Extension
        // Creator UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0xDB0C>(dataset); //Template Extension
        // Organization
        // UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0008, 0x1195>(dataset); //Transaction UID
        m_uid_container.insert(data);
        data = io::dicom::helper::dicom_data_reader::get_tag_value<0x0040, 0xA124>(dataset); //UID
        m_uid_container.insert(data);
    }

    m_uid_container.erase("");

    auto series_set = std::make_shared<data::series_set>();

    // Read DicomSeries
    io::dicom::reader::series_set::sptr reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(_src_path);
    CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Write DicomSeries
    core::os::temp_dir tmp_dir;
    auto writer = std::make_shared<io::dicom::helper::dicom_series_writer>();
    writer->set_object((*series_set)[0]);
    writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // Anonymize folder
    io::dicom::helper::dicom_anonymizer anonymizer;
    CPPUNIT_ASSERT_NO_THROW(anonymizer.anonymize(tmp_dir));

    // Read DICOM files
    std::vector<std::filesystem::path> filenames;
    io::dicom::helper::dicom_search::search_recursively(tmp_dir, filenames, true);
    for(const std::filesystem::path& filename : filenames)
    {
        this->test_anonymized_file(filename);
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
    const std::string& data_str = io::dicom::helper::dicom_data_reader::get_tag_value<GROUP, ELEMENT>(_dataset);

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

void dicom_anonymizer_test::test_anonymized_file(const std::filesystem::path& _filename)
{
    // Try to open the file
    gdcm::Reader reader;
    reader.SetFileName(_filename.string().c_str());
    CPPUNIT_ASSERT_MESSAGE("Unable to read the file: \"" + _filename.string() + "\"", reader.Read());
    gdcm::File& gdcm_file  = reader.GetFile();
    gdcm::DataSet& dataset = gdcm_file.GetDataSet();

    process_tag<0x0008, 0x0050>(dataset, "Z", m_uid_container);     //Accession Number
    process_tag<0x0018, 0x4000>(dataset, "X", m_uid_container);     //Acquisition Comments
    process_tag<0x0040, 0x0555>(dataset, "X", m_uid_container);     //Acquisition Context Sequence
    process_tag<0x0008, 0x0022>(dataset, "X/Z", m_uid_container);   //Acquisition Date
    process_tag<0x0008, 0x002A>(dataset, "X/D", m_uid_container);   //Acquisition DateTime
    process_tag<0x0018, 0x1400>(dataset, "X/D", m_uid_container);   //Acquisition Device Processing Description
    process_tag<0x0018, 0x9424>(dataset, "X", m_uid_container);     //Acquisition Protocol Description
    process_tag<0x0008, 0x0032>(dataset, "X/Z", m_uid_container);   //Acquisition Time
    process_tag<0x0040, 0x4035>(dataset, "X", m_uid_container);     //Actual Human Performers Sequence
    process_tag<0x0010, 0x21B0>(dataset, "X", m_uid_container);     //Additional Patient's History
    process_tag<0x0038, 0x0010>(dataset, "X", m_uid_container);     //Admission ID
    process_tag<0x0038, 0x0020>(dataset, "X", m_uid_container);     //Admitting Date
    process_tag<0x0008, 0x1084>(dataset, "X", m_uid_container);     //Admitting Diagnoses Code Sequence
    process_tag<0x0008, 0x1080>(dataset, "X", m_uid_container);     //Admitting Diagnoses Description
    process_tag<0x0038, 0x0021>(dataset, "X", m_uid_container);     //Admitting Time
    process_tag<0x0000, 0x1000>(dataset, "X", m_uid_container);     //Affected SOP Instance UID
    process_tag<0x0010, 0x2110>(dataset, "X", m_uid_container);     //Allergies
    process_tag<0x4000, 0x0010>(dataset, "X", m_uid_container);     //Arbitrary
    process_tag<0x0040, 0xA078>(dataset, "X", m_uid_container);     //Author Observer Sequence
    process_tag<0x0010, 0x1081>(dataset, "X", m_uid_container);     //Branch of Service
    process_tag<0x0018, 0x1007>(dataset, "X", m_uid_container);     //Cassette ID
    process_tag<0x0040, 0x0280>(dataset, "X", m_uid_container);     //Comments on Performed Procedure Step
    process_tag<0x0020, 0x9161>(dataset, "U", m_uid_container);     //Concatenation UID
    process_tag<0x0040, 0x3001>(dataset, "X", m_uid_container);     //Confidentiality Constraint on Patient Data
                                                                    // Description
    process_tag<0x0070, 0x0084>(dataset, "Z", m_uid_container);     //Content Creator's Name
    process_tag<0x0070, 0x0086>(dataset, "X", m_uid_container);     //Content Creator's Identification Code Sequence
    process_tag<0x0008, 0x0023>(dataset, "Z/D", m_uid_container);   //Content Date
    process_tag<0x0040, 0xA730>(dataset, "X", m_uid_container);     //Content Sequence
    process_tag<0x0008, 0x0033>(dataset, "Z/D", m_uid_container);   //Content Time
    process_tag<0x0008, 0x010D>(dataset, "U", m_uid_container);     //Context Group Extension Creator UID
    process_tag<0x0018, 0x0010>(dataset, "Z/D", m_uid_container);   //Contrast Bolus Agent
    process_tag<0x0018, 0xA003>(dataset, "X", m_uid_container);     //Contribution Description
    process_tag<0x0010, 0x2150>(dataset, "X", m_uid_container);     //Country of Residence
    process_tag<0x0008, 0x9123>(dataset, "U", m_uid_container);     //Creator Version UID
    process_tag<0x0038, 0x0300>(dataset, "X", m_uid_container);     //Current Patient Location
    process_tag<0x0008, 0x0025>(dataset, "X", m_uid_container);     //Curve Date
    process_tag<0x0008, 0x0035>(dataset, "X", m_uid_container);     //Curve Time
    process_tag<0x0040, 0xA07C>(dataset, "X", m_uid_container);     //Custodial Organization Sequence
    process_tag<0xFFFC, 0xFFFC>(dataset, "X", m_uid_container);     //Data Set Trailing Padding
    process_tag<0x0008, 0x2111>(dataset, "X", m_uid_container);     //Derivation Description
    process_tag<0x0018, 0x700A>(dataset, "X", m_uid_container);     //Detector ID
    process_tag<0x0018, 0x1000>(dataset, "X/Z/D", m_uid_container); //Device Serial Number
    process_tag<0x0018, 0x1002>(dataset, "U", m_uid_container);     //Device UID
    process_tag<0x0400, 0x0100>(dataset, "X", m_uid_container);     //Digital Signature UID
    process_tag<0xFFFA, 0xFFFA>(dataset, "X", m_uid_container);     //Digital Signatures Sequence
    process_tag<0x0020, 0x9164>(dataset, "U", m_uid_container);     //Dimension Organization UID
    process_tag<0x0038, 0x0040>(dataset, "X", m_uid_container);     //Discharge Diagnosis Description
    process_tag<0x4008, 0x011A>(dataset, "X", m_uid_container);     //Distribution Address
    process_tag<0x4008, 0x0119>(dataset, "X", m_uid_container);     //Distribution Name
    process_tag<0x300A, 0x0013>(dataset, "U", m_uid_container);     //Dose Reference UID
    process_tag<0x0010, 0x2160>(dataset, "X", m_uid_container);     //Ethnic Group
    process_tag<0x0008, 0x0058>(dataset, "U", m_uid_container);     //Failed SOP Instance UID List
    process_tag<0x0070, 0x031A>(dataset, "U", m_uid_container);     //Fiducial UID
    process_tag<0x0040, 0x2017>(dataset, "Z", m_uid_container);     //Filler Order Number of Imaging Service Request
    process_tag<0x0020, 0x9158>(dataset, "X", m_uid_container);     //Frame Comments
    process_tag<0x0020, 0x0052>(dataset, "U", m_uid_container);     //Frame of Reference UID
    process_tag<0x0018, 0x1008>(dataset, "X", m_uid_container);     //Gantry ID
    process_tag<0x0018, 0x1005>(dataset, "X", m_uid_container);     //Generator ID
    process_tag<0x0070, 0x0001>(dataset, "D", m_uid_container);     //Graphic Annotation Sequence
    process_tag<0x0040, 0x4037>(dataset, "X", m_uid_container);     //Human Performers Name
    process_tag<0x0040, 0x4036>(dataset, "X", m_uid_container);     //Human Performers Organization
    process_tag<0x0088, 0x0200>(dataset, "X", m_uid_container);     //Icon Image Sequence(see Note 12)
    process_tag<0x0008, 0x4000>(dataset, "X", m_uid_container);     //Identifying Comments
    process_tag<0x0020, 0x4000>(dataset, "X", m_uid_container);     //Image Comments
    process_tag<0x0028, 0x4000>(dataset, "X", m_uid_container);     //Image Presentation Comments
    process_tag<0x0040, 0x2400>(dataset, "X", m_uid_container);     //Imaging Service Request Comments
    process_tag<0x4008, 0x0300>(dataset, "X", m_uid_container);     //Impressions
    process_tag<0x0008, 0x0014>(dataset, "U", m_uid_container);     //Instance Creator UID
    process_tag<0x0008, 0x0081>(dataset, "X", m_uid_container);     //Institution Address
    process_tag<0x0008, 0x0082>(dataset, "X/Z/D", m_uid_container); //Institution Code Sequence
    process_tag<0x0008, 0x0080>(dataset, "X/Z/D", m_uid_container); //Institution Name
    process_tag<0x0008, 0x1040>(dataset, "X", m_uid_container);     //Institutional Department Name
    process_tag<0x0010, 0x1050>(dataset, "X", m_uid_container);     //Insurance Plan Identification
    process_tag<0x0040, 0x1011>(dataset, "X", m_uid_container);     //Intended Recipients of Results Identification
                                                                    // Sequence
    process_tag<0x4008, 0x0111>(dataset, "X", m_uid_container);     //Interpretation Approver Sequence
    process_tag<0x4008, 0x010C>(dataset, "X", m_uid_container);     //Interpretation Author
    process_tag<0x4008, 0x0115>(dataset, "X", m_uid_container);     //Interpretation Diagnosis Description
    process_tag<0x4008, 0x0202>(dataset, "X", m_uid_container);     //Interpretation ID Issuer
    process_tag<0x4008, 0x0102>(dataset, "X", m_uid_container);     //Interpretation Recorder
    process_tag<0x4008, 0x010B>(dataset, "X", m_uid_container);     //Interpretation Text
    process_tag<0x4008, 0x010A>(dataset, "X", m_uid_container);     //Interpretation Transcriber
    process_tag<0x0008, 0x3010>(dataset, "U", m_uid_container);     //Irradiation Event UID
    process_tag<0x0038, 0x0011>(dataset, "X", m_uid_container);     //Issuer of Admission ID
    process_tag<0x0010, 0x0021>(dataset, "X", m_uid_container);     //Issuer of Patient ID
    process_tag<0x0038, 0x0061>(dataset, "X", m_uid_container);     //Issuer of Service Episode ID
    process_tag<0x0028, 0x1214>(dataset, "U", m_uid_container);     //Large Palette Color Lookup Table UID
    process_tag<0x0010, 0x21D0>(dataset, "X", m_uid_container);     //Last Menstrual Date
    process_tag<0x0400, 0x0404>(dataset, "X", m_uid_container);     //MAC
    process_tag<0x0002, 0x0003>(dataset, "U", m_uid_container);     //Media Storage SOP Instance UID
    process_tag<0x0010, 0x2000>(dataset, "X", m_uid_container);     //Medical Alerts
    process_tag<0x0010, 0x1090>(dataset, "X", m_uid_container);     //Medical Record Locator
    process_tag<0x0010, 0x1080>(dataset, "X", m_uid_container);     //Military Rank
    process_tag<0x0400, 0x0550>(dataset, "X", m_uid_container);     //Modified Attributes Sequence
    process_tag<0x0020, 0x3406>(dataset, "X", m_uid_container);     //Modified Image Description
    process_tag<0x0020, 0x3401>(dataset, "X", m_uid_container);     //Modifying Device ID
    process_tag<0x0020, 0x3404>(dataset, "X", m_uid_container);     //Modifying Device Manufacturer
    process_tag<0x0008, 0x1060>(dataset, "X", m_uid_container);     //Name of Physician(s) Reading Study
    process_tag<0x0040, 0x1010>(dataset, "X", m_uid_container);     //Names of Intended Recipient of Results
    process_tag<0x0010, 0x2180>(dataset, "X", m_uid_container);     //Occupation
    process_tag<0x0008, 0x1072>(dataset, "X/D", m_uid_container);   //Operators' Identification Sequence
    process_tag<0x0008, 0x1070>(dataset, "X/Z/D", m_uid_container); //Operators' Name
    process_tag<0x0400, 0x0561>(dataset, "X", m_uid_container);     //Original Attributes Sequence
    process_tag<0x0040, 0x2010>(dataset, "X", m_uid_container);     //Order Callback Phone Number
    process_tag<0x0040, 0x2008>(dataset, "X", m_uid_container);     //Order Entered By
    process_tag<0x0040, 0x2009>(dataset, "X", m_uid_container);     //Order Enterer Location
    process_tag<0x0010, 0x1000>(dataset, "X", m_uid_container);     //Other Patient IDs
    process_tag<0x0010, 0x1002>(dataset, "X", m_uid_container);     //Other Patient IDs Sequence
    process_tag<0x0010, 0x1001>(dataset, "X", m_uid_container);     //Other Patient Names
    process_tag<0x0008, 0x0024>(dataset, "X", m_uid_container);     //Overlay Date
    process_tag<0x0008, 0x0034>(dataset, "X", m_uid_container);     //Overlay Time
    process_tag<0x0028, 0x1199>(dataset, "U", m_uid_container);     //Palette Color Lookup Table UID
    process_tag<0x0040, 0xA07A>(dataset, "X", m_uid_container);     //Participant Sequence
    process_tag<0x0010, 0x1040>(dataset, "X", m_uid_container);     //Patient Address
    process_tag<0x0010, 0x4000>(dataset, "X", m_uid_container);     //Patient Comments
    process_tag<0x0010, 0x0020>(dataset, "Z", m_uid_container);     //Patient ID
    process_tag<0x0010, 0x2203>(dataset, "X/Z", m_uid_container);   //Patient Sex Neutered
    process_tag<0x0038, 0x0500>(dataset, "X", m_uid_container);     //Patient State
    process_tag<0x0040, 0x1004>(dataset, "X", m_uid_container);     //Patient Transport Arrangements
    process_tag<0x0010, 0x1010>(dataset, "X", m_uid_container);     //Patient's Age
    process_tag<0x0010, 0x0030>(dataset, "Z", m_uid_container);     //Patient's Birth Date
    process_tag<0x0010, 0x1005>(dataset, "X", m_uid_container);     //Patient's Birth Name
    process_tag<0x0010, 0x0032>(dataset, "X", m_uid_container);     //Patient's Birth Time
    process_tag<0x0038, 0x0400>(dataset, "X", m_uid_container);     //Patient's Institution Residence
    process_tag<0x0010, 0x0050>(dataset, "X", m_uid_container);     //Patient's Insurance Plan Code Sequence
    process_tag<0x0010, 0x1060>(dataset, "X", m_uid_container);     //Patient's Mother's Birth Name
    process_tag<0x0010, 0x0010>(dataset, "Z", m_uid_container);     //Patient's Name
    process_tag<0x0010, 0x0101>(dataset, "X", m_uid_container);     //Patient's Primary Language Code Sequence
    process_tag<0x0010, 0x0102>(dataset, "X", m_uid_container);     //Patient's Primary Language Modifier Code Sequence
    process_tag<0x0010, 0x21F0>(dataset, "X", m_uid_container);     //Patient's Religious Preference
    process_tag<0x0010, 0x0040>(dataset, "Z", m_uid_container);     //Patient's Sex
    process_tag<0x0010, 0x1020>(dataset, "X", m_uid_container);     //Patient's Size
    process_tag<0x0010, 0x2154>(dataset, "X", m_uid_container);     //Patient's Telephone Number
    process_tag<0x0010, 0x1030>(dataset, "X", m_uid_container);     //Patient's Weight
    process_tag<0x0040, 0x0243>(dataset, "X", m_uid_container);     //Performed Location
    process_tag<0x0040, 0x0254>(dataset, "X", m_uid_container);     //Performed Procedure Step Description
    process_tag<0x0040, 0x0253>(dataset, "X", m_uid_container);     //Performed Procedure Step ID
    process_tag<0x0040, 0x0244>(dataset, "X", m_uid_container);     //Performed Procedure Step Start Date
    // processTag<0x0040,0x0245>(dataset, "X", m_uidContainer);        //Performed Procedure Step Start Time
    process_tag<0x0040, 0x0241>(dataset, "X", m_uid_container); //Performed Station AE Title
    process_tag<0x0040, 0x4030>(dataset, "X", m_uid_container); //Performed Station Geographic Location Code
                                                                // Sequence
    process_tag<0x0040, 0x0242>(dataset, "X", m_uid_container); //Performed Station Name
    // processTag<0x0040,0x0248>(dataset, "X", m_uidContainer);        //Performed Station Name Code Sequence
    process_tag<0x0008, 0x1052>(dataset, "X", m_uid_container);      //Performing Physicians' Identification Sequence
    process_tag<0x0008, 0x1050>(dataset, "X", m_uid_container);      //Performing Physicians' Name
    process_tag<0x0040, 0x1102>(dataset, "X", m_uid_container);      //Person Address
    process_tag<0x0040, 0x1101>(dataset, "D", m_uid_container);      //Person Identification Code Sequence
    process_tag<0x0040, 0xA123>(dataset, "D", m_uid_container);      //Person Name
    process_tag<0x0040, 0x1103>(dataset, "X", m_uid_container);      //Person Telephone Numbers
    process_tag<0x4008, 0x0114>(dataset, "X", m_uid_container);      //Physician Approving Interpretation
    process_tag<0x0008, 0x1062>(dataset, "X", m_uid_container);      //Physician Reading Study Identification Sequence
    process_tag<0x0008, 0x1048>(dataset, "X", m_uid_container);      //Physician(s) Record
    process_tag<0x0008, 0x1049>(dataset, "X", m_uid_container);      //Physician(s) of Record Identification Sequence
    process_tag<0x0040, 0x2016>(dataset, "Z", m_uid_container);      //Placer Order Number of Imaging Service Request
    process_tag<0x0018, 0x1004>(dataset, "X", m_uid_container);      //Plate ID
    process_tag<0x0040, 0x0012>(dataset, "X", m_uid_container);      //Pre-Medication
    process_tag<0x0010, 0x21C0>(dataset, "X", m_uid_container);      //Pregnancy Status
    process_tag<0x0018, 0x1030>(dataset, "X/D", m_uid_container);    //Protocol Name
    process_tag<0x0040, 0x2001>(dataset, "X", m_uid_container);      //Reason for Imaging Service Request
    process_tag<0x0032, 0x1030>(dataset, "X", m_uid_container);      //Reason for Study
    process_tag<0x0400, 0x0402>(dataset, "X", m_uid_container);      //Referenced Digital Signature Sequence
    process_tag<0x3006, 0x0024>(dataset, "U", m_uid_container);      //Referenced Frame of Reference UID
    process_tag<0x0040, 0x4023>(dataset, "U", m_uid_container);      //Referenced General Purpose Scheduled Procedure
                                                                     // Step Transaction UID
    process_tag<0x0008, 0x1140>(dataset, "X/Z/U*", m_uid_container); //Referenced Image Sequence
    // processTag<0x0038,0x1234>(dataset, "X", m_uidContainer);        //Referenced Patient Alias Sequence
    process_tag<0x0008, 0x1120>(dataset, "X", m_uid_container);      //Referenced Patient Sequence
    process_tag<0x0008, 0x1111>(dataset, "X/Z/D", m_uid_container);  //Referenced Performed Procedure Step Sequence
    process_tag<0x0400, 0x0403>(dataset, "X", m_uid_container);      //Referenced SOP Instance MAC Sequence
    process_tag<0x0008, 0x1155>(dataset, "U", m_uid_container);      //Referenced SOP Instance UID
    process_tag<0x0004, 0x1511>(dataset, "U", m_uid_container);      //Referenced SOP Instance UID in File
    process_tag<0x0008, 0x1110>(dataset, "X/Z", m_uid_container);    //Referenced Study Sequence
    process_tag<0x0008, 0x0092>(dataset, "X", m_uid_container);      //Referring Physician's Address
    process_tag<0x0008, 0x0096>(dataset, "X", m_uid_container);      //Referring Physician's Identification Sequence
    process_tag<0x0008, 0x0090>(dataset, "Z", m_uid_container);      //Referring Physician's Name
    process_tag<0x0008, 0x0094>(dataset, "X", m_uid_container);      //Referring Physician's Telephone Numbers
    process_tag<0x0010, 0x2152>(dataset, "X", m_uid_container);      //Region of Residence
    process_tag<0x3006, 0x00C2>(dataset, "U", m_uid_container);      //Related Frame of Reference UID
    process_tag<0x0040, 0x0275>(dataset, "X", m_uid_container);      //Request Attributes Sequence
    process_tag<0x0032, 0x1070>(dataset, "X", m_uid_container);      //Requested Contrast Agent
    process_tag<0x0040, 0x1400>(dataset, "X", m_uid_container);      //Requested Procedure Comments
    process_tag<0x0032, 0x1060>(dataset, "X/Z", m_uid_container);    //Requested Procedure Description
    process_tag<0x0040, 0x1001>(dataset, "X", m_uid_container);      //Requested Procedure ID
    process_tag<0x0040, 0x1005>(dataset, "X", m_uid_container);      //Requested Procedure Location
    process_tag<0x0000, 0x1001>(dataset, "U", m_uid_container);      //Requested SOP Instance UID
    process_tag<0x0032, 0x1032>(dataset, "X", m_uid_container);      //Requesting Physician
    process_tag<0x0032, 0x1033>(dataset, "X", m_uid_container);      //Requesting Service
    process_tag<0x0010, 0x2299>(dataset, "X", m_uid_container);      //Responsible Organization
    process_tag<0x0010, 0x2297>(dataset, "X", m_uid_container);      //Responsible Person
    process_tag<0x4008, 0x4000>(dataset, "X", m_uid_container);      //Results Comments
    process_tag<0x4008, 0x0118>(dataset, "X", m_uid_container);      //Results Distribution List Sequence
    process_tag<0x4008, 0x0042>(dataset, "X", m_uid_container);      //Results ID Issuer
    process_tag<0x300E, 0x0008>(dataset, "X/Z", m_uid_container);    //Reviewer Name
    process_tag<0x0040, 0x4034>(dataset, "X", m_uid_container);      //Scheduled Human Performers Sequence
    process_tag<0x0038, 0x001E>(dataset, "X", m_uid_container);      //Scheduled Patient Institution Residence
    process_tag<0x0040, 0x000B>(dataset, "X", m_uid_container);      //Scheduled Performing Physician Identification
                                                                     // Sequence
    process_tag<0x0040, 0x0006>(dataset, "X", m_uid_container);      //Scheduled Performing Physician Name
    process_tag<0x0040, 0x0004>(dataset, "X", m_uid_container);      //Scheduled Procedure Step End Date
    process_tag<0x0040, 0x0005>(dataset, "X", m_uid_container);      //Scheduled Procedure Step End Time
    process_tag<0x0040, 0x0007>(dataset, "X", m_uid_container);      //Scheduled Procedure Step Description
    process_tag<0x0040, 0x0011>(dataset, "X", m_uid_container);      //Scheduled Procedure Step Location
    process_tag<0x0040, 0x0002>(dataset, "X", m_uid_container);      //Scheduled Procedure Step Start Date
    process_tag<0x0040, 0x0003>(dataset, "X", m_uid_container);      //Scheduled Procedure Step Start Time
    process_tag<0x0040, 0x0001>(dataset, "X", m_uid_container);      //Scheduled Station AE Title
    process_tag<0x0040, 0x4027>(dataset, "X", m_uid_container);      //Scheduled Station Geographic Location Code
                                                                     // Sequence
    process_tag<0x0040, 0x0010>(dataset, "X", m_uid_container);      //Scheduled Station Name
    process_tag<0x0040, 0x4025>(dataset, "X", m_uid_container);      //Scheduled Station Name Code Sequence
    process_tag<0x0032, 0x1020>(dataset, "X", m_uid_container);      //Scheduled Study Location
    process_tag<0x0032, 0x1021>(dataset, "X", m_uid_container);      //Scheduled Study Location AE Title
    process_tag<0x0008, 0x0021>(dataset, "X/D", m_uid_container);    //Series Date
    process_tag<0x0008, 0x103E>(dataset, "X", m_uid_container);      //Series Description
    process_tag<0x0020, 0x000E>(dataset, "U", m_uid_container);      //Series Instance UID
    process_tag<0x0008, 0x0031>(dataset, "X/D", m_uid_container);    //Series Time
    process_tag<0x0038, 0x0062>(dataset, "X", m_uid_container);      //Service Episode Description
    process_tag<0x0038, 0x0060>(dataset, "X", m_uid_container);      //Service Episode ID
    process_tag<0x0010, 0x21A0>(dataset, "X", m_uid_container);      //Smoking Status
    process_tag<0x0008, 0x0018>(dataset, "U", m_uid_container);      //SOP Instance UID
    process_tag<0x0008, 0x2112>(dataset, "X/Z/U*", m_uid_container); //Source Image Sequence
    process_tag<0x0038, 0x0050>(dataset, "X", m_uid_container);      //Special Needs
    process_tag<0x0008, 0x1010>(dataset, "X/Z/D", m_uid_container);  //Station Name
    process_tag<0x0088, 0x0140>(dataset, "U", m_uid_container);      //Storage Media File-set UID
    process_tag<0x0032, 0x4000>(dataset, "X", m_uid_container);      //Study Comments
    process_tag<0x0008, 0x0020>(dataset, "Z", m_uid_container);      //Study Date
    process_tag<0x0008, 0x1030>(dataset, "X", m_uid_container);      //Study Description
    process_tag<0x0020, 0x0010>(dataset, "Z", m_uid_container);      //Study ID
    process_tag<0x0032, 0x0012>(dataset, "X", m_uid_container);      //Study ID Issuer
    process_tag<0x0020, 0x000D>(dataset, "U", m_uid_container);      //Study Instance UID
    process_tag<0x0008, 0x0030>(dataset, "Z", m_uid_container);      //Study Time
    process_tag<0x0020, 0x0200>(dataset, "U", m_uid_container);      //Synchronization Frame of Reference UID
    process_tag<0x0040, 0xDB0D>(dataset, "U", m_uid_container);      //Template Extension Creator UID
    process_tag<0x0040, 0xDB0C>(dataset, "U", m_uid_container);      //Template Extension Organization UID
    process_tag<0x4000, 0x4000>(dataset, "X", m_uid_container);      //Text Comments
    process_tag<0x2030, 0x0020>(dataset, "X", m_uid_container);      //Text String
    process_tag<0x0008, 0x0201>(dataset, "X", m_uid_container);      //Timezone Offset From UTC
    process_tag<0x0088, 0x0910>(dataset, "X", m_uid_container);      //Topic Author
    // processTag<0x0088,0x0912>(dataset, "X", m_uidContainer);        //Topic Key Words
    process_tag<0x0088, 0x0906>(dataset, "X", m_uid_container); //Topic Subject
    process_tag<0x0088, 0x0904>(dataset, "X", m_uid_container); //Topic Title
    process_tag<0x0008, 0x1195>(dataset, "U", m_uid_container); //Transaction UID
    process_tag<0x0040, 0xA124>(dataset, "U", m_uid_container); //UID
    process_tag<0x0040, 0xA088>(dataset, "Z", m_uid_container); //Verifying Observer Identification Code Sequence
    process_tag<0x0040, 0xA075>(dataset, "D", m_uid_container); //Verifying Observer Name
    process_tag<0x0040, 0xA073>(dataset, "D", m_uid_container); //Verifying Observer Sequence
    process_tag<0x0040, 0xA027>(dataset, "X", m_uid_container); //Verifying Organization
    process_tag<0x0038, 0x4000>(dataset, "X", m_uid_container); //Visit Comments
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
