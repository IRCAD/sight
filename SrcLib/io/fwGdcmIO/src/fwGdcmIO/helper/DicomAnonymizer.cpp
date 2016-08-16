/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomAnonymizer.hpp"
#include "fwGdcmIO/helper/DicomSearch.hpp"

#include <fwCore/base.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>
#include <fwTools/System.hpp>

#include <gdcmGlobal.h>
#include <gdcmReader.h>
#include <gdcmUIDGenerator.h>
#include <gdcmWriter.h>

#include <boost/exception/all.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <cstdint>
#include <iomanip>
#include <time.h>

namespace fwGdcmIO
{
namespace helper
{

::gdcm::UIDGenerator GENERATOR;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_D_TAGS = ::boost::assign::list_of
                                                                                    (::gdcm::Tag(0x0070, 0x0001)) //Graphic Annotation Sequence
                                                                                    (::gdcm::Tag(0x0040, 0x1101)) //Person Identification Code Sequence
                                                                                    (::gdcm::Tag(0x0040, 0xA123)) //Person Name
                                                                                    (::gdcm::Tag(0x0040, 0xA075)) //Verifying Observer Name
                                                                                    (::gdcm::Tag(0x0040, 0xA073)) //Verifying Observer Sequence
;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_Z_TAGS = ::boost::assign::list_of
                                                                                    (::gdcm::Tag(0x0008, 0x0050)) //Accession Number
                                                                                    (::gdcm::Tag(0x0070, 0x0084)) //Content Creator’s Name
                                                                                    (::gdcm::Tag(0x0008, 0x0023)) //Content Date
                                                                                    (::gdcm::Tag(0x0008, 0x0033)) //Content Time
                                                                                    (::gdcm::Tag(0x0018, 0x0010)) //Contrast Bolus Agent
                                                                                    (::gdcm::Tag(0x0040, 0x2017)) //Filler Order Number of Imaging Service Request
                                                                                    (::gdcm::Tag(0x0010, 0x0020)) //Patient ID
                                                                                    (::gdcm::Tag(0x0010, 0x0030)) //Patient’s Birth Date
                                                                                    (::gdcm::Tag(0x0010, 0x0010)) //Patient’s Name
                                                                                    (::gdcm::Tag(0x0010, 0x0040)) //Patient’s Sex
                                                                                    (::gdcm::Tag(0x0040, 0x2016)) //Placer Order Number of Imaging Service Request
                                                                                    (::gdcm::Tag(0x0008, 0x0090)) //Referring Physician’s Name
                                                                                    (::gdcm::Tag(0x0008, 0x0020)) //Study Date
                                                                                    (::gdcm::Tag(0x0020, 0x0010)) //Study ID
                                                                                    (::gdcm::Tag(0x0008, 0x0030)) //Study Time
                                                                                    (::gdcm::Tag(0x0040, 0xA088)) //Verifying Observer Identification Code Sequence
;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_X_TAGS = ::boost::assign::list_of
                                                                                    (::gdcm::Tag(0x0018, 0x4000)) //Acquisition Comments
                                                                                    (::gdcm::Tag(0x0040, 0x0555)) //Acquisition Context Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x0022)) //Acquisition Date
                                                                                    (::gdcm::Tag(0x0008, 0x002A)) //Acquisition DateTime
                                                                                    (::gdcm::Tag(0x0018, 0x1400)) //Acquisition Device Processing Description
                                                                                    (::gdcm::Tag(0x0018, 0x9424)) //Acquisition Protocol Description
                                                                                    (::gdcm::Tag(0x0008, 0x0032)) //Acquisition Time
                                                                                    (::gdcm::Tag(0x0040, 0x4035)) //Actual Human Performers Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x21B0)) //Additional Patient’s History
                                                                                    (::gdcm::Tag(0x0038, 0x0010)) //Admission ID
                                                                                    (::gdcm::Tag(0x0038, 0x0020)) //Admitting Date
                                                                                    (::gdcm::Tag(0x0008, 0x1084)) //Admitting Diagnoses Code Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1080)) //Admitting Diagnoses Description
                                                                                    (::gdcm::Tag(0x0038, 0x0021)) //Admitting Time
                                                                                    (::gdcm::Tag(0x0000, 0x1000)) //Affected SOP Instance UID
                                                                                    (::gdcm::Tag(0x0010, 0x2110)) //Allergies
                                                                                    (::gdcm::Tag(0x4000, 0x0010)) //Arbitrary
                                                                                    (::gdcm::Tag(0x0040, 0xA078)) //Author Observer Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x1081)) //Branch of Service
                                                                                    (::gdcm::Tag(0x0018, 0x1007)) //Cassette ID
                                                                                    (::gdcm::Tag(0x0040, 0x0280)) //Comments on Performed Procedure Step
                                                                                    (::gdcm::Tag(0x0040, 0x3001)) //Confidentiality Constraint on Patient Data Description
                                                                                    (::gdcm::Tag(0x0070, 0x0086)) //Content Creator’s Identification Code Sequence
                                                                                    (::gdcm::Tag(0x0040, 0xA730)) //Content Sequence
                                                                                    (::gdcm::Tag(0x0018, 0xA003)) //Contribution Description
                                                                                    (::gdcm::Tag(0x0010, 0x2150)) //Country of Residence
                                                                                    (::gdcm::Tag(0x0038, 0x0300)) //Current Patient Location
                                                                                    (::gdcm::Tag(0x0008, 0x0025)) //Curve Date
                                                                                    (::gdcm::Tag(0x0008, 0x0035)) //Curve Time
                                                                                    (::gdcm::Tag(0x0040, 0xA07C)) //Custodial Organization Sequence
                                                                                    (::gdcm::Tag(0xFFFC, 0xFFFC)) //Data Set Trailing Padding
                                                                                    (::gdcm::Tag(0x0008, 0x2111)) //Derivation Description
                                                                                    (::gdcm::Tag(0x0018, 0x700A)) //Detector ID
                                                                                    (::gdcm::Tag(0x0018, 0x1000)) //Device Serial Number
                                                                                    (::gdcm::Tag(0x0400, 0x0100)) //Digital Signature UID
                                                                                    (::gdcm::Tag(0xFFFA, 0xFFFA)) //Digital Signatures Sequence
                                                                                    (::gdcm::Tag(0x0038, 0x0040)) //Discharge Diagnosis Description
                                                                                    (::gdcm::Tag(0x4008, 0x011A)) //Distribution Address
                                                                                    (::gdcm::Tag(0x4008, 0x0119)) //Distribution Name
                                                                                    (::gdcm::Tag(0x0010, 0x2160)) //Ethnic Group
                                                                                    (::gdcm::Tag(0x0020, 0x9158)) //Frame Comments
                                                                                    (::gdcm::Tag(0x0018, 0x1008)) //Gantry ID
                                                                                    (::gdcm::Tag(0x0018, 0x1005)) //Generator ID
                                                                                    (::gdcm::Tag(0x0040, 0x4037)) //Human Performers Name
                                                                                    (::gdcm::Tag(0x0040, 0x4036)) //Human Performers Organization
                                                                                    (::gdcm::Tag(0x0088, 0x0200)) //Icon Image Sequence(see Note 12
                                                                                    (::gdcm::Tag(0x0008, 0x4000)) //Identifying Comments
                                                                                    (::gdcm::Tag(0x0020, 0x4000)) //Image Comments
                                                                                    (::gdcm::Tag(0x0028, 0x4000)) //Image Presentation Comments
                                                                                    (::gdcm::Tag(0x0040, 0x2400)) //Imaging Service Request Comments
                                                                                    (::gdcm::Tag(0x4008, 0x0300)) //Impressions
                                                                                    (::gdcm::Tag(0x0008, 0x0081)) //Institution Address
                                                                                    (::gdcm::Tag(0x0008, 0x0082)) //Institution Code Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x0080)) //Institution Name
                                                                                    (::gdcm::Tag(0x0008, 0x1040)) //Institutional Department Name
                                                                                    (::gdcm::Tag(0x0010, 0x1050)) //Insurance Plan Identification
                                                                                    (::gdcm::Tag(0x0040, 0x1011)) //Intended Recipients of Results Identification Sequence
                                                                                    (::gdcm::Tag(0x4008, 0x0111)) //Interpretation Approver Sequence
                                                                                    (::gdcm::Tag(0x4008, 0x010C)) //Interpretation Author
                                                                                    (::gdcm::Tag(0x4008, 0x0115)) //Interpretation Diagnosis Description
                                                                                    (::gdcm::Tag(0x4008, 0x0202)) //Interpretation ID Issuer
                                                                                    (::gdcm::Tag(0x4008, 0x0102)) //Interpretation Recorder
                                                                                    (::gdcm::Tag(0x4008, 0x010B)) //Interpretation Text
                                                                                    (::gdcm::Tag(0x4008, 0x010A)) //Interpretation Transcriber
                                                                                    (::gdcm::Tag(0x0038, 0x0011)) //Issuer of Admission ID
                                                                                    (::gdcm::Tag(0x0010, 0x0021)) //Issuer of Patient ID
                                                                                    (::gdcm::Tag(0x0038, 0x0061)) //Issuer of Service Episode ID
                                                                                    (::gdcm::Tag(0x0010, 0x21D0)) //Last Menstrual Date
                                                                                    (::gdcm::Tag(0x0400, 0x0404)) //MAC
                                                                                    (::gdcm::Tag(0x0010, 0x2000)) //Medical Alerts
                                                                                    (::gdcm::Tag(0x0010, 0x1090)) //Medical Record Locator
                                                                                    (::gdcm::Tag(0x0010, 0x1080)) //Military Rank
                                                                                    (::gdcm::Tag(0x0400, 0x0550)) //Modified Attributes Sequence
                                                                                    (::gdcm::Tag(0x0020, 0x3406)) //Modified Image Description
                                                                                    (::gdcm::Tag(0x0020, 0x3401)) //Modifying Device ID
                                                                                    (::gdcm::Tag(0x0020, 0x3404)) //Modifying Device Manufacturer
                                                                                    (::gdcm::Tag(0x0008, 0x1060)) //Name of Physician(s Reading Study
                                                                                    (::gdcm::Tag(0x0040, 0x1010)) //Names of Intended Recipient of Results
                                                                                    (::gdcm::Tag(0x0010, 0x2180)) //Occupation
                                                                                    (::gdcm::Tag(0x0008, 0x1072)) //Operators’ Identification Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1070)) //Operators’ Name
                                                                                    (::gdcm::Tag(0x0400, 0x0561)) //Original Attributes Sequence
                                                                                    (::gdcm::Tag(0x0040, 0x2010)) //Order Callback Phone Number
                                                                                    (::gdcm::Tag(0x0040, 0x2008)) //Order Entered By
                                                                                    (::gdcm::Tag(0x0040, 0x2009)) //Order Enterer Location
                                                                                    (::gdcm::Tag(0x0010, 0x1000)) //Other Patient IDs
                                                                                    (::gdcm::Tag(0x0010, 0x1002)) //Other Patient IDs Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x1001)) //Other Patient Names
                                                                                    (::gdcm::Tag(0x0008, 0x0024)) //Overlay Date
                                                                                    (::gdcm::Tag(0x0008, 0x0034)) //Overlay Time
                                                                                    (::gdcm::Tag(0x0040, 0xA07A)) //Participant Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x1040)) //Patient Address
                                                                                    (::gdcm::Tag(0x0010, 0x4000)) //Patient Comments
                                                                                    (::gdcm::Tag(0x0010, 0x2203)) //Patient Sex Neutered (X/Z)
                                                                                    (::gdcm::Tag(0x0038, 0x0500)) //Patient State
                                                                                    (::gdcm::Tag(0x0040, 0x1004)) //Patient Transport Arrangements
                                                                                    (::gdcm::Tag(0x0010, 0x1010)) //Patient’s Age
                                                                                    (::gdcm::Tag(0x0010, 0x1005)) //Patient’s Birth Name
                                                                                    (::gdcm::Tag(0x0010, 0x0032)) //Patient’s Birth Time
                                                                                    (::gdcm::Tag(0x0038, 0x0400)) //Patient’s Institution Residence
                                                                                    (::gdcm::Tag(0x0010, 0x0050)) //Patient’s Insurance Plan Code Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x1060)) //Patient’s Mother’s Birth Name
                                                                                    (::gdcm::Tag(0x0010, 0x0101)) //Patient’s Primary Language Code Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x0102)) //Patient’s Primary Language Modifier Code Sequence
                                                                                    (::gdcm::Tag(0x0010, 0x21F0)) //Patient’s Religious Preference
                                                                                    (::gdcm::Tag(0x0010, 0x1020)) //Patient’s Size
                                                                                    (::gdcm::Tag(0x0010, 0x2154)) //Patient’s Telephone Number
                                                                                    (::gdcm::Tag(0x0010, 0x1030)) //Patient’s Weight
                                                                                    (::gdcm::Tag(0x0040, 0x0243)) //Performed Location
                                                                                    (::gdcm::Tag(0x0040, 0x0254)) //Performed Procedure Step Description
                                                                                    (::gdcm::Tag(0x0040, 0x0253)) //Performed Procedure Step ID
                                                                                    (::gdcm::Tag(0x0040, 0x0244)) //Performed Procedure Step Start Date
                                                                                    (::gdcm::Tag(0x0040, 0x0245)) //Performed Procedure Step Start Time
                                                                                    (::gdcm::Tag(0x0040, 0x0241)) //Performed Station AE Title
                                                                                    (::gdcm::Tag(0x0040, 0x4030)) //Performed Station Geographic Location Code Sequence
                                                                                    (::gdcm::Tag(0x0040, 0x0242)) //Performed Station Name
                                                                                    (::gdcm::Tag(0x0040, 0x0248)) //Performed Station Name Code Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1052)) //Performing Physicians’ Identification Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1050)) //Performing Physicians’ Name
                                                                                    (::gdcm::Tag(0x0040, 0x1102)) //Person Address
                                                                                    (::gdcm::Tag(0x0040, 0x1103)) //Person Telephone Numbers
                                                                                    (::gdcm::Tag(0x4008, 0x0114)) //Physician Approving Interpretation
                                                                                    (::gdcm::Tag(0x0008, 0x1062)) //Physician Reading Study Identification Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1048)) //Physician(s Record
                                                                                    (::gdcm::Tag(0x0008, 0x1049)) //Physician(s of Record Identification Sequence
                                                                                    (::gdcm::Tag(0x0018, 0x1004)) //Plate ID
                                                                                    (::gdcm::Tag(0x0040, 0x0012)) //Pre-Medication
                                                                                    (::gdcm::Tag(0x0010, 0x21C0)) //Pregnancy Status
                                                                                    (::gdcm::Tag(0x0018, 0x1030)) //Protocol Name (X/D)
                                                                                    (::gdcm::Tag(0x0040, 0x2001)) //Reason for Imaging Service Request
                                                                                    (::gdcm::Tag(0x0032, 0x1030)) //Reason for Study
                                                                                    (::gdcm::Tag(0x0400, 0x0402)) //Referenced Digital Signature Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1140)) //Referenced Image Sequence (X/Z/U*)
                                                                                    (::gdcm::Tag(0x0038, 0x1234)) //Referenced Patient Alias Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1120)) //Referenced Patient Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1111)) //Referenced Performed Procedure Step Sequence (X/Z/D)
                                                                                    (::gdcm::Tag(0x0400, 0x0403)) //Referenced SOP Instance MAC Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x1110)) //Referenced Study Sequence (X/Z)
                                                                                    (::gdcm::Tag(0x0008, 0x0092)) //Referring Physician’s Address
                                                                                    (::gdcm::Tag(0x0008, 0x0096)) //Referring Physician’s Identification Sequence
                                                                                    (::gdcm::Tag(0x0008, 0x0094)) //Referring Physician’s Telephone Numbers
                                                                                    (::gdcm::Tag(0x0010, 0x2152)) //Region of Residence
                                                                                    (::gdcm::Tag(0x0040, 0x0275)) //Request Attributes Sequence
                                                                                    (::gdcm::Tag(0x0032, 0x1070)) //Requested Contrast Agent
                                                                                    (::gdcm::Tag(0x0040, 0x1400)) //Requested Procedure Comments
                                                                                    (::gdcm::Tag(0x0032, 0x1060)) //Requested Procedure Description (X/Z)
                                                                                    (::gdcm::Tag(0x0040, 0x1001)) //Requested Procedure ID
                                                                                    (::gdcm::Tag(0x0040, 0x1005)) //Requested Procedure Location
                                                                                    (::gdcm::Tag(0x0032, 0x1032)) //Requesting Physician
                                                                                    (::gdcm::Tag(0x0032, 0x1033)) //Requesting Service
                                                                                    (::gdcm::Tag(0x0010, 0x2299)) //Responsible Organization
                                                                                    (::gdcm::Tag(0x0010, 0x2297)) //Responsible Person
                                                                                    (::gdcm::Tag(0x4008, 0x4000)) //Results Comments
                                                                                    (::gdcm::Tag(0x4008, 0x0118)) //Results Distribution List Sequence
                                                                                    (::gdcm::Tag(0x4008, 0x0042)) //Results ID Issuer
                                                                                    (::gdcm::Tag(0x300E, 0x0008)) //Reviewer Name (X/Z)
                                                                                    (::gdcm::Tag(0x0040, 0x4034)) //Scheduled Human Performers Sequence
                                                                                    (::gdcm::Tag(0x0038, 0x001E)) //Scheduled Patient Institution Residence
                                                                                    (::gdcm::Tag(0x0040, 0x000B)) //Scheduled Performing Physician Identification Sequence
                                                                                    (::gdcm::Tag(0x0040, 0x0006)) //Scheduled Performing Physician Name
                                                                                    (::gdcm::Tag(0x0040, 0x0004)) //Scheduled Procedure Step End Date
                                                                                    (::gdcm::Tag(0x0040, 0x0005)) //Scheduled Procedure Step End Time
                                                                                    (::gdcm::Tag(0x0040, 0x0007)) //Scheduled Procedure Step Description
                                                                                    (::gdcm::Tag(0x0040, 0x0011)) //Scheduled Procedure Step Location
                                                                                    (::gdcm::Tag(0x0040, 0x0002)) //Scheduled Procedure Step Start Date
                                                                                    (::gdcm::Tag(0x0040, 0x0003)) //Scheduled Procedure Step Start Time
                                                                                    (::gdcm::Tag(0x0040, 0x0001)) //Scheduled Station AE Title
                                                                                    (::gdcm::Tag(0x0040, 0x4027)) //Scheduled Station Geographic Location Code Sequence
                                                                                    (::gdcm::Tag(0x0040, 0x0010)) //Scheduled Station Name
                                                                                    (::gdcm::Tag(0x0040, 0x4025)) //Scheduled Station Name Code Sequence
                                                                                    (::gdcm::Tag(0x0032, 0x1020)) //Scheduled Study Location
                                                                                    (::gdcm::Tag(0x0032, 0x1021)) //Scheduled Study Location AE Title
                                                                                    (::gdcm::Tag(0x0008, 0x0021)) //Series Date (X/D)
                                                                                    (::gdcm::Tag(0x0008, 0x0031)) //Series Time (X/D)
                                                                                    (::gdcm::Tag(0x0038, 0x0062)) //Service Episode Description
                                                                                    (::gdcm::Tag(0x0038, 0x0060)) //Service Episode ID
                                                                                    (::gdcm::Tag(0x0010, 0x21A0)) //Smoking Status
                                                                                    (::gdcm::Tag(0x0008, 0x2112)) //Source Image Sequence (X/Z/U*)
                                                                                    (::gdcm::Tag(0x0038, 0x0050)) //Special Needs
                                                                                    (::gdcm::Tag(0x0008, 0x1010)) //Station Name (X/Z/D)
                                                                                    (::gdcm::Tag(0x0032, 0x4000)) //Study Comments
                                                                                    (::gdcm::Tag(0x0008, 0x1030)) //Study Description
                                                                                    (::gdcm::Tag(0x0032, 0x0012)) //Study ID Issuer
                                                                                    (::gdcm::Tag(0x4000, 0x4000)) //Text Comments
                                                                                    (::gdcm::Tag(0x2030, 0x0020)) //Text String
                                                                                    (::gdcm::Tag(0x0008, 0x0201)) //Timezone Offset From UTC
                                                                                    (::gdcm::Tag(0x0088, 0x0910)) //Topic Author
                                                                                    (::gdcm::Tag(0x0088, 0x0912)) //Topic Key Words
                                                                                    (::gdcm::Tag(0x0088, 0x0906)) //Topic Subject
                                                                                    (::gdcm::Tag(0x0088, 0x0904)) //Topic Title
                                                                                    (::gdcm::Tag(0x0040, 0xA027)) //Verifying Organization
                                                                                    (::gdcm::Tag(0x0038, 0x4000)) //Visit Comments
;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_K_TAGS = ::boost::assign::list_of
                                                                                    (::gdcm::Tag(0x0008, 0x103E)) //Series Description (action code should probably be X for anonymity reasons)
;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_C_TAGS;

const DicomAnonymizer::TagContainerType DicomAnonymizer::s_ACTION_CODE_U_TAGS = ::boost::assign::list_of
                                                                                    (::gdcm::Tag(0x0020, 0x9161)) //Concatenation UID
                                                                                    (::gdcm::Tag(0x0008, 0x010D)) //Context Group Extension Creator UID
                                                                                    (::gdcm::Tag(0x0008, 0x9123)) //Creator Version UID
                                                                                    (::gdcm::Tag(0x0018, 0x1002)) //Device UID
                                                                                    (::gdcm::Tag(0x0020, 0x9164)) //Dimension Organization UID
                                                                                    (::gdcm::Tag(0x300A, 0x0013)) //Dose Reference UID
                                                                                    (::gdcm::Tag(0x0008, 0x0058)) //Failed SOP Instance UID List
                                                                                    (::gdcm::Tag(0x0070, 0x031A)) //Fiducial UID
                                                                                    (::gdcm::Tag(0x0020, 0x0052)) //Frame of Reference UID
                                                                                    (::gdcm::Tag(0x0008, 0x0014)) //Instance Creator UID
                                                                                    (::gdcm::Tag(0x0008, 0x3010)) //Irradiation Event UID
                                                                                    (::gdcm::Tag(0x0028, 0x1214)) //Large Palette Color Lookup Table UID
                                                                                    (::gdcm::Tag(0x0002, 0x0003)) //Media Storage SOP Instance UID
                                                                                    (::gdcm::Tag(0x0028, 0x1199)) //Palette Color Lookup Table UID
                                                                                    (::gdcm::Tag(0x3006, 0x0024)) //Referenced Frame of Reference UID
                                                                                    (::gdcm::Tag(0x0040, 0x4023)) //Referenced General Purpose Scheduled Procedure Step Transaction UID
                                                                                    (::gdcm::Tag(0x0008, 0x1155)) //Referenced SOP Instance UID
                                                                                    (::gdcm::Tag(0x0004, 0x1511)) //Referenced SOP Instance UID in File
                                                                                    (::gdcm::Tag(0x3006, 0x00C2)) //Related Frame of Reference UID
                                                                                    (::gdcm::Tag(0x0000, 0x1001)) //Requested SOP Instance UID
                                                                                    (::gdcm::Tag(0x0020, 0x000E)) //Series Instance UID
                                                                                    (::gdcm::Tag(0x0008, 0x0018)) //SOP Instance UID
                                                                                    (::gdcm::Tag(0x0088, 0x0140)) //Storage Media File-set UID
                                                                                    (::gdcm::Tag(0x0020, 0x000D)) //Study Instance UID
                                                                                    (::gdcm::Tag(0x0020, 0x0200)) //Synchronization Frame of Reference UID
                                                                                    (::gdcm::Tag(0x0040, 0xDB0D)) //Template Extension Creator UID
                                                                                    (::gdcm::Tag(0x0040, 0xDB0C)) //Template Extension Organization UID
                                                                                    (::gdcm::Tag(0x0008, 0x1195)) //Transaction UID
                                                                                    (::gdcm::Tag(0x0040, 0xA124)) //UID
;

DicomAnonymizer::DicomAnonymizer() :
    m_publicDictionary(::gdcm::Global::GetInstance().GetDicts().GetPublicDict()),
    m_observer(::fwJobs::Observer::New("Anonymization process")),
    m_archiving(false),
    m_fileIndex(0),
    m_actionCodeDTags(s_ACTION_CODE_D_TAGS),
    m_actionCodeZTags(s_ACTION_CODE_Z_TAGS),
    m_actionCodeXTags(s_ACTION_CODE_X_TAGS),
    m_actionCodeKTags(s_ACTION_CODE_K_TAGS),
    m_actionCodeCTags(s_ACTION_CODE_C_TAGS),
    m_actionCodeUTags(s_ACTION_CODE_U_TAGS)
{
}

//------------------------------------------------------------------------------

DicomAnonymizer::~DicomAnonymizer()
{
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymize(const ::boost::filesystem::path& dirPath)
{
    m_archiving = false;
    m_observer->setTotalWorkUnits(100);
    this->anonymizationProcess(dirPath);
    m_observer->finish();
}

void moveDirectory(::boost::filesystem::path input, ::boost::filesystem::path output)
{
    ::boost::system::error_code ec;
    ::boost::filesystem::copy_directory(input, output, ec);
    FW_RAISE_IF("copy_directory " << input.string() << " " << output.string()
                                  << " error : " << ec.message(), ec.value());

    ::boost::filesystem::directory_iterator it(input);
    ::boost::filesystem::directory_iterator end;
    ::boost::filesystem::permissions(output, ::boost::filesystem::owner_all, ec);
    OSLM_ERROR_IF("set " << output.string() << " permission error : " << ec.message(), ec.value());

    for(; it != end; ++it)
    {
        ::boost::filesystem::path dest = output / it->path().filename();
        if(::boost::filesystem::is_directory(*it))
        {
            moveDirectory(*it, dest);
        }
        else
        {
            ::boost::filesystem::rename(*it, dest, ec);
            FW_RAISE_IF(
                "rename " << it->path().string() << " " << dest.string() << " error : " << ec.message(),
                ec.value());
        }

        ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, ec);
        OSLM_ERROR_IF("set " << dest.string() << " permission error : " << ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::removeAnonymizeTag(const ::gdcm::Tag& tag)
{
    m_actionCodeDTags.erase(tag);
    m_actionCodeZTags.erase(tag);
    m_actionCodeXTags.erase(tag);
    m_actionCodeKTags.erase(tag);
    m_actionCodeCTags.erase(tag);
    m_actionCodeUTags.erase(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymizationProcess(const ::boost::filesystem::path& dirPath)
{
    // Create temporary directory
    ::boost::filesystem::path tmpPath = ::fwTools::System::getTemporaryFolder("DicomAnonymizer");
    tmpPath                          /= "tmp";

    // Doesn't use ::boost::filesystem::rename because of potential issues when moving folders across volumes
    moveDirectory(dirPath, tmpPath);

    ::boost::system::error_code ec;

    ::boost::filesystem::directory_iterator it(dirPath);
    ::boost::filesystem::directory_iterator end;

    for(; it != end; ++it)
    {
        if(::boost::filesystem::is_directory(*it))
        {
            ::boost::filesystem::remove_all((*it), ec);
            FW_RAISE_IF("remove_all " + dirPath.string() + " error : " + ec.message(), ec.value());
        }
    }

    std::vector<std::string> dicomFiles;
    ::fwGdcmIO::helper::DicomSearch::searchRecursively(tmpPath, dicomFiles, false);

    unsigned int fileIndex = 0;
    for(std::string file: dicomFiles)
    {
        if(m_observer->cancelRequested())
        {
            break;
        }

        std::ifstream inStream(file, std::ios::binary);

        std::stringstream ss;
        ss << "im" << std::setfill('0') << std::setw(5) << fileIndex++;

        std::ofstream outStream((dirPath / ss.str()).string(), std::ios::binary | std::ios::trunc);

        this->anonymize(inStream, outStream);

        std::uint64_t progress = static_cast<std::uint64_t>(
            ((m_archiving) ? 50 : 100) * static_cast<float>(fileIndex) / static_cast<float>(dicomFiles.size()));
        m_observer->doneWork(progress);
    }

}

//------------------------------------------------------------------------------

void DicomAnonymizer::resetIndex()
{
    m_fileIndex = 0;
}

//------------------------------------------------------------------------------

unsigned int DicomAnonymizer::getNextIndex()
{
    return m_fileIndex++;
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymize(std::istream& inputStream, std::ostream& outputStream)
{
    // File Reader
    ::gdcm::Reader reader;
    reader.SetStream(inputStream);
    FW_RAISE_IF("Unable to anonymize (file read failed)", !reader.Read());

    // String filter
    m_stringFilter.SetFile(reader.GetFile());

    // Objects used to scan groups of elements
    ::gdcm::Tag tag;
    ::gdcm::DataElement dataElement;
    const ::gdcm::File& datasetFile = reader.GetFile();
    ::gdcm::DataSet dataset = datasetFile.GetDataSet();

    m_anonymizer.SetFile( reader.GetFile() );

    m_anonymizer.RemoveGroupLength();
    m_anonymizer.RemoveRetired();

    for(ExceptionTagMapType::value_type exception: m_exceptionTagMap)
    {
        m_anonymizer.Replace(exception.first, exception.second.c_str());
    }

    ::boost::range::for_each(m_actionCodeDTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeD(tag);
            });
    ::boost::range::for_each(m_actionCodeZTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeZ(tag);
            });
    ::boost::range::for_each(m_actionCodeXTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeX(tag);
            });
    ::boost::range::for_each(m_actionCodeKTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeK(tag);
            });
    ::boost::range::for_each(m_actionCodeCTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeC(tag);
            });
    ::boost::range::for_each(m_actionCodeUTags, [this](const ::gdcm::Tag& tag)
            {
                this->applyActionCodeU(tag);
            });

    auto applyActionCodeXWithException = [this](const ::gdcm::Tag& tag)
                                         {
                                             if(m_exceptionTagMap.find(tag) == m_exceptionTagMap.end())
                                             {
                                                 this->applyActionCodeX(tag);
                                             }
                                         };

    // Curve Data (0x50xx,0xxxxx)
    dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x5000, 0x0));
    tag         = dataElement.GetTag();
    while (((tag.GetGroup() >> 8) & 0xff)  == 0x50 )
    {
        applyActionCodeXWithException(tag);    //Curve Data
        dataElement = dataset.FindNextDataElement(
            ::gdcm::Tag(tag.GetGroup(), static_cast< uint16_t >(tag.GetElement() + 1)));
        tag = dataElement.GetTag();
    }

    // Overlay Comments (0x60xx,0x4000)
    dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x6000, 0x4000));
    tag         = dataElement.GetTag();
    while (((tag.GetGroup() >> 8) & 0xff)  == 0x60 )
    {
        if(tag.GetElement() == 0x4000)
        {
            applyActionCodeXWithException(tag);
        }
        dataElement = dataset.FindNextDataElement(
            ::gdcm::Tag(static_cast< uint16_t >(tag.GetGroup() + 1), 0x4000));
        tag = dataElement.GetTag();
    }

    // Overlay Data (0x60xx,0x3000)
    dataElement = dataset.FindNextDataElement(::gdcm::Tag(0x6000, 0x3000));
    tag         = dataElement.GetTag();
    while (((tag.GetGroup() >> 8) & 0xff)  == 0x60 )
    {
        if(tag.GetElement() == 0x3000)
        {
            applyActionCodeXWithException(tag);
        }
        dataElement = dataset.FindNextDataElement(::gdcm::Tag(static_cast< uint16_t >(tag.GetGroup() + 1), 0x3000));
        tag         = dataElement.GetTag();
    }

    m_anonymizer.RemovePrivateTags();   //Private attributes (X)

    // Write file
    ::gdcm::Writer writer;
    writer.SetStream(outputStream);
    writer.SetFile(reader.GetFile());

    FW_RAISE_IF("Unable to anonymize (file write failed)", !writer.Write());
}

//------------------------------------------------------------------------------

void DicomAnonymizer::addExceptionTag(uint16_t group, uint16_t element, const std::string& value)
{
    ::gdcm::Tag tag(group,element);

    m_exceptionTagMap[tag] = value;

    m_actionCodeDTags.erase(tag);
    m_actionCodeZTags.erase(tag);
    m_actionCodeXTags.erase(tag);
    m_actionCodeKTags.erase(tag);
    m_actionCodeCTags.erase(tag);
    m_actionCodeUTags.erase(tag);
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
    const std::string oldUID = m_stringFilter.ToString(tag);
    if(oldUID.empty())
    {
        m_anonymizer.Replace(tag, GENERATOR.Generate());
    }
    else
    {
        std::string uid = m_uidMap[oldUID];
        if(uid.empty())
        {
            uid              = GENERATOR.Generate();
            m_uidMap[oldUID] = uid;
        }
        m_anonymizer.Replace(tag, uid.c_str());
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
            if(tag == ::gdcm::Tag(0x0010, 0x0040))
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

    ec.clear();
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
            // Use stream instead of boost::copy_file (Unix c++11 issue)
            ::boost::filesystem::ifstream inStream(it->path(), std::ios::binary);
            FW_RAISE_IF("Unable to read file :" << it->path().string(), !inStream.good());
            ::boost::filesystem::ofstream outStream(dest, std::ios::binary);
            FW_RAISE_IF("Unable to write file :" << dest.string(), !outStream.good());

            outStream << inStream.rdbuf();

            inStream.close();
            outStream.close();
        }

        ec.clear();
        ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, ec);
        SLM_ERROR_IF("set " + dest.string() + " permission error : " + ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

SPTR(::fwJobs::IJob) DicomAnonymizer::getJob() const
{
    return m_observer;
}


} // namespace helper
} // namespace fwGdcmIO


