/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/ImageSeries.hpp"
#include "data/ModelSeries.hpp"

#include "data/detail/SeriesImpl.hpp"
#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/tools/compare.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <chrono>
#include <utility>

SIGHT_REGISTER_DATA(sight::data::Series)

namespace sight::data
{

/// helper function to get the value of a tag as a string like "(0020,0011)", which can be searched on the internet.
inline static std::string tagToString(const gdcm::Tag& tag)
{
    std::stringstream ss;
    ss << "("
    << std::hex << std::setfill('0') << std::setw(4) << tag.GetGroup() << ","
    << std::hex << std::setfill('0') << std::setw(4) << tag.GetElement() << ")";

    return ss.str();
}

Series::Series(Object::Key /*unused*/) :
    m_pimpl(std::make_unique<detail::SeriesImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
Series::~Series() noexcept = default;

//------------------------------------------------------------------------------

void Series::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !bool(other)
    );

    m_pimpl->m_frame_datasets = other->m_pimpl->m_frame_datasets;

    BaseClass::shallowCopy(source);
}

//------------------------------------------------------------------------------

void Series::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !bool(other)
    );

    m_pimpl->m_frame_datasets = other->m_pimpl->m_frame_datasets;

    BaseClass::deepCopy(source, cache);
}

//------------------------------------------------------------------------------

void Series::copyPatientModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::PatientName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfPatientID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfPatientIDQualifiersSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientBirthDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSex>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedPatientSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientBirthTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientIDs>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientIDsSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientNames>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::EthnicGroup>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientComments>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSpeciesDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSpeciesCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientBreedDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientBreedCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::BreedRegistrationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ResponsiblePerson>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ResponsiblePersonRole>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ResponsibleOrganization>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientIdentityRemoved>(source_dataset);
    // cspell:ignore Deidentification
    m_pimpl->copyElement<gdcm::Keywords::DeidentificationMethod>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DeidentificationMethodCodeSequence>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialSubjectModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSponsorName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSiteID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSiteName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSubjectID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSubjectReadingID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeApprovalNumber>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyGeneralStudyModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::StudyInstanceUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::StudyDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::StudyTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferringPhysicianName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferringPhysicianIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::StudyID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AccessionNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfAccessionNumberSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::StudyDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansOfRecord>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansOfRecordIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::NameOfPhysiciansReadingStudy>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansReadingStudyIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RequestingServiceCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedStudySequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ProcedureCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReasonForPerformedProcedureCodeSequence>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyPatientStudyModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::AdmittingDiagnosesDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AdmittingDiagnosesCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientAge>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSize>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientWeight>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSizeCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::Occupation>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AdditionalPatientHistory>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AdmissionID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfAdmissionIDSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ServiceEpisodeID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfServiceEpisodeIDSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ServiceEpisodeDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientSexNeutered>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialStudyModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialTimePointID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialTimePointDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ConsentForClinicalTrialUseSequence>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyGeneralSeriesModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::Modality>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesInstanceUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::Laterality>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformingPhysicianName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformingPhysicianIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ProtocolName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDescriptionCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OperatorsName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OperatorIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedPerformedProcedureStepSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RelatedSeriesSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::BodyPartExamined>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientPosition>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RequestAttributesSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepStartDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepStartTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepEndDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepEndTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProtocolCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::CommentsOnThePerformedProcedureStep>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AnatomicalOrientationType>(source_dataset);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0108)); // SmallestImagePixelValue
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0109)); // LargestPixelValueInSeries
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialSeriesModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialCoordinatingCenterName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSeriesID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSeriesDescription>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyGeneralEquipmentModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::Manufacturer>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionAddress>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::StationName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionalDepartmentName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ManufacturerModelName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DeviceSerialNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SoftwareVersions>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::GantryID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SpatialResolution>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DateOfLastCalibration>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::TimeOfLastCalibration>(source_dataset);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0120)); // PixelPaddingValue
}

//------------------------------------------------------------------------------

void Series::copyFrameOfReferenceModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::FrameOfReferenceUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PositionReferenceIndicator>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copySOPCommonModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::SOPClassUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SOPInstanceUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SpecificCharacterSet>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreationDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreationTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreatorUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RelatedGeneralSOPClassUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OriginalSpecializedSOPClassUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::CodingSchemeIdentificationSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::TimezoneOffsetFromUTC>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ContributingEquipmentSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::InstanceNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SOPInstanceStatus>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SOPAuthorizationDateTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SOPAuthorizationComment>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AuthorizationEquipmentCertificationNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::MACParametersSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DigitalSignaturesSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::EncryptedAttributesSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::OriginalAttributesSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::HL7StructuredDocumentReferenceSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::LongitudinalTemporalInformationModified>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copyGeneralImageModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet();
    m_pimpl->copyElement<gdcm::Keywords::InstanceNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PatientOrientation>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ContentDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ContentTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ImageType>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionNumber>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionDate>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionDateTime>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedImageSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DerivationDescription>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::DerivationCodeSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::SourceImageSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedInstanceSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ImagesInAcquisition>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::ImageComments>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::QualityControlImage>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::BurnedInAnnotation>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RecognizableVisualFeatures>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompression>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompressionRatio>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompressionMethod>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IconImageSequence>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PresentationLUTShape>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::IrradiationEventUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::RealWorldValueMappingSequence>(source_dataset);
}

//------------------------------------------------------------------------------

bool Series::operator==(const Series& other) const noexcept
{
    // Check number of frames
    if(m_pimpl->m_frame_datasets.size() != other.m_pimpl->m_frame_datasets.size())
    {
        return false;
    }

    // Check frames specific attributes
    for(std::size_t i = 0, end = m_pimpl->m_frame_datasets.size() ; i < end ; ++i)
    {
        if(m_pimpl->m_frame_datasets[i].first.GetDES() != other.m_pimpl->m_frame_datasets[i].first.GetDES())
        {
            return false;
        }
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Series::operator!=(const Series& other) const noexcept
{
    return !(*this == other);
}

//-----------------------------------------------------------------------------

std::string Series::getByteValue(std::uint16_t group, std::uint16_t element, std::size_t instance) const
{
    const auto& dataset = m_pimpl->getDataSet(instance);
    gdcm::Tag tag(group, element);

    if(dataset.FindDataElement(tag))
    {
        if(const auto& data_element = dataset.GetDataElement(tag); !data_element.IsEmpty())
        {
            if(const auto* byte_value = data_element.GetByteValue();
               byte_value != nullptr && byte_value->GetPointer() != nullptr)
            {
                return detail::shrink(gdcm::String<>(byte_value->GetPointer(), byte_value->GetLength()).Trim());
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

void Series::setByteValue(std::uint16_t group, std::uint16_t element, const std::string& value, std::size_t instance)
{
    // Get the VR
    const gdcm::Tag tag(group, element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Get the padding char.
    const auto [size, fixed, padding] = detail::getVRFormat(vr);

    const auto& padded =
        [&](char padding_char)
        {
            if((value.size() % 2) != 0)
            {
                std::string padded_value(value);
                padded_value.push_back(padding_char);
                return padded_value;
            }

            return value;
        }(padding);

    // Create a new data element and assign the buffer from the string
    gdcm::DataElement data_element(tag);
    data_element.SetVR(vr);
    data_element.SetByteValue(padded.c_str(), std::uint32_t(padded.size()));

    // Store back the data element to the data set
    m_pimpl->getDataSet(instance).Replace(data_element);
}

//------------------------------------------------------------------------------

void Series::setStringValue(std::uint16_t group, std::uint16_t element, const std::string& value, std::size_t instance)
{
    // cspell:ignore stoull
    // Get the VR
    const gdcm::Tag tag(group, element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Nothing to do if the VR is an ASCII one
    if(gdcm::VR::IsASCII(vr))
    {
        setByteValue(group, element, value, instance);
    }
    else if(vr == gdcm::VR::FL)
    {
        m_pimpl->setArithmeticValue(tag, vr, std::stof(value), instance);
    }
    else if(vr == gdcm::VR::FD)
    {
        m_pimpl->setArithmeticValue(tag, vr, std::stod(value), instance);
    }
    else if(vr == gdcm::VR::US)
    {
        m_pimpl->setArithmeticValue(tag, vr, static_cast<uint16_t>(std::stoul(value)), instance);
    }
    else if(vr == gdcm::VR::UL)
    {
        m_pimpl->setArithmeticValue(tag, vr, static_cast<uint32_t>(std::stoul(value)), instance);
    }
    else if(vr == gdcm::VR::UV)
    {
        m_pimpl->setArithmeticValue(tag, vr, std::stoull(value), instance);
    }
    else if(vr == gdcm::VR::SS)
    {
        m_pimpl->setArithmeticValue(tag, vr, static_cast<int16_t>(std::stol(value)), instance);
    }
    else if(vr == gdcm::VR::SL)
    {
        m_pimpl->setArithmeticValue(tag, vr, static_cast<int32_t>(std::stol(value)), instance);
    }
    else if(vr == gdcm::VR::SV)
    {
        // cspell:ignore stoll
        m_pimpl->setArithmeticValue(tag, vr, std::stoll(value), instance);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(Exception("The value at " + tagToString(tag) + " cannot be converted from a string."));
    }
}

//------------------------------------------------------------------------------

std::string Series::getStringValue(std::uint16_t group, std::uint16_t element, std::size_t instance) const
{
    // Get the VR
    const gdcm::Tag tag(group, element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Nothing to do if the VR is an ASCII one
    if(gdcm::VR::IsASCII(vr))
    {
        return getByteValue(group, element, instance);
    }

    if(vr == gdcm::VR::FL)
    {
        return m_pimpl->getArithmeticValue<float>(tag, instance);
    }

    if(vr == gdcm::VR::FD)
    {
        return m_pimpl->getArithmeticValue<double>(tag, instance);
    }

    if(vr == gdcm::VR::US)
    {
        return m_pimpl->getArithmeticValue<std::uint16_t>(tag, instance);
    }

    if(vr == gdcm::VR::UL)
    {
        return m_pimpl->getArithmeticValue<std::uint32_t>(tag, instance);
    }

    if(vr == gdcm::VR::UV)
    {
        return m_pimpl->getArithmeticValue<std::uint64_t>(tag, instance);
    }

    if(vr == gdcm::VR::SS)
    {
        return m_pimpl->getArithmeticValue<std::int16_t>(tag, instance);
    }

    if(vr == gdcm::VR::SL)
    {
        return m_pimpl->getArithmeticValue<std::int32_t>(tag, instance);
    }

    if(vr == gdcm::VR::SV)
    {
        return m_pimpl->getArithmeticValue<std::int64_t>(tag, instance);
    }

    SIGHT_THROW_EXCEPTION(Exception("The value at " + tagToString(tag) + " cannot be converted to a string."));
}

//------------------------------------------------------------------------------

std::vector<std::string> Series::getByteValues(std::uint16_t group, std::uint16_t element, std::size_t instance) const
{
    std::vector<std::string> values;

    const std::string joined = getByteValue(group, element, instance);
    boost::split(values, joined, boost::is_any_of(detail::BACKSLASH_SEPARATOR));

    return values;
}

//------------------------------------------------------------------------------

void Series::setByteValues(
    std::uint16_t group,
    std::uint16_t element,
    const std::vector<std::string>& values,
    std::size_t instance
)
{
    const auto& joined = boost::join(values, detail::BACKSLASH_SEPARATOR);
    setByteValue(group, element, joined, instance);
}

//------------------------------------------------------------------------------

const gdcm::DataSet& Series::getDataSet(std::size_t instance) const
{
    return m_pimpl->getDataSet(instance);
}

//------------------------------------------------------------------------------

gdcm::DataSet& Series::getDataSet(std::size_t instance)
{
    return m_pimpl->getDataSet(instance);
}

//------------------------------------------------------------------------------

void Series::setDataSet(const gdcm::DataSet& dataset, std::size_t instance)
{
    m_pimpl->getDataSet(instance) = dataset;
}

//------------------------------------------------------------------------------

std::filesystem::path Series::getFile(std::size_t instance) const
{
    return m_pimpl->getFile(instance);
}

//------------------------------------------------------------------------------

void Series::setFile(const std::filesystem::path& file, std::size_t instance)
{
    m_pimpl->getFile(instance) = file;
}

//------------------------------------------------------------------------------

std::size_t Series::numInstances() const noexcept
{
    return m_pimpl->m_frame_datasets.size();
}

//------------------------------------------------------------------------------

bool Series::sort(const std::vector<std::size_t>& sorted)
{
    // Some checks to be sure everything is fine
    // If almost no frame are kept, we may want to try to sort the frames differently.
    if(sorted.size() < 2)
    {
        return false;
    }

    SIGHT_WARN_IF(
        "Some duplicated frames have been dropped during the sorting process.",
        sorted.size() != m_pimpl->m_frame_datasets.size()
    );

    // Finally, we can sort the frames in the series
    detail::FrameDatasets sorted_frame_datasets;

    for(const auto& from : sorted)
    {
        sorted_frame_datasets.push_back(std::move(m_pimpl->m_frame_datasets[from]));
    }

    m_pimpl->m_frame_datasets = std::move(sorted_frame_datasets);

    return true;
}

//------------------------------------------------------------------------------

std::string Series::getSOPClassUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SOPClassUID>();
}

//------------------------------------------------------------------------------

std::string Series::getSOPClassName() const noexcept
{
    if(const auto& sop_class_uid = m_pimpl->getValue<gdcm::Keywords::SOPClassUID>(); sop_class_uid)
    {
        gdcm::UIDs uid {};
        if(uid.SetFromUID(*sop_class_uid))
        {
            return {uid.GetName()};
        }
    }

    return getClassname();
}

//------------------------------------------------------------------------------

void Series::setSOPClassUID(const std::string& sopClassUID)
{
    m_pimpl->setValue<gdcm::Keywords::SOPClassUID>(sopClassUID);
}

//------------------------------------------------------------------------------

std::string Series::getSOPInstanceUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SOPInstanceUID>();
}

//------------------------------------------------------------------------------

void Series::setSOPInstanceUID(const std::string& sopInstanceUID)
{
    m_pimpl->setValue<gdcm::Keywords::SOPInstanceUID>(sopInstanceUID);
}

//------------------------------------------------------------------------------

std::string Series::getSpecificCharacterSet() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::SpecificCharacterSet>();
}

//------------------------------------------------------------------------------

void Series::setSpecificCharacterSet(const std::string& specificCharacterSet)
{
    m_pimpl->setJoinedValues<gdcm::Keywords::SpecificCharacterSet>(specificCharacterSet);
}

//------------------------------------------------------------------------------

std::string Series::getEncoding() const noexcept
{
    const auto& specific_character_set = getSpecificCharacterSet();

    if(specific_character_set.empty())
    {
        // Legally, we should return ASCII, but since ASCII can be directly mapped to UTF-8, we return UTF-8
        return "UTF-8";
    }

    if(specific_character_set == "GB18030")
    {
        // Chinese (multi-byte)
        return "GB18030";
    }

    if(specific_character_set == "GBK")
    {
        // Chinese (multi-byte, subset of "GB 18030")
        return "GBK";
    }

    // Transform a bit the string to help conversion
    // Basically, there are three possible versions of the same meaning:
    // - (empty) => ASCII
    // - ISO_IR 6
    // - ISO 2022 IR 6
    // - ISO 2022\IR 6
    const std::size_t pos                 = specific_character_set.find("IR ");
    const std::string& simplified_charset = pos == std::string::npos
                                            ? specific_character_set
                                            : specific_character_set.substr(pos + 3);

    if(simplified_charset == "6")
    {
        // Legally, we should return ASCII, but since ASCII can be directly mapped to UTF-8, we return UTF-8
        return "UTF-8";
    }

    if(simplified_charset == "100")
    {
        // Latin alphabet No. 1
        return "ISO-8859-1";
    }

    if(simplified_charset == "101")
    {
        // Latin alphabet No. 2
        return "ISO-8859-2";
    }

    if(simplified_charset == "109")
    {
        // Latin alphabet No. 3
        return "ISO-8859-3";
    }

    if(simplified_charset == "110")
    {
        // Latin alphabet No. 4
        return "ISO-8859-4";
    }

    if(simplified_charset == "144")
    {
        // Cyrillic
        return "ISO-8859-5";
    }

    if(simplified_charset == "127")
    {
        // Arabic
        return "ISO-8859-6";
    }

    if(simplified_charset == "126")
    {
        // Greek
        return "ISO-8859-7";
    }

    if(simplified_charset == "138")
    {
        // Hebrew
        return "ISO-8859-8";
    }

    if(simplified_charset == "148")
    {
        // Latin alphabet No. 5
        return "ISO-8859-9";
    }

    if(simplified_charset == "13")
    {
        // Japanese
        /// @note: this is not a perfect match, but better than nothing
        return "Shift_JIS";
    }

    if(simplified_charset == "166")
    {
        // Thai
        /// @note: this is not a perfect match, but better than nothing
        return "TIS-620";
    }

    if(simplified_charset == "192")
    {
        // Unicode in UTF-8 (multi-byte)
        return "UTF-8";
    }

    if(simplified_charset == "87")
    {
        // Japanese (multi-byte)
        return "ISO-2022-JP-1";
    }

    if(simplified_charset == "159")
    {
        // Japanese (multi-byte)
        return "ISO-2022-JP-2";
    }

    if(simplified_charset == "149")
    {
        // Korean (multi-byte)
        return "ISO-2022-KR";
    }

    if(simplified_charset == "58")
    {
        // Simplified Chinese (multi-byte)
        return "ISO-2022-CN";
    }

    // We hope that the underlying codec will deal with it...
    // This case is used for DICOM official sample with one "WINDOWS_1252" encoding
    return specific_character_set;
}

//-----------------------------------------------------------------------------

std::string Series::getSeriesDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SeriesDate>();
}

//-----------------------------------------------------------------------------

void Series::setSeriesDate(const std::string& seriesDate)
{
    m_pimpl->setValue<gdcm::Keywords::SeriesDate>(seriesDate);
}

//-----------------------------------------------------------------------------

std::string Series::getSeriesTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SeriesTime>();
}

//-----------------------------------------------------------------------------

void Series::setSeriesTime(const std::string& seriesTime)
{
    m_pimpl->setValue<gdcm::Keywords::SeriesTime>(seriesTime);
}

//-----------------------------------------------------------------------------

std::string Series::getModality() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::Modality>();
}

//-----------------------------------------------------------------------------

void Series::setModality(const std::string& modality)
{
    m_pimpl->setValue<gdcm::Keywords::Modality>(modality);
}

//-----------------------------------------------------------------------------

std::string Series::getSeriesDescription() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SeriesDescription>();
}

//-----------------------------------------------------------------------------

void Series::setSeriesDescription(const std::string& seriesDescription)
{
    m_pimpl->setValue<gdcm::Keywords::SeriesDescription>(seriesDescription);
}

//------------------------------------------------------------------------------

std::vector<std::string> Series::getPerformingPhysicianNames() const noexcept
{
    return m_pimpl->getStringValues<gdcm::Keywords::PerformingPhysicianName>();
}

//------------------------------------------------------------------------------

void Series::setPerformingPhysicianNames(const std::vector<std::string>& performingPhysicianName)
{
    m_pimpl->setStringValues<gdcm::Keywords::PerformingPhysicianName>(performingPhysicianName);
}

//------------------------------------------------------------------------------

std::string Series::getPerformingPhysicianName() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::PerformingPhysicianName>();
}

//------------------------------------------------------------------------------

void Series::setPerformingPhysicianName(const std::string& performingPhysicianName)
{
    return m_pimpl->setJoinedValues<gdcm::Keywords::PerformingPhysicianName>(performingPhysicianName);
}

//------------------------------------------------------------------------------

std::string Series::getAnatomicalOrientationType() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::AnatomicalOrientationType>();
}

//------------------------------------------------------------------------------

void Series::setAnatomicalOrientationType(const std::string& anatomicalOrientationType)
{
    m_pimpl->setValue<gdcm::Keywords::AnatomicalOrientationType>(anatomicalOrientationType);
}

//------------------------------------------------------------------------------

std::string Series::getBodyPartExamined() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::BodyPartExamined>();
}

//------------------------------------------------------------------------------

void Series::setBodyPartExamined(const std::string& bodyPartExamined)
{
    m_pimpl->setValue<gdcm::Keywords::BodyPartExamined>(bodyPartExamined);
}

//------------------------------------------------------------------------------

std::string Series::getProtocolName() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ProtocolName>();
}

//------------------------------------------------------------------------------

void Series::setProtocolName(const std::string& protocolName)
{
    m_pimpl->setValue<gdcm::Keywords::ProtocolName>(protocolName);
}

//------------------------------------------------------------------------------

std::string Series::getPatientPosition() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientPosition>();
}

//------------------------------------------------------------------------------

std::string Series::getPatientPositionString() const noexcept
{
    const auto& patient_position = getPatientPosition();

    if(patient_position == "HFP")
    {
        return "Head First-Prone";
    }

    if(patient_position == "HFS")
    {
        return "Head First-Supine";
    }

    if(patient_position == "HFDR")
    {
        return "Head First-Decubitus Right";
    }

    if(patient_position == "HFDL")
    {
        return "Head First-Decubitus Left";
    }

    if(patient_position == "FFDR")
    {
        return "Feet First-Decubitus Right";
    }

    if(patient_position == "FFDL")
    {
        return "Feet First-Decubitus Left";
    }

    if(patient_position == "FFP")
    {
        return "Feet First-Prone";
    }

    if(patient_position == "FFS")
    {
        return "Feet First-Supine";
    }

    if(patient_position == "LFP")
    {
        return "Left First-Prone";
    }

    if(patient_position == "LFS")
    {
        return "Left First-Supine";
    }

    if(patient_position == "RFP")
    {
        return "Right First-Prone";
    }

    if(patient_position == "RFS")
    {
        return "Right First-Supine";
    }

    if(patient_position == "AFDR")
    {
        return "Anterior First-Decubitus Right";
    }

    if(patient_position == "AFDL")
    {
        return "Anterior First-Decubitus Left";
    }

    if(patient_position == "PFDR")
    {
        return "Posterior First-Decubitus Right";
    }

    if(patient_position == "PFDL")
    {
        return "Posterior First-Decubitus Left";
    }

    return patient_position;
}

//------------------------------------------------------------------------------

void Series::setPatientPosition(const std::string& patientPosition)
{
    m_pimpl->setValue<gdcm::Keywords::PatientPosition>(patientPosition);
}

//------------------------------------------------------------------------------

std::string Series::getSeriesInstanceUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SeriesInstanceUID>();
}

//-----------------------------------------------------------------------------

void Series::setSeriesInstanceUID(const std::string& seriesInstanceUID)
{
    m_pimpl->setValue<gdcm::Keywords::SeriesInstanceUID>(seriesInstanceUID);
}

//-----------------------------------------------------------------------------

std::optional<std::int32_t> Series::getSeriesNumber() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::SeriesNumber>();
}

//-----------------------------------------------------------------------------

void Series::setSeriesNumber(const std::optional<std::int32_t>& seriesNumber)
{
    m_pimpl->setValue<gdcm::Keywords::SeriesNumber>(seriesNumber);
}

//-----------------------------------------------------------------------------

std::string Series::getLaterality() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::Laterality>();
}

//-----------------------------------------------------------------------------

void Series::setLaterality(const std::string& laterality)
{
    m_pimpl->setValue<gdcm::Keywords::Laterality>(laterality);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepStartDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepStartDate>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepStartDate(const std::string& performedProcedureStepStartDate)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepStartDate>(performedProcedureStepStartDate);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepStartTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepStartTime>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepStartTime(const std::string& performedProcedureStepStartTime)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepStartTime>(performedProcedureStepStartTime);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepEndDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepEndDate>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepEndDate(const std::string& performedProcedureStepEndDate)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepEndDate>(performedProcedureStepEndDate);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepEndTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepEndTime>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepEndTime(const std::string& performedProcedureStepEndTime)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepEndTime>(performedProcedureStepEndTime);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepID>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepID(const std::string& performedProcedureStepID)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepID>(performedProcedureStepID);
}

//------------------------------------------------------------------------------

std::string Series::getPerformedProcedureStepDescription() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PerformedProcedureStepDescription>();
}

//------------------------------------------------------------------------------

void Series::setPerformedProcedureStepDescription(const std::string& performedProcedureStepDescription)
{
    m_pimpl->setValue<gdcm::Keywords::PerformedProcedureStepDescription>(performedProcedureStepDescription);
}

//------------------------------------------------------------------------------

std::string Series::getCommentsOnThePerformedProcedureStep() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::CommentsOnThePerformedProcedureStep>();
}

//------------------------------------------------------------------------------

void Series::setCommentsOnThePerformedProcedureStep(const std::string& commentsOnThePerformedProcedureStep)
{
    m_pimpl->setValue<gdcm::Keywords::CommentsOnThePerformedProcedureStep>(commentsOnThePerformedProcedureStep);
}

//------------------------------------------------------------------------------

std::string Series::getInstitutionName() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::InstitutionName>();
}

//------------------------------------------------------------------------------

void Series::setInstitutionName(const std::string& institutionName)
{
    m_pimpl->setValue<gdcm::Keywords::InstitutionName>(institutionName);
}

//------------------------------------------------------------------------------

std::string Series::getPatientName() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientName>();
}

//------------------------------------------------------------------------------

void Series::setPatientName(const std::string& patientName)
{
    m_pimpl->setValue<gdcm::Keywords::PatientName>(patientName);
}

//------------------------------------------------------------------------------

std::string Series::getPatientID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientID>();
}

//------------------------------------------------------------------------------

void Series::setPatientID(const std::string& patientID)
{
    m_pimpl->setValue<gdcm::Keywords::PatientID>(patientID);
}

//------------------------------------------------------------------------------

std::string Series::getPatientBirthDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientBirthDate>();
}

//------------------------------------------------------------------------------

void Series::setPatientBirthDate(const std::string& patientBirthDate)
{
    m_pimpl->setValue<gdcm::Keywords::PatientBirthDate>(patientBirthDate);
}

//------------------------------------------------------------------------------

std::string Series::getPatientSex() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientSex>();
}

//------------------------------------------------------------------------------

void Series::setPatientSex(const std::string& patientSex)
{
    m_pimpl->setValue<gdcm::Keywords::PatientSex>(patientSex);
}

//------------------------------------------------------------------------------

std::string Series::getStudyDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::StudyDate>();
}

//------------------------------------------------------------------------------

void Series::setStudyDate(const std::string& studyDate)
{
    m_pimpl->setValue<gdcm::Keywords::StudyDate>(studyDate);
}

//------------------------------------------------------------------------------

std::string Series::getStudyTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::StudyTime>();
}

//------------------------------------------------------------------------------

void Series::setStudyTime(const std::string& studyTime)
{
    m_pimpl->setValue<gdcm::Keywords::StudyTime>(studyTime);
}

//------------------------------------------------------------------------------

std::string Series::getReferringPhysicianName() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ReferringPhysicianName>();
}

//------------------------------------------------------------------------------

void Series::setReferringPhysicianName(const std::string& referringPhysicianName)
{
    m_pimpl->setValue<gdcm::Keywords::ReferringPhysicianName>(referringPhysicianName);
}

//------------------------------------------------------------------------------

std::string Series::getStudyDescription() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::StudyDescription>();
}

//------------------------------------------------------------------------------

void Series::setStudyDescription(const std::string& studyDescription)
{
    m_pimpl->setValue<gdcm::Keywords::StudyDescription>(studyDescription);
}

//------------------------------------------------------------------------------

std::string Series::getStudyInstanceUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::StudyInstanceUID>();
}

//------------------------------------------------------------------------------

void Series::setStudyInstanceUID(const std::string& studyInstanceUID)
{
    m_pimpl->setValue<gdcm::Keywords::StudyInstanceUID>(studyInstanceUID);
}

//------------------------------------------------------------------------------

std::string Series::getStudyID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::StudyID>();
}

//------------------------------------------------------------------------------

void Series::setStudyID(const std::string& studyID)
{
    m_pimpl->setValue<gdcm::Keywords::StudyID>(studyID);
}

//------------------------------------------------------------------------------

std::string Series::getPatientAge() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::PatientAge>();
}

//------------------------------------------------------------------------------

void Series::setPatientAge(const std::string& patientAge)
{
    m_pimpl->setValue<gdcm::Keywords::PatientAge>(patientAge);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getPatientSize() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::PatientSize>();
}

//------------------------------------------------------------------------------

void Series::setPatientSize(const std::optional<double>& patientSize)
{
    m_pimpl->setValue<gdcm::Keywords::PatientSize>(patientSize);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getPatientWeight() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::PatientWeight>();
}

//------------------------------------------------------------------------------

void Series::setPatientWeight(const std::optional<double>& patientWeight)
{
    m_pimpl->setValue<gdcm::Keywords::PatientWeight>(patientWeight);
}

//------------------------------------------------------------------------------

Series::DicomType Series::getDicomType() const noexcept
{
    // First try with dynamic cast
    if(dynamic_cast<const ImageSeries*>(this) != nullptr)
    {
        return DicomType::IMAGE;
    }

    if(dynamic_cast<const ModelSeries*>(this) != nullptr)
    {
        return DicomType::MODEL;
    }

    // Then try with the SOPClassUID
    if(const auto& sop_class_uid = m_pimpl->getValue<gdcm::Keywords::SOPClassUID>(); sop_class_uid)
    {
        if(const DicomType result = getDicomType(*sop_class_uid); result != DicomType::UNKNOWN)
        {
            return result;
        }
    }

    // On last resort, try with semi deprecated GDCM MediaStorage
    /// @note This GDCM class is no more updated and many things are missing: for example, Surface Scan Mesh Storage
    /// (1.2.840.10008.5.1.4.1.1.68.1) and Surface Scan Point Cloud Storage (1.2.840.10008.5.1.4.1.1.68.2) which are
    /// somewhat "models", are unknown to GDCM MediaStorage.
    gdcm::MediaStorage media_storage;
    media_storage.SetFromDataSet(getDataSet());

    if(gdcm::MediaStorage::IsImage(media_storage))
    {
        return DicomType::IMAGE;
    }

    if(media_storage == gdcm::MediaStorage::SurfaceSegmentationStorage)
    {
        return DicomType::MODEL;
    }

    // If we are here, nothing worked...
    return DicomType::UNKNOWN;
}

//------------------------------------------------------------------------------

Series::DicomType Series::getDicomType(const std::string& sop_class_uid) noexcept
{
    gdcm::UIDs uid {};
    if(uid.SetFromUID(sop_class_uid.c_str()))
    {
        // cspell:ignore Multiframe Radiofluoroscopic Tomosynthesis Bscan
        const unsigned int ts = uid;

        switch(ts)
        {
            case gdcm::UIDs::SurfaceSegmentationStorage:
            case gdcm::UIDs::SurfaceScanMeshStorage:
            case gdcm::UIDs::SurfaceScanPointCloudStorage:
                return DicomType::MODEL;

            case gdcm::UIDs::ComputedRadiographyImageStorage:
            case gdcm::UIDs::CTImageStorage:
            case gdcm::UIDs::MRImageStorage:
            case gdcm::UIDs::NuclearMedicineImageStorage:
            case gdcm::UIDs::UltrasoundImageStorage:
            case gdcm::UIDs::UltrasoundMultiframeImageStorage:
            case gdcm::UIDs::SecondaryCaptureImageStorage:
            case gdcm::UIDs::MultiframeSingleBitSecondaryCaptureImageStorage:
            case gdcm::UIDs::MultiframeGrayscaleByteSecondaryCaptureImageStorage:
            case gdcm::UIDs::MultiframeGrayscaleWordSecondaryCaptureImageStorage:
            case gdcm::UIDs::MultiframeTrueColorSecondaryCaptureImageStorage:
            case gdcm::UIDs::XRayAngiographicImageStorage:
            case gdcm::UIDs::XRayRadiofluoroscopicImageStorage:
            case gdcm::UIDs::RTImageStorage:
            case gdcm::UIDs::RTDoseStorage:
            case gdcm::UIDs::PositronEmissionTomographyImageStorage:
            case gdcm::UIDs::DigitalXRayImageStorageForPresentation:
            case gdcm::UIDs::DigitalXRayImageStorageForProcessing:
            case gdcm::UIDs::DigitalMammographyXRayImageStorageForPresentation:
            case gdcm::UIDs::DigitalMammographyXRayImageStorageForProcessing:
            case gdcm::UIDs::DigitalIntraoralXRayImageStorageForPresentation:
            case gdcm::UIDs::DigitalIntraoralXRayImageStorageForProcessing:
            case gdcm::UIDs::VLEndoscopicImageStorage:
            case gdcm::UIDs::VLMicroscopicImageStorage:
            case gdcm::UIDs::VLSlideCoordinatesMicroscopicImageStorage:
            case gdcm::UIDs::VLPhotographicImageStorage:
            case gdcm::UIDs::VideoEndoscopicImageStorage:
            case gdcm::UIDs::VideoMicroscopicImageStorage:
            case gdcm::UIDs::VideoPhotographicImageStorage:
            case gdcm::UIDs::VLWholeSlideMicroscopyImageStorage:
            case gdcm::UIDs::EnhancedMRImageStorage:
            case gdcm::UIDs::EnhancedMRColorImageStorage:
            case gdcm::UIDs::EnhancedCTImageStorage:
            case gdcm::UIDs::OphthalmicPhotography8BitImageStorage:
            case gdcm::UIDs::OphthalmicPhotography16BitImageStorage:
            case gdcm::UIDs::EnhancedXAImageStorage:
            case gdcm::UIDs::EnhancedXRFImageStorage:
            case gdcm::UIDs::SegmentationStorage:
            case gdcm::UIDs::OphthalmicTomographyImageStorage:
            case gdcm::UIDs::XRay3DAngiographicImageStorage:
            case gdcm::UIDs::XRay3DCraniofacialImageStorage:
            case gdcm::UIDs::BreastTomosynthesisImageStorage:
            case gdcm::UIDs::EnhancedPETImageStorage:
            case gdcm::UIDs::EnhancedUSVolumeStorage:
            case gdcm::UIDs::IntravascularOpticalCoherenceTomographyImageStorageForPresentation:
            case gdcm::UIDs::IntravascularOpticalCoherenceTomographyImageStorageForProcessing:
            case gdcm::UIDs::OphthalmicThicknessMapStorage:
            case gdcm::UIDs::LegacyConvertedEnhancedCTImageStorage:
            case gdcm::UIDs::LegacyConvertedEnhancedMRImageStorage:
            case gdcm::UIDs::LegacyConvertedEnhancedPETImageStorage:
            case gdcm::UIDs::CornealTopographyMapStorage:
            case gdcm::UIDs::BreastProjectionXRayImageStorageForPresentation:
            case gdcm::UIDs::BreastProjectionXRayImageStorageForProcessing:
            case gdcm::UIDs::ParametricMapStorage:
            case gdcm::UIDs::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage:
            case gdcm::UIDs::WideFieldOphthalmicPhotography3DCoordinatesImageStorage:
            case gdcm::UIDs::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage:
            case gdcm::UIDs::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage:
                return DicomType::IMAGE;

            default:
                return DicomType::UNKNOWN;
        }
    }

    return DicomType::UNKNOWN;
}

//------------------------------------------------------------------------------

std::string Series::dicomTypesToString(Series::DicomTypes types) noexcept
{
    std::string dicom_types;

    if((types & static_cast<DicomTypes>(DicomType::IMAGE)) == types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicomTypeToString(DicomType::IMAGE);
    }

    if((types & static_cast<DicomTypes>(DicomType::MODEL)) == types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicomTypeToString(DicomType::MODEL);
    }

    if((types & static_cast<DicomTypes>(DicomType::REPORT)) == types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicomTypeToString(DicomType::REPORT);
    }

    return dicom_types;
}

//------------------------------------------------------------------------------

Series::DicomTypes Series::stringToDicomTypes(const std::string& types) noexcept
{
    DicomTypes dicom_types = 0;

    std::vector<std::string> split;
    boost::split(split, types, boost::is_any_of(","));

    for(const auto& type : split)
    {
        const auto& trimmed = boost::trim_copy(type);

        if(!trimmed.empty())
        {
            if(const auto& dicom_type = stringToDicomType(trimmed); dicom_type != DicomType::UNKNOWN)
            {
                dicom_types |= static_cast<DicomTypes>(dicom_type);
            }
        }
    }

    return dicom_types;
}

//------------------------------------------------------------------------------

std::string Series::getAcquisitionDate(std::size_t instance) const
{
    return m_pimpl->getStringValue<gdcm::Keywords::AcquisitionDate>(instance);
}

//------------------------------------------------------------------------------

void Series::setAcquisitionDate(const std::string& acquisitionDate, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::AcquisitionDate>(acquisitionDate, instance);
}

//------------------------------------------------------------------------------

std::string Series::getAcquisitionTime(std::size_t instance) const
{
    return m_pimpl->getStringValue<gdcm::Keywords::AcquisitionTime>(instance);
}

//------------------------------------------------------------------------------

void Series::setAcquisitionTime(const std::string& acquisitionTime, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::AcquisitionTime>(acquisitionTime, instance);
}

//------------------------------------------------------------------------------

std::string Series::getContentTime(std::size_t instance) const
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContentTime>(instance);
}

//------------------------------------------------------------------------------

void Series::setContentTime(const std::string& contentTime, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::ContentTime>(contentTime, instance);
}

//------------------------------------------------------------------------------

std::optional<std::int32_t> Series::getInstanceNumber(std::size_t instance) const
{
    return m_pimpl->getValue<gdcm::Keywords::InstanceNumber>(instance);
}

//------------------------------------------------------------------------------

void Series::setInstanceNumber(const std::optional<std::int32_t>& instanceNumber, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::InstanceNumber>(instanceNumber, instance);
}

//------------------------------------------------------------------------------

std::optional<std::int32_t> Series::getAcquisitionNumber(std::size_t instance) const
{
    return m_pimpl->getValue<gdcm::Keywords::AcquisitionNumber>(instance);
}

//------------------------------------------------------------------------------

void Series::setAcquisitionNumber(std::optional<std::int32_t> acquisitionNumber, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::AcquisitionNumber>(acquisitionNumber, instance);
}

//------------------------------------------------------------------------------

std::string Series::getContrastBolusAgent() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusAgent>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusAgent(const std::string& contrastBolusAgent)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusAgent>(contrastBolusAgent);
}

//------------------------------------------------------------------------------

std::string Series::getContrastBolusRoute() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusRoute>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusRoute(const std::string& contrastBolusRoute)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusRoute>(contrastBolusRoute);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getContrastBolusVolume() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusVolume>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusVolume(const std::optional<double>& contrastBolusVolume)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusVolume>(contrastBolusVolume);
}

//------------------------------------------------------------------------------

std::string Series::getContrastBolusStartTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusStartTime>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusStartTime(const std::string& contrastBolusStartTime)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusStartTime>(contrastBolusStartTime);
}

//------------------------------------------------------------------------------

std::string Series::getContrastBolusStopTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusStopTime>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusStopTime(const std::string& contrastBolusStopTime)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusStopTime>(contrastBolusStopTime);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getContrastBolusTotalDose() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusTotalDose>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusTotalDose(const std::optional<double>& contrastBolusTotalDose)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusTotalDose>(contrastBolusTotalDose);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getContrastFlowRates() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::ContrastFlowRate>();
}

//------------------------------------------------------------------------------

std::string Series::getContrastFlowRate() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::ContrastFlowRate>();
}

//------------------------------------------------------------------------------

void Series::setContrastFlowRates(const std::vector<double>& contrastFlowRates)
{
    m_pimpl->setValues<gdcm::Keywords::ContrastFlowRate>(contrastFlowRates);
}

//------------------------------------------------------------------------------

void Series::setContrastFlowRate(const std::string& contrastFlowRates)
{
    m_pimpl->setJoinedValues<gdcm::Keywords::ContrastFlowRate>(contrastFlowRates);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getContrastFlowDurations() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::ContrastFlowDuration>();
}

//------------------------------------------------------------------------------

std::string Series::getContrastFlowDuration() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::ContrastFlowDuration>();
}

//------------------------------------------------------------------------------

void Series::setContrastFlowDurations(const std::vector<double>& contrastFlowDurations)
{
    m_pimpl->setValues<gdcm::Keywords::ContrastFlowDuration>(contrastFlowDurations);
}

//------------------------------------------------------------------------------

void Series::setContrastFlowDuration(const std::string& contrastFlowDurations)
{
    m_pimpl->setJoinedValues<gdcm::Keywords::ContrastFlowDuration>(contrastFlowDurations);
}

//------------------------------------------------------------------------------

std::string Series::getContrastBolusIngredient() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusIngredient>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusIngredient(const std::string& contrastBolusIngredient)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredient>(contrastBolusIngredient);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getContrastBolusIngredientConcentration() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusIngredientConcentration>();
}

//------------------------------------------------------------------------------

void Series::setContrastBolusIngredientConcentration(
    const std::optional<double>& contrastBolusIngredientConcentration
)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredientConcentration>(contrastBolusIngredientConcentration);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getWindowCenter() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::WindowCenter>();
}

//------------------------------------------------------------------------------

void Series::setWindowCenter(const std::vector<double>& windowCenters)
{
    m_pimpl->setValues<gdcm::Keywords::WindowCenter>(windowCenters);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getWindowWidth() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::WindowWidth>();
}

//------------------------------------------------------------------------------

void Series::setWindowWidth(const std::vector<double>& windowWidths)
{
    m_pimpl->setValues<gdcm::Keywords::WindowWidth>(windowWidths);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getRescaleIntercept(std::size_t instance) const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::RescaleIntercept>(instance);
}

//------------------------------------------------------------------------------

void Series::setRescaleIntercept(const std::optional<double>& rescaleIntercept, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredientConcentration>(rescaleIntercept, instance);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getRescaleSlope(std::size_t instance) const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::RescaleSlope>(instance);
}

//------------------------------------------------------------------------------

void Series::setRescaleSlope(const std::optional<double>& rescaleSlope, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredientConcentration>(rescaleSlope, instance);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImagePositionPatient(std::size_t instance) const
{
    return m_pimpl->getValues<gdcm::Keywords::ImagePositionPatient>(instance);
}

//------------------------------------------------------------------------------

void Series::setImagePositionPatient(const std::vector<double>& imagePositionPatient, std::size_t instance)
{
    m_pimpl->setValues<gdcm::Keywords::ImagePositionPatient>(imagePositionPatient, instance);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImageOrientationPatient(std::size_t instance) const
{
    return m_pimpl->getValues<gdcm::Keywords::ImageOrientationPatient>(instance);
}

//------------------------------------------------------------------------------

void Series::setImageOrientationPatient(const std::vector<double>& imageOrientationPatient, std::size_t instance)
{
    m_pimpl->setValues<gdcm::Keywords::ImageOrientationPatient>(imageOrientationPatient, instance);
}

//------------------------------------------------------------------------------

sight::data::Matrix4 Series::getImageTransformPatient(std::size_t instance) const
{
    const auto position = this->getImagePositionPatient(instance);
    SIGHT_ASSERT("Unexpected orientation vector size", position.size() == 3);

    const auto orientation = this->getImageOrientationPatient(instance);
    SIGHT_ASSERT("Unexpected orientation vector size", orientation.size() == 6);

    const glm::dvec3 x(orientation[0], orientation[1], orientation[2]);
    const glm::dvec3 y(orientation[3], orientation[4], orientation[5]);
    const glm::dvec3 z = glm::cross(x, y);

    return {
        orientation[0], orientation[3], z[0], position[0],
        orientation[1], orientation[4], z[1], position[1],
        orientation[2], orientation[5], z[2], position[2],
        0., 0., 0., 1.
    };
}

//------------------------------------------------------------------------------

void Series::setImageTransformPatient(const sight::data::Matrix4& transform, std::size_t instance)
{
    this->setImagePositionPatient({transform[3], transform[7], transform[11]}, instance);

    [[maybe_unused]] const glm::dvec3 x(transform[0], transform[4], transform[8]);
    [[maybe_unused]] const glm::dvec3 y(transform[1], transform[5], transform[9]);
    [[maybe_unused]] const glm::dvec3 z(transform[2], transform[6], transform[10]);
    [[maybe_unused]] const glm::dvec3 computedZ = glm::cross(x, y);
    SIGHT_ASSERT("Unexpected orientation vector size", glm::all(glm::epsilonEqual(computedZ, z, 1e-2)));

    this->setImageOrientationPatient(
        {transform[0], transform[4], transform[8], transform[1], transform[5], transform[9]},
        instance
    );
}

//------------------------------------------------------------------------------

std::optional<double> Series::getSliceThickness() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::SliceThickness>();
}

//------------------------------------------------------------------------------

void Series::setSliceThickness(const std::optional<double>& sliceThickness)
{
    m_pimpl->setValue<gdcm::Keywords::SliceThickness>(sliceThickness);
}

//------------------------------------------------------------------------------

std::optional<std::uint16_t> Series::getRows() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::Rows>();
}

//------------------------------------------------------------------------------

void Series::setRows(const std::optional<std::uint16_t>& rows)
{
    m_pimpl->setValue<gdcm::Keywords::Rows>(rows);
}

//------------------------------------------------------------------------------

std::optional<std::uint16_t> Series::getColumns() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::Columns>();
}

//------------------------------------------------------------------------------

void Series::setColumns(const std::optional<std::uint16_t>& columns)
{
    m_pimpl->setValue<gdcm::Keywords::Columns>(columns);
}

} // namespace sight::data
