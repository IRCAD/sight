/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem/operations.hpp>

#include <gdcmGlobal.h>
#include <gdcmReader.h>
#include <gdcmUIDGenerator.h>
#include <gdcmWriter.h>

#include <fwCore/base.hpp>
#include <fwTools/System.hpp>

#include "vtkGdcmIO/helper/GdcmHelper.hpp"
#include "fwDicomIOExt/gdcm/helper/DicomAnonymizer.hpp"

namespace fwDicomIOExt
{
namespace gdcm
{
namespace helper
{

const unsigned int DicomAnonymizer::s_NUMBER_OF_TAGS = 243;

DicomAnonymizer::DicomAnonymizer() :
    m_publicDictionary(::gdcm::Global::GetInstance().GetDicts().GetPublicDict())
{
}

//------------------------------------------------------------------------------

DicomAnonymizer::~DicomAnonymizer()
{

}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymize(const ::boost::filesystem::path &dirPath)
{
    // Create temporary directory
    ::boost::filesystem::path tmpPath = ::fwTools::System::getTemporaryFolder("DicomAnonymizer");
    tmpPath                          /= "tmp";

    ::boost::system::error_code ec;
    ::boost::filesystem::rename(dirPath, tmpPath, ec);
    FW_RAISE_IF("rename " << dirPath.string() << " to " << tmpPath.string() << " error : " << ec.message(), ec.value());

    ::boost::filesystem::create_directory(dirPath, ec);
    FW_RAISE_IF("create_directory " << dirPath.string() << " error : " << ec.message(), ec.value());

    std::vector<std::string> dicomFiles;
    ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(tmpPath, dicomFiles);

    // Reset progress
    m_totalNumberOfTags = s_NUMBER_OF_TAGS * dicomFiles.size();
    m_tagIndex          = 0;
    std::string oldSOPInstanceUID;

    unsigned int fileIndex = 0;
    BOOST_FOREACH(std::string file, dicomFiles)
    {
        // Create a scope for the GDCM Reader/Writer so the file is released when we want to rename it
        {
            // File Reader
            ::gdcm::Reader reader;
            reader.SetFileName( file.c_str() );
            if( !reader.Read() )
            {
                SLM_ERROR ( "Unable to anonymise this file (File read failed) : " + file);
            }

            // String filter
            m_stringFilter.SetFile(reader.GetFile());

            // Objects used to scan groups of elements
            ::gdcm::Tag tag;
            ::gdcm::DataElement dataElement;
            ::gdcm::File &datasetFile = reader.GetFile();
            ::gdcm::DataSet &dataset  = datasetFile.GetDataSet();

            // Save old SOP instance uid to find the new one in the map
            oldSOPInstanceUID = m_stringFilter.ToString(::gdcm::Tag(0x0008,0x0018));

            m_anonymizer.SetFile( reader.GetFile() );

            // Remove group length
            m_anonymizer.RemoveGroupLength();

            // Remove retired tags
            m_anonymizer.RemoveRetired();

            this->processTag(::gdcm::Tag(0x0008,0x0050), "Z");        //Accession Number
            this->processTag(::gdcm::Tag(0x0018,0x4000), "X");        //Acquisition Comments
            this->processTag(::gdcm::Tag(0x0040,0x0555), "X");        //Acquisition Context Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0022), "X/Z");      //Acquisition Date
            this->processTag(::gdcm::Tag(0x0008,0x002A), "X/D");      //Acquisition DateTime
            this->processTag(::gdcm::Tag(0x0018,0x1400), "X/D");      //Acquisition Device Processing Description
            this->processTag(::gdcm::Tag(0x0018,0x9424), "X");        //Acquisition Protocol Description
            this->processTag(::gdcm::Tag(0x0008,0x0032), "X/Z");      //Acquisition Time
            this->processTag(::gdcm::Tag(0x0040,0x4035), "X");        //Actual Human Performers Sequence
            this->processTag(::gdcm::Tag(0x0010,0x21B0), "X");        //Additional Patient’s History
            this->processTag(::gdcm::Tag(0x0038,0x0010), "X");        //Admission ID
            this->processTag(::gdcm::Tag(0x0038,0x0020), "X");        //Admitting Date
            this->processTag(::gdcm::Tag(0x0008,0x1084), "X");        //Admitting Diagnoses Code Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1080), "X");        //Admitting Diagnoses Description
            this->processTag(::gdcm::Tag(0x0038,0x0021), "X");        //Admitting Time
            this->processTag(::gdcm::Tag(0x0000,0x1000), "X");        //Affected SOP Instance UID
            this->processTag(::gdcm::Tag(0x0010,0x2110), "X");        //Allergies
            this->processTag(::gdcm::Tag(0x4000,0x0010), "X");        //Arbitrary
            this->processTag(::gdcm::Tag(0x0040,0xA078), "X");        //Author Observer Sequence
            this->processTag(::gdcm::Tag(0x0010,0x1081), "X");        //Branch of Service
            this->processTag(::gdcm::Tag(0x0018,0x1007), "X");        //Cassette ID
            this->processTag(::gdcm::Tag(0x0040,0x0280), "X");        //Comments on Performed Procedure Step
            this->processTag(::gdcm::Tag(0x0020,0x9161), "U");        //Concatenation UID
            this->processTag(::gdcm::Tag(0x0040,0x3001), "X");        //Confidentiality Constraint on Patient Data Description
            this->processTag(::gdcm::Tag(0x0070,0x0084), "Z");        //Content Creator’s Name
            this->processTag(::gdcm::Tag(0x0070,0x0086), "X");        //Content Creator’s Identification Code Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0023), "Z/D");      //Content Date
            this->processTag(::gdcm::Tag(0x0040,0xA730), "X");        //Content Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0033), "Z/D");      //Content Time
            this->processTag(::gdcm::Tag(0x0008,0x010D), "U");        //Context Group Extension Creator UID
            this->processTag(::gdcm::Tag(0x0018,0x0010), "Z/D");      //Contrast Bolus Agent
            this->processTag(::gdcm::Tag(0x0018,0xA003), "X");        //Contribution Description
            this->processTag(::gdcm::Tag(0x0010,0x2150), "X");        //Country of Residence
            this->processTag(::gdcm::Tag(0x0008,0x9123), "U");        //Creator Version UID
            this->processTag(::gdcm::Tag(0x0038,0x0300), "X");        //Current Patient Location

            // Curve Data (0x50xx,0xxxxx)
            dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x5000,0x0));
            tag         = dataElement.GetTag();
            while (((tag.GetGroup() >> 8) & 0xff)  == 0x50 )
            {
                --m_tagIndex;   //Do not increase progress bar as we do not know how much tags meet the criteria
                this->processTag(tag, "X");    //Curve Data
                dataElement = dataset.FindNextDataElement(::gdcm::Tag(tag.GetGroup(), tag.GetElement()+1));
                tag         = dataElement.GetTag();
            }

            this->processTag(::gdcm::Tag(0x0008,0x0025), "X");        //Curve Date
            this->processTag(::gdcm::Tag(0x0008,0x0035), "X");        //Curve Time
            this->processTag(::gdcm::Tag(0x0040,0xA07C), "X");        //Custodial Organization Sequence
            this->processTag(::gdcm::Tag(0xFFFC,0xFFFC), "X");        //Data Set Trailing Padding
            this->processTag(::gdcm::Tag(0x0008,0x2111), "X");        //Derivation Description
            this->processTag(::gdcm::Tag(0x0018,0x700A), "X");        //Detector ID
            this->processTag(::gdcm::Tag(0x0018,0x1000), "X/Z/D");    //Device Serial Number
            this->processTag(::gdcm::Tag(0x0018,0x1002), "U");        //Device UID
            this->processTag(::gdcm::Tag(0x0400,0x0100), "X");        //Digital Signature UID
            this->processTag(::gdcm::Tag(0xFFFA,0xFFFA), "X");        //Digital Signatures Sequence
            this->processTag(::gdcm::Tag(0x0020,0x9164), "U");        //Dimension Organization UID
            this->processTag(::gdcm::Tag(0x0038,0x0040), "X");        //Discharge Diagnosis Description
            this->processTag(::gdcm::Tag(0x4008,0x011A), "X");        //Distribution Address
            this->processTag(::gdcm::Tag(0x4008,0x0119), "X");        //Distribution Name
            this->processTag(::gdcm::Tag(0x300A,0x0013), "U");        //Dose Reference UID
            this->processTag(::gdcm::Tag(0x0010,0x2160), "X");        //Ethnic Group
            this->processTag(::gdcm::Tag(0x0008,0x0058), "U");        //Failed SOP Instance UID List
            this->processTag(::gdcm::Tag(0x0070,0x031A), "U");        //Fiducial UID
            this->processTag(::gdcm::Tag(0x0040,0x2017), "Z");        //Filler Order Number of Imaging Service Request
            this->processTag(::gdcm::Tag(0x0020,0x9158), "X");        //Frame Comments
            this->processTag(::gdcm::Tag(0x0020,0x0052), "U");        //Frame of Reference UID
            this->processTag(::gdcm::Tag(0x0018,0x1008), "X");        //Gantry ID
            this->processTag(::gdcm::Tag(0x0018,0x1005), "X");        //Generator ID
            this->processTag(::gdcm::Tag(0x0070,0x0001), "D");        //Graphic Annotation Sequence
            this->processTag(::gdcm::Tag(0x0040,0x4037), "X");        //Human Performers Name
            this->processTag(::gdcm::Tag(0x0040,0x4036), "X");        //Human Performers Organization
            this->processTag(::gdcm::Tag(0x0088,0x0200), "X");        //Icon Image Sequence(see Note 12)
            this->processTag(::gdcm::Tag(0x0008,0x4000), "X");        //Identifying Comments
            this->processTag(::gdcm::Tag(0x0020,0x4000), "X");        //Image Comments
            this->processTag(::gdcm::Tag(0x0028,0x4000), "X");        //Image Presentation Comments
            this->processTag(::gdcm::Tag(0x0040,0x2400), "X");        //Imaging Service Request Comments
            this->processTag(::gdcm::Tag(0x4008,0x0300), "X");        //Impressions
            this->processTag(::gdcm::Tag(0x0008,0x0014), "U");        //Instance Creator UID
            this->processTag(::gdcm::Tag(0x0008,0x0081), "X");        //Institution Address
            this->processTag(::gdcm::Tag(0x0008,0x0082), "X/Z/D");    //Institution Code Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0080), "X/Z/D");    //Institution Name
            this->processTag(::gdcm::Tag(0x0008,0x1040), "X");        //Institutional Department Name
            this->processTag(::gdcm::Tag(0x0010,0x1050), "X");        //Insurance Plan Identification
            this->processTag(::gdcm::Tag(0x0040,0x1011), "X");        //Intended Recipients of Results Identification Sequence
            this->processTag(::gdcm::Tag(0x4008,0x0111), "X");        //Interpretation Approver Sequence
            this->processTag(::gdcm::Tag(0x4008,0x010C), "X");        //Interpretation Author
            this->processTag(::gdcm::Tag(0x4008,0x0115), "X");        //Interpretation Diagnosis Description
            this->processTag(::gdcm::Tag(0x4008,0x0202), "X");        //Interpretation ID Issuer
            this->processTag(::gdcm::Tag(0x4008,0x0102), "X");        //Interpretation Recorder
            this->processTag(::gdcm::Tag(0x4008,0x010B), "X");        //Interpretation Text
            this->processTag(::gdcm::Tag(0x4008,0x010A), "X");        //Interpretation Transcriber
            this->processTag(::gdcm::Tag(0x0008,0x3010), "U");        //Irradiation Event UID
            this->processTag(::gdcm::Tag(0x0038,0x0011), "X");        //Issuer of Admission ID
            this->processTag(::gdcm::Tag(0x0010,0x0021), "X");        //Issuer of Patient ID
            this->processTag(::gdcm::Tag(0x0038,0x0061), "X");        //Issuer of Service Episode ID
            this->processTag(::gdcm::Tag(0x0028,0x1214), "U");        //Large Palette Color Lookup Table UID
            this->processTag(::gdcm::Tag(0x0010,0x21D0), "X");        //Last Menstrual Date
            this->processTag(::gdcm::Tag(0x0400,0x0404), "X");        //MAC
            this->processTag(::gdcm::Tag(0x0002,0x0003), "U");        //Media Storage SOP Instance UID
            this->processTag(::gdcm::Tag(0x0010,0x2000), "X");        //Medical Alerts
            this->processTag(::gdcm::Tag(0x0010,0x1090), "X");        //Medical Record Locator
            this->processTag(::gdcm::Tag(0x0010,0x1080), "X");        //Military Rank
            this->processTag(::gdcm::Tag(0x0400,0x0550), "X");        //Modified Attributes Sequence
            this->processTag(::gdcm::Tag(0x0020,0x3406), "X");        //Modified Image Description
            this->processTag(::gdcm::Tag(0x0020,0x3401), "X");        //Modifying Device ID
            this->processTag(::gdcm::Tag(0x0020,0x3404), "X");        //Modifying Device Manufacturer
            this->processTag(::gdcm::Tag(0x0008,0x1060), "X");        //Name of Physician(s) Reading Study
            this->processTag(::gdcm::Tag(0x0040,0x1010), "X");        //Names of Intended Recipient of Results
            this->processTag(::gdcm::Tag(0x0010,0x2180), "X");        //Occupation
            this->processTag(::gdcm::Tag(0x0008,0x1072), "X/D");      //Operators’ Identification Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1070), "X/Z/D");    //Operators’ Name
            this->processTag(::gdcm::Tag(0x0400,0x0561), "X");        //Original Attributes Sequence
            this->processTag(::gdcm::Tag(0x0040,0x2010), "X");        //Order Callback Phone Number
            this->processTag(::gdcm::Tag(0x0040,0x2008), "X");        //Order Entered By
            this->processTag(::gdcm::Tag(0x0040,0x2009), "X");        //Order Enterer Location
            this->processTag(::gdcm::Tag(0x0010,0x1000), "X");        //Other Patient IDs
            this->processTag(::gdcm::Tag(0x0010,0x1002), "X");        //Other Patient IDs Sequence
            this->processTag(::gdcm::Tag(0x0010,0x1001), "X");        //Other Patient Names

            // Overlay Comments (0x60xx,0x4000)
            dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x6000,0x4000));
            tag         = dataElement.GetTag();
            while (((tag.GetGroup() >> 8) & 0xff)  == 0x60 )
            {
                if(tag.GetElement() == 0x4000)
                {
                    --m_tagIndex;   //Do not increase progress bar as we do not know how much tags meet the criteria
                    this->processTag(tag, "X");
                }
                dataElement = dataset.FindNextDataElement(::gdcm::Tag(tag.GetGroup()+1, 0x4000));
                tag         = dataElement.GetTag();
            }

            // Overlay Data (0x60xx,0x3000)
            dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x6000,0x3000));
            tag         = dataElement.GetTag();
            while (((tag.GetGroup() >> 8) & 0xff)  == 0x60 )
            {
                if(tag.GetElement() == 0x3000)
                {
                    --m_tagIndex;   //Do not increase progress bar as we do not know how much tags meet the criteria
                    this->processTag(tag, "X");
                }
                dataElement = dataset.FindNextDataElement(::gdcm::Tag(tag.GetGroup()+1, 0x3000));
                tag         = dataElement.GetTag();
            }

            this->processTag(::gdcm::Tag(0x0008,0x0024), "X");        //Overlay Date
            this->processTag(::gdcm::Tag(0x0008,0x0034), "X");        //Overlay Time
            this->processTag(::gdcm::Tag(0x0028,0x1199), "U");        //Palette Color Lookup Table UID
            this->processTag(::gdcm::Tag(0x0040,0xA07A), "X");        //Participant Sequence
            this->processTag(::gdcm::Tag(0x0010,0x1040), "X");        //Patient Address
            this->processTag(::gdcm::Tag(0x0010,0x4000), "X");        //Patient Comments
            this->processTag(::gdcm::Tag(0x0010,0x0020), "Z");        //Patient ID
            this->processTag(::gdcm::Tag(0x0010,0x2203), "X/Z");      //Patient Sex Neutered
            this->processTag(::gdcm::Tag(0x0038,0x0500), "X");        //Patient State
            this->processTag(::gdcm::Tag(0x0040,0x1004), "X");        //Patient Transport Arrangements
            this->processTag(::gdcm::Tag(0x0010,0x1010), "X");        //Patient’s Age
            this->processTag(::gdcm::Tag(0x0010,0x0030), "Z");        //Patient’s Birth Date
            this->processTag(::gdcm::Tag(0x0010,0x1005), "X");        //Patient’s Birth Name
            this->processTag(::gdcm::Tag(0x0010,0x0032), "X");        //Patient’s Birth Time
            this->processTag(::gdcm::Tag(0x0038,0x0400), "X");        //Patient’s Institution Residence
            this->processTag(::gdcm::Tag(0x0010,0x0050), "X");        //Patient’s Insurance Plan Code Sequence
            this->processTag(::gdcm::Tag(0x0010,0x1060), "X");        //Patient’s Mother’s Birth Name
            this->processTag(::gdcm::Tag(0x0010,0x0010), "Z");        //Patient’s Name
            this->processTag(::gdcm::Tag(0x0010,0x0101), "X");        //Patient’s Primary Language Code Sequence
            this->processTag(::gdcm::Tag(0x0010,0x0102), "X");        //Patient’s Primary Language Modifier Code Sequence
            this->processTag(::gdcm::Tag(0x0010,0x21F0), "X");        //Patient’s Religious Preference
            this->processTag(::gdcm::Tag(0x0010,0x0040), "Z");        //Patient’s Sex
            this->processTag(::gdcm::Tag(0x0010,0x1020), "X");        //Patient’s Size
            this->processTag(::gdcm::Tag(0x0010,0x2154), "X");        //Patient’s Telephone Number
            this->processTag(::gdcm::Tag(0x0010,0x1030), "X");        //Patient’s Weight
            this->processTag(::gdcm::Tag(0x0040,0x0243), "X");        //Performed Location
            this->processTag(::gdcm::Tag(0x0040,0x0254), "X");        //Performed Procedure Step Description
            this->processTag(::gdcm::Tag(0x0040,0x0253), "X");        //Performed Procedure Step ID
            this->processTag(::gdcm::Tag(0x0040,0x0244), "X");        //Performed Procedure Step Start Date
            this->processTag(::gdcm::Tag(0x0040,0x0245), "X");        //Performed Procedure Step Start Time
            this->processTag(::gdcm::Tag(0x0040,0x0241), "X");        //Performed Station AE Title
            this->processTag(::gdcm::Tag(0x0040,0x4030), "X");        //Performed Station Geographic Location Code Sequence
            this->processTag(::gdcm::Tag(0x0040,0x0242), "X");        //Performed Station Name
            this->processTag(::gdcm::Tag(0x0040,0x0248), "X");        //Performed Station Name Code Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1052), "X");        //Performing Physicians’ Identification Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1050), "X");        //Performing Physicians’ Name
            this->processTag(::gdcm::Tag(0x0040,0x1102), "X");        //Person Address
            this->processTag(::gdcm::Tag(0x0040,0x1101), "D");        //Person Identification Code Sequence
            this->processTag(::gdcm::Tag(0x0040,0xA123), "D");        //Person Name
            this->processTag(::gdcm::Tag(0x0040,0x1103), "X");        //Person Telephone Numbers
            this->processTag(::gdcm::Tag(0x4008,0x0114), "X");        //Physician Approving Interpretation
            this->processTag(::gdcm::Tag(0x0008,0x1062), "X");        //Physician Reading Study Identification Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1048), "X");        //Physician(s) Record
            this->processTag(::gdcm::Tag(0x0008,0x1049), "X");        //Physician(s) of Record Identification Sequence
            this->processTag(::gdcm::Tag(0x0040,0x2016), "Z");        //Placer Order Number of Imaging Service Request
            this->processTag(::gdcm::Tag(0x0018,0x1004), "X");        //Plate ID
            this->processTag(::gdcm::Tag(0x0040,0x0012), "X");        //Pre-Medication
            this->processTag(::gdcm::Tag(0x0010,0x21C0), "X");        //Pregnancy Status

            m_anonymizer.RemovePrivateTags();                                        //Private attributes (X)

            this->processTag(::gdcm::Tag(0x0018,0x1030), "X/D");      //Protocol Name
            this->processTag(::gdcm::Tag(0x0040,0x2001), "X");        //Reason for Imaging Service Request
            this->processTag(::gdcm::Tag(0x0032,0x1030), "X");        //Reason for Study
            this->processTag(::gdcm::Tag(0x0400,0x0402), "X");        //Referenced Digital Signature Sequence
            this->processTag(::gdcm::Tag(0x3006,0x0024), "U");        //Referenced Frame of Reference UID
            this->processTag(::gdcm::Tag(0x0040,0x4023), "U");        //Referenced General Purpose Scheduled Procedure Step Transaction UID
            this->processTag(::gdcm::Tag(0x0008,0x1140), "X/Z/U*");   //Referenced Image Sequence
            this->processTag(::gdcm::Tag(0x0038,0x1234), "X");        //Referenced Patient Alias Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1120), "X");        //Referenced Patient Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1111), "X/Z/D");    //Referenced Performed Procedure Step Sequence
            this->processTag(::gdcm::Tag(0x0400,0x0403), "X");        //Referenced SOP Instance MAC Sequence
            this->processTag(::gdcm::Tag(0x0008,0x1155), "U");        //Referenced SOP Instance UID
            this->processTag(::gdcm::Tag(0x0004,0x1511), "U");        //Referenced SOP Instance UID in File
            this->processTag(::gdcm::Tag(0x0008,0x1110), "X/Z");      //Referenced Study Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0092), "X");        //Referring Physician’s Address
            this->processTag(::gdcm::Tag(0x0008,0x0096), "X");        //Referring Physician’s Identification Sequence
            this->processTag(::gdcm::Tag(0x0008,0x0090), "Z");        //Referring Physician’s Name
            this->processTag(::gdcm::Tag(0x0008,0x0094), "X");        //Referring Physician’s Telephone Numbers
            this->processTag(::gdcm::Tag(0x0010,0x2152), "X");        //Region of Residence
            this->processTag(::gdcm::Tag(0x3006,0x00C2), "U");        //Related Frame of Reference UID
            this->processTag(::gdcm::Tag(0x0040,0x0275), "X");        //Request Attributes Sequence
            this->processTag(::gdcm::Tag(0x0032,0x1070), "X");        //Requested Contrast Agent
            this->processTag(::gdcm::Tag(0x0040,0x1400), "X");        //Requested Procedure Comments
            this->processTag(::gdcm::Tag(0x0032,0x1060), "X/Z");      //Requested Procedure Description
            this->processTag(::gdcm::Tag(0x0040,0x1001), "X");        //Requested Procedure ID
            this->processTag(::gdcm::Tag(0x0040,0x1005), "X");        //Requested Procedure Location
            this->processTag(::gdcm::Tag(0x0000,0x1001), "U");        //Requested SOP Instance UID
            this->processTag(::gdcm::Tag(0x0032,0x1032), "X");        //Requesting Physician
            this->processTag(::gdcm::Tag(0x0032,0x1033), "X");        //Requesting Service
            this->processTag(::gdcm::Tag(0x0010,0x2299), "X");        //Responsible Organization
            this->processTag(::gdcm::Tag(0x0010,0x2297), "X");        //Responsible Person
            this->processTag(::gdcm::Tag(0x4008,0x4000), "X");        //Results Comments
            this->processTag(::gdcm::Tag(0x4008,0x0118), "X");        //Results Distribution List Sequence
            this->processTag(::gdcm::Tag(0x4008,0x0042), "X");        //Results ID Issuer
            this->processTag(::gdcm::Tag(0x300E,0x0008), "X/Z");      //Reviewer Name
            this->processTag(::gdcm::Tag(0x0040,0x4034), "X");        //Scheduled Human Performers Sequence
            this->processTag(::gdcm::Tag(0x0038,0x001E), "X");        //Scheduled Patient Institution Residence
            this->processTag(::gdcm::Tag(0x0040,0x000B), "X");        //Scheduled Performing Physician Identification Sequence
            this->processTag(::gdcm::Tag(0x0040,0x0006), "X");        //Scheduled Performing Physician Name
            this->processTag(::gdcm::Tag(0x0040,0x0004), "X");        //Scheduled Procedure Step End Date
            this->processTag(::gdcm::Tag(0x0040,0x0005), "X");        //Scheduled Procedure Step End Time
            this->processTag(::gdcm::Tag(0x0040,0x0007), "X");        //Scheduled Procedure Step Description
            this->processTag(::gdcm::Tag(0x0040,0x0011), "X");        //Scheduled Procedure Step Location
            this->processTag(::gdcm::Tag(0x0040,0x0002), "X");        //Scheduled Procedure Step Start Date
            this->processTag(::gdcm::Tag(0x0040,0x0003), "X");        //Scheduled Procedure Step Start Time
            this->processTag(::gdcm::Tag(0x0040,0x0001), "X");        //Scheduled Station AE Title
            this->processTag(::gdcm::Tag(0x0040,0x4027), "X");        //Scheduled Station Geographic Location Code Sequence
            this->processTag(::gdcm::Tag(0x0040,0x0010), "X");        //Scheduled Station Name
            this->processTag(::gdcm::Tag(0x0040,0x4025), "X");        //Scheduled Station Name Code Sequence
            this->processTag(::gdcm::Tag(0x0032,0x1020), "X");        //Scheduled Study Location
            this->processTag(::gdcm::Tag(0x0032,0x1021), "X");        //Scheduled Study Location AE Title
            this->processTag(::gdcm::Tag(0x0008,0x0021), "X/D");      //Series Date
            this->processTag(::gdcm::Tag(0x0008,0x103E), "X");        //Series Description
            this->processTag(::gdcm::Tag(0x0020,0x000E), "U");        //Series Instance UID
            this->processTag(::gdcm::Tag(0x0008,0x0031), "X/D");      //Series Time
            this->processTag(::gdcm::Tag(0x0038,0x0062), "X");        //Service Episode Description
            this->processTag(::gdcm::Tag(0x0038,0x0060), "X");        //Service Episode ID
            this->processTag(::gdcm::Tag(0x0010,0x21A0), "X");        //Smoking Status
            this->processTag(::gdcm::Tag(0x0008,0x0018), "U");        //SOP Instance UID
            this->processTag(::gdcm::Tag(0x0008,0x2112), "X/Z/U*");   //Source Image Sequence
            this->processTag(::gdcm::Tag(0x0038,0x0050), "X");        //Special Needs
            this->processTag(::gdcm::Tag(0x0008,0x1010), "X/Z/D");    //Station Name
            this->processTag(::gdcm::Tag(0x0088,0x0140), "U");        //Storage Media File-set UID
            this->processTag(::gdcm::Tag(0x0032,0x4000), "X");        //Study Comments
            this->processTag(::gdcm::Tag(0x0008,0x0020), "Z");        //Study Date
            this->processTag(::gdcm::Tag(0x0008,0x1030), "X");        //Study Description
            this->processTag(::gdcm::Tag(0x0020,0x0010), "Z");        //Study ID
            this->processTag(::gdcm::Tag(0x0032,0x0012), "X");        //Study ID Issuer
            this->processTag(::gdcm::Tag(0x0020,0x000D), "U");        //Study Instance UID
            this->processTag(::gdcm::Tag(0x0008,0x0030), "Z");        //Study Time
            this->processTag(::gdcm::Tag(0x0020,0x0200), "U");        //Synchronization Frame of Reference UID
            this->processTag(::gdcm::Tag(0x0040,0xDB0D), "U");        //Template Extension Creator UID
            this->processTag(::gdcm::Tag(0x0040,0xDB0C), "U");        //Template Extension Organization UID
            this->processTag(::gdcm::Tag(0x4000,0x4000), "X");        //Text Comments
            this->processTag(::gdcm::Tag(0x2030,0x0020), "X");        //Text String
            this->processTag(::gdcm::Tag(0x0008,0x0201), "X");        //Timezone Offset From UTC
            this->processTag(::gdcm::Tag(0x0088,0x0910), "X");        //Topic Author
            this->processTag(::gdcm::Tag(0x0088,0x0912), "X");        //Topic Key Words
            this->processTag(::gdcm::Tag(0x0088,0x0906), "X");        //Topic Subject
            this->processTag(::gdcm::Tag(0x0088,0x0904), "X");        //Topic Title
            this->processTag(::gdcm::Tag(0x0008,0x1195), "U");        //Transaction UID
            this->processTag(::gdcm::Tag(0x0040,0xA124), "U");        //UID
            this->processTag(::gdcm::Tag(0x0040,0xA088), "Z");        //Verifying Observer Identification Code Sequence
            this->processTag(::gdcm::Tag(0x0040,0xA075), "D");        //Verifying Observer Name
            this->processTag(::gdcm::Tag(0x0040,0xA073), "D");        //Verifying Observer Sequence
            this->processTag(::gdcm::Tag(0x0040,0xA027), "X");        //Verifying Organization
            this->processTag(::gdcm::Tag(0x0038,0x4000), "X");        //Visit Comments


            // Write file
            ::gdcm::Writer writer;
            writer.SetFile( reader.GetFile() );

            std::stringstream ss;
            ss << "im" << std::setfill('0') << std::setw(5) << fileIndex;

            ::boost::filesystem::path destination = dirPath / ss.str();
            writer.SetFileName( destination.string().c_str() );
            if( !writer.Write() )
            {
                SLM_ERROR ( "Unable to anonymise this file (File write failed) : " + file);
            }

            ::boost::system::error_code ec;
            ::boost::filesystem::remove(tmpPath / ss.str(), ec);
            FW_RAISE_IF("remove file " << (tmpPath / ss.str()).string() << " error : " << ec.message(), ec.value());

        }    // Delete the scope
        ++fileIndex;
    }

    ::boost::filesystem::remove_all(tmpPath, ec);
    FW_RAISE_IF("remove anonymize dir " << tmpPath.string() << " error : " << ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymizeAndZip(const ::boost::filesystem::path &dirPath, ::fwZip::IWriteArchive::sptr archive)
{
    this->anonymize(dirPath);

    std::vector<std::string> dicomFiles;
    ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(dirPath, dicomFiles);

    const int bufferSize = 8192;
    char bufferIn[bufferSize];
    char bufferOut[bufferSize];

    unsigned int i = 0;
    BOOST_FOREACH(std::string file, dicomFiles)
    {
        std::ifstream src(file.c_str(), std::ios::binary);
        src.rdbuf()->pubsetbuf(bufferIn, bufferSize);

        SPTR(std::ostream) os = archive->createFile(::boost::filesystem::path(file).filename());
        os->rdbuf()->pubsetbuf(bufferOut, bufferSize);
        *os << src.rdbuf();

        this->notifyProgress(float(++i)/dicomFiles.size(), "Saving zip file...");
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::addExceptionTag(uint16_t group, uint16_t element, const std::string &value)
{
    m_exceptionTagMap[ ::gdcm::Tag(group,element)] = value;
}

//------------------------------------------------------------------------------

void DicomAnonymizer::processTag(const ::gdcm::Tag& tag, const std::string &actionCode)
{
    OSLM_TRACE("Anonymizing tag: (" << std::hex << tag.GetGroup() << "," << std::hex << tag.GetElement() << ")");

    const ::gdcm::DataSet& dataSet        = m_anonymizer.GetFile().GetDataSet();
    const ::gdcm::DataElement dataElement = dataSet.GetDataElement(tag);

    //Exception
    if(m_exceptionTagMap.find(tag) != m_exceptionTagMap.end())
    {
        m_anonymizer.Replace(tag, m_exceptionTagMap[tag].c_str());
    }
    //X – remove
    else if(actionCode == "X")
    {
        this->applyActionCodeX(tag);
    }
    //Z – replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with the VR
    else if(actionCode == "Z")
    {
        this->applyActionCodeZ(tag);
    }
    //D – replace with a non-zero length value that may be a dummy value and consistent with the VR
    else if(actionCode == "D")
    {
        this->applyActionCodeD(tag);
    }
    //K – keep (unchanged for non-sequence attributes, cleaned for sequences)
    else if(actionCode == "K")
    {
        this->applyActionCodeK(tag);
    }
    //C – clean, that is replace with values of similar meaning known not to contain identifying information and consistent with the VR
    else if(actionCode == "C")
    {
        this->applyActionCodeC(tag);
    }
    //U – replace with a non-zero length UID that is internally consistent within a set of Instances
    else if(actionCode == "U")
    {
        this->applyActionCodeU(tag);
    }
    //Z/D – Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
    else if(actionCode == "Z/D")
    {
        this->applyActionCodeZ(tag);
    }
    //X/Z – X unless Z is required to maintain IOD conformance (Type 3 versus Type 2)
    else if(actionCode == "X/Z")
    {
        this->applyActionCodeX(tag);
    }
    //X/D – X unless D is required to maintain IOD conformance (Type 3 versus Type 1)
    else if(actionCode == "X/D")
    {
        this->applyActionCodeX(tag);
    }
    //X/Z/D – X unless Z or D is required to maintain IOD conformance (Type 3 versus Type 2 versus Type 1)
    else if(actionCode == "X/Z/D")
    {
        this->applyActionCodeX(tag);
    }
    //X/Z/U* - X unless Z or replacement of contained instance UIDs (U) is required to maintain IOD conformance
    //(Type 3 versus Type 2 versus Type 1 sequences containing UID references)
    else if(actionCode == "X/Z/U*")
    {
        this->applyActionCodeX(tag);
    }
    else
    {
        SLM_FATAL("Action code \"" + actionCode + "\" is not specified in the DICOM Standard !");
    }

    this->notifyProgress(float(++m_tagIndex)/m_totalNumberOfTags, "Processing tags...");
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeD(const ::gdcm::Tag& tag)
{
    // Sequence of Items
    if(m_publicDictionary.GetDictEntry(tag).GetVR() == ::gdcm::VR::SQ)
    {
        m_anonymizer.Empty(tag);
    }
    else
    {
        this->generateDummyValue(tag);
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeZ(const ::gdcm::Tag& tag)
{
    this->applyActionCodeD(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeX(const ::gdcm::Tag& tag)
{
    m_anonymizer.Remove(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeK(const ::gdcm::Tag& tag)
{
    // Sequence of Items
    if(m_publicDictionary.GetDictEntry(tag).GetVR() == ::gdcm::VR::SQ)
    {
        m_anonymizer.Empty(tag);
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeC(const ::gdcm::Tag& tag)
{
    SLM_FATAL(
        "Basic profile \"C\" is not supported yet : Only basic profile is supported by the current implementation.");
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeU(const ::gdcm::Tag& tag)
{
    ::gdcm::UIDGenerator generator;
    std::string oldUID = m_stringFilter.ToString(tag);
    if(oldUID.empty())
    {
        m_anonymizer.Replace(tag, generator.Generate());
    }
    else if(m_uidMap.find(oldUID) != m_uidMap.end())
    {
        m_anonymizer.Replace(tag, m_uidMap[oldUID].c_str());
    }
    else
    {
        m_uidMap[oldUID] = generator.Generate();
        m_anonymizer.Replace(tag, m_uidMap[oldUID].c_str());
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::generateDummyValue(const ::gdcm::Tag& tag)
{

    switch (m_publicDictionary.GetDictEntry(tag).GetVR())
    {
        case ::gdcm::VR::AE:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::AS:
        {
            m_anonymizer.Replace(tag, "000Y");
            break;
        }
        case ::gdcm::VR::AT:
        {
            m_anonymizer.Replace(tag, "00H,00H,00H,00H");
            break;
        }
        case ::gdcm::VR::CS:
        {
            //Patient’s Sex
            if(tag == ::gdcm::Tag(0x0010,0x0040))
            {
                m_anonymizer.Replace(tag, "O");
            }
            else
            {
                m_anonymizer.Replace(tag, "ANONYMIZED");
            }
            break;
        }
        case ::gdcm::VR::DA:
        {
            m_anonymizer.Replace(tag, "19700101");
            break;
        }
        case ::gdcm::VR::DS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::DT:
        {
            m_anonymizer.Replace(tag, "19700101000000.000000");
            break;
        }
        case ::gdcm::VR::FD:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::FL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::IS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::LO:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::LT:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::OB:
        {
            m_anonymizer.Replace(tag, "00H00H");
            break;
        }
        case ::gdcm::VR::OF:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::OW:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::PN:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED^ANONYMIZED");
            break;
        }
        case ::gdcm::VR::SH:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::SL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::SQ:
        {
            m_anonymizer.Empty(tag);
            break;
        }
        case ::gdcm::VR::SS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::ST:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::TM:
        {
            m_anonymizer.Replace(tag, "000000.000000");
            break;
        }
        case ::gdcm::VR::UI:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::UL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::UN:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::US:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::UT:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        default:
        {
            SLM_FATAL("Unknown value representation.");
            break;
        }
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::copyDirectory(::boost::filesystem::path input, ::boost::filesystem::path output)
{
    ::boost::system::error_code ec;
    ::boost::filesystem::copy_directory(input, output, ec);
    FW_RAISE_IF("copy_directory " << input.string() << " " << output.string() << " error : " << ec.message(),
                ec.value());

    ::boost::filesystem::directory_iterator it(input);
    ::boost::filesystem::directory_iterator end;
    ::boost::filesystem::permissions(output, ::boost::filesystem::owner_all, ec);
    SLM_ERROR_IF("set " + output.string() + " permission error : " + ec.message(), ec.value());

    for(; it != end; ++it)
    {
        ::boost::filesystem::path dest = output / it->path().filename();
        if(::boost::filesystem::is_directory(*it))
        {
            DicomAnonymizer::copyDirectory(*it, dest);
        }
        else
        {
            ::boost::filesystem::copy(*it, dest, ec);
            FW_RAISE_IF(
                "copy_directory " << it->path().string() << " " << dest.string() << " error : " << ec.message(),
                ec.value());
        }

        ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, ec);
        SLM_ERROR_IF("set " + dest.string() + " permission error : " + ec.message(), ec.value());
    }
}

} // namespace helper
} // namespace gdcm
} // namespace fwDicomIOExt


