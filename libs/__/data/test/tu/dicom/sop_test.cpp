/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

// This file was automatically generated by the '/libs/core/data/dicom/class_generator.py' script. DO NOT EDIT !
// Used arguments: 'Namespace(verbose=False, source_url='http://dicom.nema.org/medical/dicom/current/source/docbook',
// sop_list=None, mandatory_tags=[Tag(group='7FE0', element='0010'), Tag(group='0062', element='0002'),
// Tag(group='0066', element='0002'), Tag(group='0066', element='0011'), Tag(group='0070', element='031C')])'

#include "sop_test.hpp"

#include <data/dicom/Sop.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::dicom::ut::sop_test);

// cspell: ignore Wundefined afterloader angio autorefraction autosequence boli bscan cadsr ciexy ctdi ctdivol decf
// cspell: ignore deidentification dermoscopic diconde dicos dvhroi exif fiducials fluence fnumber frameof gpsdop hicpp
// cspell: ignore hpgl imager inms inua inus iods isop ivol ivus keratoconus keratometer keratometric kerma lensometry
// cspell: ignore lookat meterset metersets mrfov multiway nmpet nmtomo nolintbegin nolintend octb octz oecf olink
// cspell: ignore postamble powerline prcs presaturation radiofluoroscopic relaxivity reprojection rgblut rtdvh rtroi
// cspell: ignore sddn sdhn sdvn softcopy soundpath stereometric stowrs tlhc tomo tomosynthesis tomotherapeutic toric
// cspell: ignore tractography xaxrf

// NOLINTBEGIN

namespace sight::data::dicom::ut
{

//------------------------------------------------------------------------------

void sop_test::setUp()
{
}

//------------------------------------------------------------------------------

void sop_test::tearDown()
{
}

//------------------------------------------------------------------------------

void sop_test::sopTest()
{
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ComputedRadiographyImageStorage>::s_keyword,
        sop::get(sop::Keyword::ComputedRadiographyImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ComputedRadiographyImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalXRayImageStorageForPresentation>::s_keyword,
        sop::get(sop::Keyword::DigitalXRayImageStorageForPresentation).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalXRayImageStorageForPresentation>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalXRayImageStorageForProcessing>::s_keyword,
        sop::get(sop::Keyword::DigitalXRayImageStorageForProcessing).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalXRayImageStorageForProcessing>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForPresentation>::s_keyword,
        sop::get(sop::Keyword::DigitalMammographyXRayImageStorageForPresentation).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForPresentation>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForProcessing>::s_keyword,
        sop::get(sop::Keyword::DigitalMammographyXRayImageStorageForProcessing).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForProcessing>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.2.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation>::s_keyword,
        sop::get(sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing>::s_keyword,
        sop::get(sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.1.3.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedPDFStorage>::s_keyword,
        sop::get(sop::Keyword::EncapsulatedPDFStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedPDFStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.104.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedCDAStorage>::s_keyword,
        sop::get(sop::Keyword::EncapsulatedCDAStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedCDAStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.104.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedSTLStorage>::s_keyword,
        sop::get(sop::Keyword::EncapsulatedSTLStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedSTLStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.104.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedOBJStorage>::s_keyword,
        sop::get(sop::Keyword::EncapsulatedOBJStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedOBJStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.104.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedMTLStorage>::s_keyword,
        sop::get(sop::Keyword::EncapsulatedMTLStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EncapsulatedMTLStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.104.5").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayAngiographicImageStorage>::s_keyword,
        sop::get(sop::Keyword::XRayAngiographicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayAngiographicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.12.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXAImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedXAImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXAImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.12.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayRadiofluoroscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::XRayRadiofluoroscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayRadiofluoroscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.12.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXRFImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedXRFImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXRFImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.12.2.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PositronEmissionTomographyImageStorage>::s_keyword,
        sop::get(sop::Keyword::PositronEmissionTomographyImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PositronEmissionTomographyImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.128").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedPETImageStorage>::s_keyword,
        sop::get(sop::Keyword::LegacyConvertedEnhancedPETImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedPETImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.128.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRay3DAngiographicImageStorage>::s_keyword,
        sop::get(sop::Keyword::XRay3DAngiographicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRay3DAngiographicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.13.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRay3DCraniofacialImageStorage>::s_keyword,
        sop::get(sop::Keyword::XRay3DCraniofacialImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRay3DCraniofacialImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.13.1.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastTomosynthesisImageStorage>::s_keyword,
        sop::get(sop::Keyword::BreastTomosynthesisImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastTomosynthesisImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.13.1.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForPresentation>::s_keyword,
        sop::get(sop::Keyword::BreastProjectionXRayImageStorageForPresentation).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForPresentation>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.13.1.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForProcessing>::s_keyword,
        sop::get(sop::Keyword::BreastProjectionXRayImageStorageForProcessing).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForProcessing>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.13.1.5").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedPETImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedPETImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedPETImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.130").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BasicStructuredDisplayStorage>::s_keyword,
        sop::get(sop::Keyword::BasicStructuredDisplayStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BasicStructuredDisplayStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.131").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation>::s_keyword,
        sop::get(sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.14.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing>::s_keyword,
        sop::get(sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.14.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::CTImageStorage>::s_keyword,
        sop::get(sop::Keyword::CTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::CTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedCTImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedCTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedCTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.2.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedCTImageStorage>::s_keyword,
        sop::get(sop::Keyword::LegacyConvertedEnhancedCTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedCTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.2.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::NuclearMedicineImageStorage>::s_keyword,
        sop::get(sop::Keyword::NuclearMedicineImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::NuclearMedicineImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.20").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::UltrasoundMultiFrameImageStorage>::s_keyword,
        sop::get(sop::Keyword::UltrasoundMultiFrameImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::UltrasoundMultiFrameImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.3.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ParametricMapStorage>::s_keyword,
        sop::get(sop::Keyword::ParametricMapStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ParametricMapStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.30").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MRImageStorage>::s_keyword,
        sop::get(sop::Keyword::MRImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MRImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedMRImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedMRImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedMRImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.4.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedMRColorImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedMRColorImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedMRColorImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.4.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedMRImageStorage>::s_keyword,
        sop::get(sop::Keyword::LegacyConvertedEnhancedMRImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::LegacyConvertedEnhancedMRImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.4.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RTImageStorage>::s_keyword,
        sop::get(sop::Keyword::RTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.481.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RTDoseStorage>::s_keyword,
        sop::get(sop::Keyword::RTDoseStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RTDoseStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.481.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedRTImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedRTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedRTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.481.23").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedContinuousRTImageStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedContinuousRTImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedContinuousRTImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.481.24").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::UltrasoundImageStorage>::s_keyword,
        sop::get(sop::Keyword::UltrasoundImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::UltrasoundImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.6.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedUSVolumeStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedUSVolumeStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedUSVolumeStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.6.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PhotoacousticImageStorage>::s_keyword,
        sop::get(sop::Keyword::PhotoacousticImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PhotoacousticImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.6.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SpatialFiducialsStorage>::s_keyword,
        sop::get(sop::Keyword::SpatialFiducialsStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SpatialFiducialsStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.66.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SegmentationStorage>::s_keyword,
        sop::get(sop::Keyword::SegmentationStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SegmentationStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.66.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceSegmentationStorage>::s_keyword,
        sop::get(sop::Keyword::SurfaceSegmentationStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceSegmentationStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.66.5").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceScanMeshStorage>::s_keyword,
        sop::get(sop::Keyword::SurfaceScanMeshStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceScanMeshStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.68.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceScanPointCloudStorage>::s_keyword,
        sop::get(sop::Keyword::SurfaceScanPointCloudStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SurfaceScanPointCloudStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.68.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SecondaryCaptureImageStorage>::s_keyword,
        sop::get(sop::Keyword::SecondaryCaptureImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SecondaryCaptureImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.7").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage>::s_keyword,
        sop::get(sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.7.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage>::s_keyword,
        sop::get(sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.7.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage>::s_keyword,
        sop::get(sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.7.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage>::s_keyword,
        sop::get(sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.7.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLEndoscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VLEndoscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLEndoscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoEndoscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VideoEndoscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoEndoscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.1.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLMicroscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VLMicroscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLMicroscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoMicroscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VideoMicroscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoMicroscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.2.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.3").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLPhotographicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VLPhotographicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLPhotographicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoPhotographicImageStorage>::s_keyword,
        sop::get(sop::Keyword::VideoPhotographicImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VideoPhotographicImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.4.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicPhotography8BitImageStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicPhotography8BitImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicPhotography8BitImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicPhotography16BitImageStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicPhotography16BitImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicPhotography16BitImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.2").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicTomographyImageStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicTomographyImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicTomographyImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.4").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage>::s_keyword,
        sop::get(sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.5").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage>::s_keyword,
        sop::get(sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.6").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.7").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.8").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLWholeSlideMicroscopyImageStorage>::s_keyword,
        sop::get(sop::Keyword::VLWholeSlideMicroscopyImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::VLWholeSlideMicroscopyImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.6").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DermoscopicPhotographyImageStorage>::s_keyword,
        sop::get(sop::Keyword::DermoscopicPhotographyImageStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::DermoscopicPhotographyImageStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.77.1.7").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SpectaclePrescriptionReportStorage>::s_keyword,
        sop::get(sop::Keyword::SpectaclePrescriptionReportStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SpectaclePrescriptionReportStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.78.6").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MacularGridThicknessAndVolumeReportStorage>::s_keyword,
        sop::get(sop::Keyword::MacularGridThicknessAndVolumeReportStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MacularGridThicknessAndVolumeReportStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.79.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicThicknessMapStorage>::s_keyword,
        sop::get(sop::Keyword::OphthalmicThicknessMapStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::OphthalmicThicknessMapStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.81.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::CornealTopographyMapStorage>::s_keyword,
        sop::get(sop::Keyword::CornealTopographyMapStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::CornealTopographyMapStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.82.1").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BasicTextSRStorage>::s_keyword,
        sop::get(sop::Keyword::BasicTextSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::BasicTextSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.11").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedSRStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.22").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ComprehensiveSRStorage>::s_keyword,
        sop::get(sop::Keyword::ComprehensiveSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ComprehensiveSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.33").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::Comprehensive3DSRStorage>::s_keyword,
        sop::get(sop::Keyword::Comprehensive3DSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::Comprehensive3DSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.34").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ExtensibleSRStorage>::s_keyword,
        sop::get(sop::Keyword::ExtensibleSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ExtensibleSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.35").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ProcedureLogStorage>::s_keyword,
        sop::get(sop::Keyword::ProcedureLogStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ProcedureLogStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.40").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MammographyCADSRStorage>::s_keyword,
        sop::get(sop::Keyword::MammographyCADSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::MammographyCADSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.50").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::KeyObjectSelectionDocumentStorage>::s_keyword,
        sop::get(sop::Keyword::KeyObjectSelectionDocumentStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::KeyObjectSelectionDocumentStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.59").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ChestCADSRStorage>::s_keyword,
        sop::get(sop::Keyword::ChestCADSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ChestCADSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.65").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayRadiationDoseSRStorage>::s_keyword,
        sop::get(sop::Keyword::XRayRadiationDoseSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::XRayRadiationDoseSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.67").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RadiopharmaceuticalRadiationDoseSRStorage>::s_keyword,
        sop::get(sop::Keyword::RadiopharmaceuticalRadiationDoseSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::RadiopharmaceuticalRadiationDoseSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.68").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ColonCADSRStorage>::s_keyword,
        sop::get(sop::Keyword::ColonCADSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ColonCADSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.69").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ImplantationPlanSRStorage>::s_keyword,
        sop::get(sop::Keyword::ImplantationPlanSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::ImplantationPlanSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.70").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::AcquisitionContextSRStorage>::s_keyword,
        sop::get(sop::Keyword::AcquisitionContextSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::AcquisitionContextSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.71").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SimplifiedAdultEchoSRStorage>::s_keyword,
        sop::get(sop::Keyword::SimplifiedAdultEchoSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::SimplifiedAdultEchoSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.72").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PatientRadiationDoseSRStorage>::s_keyword,
        sop::get(sop::Keyword::PatientRadiationDoseSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PatientRadiationDoseSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.73").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PlannedImagingAgentAdministrationSRStorage>::s_keyword,
        sop::get(sop::Keyword::PlannedImagingAgentAdministrationSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PlannedImagingAgentAdministrationSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.74").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PerformedImagingAgentAdministrationSRStorage>::s_keyword,
        sop::get(sop::Keyword::PerformedImagingAgentAdministrationSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::PerformedImagingAgentAdministrationSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.75").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXRayRadiationDoseSRStorage>::s_keyword,
        sop::get(sop::Keyword::EnhancedXRayRadiationDoseSRStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::EnhancedXRayRadiationDoseSRStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.1.1.88.76").m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::GenericImplantTemplateStorage>::s_keyword,
        sop::get(sop::Keyword::GenericImplantTemplateStorage).m_keyword
    );
    CPPUNIT_ASSERT_EQUAL(
        sop::Sop<sop::Keyword::GenericImplantTemplateStorage>::s_keyword,
        sop::get("1.2.840.10008.5.1.4.43.1").m_keyword
    );
}

//------------------------------------------------------------------------------

} // namespace sight::data::dicom::ut

// NOLINTEND
