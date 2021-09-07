/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "DicomReaderTest.hpp"

#include <core/tools/dateAndTime.hpp>
#include <core/tools/Type.hpp>

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/Study.hpp>

namespace sight::utestData
{

//------------------------------------------------------------------------------
#define CHECK_VALUE(check, message, val1, val2) \
    { \
        check &= (val1 == val2); \
        SIGHT_ERROR_IF(message << " <" << val1 << "> != <" << val2 << ">", val1 != val2); \
    } \

//------------------------------------------------------------------------------

#define CHECK_VALUE_WITH_TOLERANCE(check, message, val1, val2, tol) \
    { \
        check &= (val1 - tol <= val2 && val2 <= val1 + tol); \
        SIGHT_ERROR_IF(message << val1 << " != " << val2, val1 - tol > val2 || val2 > val1 + tol); \
    }

//------------------------------------------------------------------------------

bool DicomReaderTest::checkSeriesJMSGenou(const data::ImageSeries::sptr& series)
{
    bool ok               = true;
    bool notReallyChecked = true;

    data::Patient::sptr patient     = series->getPatient();
    data::Study::sptr study         = series->getStudy();
    data::Equipment::sptr equipment = series->getEquipment();

    // Study, Acquisition
    data::Image::sptr img = series->getImage();

    //# Dicom-File-Format
    //
    //# Dicom-Meta-Information-Header
    //# Used TransferSyntax:
    //(0002,0000) UL 224                                                # 4,1 File Meta Information Group Length
    //(0002,0001) OB 00\01                                              # 2,1 File Meta Information Version
    //(0002,0002) UI [1.2.840.10008.5.1.4.1.1.2]                        # 26,1 Media Storage SOP Class UID
    //(0002,0003) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184712.380696]         # 58,1 Media Storage SOP
    // Instance UID
    //(0002,0010) UI [1.2.840.10008.1.2.1]                              # 20,1 Transfer Syntax UID
    //(0002,0012) UI [1.2.250.1.119.1.1.1.1.1.1.33.3.8.13.7]            # 38,1 Implementation Class UID
    //(0002,0013) SH [scanplus_33 ]                                     # 12,1 Implementation Version Name
    //(0002,0016) AE [scanplus]                                         # 8,1 Source Application Entity Title
    //
    //# Dicom-Data-Set
    //# Used TransferSyntax: 1.2.840.10008.1.2.1
    //(0008,0000) UL 426                                                # 4,1 Generic Group Length
    //(0008,0008) CS [ORIGINAL\PRIMARY\AXIAL]                           # 22,2-n Image Type
    //(0008,0016) UI [1.2.840.10008.5.1.4.1.1.2]                        # 26,1 SOP Class UID
    //(0008,0018) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184712.380696]         # 58,1 SOP Instance UID
    //(0008,0020) DA [20171028]                                         # 8,1 Study Date
    CHECK_VALUE(ok, "Study Date doesn't match : ", "20171028", study->getDate());
    //(0008,0021) DA [20171028]                                         # 8,1 Series Date
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "20171028", series->getDate());
    //(0008,0022) DA [20171028]                                         # 8,1 Acquisition Date
    //(0008,0023) DA [20171028]                                         # 8,1 Content Date
    //(0008,0030) TM [174327.000]                                       # 10,1 Study Time
    CHECK_VALUE(ok, "Study Time doesn't match : ", "174327.000", study->getTime());
    //(0008,0031) TM [180156.734]                                       # 10,1 Series Time
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "180156.734", series->getTime());
    //(0008,0032) TM [174446.850]                                       # 10,1 Acquisition Time
    //(0008,0033) TM [174502.095]                                       # 10,1 Content Time
    //(0008,0050) SH [12514 ]                                           # 6,1 Accession Number
    //(0008,0060) CS [CT]                                               # 2,1 Modality
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "CT", series->getModality());
    //(0008,0070) LO [TOSHIBA ]                                         # 8,1 Manufacturer
    //(0008,0080) LO [SCANNER DU MORDOR ]                               # 20,1 Institution Name
    CHECK_VALUE(
        ok,
        "Equipment's Institution Name doesn't match : ",
        "SCANNER DU MORDOR ",
        equipment->getInstitutionName()
    );
    //(0008,0090) PN [DR MOREL]                                         # 8,1 Referring Physician's Name
    CHECK_VALUE(
        ok,
        "Study Referring Physician's Name doesn't match : ",
        "DR MOREL",
        study->getReferringPhysicianName()
    );
    //(0008,1010) SH [00000000001 ]                                     # 12,1 Station Name
    //(0008,103e) LO [ OS 0.5   ]                                       # 10,1 Series Description
    CHECK_VALUE(ok, "Study Description doesn't match : ", " OS 0.5 ", series->getDescription());
    CHECK_VALUE(ok, "Study Description doesn't match : ", "", study->getDescription()); // 0008,1030
    //(0008,1040) LO [ID_DEPARTMENT ]                                   # 14,1 Institutional Department Name
    {
        data::DicomValuesType physiciansName;
        ok &= (physiciansName == series->getPerformingPhysiciansName());
        SIGHT_ERROR_IF(
            "Name of the physician(s) administering the Series doesn't match : ",
            (physiciansName == series->getPerformingPhysiciansName())
        );
    }
    //(0008,1090) LO [Aquilion]                                         # 8,1 Manufacturer's Model Name
    //(0010,0000) UL 104                                                # 4,1 Generic Group Length
    //(0010,0010) PN [SAVA JEAN-MICHEL]                                   # 14,1 Patient's Name
    CHECK_VALUE(ok, "Patient's Name doesn't match : ", "SAVA JEAN-MICHEL", patient->getName());
    //(0010,0020) LO [12592 ARTHRO GENOU  G ]                           # 22,1 Patient ID
    CHECK_VALUE(ok, "Patient ID doesn't match : ", "12592 ARTHRO GENOU  G ", patient->getPatientId());
    //(0010,0030) DA [19970926]                                         # 8,1 Patient's Birth Date
    CHECK_VALUE(ok, "Patient's Birth Date doesn't match : ", "19970926", patient->getBirthdate());
    //(0010,0040) CS [M ]                                               # 2,1 Patient's Sex
    CHECK_VALUE(ok, "Patient's Sex doesn't match :", "M ", patient->getSex());
    //(0010,1010) AS [029Y]                                             # 4,1 Patient's Age
    CHECK_VALUE(ok, "Study Patient's Age doesn't match :", "029Y", study->getPatientAge());
    //(0010,4000) LT [ARTHRO]                                           # 6,1 Patient Comments
    //(0018,0000) UL 284                                                # 4,1 Generic Group Length
    //(0018,0015) CS [DR MOREL]                                         # 8,1 Body Part Examined
    //(0018,0022) CS [HELICAL_CT]                                       # 10,1-n Scan Options
    //(0018,0050) DS [0.5 ]                                             # 4,1 Slice Thickness
    //(0018,0060) DS [120 ]                                             # 4,1 KVP
    //(0018,0090) DS [400.00]                                           # 6,1 Data Collection Diameter
    //(0018,1000) LO [C4522344]                                         # 8,1 Device Serial Number
    //(0018,1020) LO [V3.20ER005]                                       # 10,1-n Software Version(s)
    //(0018,1030) LO [ARTHRO GENOU]                                     # 12,1 Protocol Name
    //(0018,1100) DS [196.875 ]                                         # 8,1 Reconstruction Diameter
    //(0018,1120) DS [+0.0]                                             # 4,1 Gantry/Detector Tilt
    //(0018,1130) DS [+90.00]                                           # 6,1 Table Height
    //(0018,1140) CS [CW]                                               # 2,1 Rotation Direction
    //(0018,1150) IS [500 ]                                             # 4,1 Exposure Time
    //(0018,1151) IS [200 ]                                             # 4,1 X-Ray Tube Current
    //(0018,1152) IS [100 ]                                             # 4,1 Exposure
    //(0018,1170) IS [24]                                               # 2,1 Generator Power
    //(0018,1190) DS [0.9\0.8 ]                                         # 8,1-n Focal Spot(s)
    //(0018,1210) SH [FC30]                                             # 4,1-n Convolution Kernel
    //(0018,5100) CS [FFS ]                                             # 4,1 Patient Position
    //(0018,9345) UN (FD) 23.1                                          # 8,1 CTDIvol
    //(0020,0000) UL 370                                                # 4,1 Generic Group Length
    //(0020,000d) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225183167.375775]         # 58,1 Study Instance UID
    CHECK_VALUE(
        ok,
        "Study Instance UID doesn't match :",
        "1.2.392.200036.9116.2.6.1.48.1211418863.1225183167.375775",
        study->getInstanceUID()
    );
    //(0020,000e) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855]         # 58,1 Series Instance UID
    CHECK_VALUE(
        ok,
        "Series Instance UID doesn't match :",
        "1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855",
        series->getInstanceUID()
    );
    //(0020,0010) SH [12514 ]                                           # 6,1 Study ID
    //(0020,0011) IS [3 ]                                               # 2,1 Series Number
    //(0020,0012) IS [3 ]                                               # 2,1 Acquisition Number
    //(0020,0013) IS [404 ]                                             # 4,1 Instance Number
    //(0020,0020) CS [L\P ]                                             # 4,2 Patient Orientation
    //(0020,0032) DS [-36.71875\-88.28125\1350.300]                     # 28,3 Image Position (Patient)

    if(!img)
    {
        SIGHT_ERROR("Missing image.");
        return false;
    }

    CHECK_VALUE_WITH_TOLERANCE(ok, "Image x origin doesn't match  :", -36.71875, img->getOrigin2()[0], 0.01);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image y origin doesn't match  :", -88.28125, img->getOrigin2()[1], 0.01);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image z origin doesn't match  :", 1350.300, img->getOrigin2()[2], 0.01);
    //(0020,0037) DS [1.00000\0.00000\0.00000\0.00000\1.00000\0.00000 ]         # 48,6 Image Orientation (Patient)
    //(0020,0052) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225183409.15274]         # 56,1 Frame of Reference UID
    //(0020,1040) LO (no value)                                         # 0,1 Position Reference Indicator
    //(0020,1041) DS [+161.20 ]                                         # 8,1 Slice Location
    //(0028,0000) UL 158                                                # 4,1 Generic Group Length
    //(0028,0002) US 1                                                  # 2,1 Samples per Pixel
    //(0028,0004) CS [MONOCHROME2 ]                                     # 12,1 Photometric Interpretation
    //(0028,0010) US 512                                                # 2,1 Rows
    //(0028,0011) US 512                                                # 2,1 Columns
    CHECK_VALUE(ok, "Image x size doesn't match  :", 512, img->getSize2()[0]);
    CHECK_VALUE(ok, "Image y size doesn't match  :", 512, img->getSize2()[1]);
    CHECK_VALUE(ok, "Image z size doesn't match  :", 404, img->getSize2()[2]);
    //(0028,0030) DS [0.384\0.384 ]                                     # 12,2 Pixel Spacing
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image x spacing doesn't match  :", 0.384, img->getSpacing2()[0], 0.001);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image y spacing doesn't match  :", 0.384, img->getSpacing2()[1], 0.001);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image z spacing doesn't match  :", 0.399, img->getSpacing2()[2], 0.001);
    //(0028,0100) US 16                                                 # 2,1 Bits Allocated
    //(0028,0101) US 16                                                 # 2,1 Bits Stored
    CHECK_VALUE(notReallyChecked, "Image Bits Allocated correspond  :", 16, img->getType().sizeOf() * 8);
    //(0028,0102) US 15                                                 # 2,1 High Bit
    //(0028,0103) US 1                                                  # 2,1 Pixel Representation
    CHECK_VALUE(notReallyChecked, "Image Bits Allocated correspond  :", false, img->getType().isSigned());
    //(0028,1050) DS [500 ]                                             # 4,1-n Window Center
    CHECK_VALUE(ok, "Image Window Center correspond  :", 500, img->getWindowCenter());
    //(0028,1051) DS [2500]                                             # 4,1-n Window Width
    CHECK_VALUE(ok, "Image Window Width correspond  :", 2500, img->getWindowWidth());
    //(0028,1052) DS [-1024 ]                                           # 6,1 Rescale Intercept
    //(0028,1053) DS [1 ]                                               # 2,1 Rescale Slope
    //(0040,0000) UL 116                                                # 4,1 Generic Group Length
    //(0040,0002) DA [20171028]                                         # 8,1 Scheduled Procedure Step Start Date
    //(0040,0003) TM [174327.000]                                       # 10,1 Scheduled Procedure Step Start Time
    //(0040,0004) DA [20171028]                                         # 8,1 Scheduled Procedure Step End Date
    //(0040,0005) TM [181327.000]                                       # 10,1 Scheduled Procedure Step End Time
    //(0040,0244) DA [20171028]                                         # 8,1 Performed Procedure Step Start Date
    //(0040,0245) TM [174327.000]                                       # 10,1 Performed Procedure Step Start Time
    //(0040,0253) SH [12325 ]                                           # 6,1 Performed Procedure Step ID
    //(7005,0000) UL 546                                                # 4,1 Generic Group Length
    //(7005,0010) LO [TOSHIBA_MEC_CT3 ]                                 # 16,1 Private Creator
    //(7005,1007) UN (DS) [335\269 ]                                    # 8,2 Reconstruction Center
    //(7005,1008) UN (DS) [0.5 ]                                        # 4,1 Detector Slice Thickness in mm
    //(7005,1009) UN (LO) [1111111111111111]                            # 16,1 Number of Detector rows to Reconstruct
    //(7005,100a) UN (DS) [+5.50 ]                                      # 6,1 Table Speed in mm/rot
    //(7005,100b) UN (SH) [ORG ]                                        # 4,1 Filter
    //(7005,100d) UN (CS) [DR MOREL]                                    # 8,1 Organ
    //(7005,100e) UN (SH) [IMG ]                                        # 4,1 File Type Remarks
    //(7005,100f) UN (SH) [FF]                                          # 2,1 Direction
    //(7005,1011) UN (LT) [Vol.]                                        # 4,1 Series Comment
    //(7005,1012) UN (SH) [SU]                                          # 2,1 Position
    //(7005,1013) UN (US) 1                                             # 2,1 Expert Plan No.
    //(7005,1016) UN (UI) [1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.747732]         # 58,1 Volume UID
    //(7005,1017) UN (US) 404                                           # 2,1 Total Frame Count in the Volume
    //(7005,1018) UN (US) 404                                           # 2,1 Frame No.
    //(7005,1019) UN (UL) 1642763                                       # 4,1 Frame Sort Key
    //(7005,101a) UN (US) 1                                             # 2,1 Frame Sort Order
    //(7005,101b) UN (SH) [FC30]                                        # 4,1 Convolution Kernel
    //(7005,101d) UN (UL) 9                                             # 4,1 Reconstruction Number
    //(7005,101e) UN (UL) 13                                            # 4,1 Raw Data Number
    //(7005,101f) UN (LO) [20171028180156768480]                        # 20,1 Volume Number
    //(7005,1020) UN (UL) 3                                             # 4,1 Local Series Number
    //(7005,1021) UN (LO) [BOOST ]                                      # 6,1 Decrease in Artifact Filter
    //(7005,1022) UN (DS) [0.40]                                        # 4,1 Reconstruction Interval
    //(7005,1023) UN (DS) [0.688 ]                                      # 6,1 Pitch Factor
    //(7005,1024) UN (DA) [20081024]                                    # 8,1 The Acquisition Date of NRA
    //(7005,1030) UN (CS) [CT]                                          # 2,1 Main Modality in Study
    //(7005,1040) UN (FD) 402.4                                         # 8,1 DLP Dose Length Product

    return ok;
}

//------------------------------------------------------------------------------

bool DicomReaderTest::checkSeriesJMSGenouTrimmed(const data::ImageSeries::sptr& series)
{
    bool ok               = true;
    bool notReallyChecked = true;

    data::Patient::sptr patient     = series->getPatient();
    data::Study::sptr study         = series->getStudy();
    data::Equipment::sptr equipment = series->getEquipment();

    // Study, Acquisition
    data::Image::sptr img = series->getImage();

    //# Dicom-File-Format
    //
    //# Dicom-Meta-Information-Header
    //# Used TransferSyntax:
    //(0002,0000) UL 224                                                # 4,1 File Meta Information Group Length
    //(0002,0001) OB 00\01                                              # 2,1 File Meta Information Version
    //(0002,0002) UI [1.2.840.10008.5.1.4.1.1.2]                        # 26,1 Media Storage SOP Class UID
    //(0002,0003) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184712.380696]         # 58,1 Media Storage SOP
    // Instance UID
    //(0002,0010) UI [1.2.840.10008.1.2.1]                              # 20,1 Transfer Syntax UID
    //(0002,0012) UI [1.2.250.1.119.1.1.1.1.1.1.33.3.8.13.7]            # 38,1 Implementation Class UID
    //(0002,0013) SH [scanplus_33 ]                                     # 12,1 Implementation Version Name
    //(0002,0016) AE [scanplus]                                         # 8,1 Source Application Entity Title
    //
    //# Dicom-Data-Set
    //# Used TransferSyntax: 1.2.840.10008.1.2.1
    //(0008,0000) UL 426                                                # 4,1 Generic Group Length
    //(0008,0008) CS [ORIGINAL\PRIMARY\AXIAL]                           # 22,2-n Image Type
    //(0008,0016) UI [1.2.840.10008.5.1.4.1.1.2]                        # 26,1 SOP Class UID
    //(0008,0018) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184712.380696]         # 58,1 SOP Instance UID
    //(0008,0020) DA [20171028]                                         # 8,1 Study Date
    CHECK_VALUE(ok, "Study Date doesn't match : ", "20171028", study->getDate());
    //(0008,0021) DA [20171028]                                         # 8,1 Series Date
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "20171028", series->getDate());
    //(0008,0022) DA [20171028]                                         # 8,1 Acquisition Date
    //(0008,0023) DA [20171028]                                         # 8,1 Content Date
    //(0008,0030) TM [174327.000]                                       # 10,1 Study Time
    CHECK_VALUE(ok, "Study Time doesn't match : ", "174327.000", study->getTime());
    //(0008,0031) TM [180156.734]                                       # 10,1 Series Time
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "180156.734", series->getTime());
    //(0008,0032) TM [174446.850]                                       # 10,1 Acquisition Time
    //(0008,0033) TM [174502.095]                                       # 10,1 Content Time
    //(0008,0050) SH [12514 ]                                           # 6,1 Accession Number
    //(0008,0060) CS [CT]                                               # 2,1 Modality
    CHECK_VALUE(ok, "Series Modality doesn't match : ", "CT", series->getModality());
    //(0008,0070) LO [TOSHIBA ]                                         # 8,1 Manufacturer
    //(0008,0080) LO [SCANNER DU MORDOR ]                               # 20,1 Institution Name
    CHECK_VALUE(
        ok,
        "Equipment's Institution Name doesn't match : ",
        "SCANNER DU MORDOR",
        equipment->getInstitutionName()
    );
    //(0008,0090) PN [DR MOREL]                                         # 8,1 Referring Physician's Name
    CHECK_VALUE(
        ok,
        "Study Referring Physician's Name doesn't match : ",
        "DR MOREL",
        study->getReferringPhysicianName()
    );
    //(0008,1010) SH [00000000001 ]                                     # 12,1 Station Name
    //(0008,103e) LO [ OS 0.5   ]                                       # 10,1 Series Description
    CHECK_VALUE(ok, "Study Description doesn't match : ", "OS 0.5", series->getDescription());
    CHECK_VALUE(ok, "Study Description doesn't match : ", "", study->getDescription()); // 0008,1030
    //(0008,1040) LO [ID_DEPARTMENT ]                                   # 14,1 Institutional Department Name
    {
        data::DicomValuesType physiciansName;
        ok &= (physiciansName == series->getPerformingPhysiciansName());
        SIGHT_ERROR_IF(
            "Name of the physician(s) administering the Series doesn't match : ",
            (physiciansName == series->getPerformingPhysiciansName())
        );
    }
    //(0008,1090) LO [Aquilion]                                         # 8,1 Manufacturer's Model Name
    //(0010,0000) UL 104                                                # 4,1 Generic Group Length
    //(0010,0010) PN [SAVA JEAN-MICHEL]                                   # 14,1 Patient's Name
    CHECK_VALUE(ok, "Patient's Name doesn't match : ", "SAVA JEAN-MICHEL", patient->getName());
    //(0010,0020) LO [12592 ARTHRO GENOU  G ]                           # 22,1 Patient ID
    CHECK_VALUE(ok, "Patient ID doesn't match : ", "12592 ARTHRO GENOU  G", patient->getPatientId());
    //(0010,0030) DA [19970926]                                         # 8,1 Patient's Birth Date
    CHECK_VALUE(ok, "Patient's Birth Date doesn't match : ", "19970926", patient->getBirthdate());
    //(0010,0040) CS [M ]                                               # 2,1 Patient's Sex
    CHECK_VALUE(ok, "Patient's Sex doesn't match :", "M", patient->getSex());
    //(0010,1010) AS [029Y]                                             # 4,1 Patient's Age
    CHECK_VALUE(ok, "Study Patient's Age doesn't match :", "029Y", study->getPatientAge());
    //(0010,4000) LT [ARTHRO]                                           # 6,1 Patient Comments
    //(0018,0000) UL 284                                                # 4,1 Generic Group Length
    //(0018,0015) CS [DR MOREL]                                         # 8,1 Body Part Examined
    //(0018,0022) CS [HELICAL_CT]                                       # 10,1-n Scan Options
    //(0018,0050) DS [0.5 ]                                             # 4,1 Slice Thickness
    //(0018,0060) DS [120 ]                                             # 4,1 KVP
    //(0018,0090) DS [400.00]                                           # 6,1 Data Collection Diameter
    //(0018,1000) LO [C4522344]                                         # 8,1 Device Serial Number
    //(0018,1020) LO [V3.20ER005]                                       # 10,1-n Software Version(s)
    //(0018,1030) LO [ARTHRO GENOU]                                     # 12,1 Protocol Name
    //(0018,1100) DS [196.875 ]                                         # 8,1 Reconstruction Diameter
    //(0018,1120) DS [+0.0]                                             # 4,1 Gantry/Detector Tilt
    //(0018,1130) DS [+90.00]                                           # 6,1 Table Height
    //(0018,1140) CS [CW]                                               # 2,1 Rotation Direction
    //(0018,1150) IS [500 ]                                             # 4,1 Exposure Time
    //(0018,1151) IS [200 ]                                             # 4,1 X-Ray Tube Current
    //(0018,1152) IS [100 ]                                             # 4,1 Exposure
    //(0018,1170) IS [24]                                               # 2,1 Generator Power
    //(0018,1190) DS [0.9\0.8 ]                                         # 8,1-n Focal Spot(s)
    //(0018,1210) SH [FC30]                                             # 4,1-n Convolution Kernel
    //(0018,5100) CS [FFS ]                                             # 4,1 Patient Position
    //(0018,9345) UN (FD) 23.1                                          # 8,1 CTDIvol
    //(0020,0000) UL 370                                                # 4,1 Generic Group Length
    //(0020,000d) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225183167.375775]         # 58,1 Study Instance UID
    CHECK_VALUE(
        ok,
        "Study Instance UID doesn't match :",
        "1.2.392.200036.9116.2.6.1.48.1211418863.1225183167.375775",
        study->getInstanceUID()
    );
    //(0020,000e) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855]         # 58,1 Series Instance UID
    CHECK_VALUE(
        ok,
        "Series Instance UID doesn't match :",
        "1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855",
        series->getInstanceUID()
    );
    //(0020,0010) SH [12514 ]                                           # 6,1 Study ID
    //(0020,0011) IS [3 ]                                               # 2,1 Series Number
    //(0020,0012) IS [3 ]                                               # 2,1 Acquisition Number
    //(0020,0013) IS [404 ]                                             # 4,1 Instance Number
    //(0020,0020) CS [L\P ]                                             # 4,2 Patient Orientation
    //(0020,0032) DS [-36.71875\-88.28125\1350.300]                     # 28,3 Image Position (Patient)

    if(!img)
    {
        SIGHT_ERROR("Missing image.");
        return false;
    }

    CHECK_VALUE_WITH_TOLERANCE(ok, "Image x origin doesn't match  :", -36.71875, img->getOrigin2()[0], 0.01);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image y origin doesn't match  :", -88.28125, img->getOrigin2()[1], 0.01);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image z origin doesn't match  :", 1350.300, img->getOrigin2()[2], 0.01);
    //(0020,0037) DS [1.00000\0.00000\0.00000\0.00000\1.00000\0.00000 ]         # 48,6 Image Orientation (Patient)
    //(0020,0052) UI [1.2.392.200036.9116.2.6.1.48.1211418863.1225183409.15274]         # 56,1 Frame of Reference UID
    //(0020,1040) LO (no value)                                         # 0,1 Position Reference Indicator
    //(0020,1041) DS [+161.20 ]                                         # 8,1 Slice Location
    //(0028,0000) UL 158                                                # 4,1 Generic Group Length
    //(0028,0002) US 1                                                  # 2,1 Samples per Pixel
    //(0028,0004) CS [MONOCHROME2 ]                                     # 12,1 Photometric Interpretation
    //(0028,0010) US 512                                                # 2,1 Rows
    //(0028,0011) US 512                                                # 2,1 Columns
    CHECK_VALUE(ok, "Image x size doesn't match  :", 512, img->getSize2()[0]);
    CHECK_VALUE(ok, "Image y size doesn't match  :", 512, img->getSize2()[1]);
    CHECK_VALUE(ok, "Image z size doesn't match  :", 404, img->getSize2()[2]);
    //(0028,0030) DS [0.384\0.384 ]                                     # 12,2 Pixel Spacing
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image x spacing doesn't match  :", 0.384, img->getSpacing2()[0], 0.001);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image y spacing doesn't match  :", 0.384, img->getSpacing2()[1], 0.001);
    CHECK_VALUE_WITH_TOLERANCE(ok, "Image z spacing doesn't match  :", 0.399, img->getSpacing2()[2], 0.001);
    //(0028,0100) US 16                                                 # 2,1 Bits Allocated
    //(0028,0101) US 16                                                 # 2,1 Bits Stored
    CHECK_VALUE(notReallyChecked, "Image Bits Allocated correspond  :", 16, img->getType().sizeOf() * 8);
    //(0028,0102) US 15                                                 # 2,1 High Bit
    //(0028,0103) US 1                                                  # 2,1 Pixel Representation
    CHECK_VALUE(notReallyChecked, "Image Bits Allocated correspond  :", false, img->getType().isSigned());
    //(0028,1050) DS [500 ]                                             # 4,1-n Window Center
    CHECK_VALUE(ok, "Image Window Center correspond  :", 500, img->getWindowCenter());
    //(0028,1051) DS [2500]                                             # 4,1-n Window Width
    CHECK_VALUE(ok, "Image Window Width correspond  :", 2500, img->getWindowWidth());
    //(0028,1052) DS [-1024 ]                                           # 6,1 Rescale Intercept
    //(0028,1053) DS [1 ]                                               # 2,1 Rescale Slope
    //(0040,0000) UL 116                                                # 4,1 Generic Group Length
    //(0040,0002) DA [20171028]                                         # 8,1 Scheduled Procedure Step Start Date
    //(0040,0003) TM [174327.000]                                       # 10,1 Scheduled Procedure Step Start Time
    //(0040,0004) DA [20171028]                                         # 8,1 Scheduled Procedure Step End Date
    //(0040,0005) TM [181327.000]                                       # 10,1 Scheduled Procedure Step End Time
    //(0040,0244) DA [20171028]                                         # 8,1 Performed Procedure Step Start Date
    //(0040,0245) TM [174327.000]                                       # 10,1 Performed Procedure Step Start Time
    //(0040,0253) SH [12325 ]                                           # 6,1 Performed Procedure Step ID
    //(7005,0000) UL 546                                                # 4,1 Generic Group Length
    //(7005,0010) LO [TOSHIBA_MEC_CT3 ]                                 # 16,1 Private Creator
    //(7005,1007) UN (DS) [335\269 ]                                    # 8,2 Reconstruction Center
    //(7005,1008) UN (DS) [0.5 ]                                        # 4,1 Detector Slice Thickness in mm
    //(7005,1009) UN (LO) [1111111111111111]                            # 16,1 Number of Detector rows to Reconstruct
    //(7005,100a) UN (DS) [+5.50 ]                                      # 6,1 Table Speed in mm/rot
    //(7005,100b) UN (SH) [ORG ]                                        # 4,1 Filter
    //(7005,100d) UN (CS) [DR MOREL]                                    # 8,1 Organ
    //(7005,100e) UN (SH) [IMG ]                                        # 4,1 File Type Remarks
    //(7005,100f) UN (SH) [FF]                                          # 2,1 Direction
    //(7005,1011) UN (LT) [Vol.]                                        # 4,1 Series Comment
    //(7005,1012) UN (SH) [SU]                                          # 2,1 Position
    //(7005,1013) UN (US) 1                                             # 2,1 Expert Plan No.
    //(7005,1016) UN (UI) [1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.747732]         # 58,1 Volume UID
    //(7005,1017) UN (US) 404                                           # 2,1 Total Frame Count in the Volume
    //(7005,1018) UN (US) 404                                           # 2,1 Frame No.
    //(7005,1019) UN (UL) 1642763                                       # 4,1 Frame Sort Key
    //(7005,101a) UN (US) 1                                             # 2,1 Frame Sort Order
    //(7005,101b) UN (SH) [FC30]                                        # 4,1 Convolution Kernel
    //(7005,101d) UN (UL) 9                                             # 4,1 Reconstruction Number
    //(7005,101e) UN (UL) 13                                            # 4,1 Raw Data Number
    //(7005,101f) UN (LO) [20171028180156768480]                        # 20,1 Volume Number
    //(7005,1020) UN (UL) 3                                             # 4,1 Local Series Number
    //(7005,1021) UN (LO) [BOOST ]                                      # 6,1 Decrease in Artifact Filter
    //(7005,1022) UN (DS) [0.40]                                        # 4,1 Reconstruction Interval
    //(7005,1023) UN (DS) [0.688 ]                                      # 6,1 Pitch Factor
    //(7005,1024) UN (DA) [20081024]                                    # 8,1 The Acquisition Date of NRA
    //(7005,1030) UN (CS) [CT]                                          # 2,1 Main Modality in Study
    //(7005,1040) UN (FD) 402.4                                         # 8,1 DLP Dose Length Product

    return ok;
}

} // namespace sight::utestData
