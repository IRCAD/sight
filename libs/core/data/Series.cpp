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

#include "data/ImageSeries.hpp"
#include "data/ModelSeries.hpp"

#include "data/detail/SeriesImpl.hxx"
#include "data/dicom/Sop.hpp"
#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/tools/compare.hpp>

#include <gdcmDict.h>
#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmPrivateTag.h>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <iomanip>
#include <sstream>
#include <utility>

SIGHT_REGISTER_DATA(sight::data::Series)

namespace sight::data
{

// This allows to register private tags in the private dictionary and so to set and get value from them
static const class GDCMLoader final
{
public:

    GDCMLoader()
    {
        // Load the GDCM resource
        auto& gdcm_instance {gdcm::Global::GetInstance()};
        gdcm_instance.LoadResourcesFiles();

        // Get the private dictionary
        auto& dictionaries       = gdcm_instance.GetDicts();
        auto& private_dictionary = dictionaries.GetPrivateDict();

        // Add private tags to the private dictionary
        private_dictionary.AddDictEntry(
            gdcm::PrivateTag(detail::PRIVATE_GROUP, detail::PRIVATE_CREATOR_ELEMENT, detail::PRIVATE_CREATOR.c_str()),
            gdcm::DictEntry("Sight Private Data", "SightPrivateData", gdcm::VR::UT, gdcm::VM::VM1)
        );
    }
} loader;

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

std::chrono::system_clock::time_point Series::dateTimeToTimePoint(const std::string& dateTime)
{
    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html

    std::tm time_info {};

    if(dateTime.length() >= 4)
    {
        time_info.tm_year = std::stoi(dateTime.substr(0, 4)) - 1900;
    }

    if(dateTime.length() >= 6)
    {
        time_info.tm_mon = std::stoi(dateTime.substr(4, 2)) - 1;
    }

    if(dateTime.length() >= 8)
    {
        time_info.tm_mday = std::stoi(dateTime.substr(6, 2));
    }
    else
    {
        time_info.tm_mday = 1;
    }

    if(dateTime.length() >= 10)
    {
        time_info.tm_hour = std::stoi(dateTime.substr(8, 2));
    }

    if(dateTime.length() >= 12)
    {
        time_info.tm_min = std::stoi(dateTime.substr(10, 2));
    }

    if(dateTime.length() >= 14)
    {
        time_info.tm_sec = std::stoi(dateTime.substr(12, 2));
    }

#ifdef _WIN32
    // cspell:ignore mkgmtime
    std::time_t time = _mkgmtime(&time_info);
#else
    std::time_t time = timegm(&time_info);
#endif

    std::chrono::microseconds microseconds {0};

    if(dateTime.length() > 15)
    {
        // Do not forget '.' after the seconds
        auto us = dateTime.substr(15);

        // Fill with trailing 0 to always have microseconds
        us.resize(6, '0');

        microseconds = std::chrono::microseconds(std::stoi(us));
    }

    return std::chrono::system_clock::from_time_t(time) + microseconds;
}

//------------------------------------------------------------------------------

std::string Series::timePointToDateTime(const std::chrono::system_clock::time_point& timePoint)
{
    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html

    // Get the date
    const auto days_time_point = std::chrono::floor<std::chrono::days>(timePoint);
    const std::chrono::year_month_day year_month_day {days_time_point};

    // Get the time
    const auto microseconds_time_point = std::chrono::floor<std::chrono::microseconds>(timePoint - days_time_point);
    const std::chrono::hh_mm_ss hh_mm_ss {microseconds_time_point};

    // Build the string
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << static_cast<int>(year_month_day.year());
    ss << std::setfill('0') << std::setw(2) << static_cast<unsigned>(year_month_day.month());
    ss << std::setfill('0') << std::setw(2) << static_cast<unsigned>(year_month_day.day());
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.hours().count();
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.minutes().count();
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.seconds().count();
    ss << ".";
    ss << std::setfill('0') << std::setw(6) << std::right << hh_mm_ss.subseconds().count();

    return ss.str();
}

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

    m_pimpl->copyFrameDatasets(other->m_pimpl->m_frame_datasets);

    BaseClass::deepCopy(source, cache);
}

//------------------------------------------------------------------------------

void Series::copyPatientModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfPatientID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfPatientIDQualifiersSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientBirthDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSex>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedPatientSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientBirthTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientIDs>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientIDsSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OtherPatientNames>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::EthnicGroup>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientComments>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSpeciesDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSpeciesCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientBreedDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientBreedCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::BreedRegistrationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ResponsiblePerson>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ResponsiblePersonRole>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ResponsibleOrganization>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientIdentityRemoved>(source_dataset, instance);
    // cspell:ignore Deidentification
    m_pimpl->copyElement<gdcm::Keywords::DeidentificationMethod>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DeidentificationMethodCodeSequence>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialSubjectModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSponsorName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSiteID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSiteName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSubjectID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSubjectReadingID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeApprovalNumber>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyGeneralStudyModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::StudyInstanceUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::StudyDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::StudyTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferringPhysicianName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferringPhysicianIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::StudyID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AccessionNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfAccessionNumberSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::StudyDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansOfRecord>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansOfRecordIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::NameOfPhysiciansReadingStudy>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PhysiciansReadingStudyIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RequestingServiceCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedStudySequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ProcedureCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReasonForPerformedProcedureCodeSequence>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyPatientStudyModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::AdmittingDiagnosesDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AdmittingDiagnosesCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientAge>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSize>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientWeight>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSizeCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::Occupation>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AdditionalPatientHistory>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AdmissionID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfAdmissionIDSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ServiceEpisodeID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IssuerOfServiceEpisodeIDSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ServiceEpisodeDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientSexNeutered>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialStudyModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialTimePointID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialTimePointDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ConsentForClinicalTrialUseSequence>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyGeneralSeriesModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::Modality>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesInstanceUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::Laterality>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformingPhysicianName>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PerformingPhysicianIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ProtocolName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SeriesDescriptionCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OperatorsName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OperatorIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedPerformedProcedureStepSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RelatedSeriesSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::BodyPartExamined>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientPosition>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RequestAttributesSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepStartDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepStartTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepEndDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepEndTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProcedureStepDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PerformedProtocolCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::CommentsOnThePerformedProcedureStep>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AnatomicalOrientationType>(source_dataset, instance);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0108), instance); // SmallestImagePixelValue
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0109), instance); // LargestPixelValueInSeries
}

//------------------------------------------------------------------------------

void Series::copyClinicalTrialSeriesModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialCoordinatingCenterName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSeriesID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ClinicalTrialSeriesDescription>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyGeneralEquipmentModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::Manufacturer>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionAddress>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::StationName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstitutionalDepartmentName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ManufacturerModelName>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DeviceSerialNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SoftwareVersions>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::GantryID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SpatialResolution>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DateOfLastCalibration>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::TimeOfLastCalibration>(source_dataset, instance);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copyElement(source_dataset, gdcm::Tag(0x0028, 0x0120), instance); // PixelPaddingValue
}

//------------------------------------------------------------------------------

void Series::copyFrameOfReferenceModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::FrameOfReferenceUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PositionReferenceIndicator>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copySOPCommonModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::SOPClassUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SOPInstanceUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SpecificCharacterSet>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreationDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreationTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstanceCreatorUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RelatedGeneralSOPClassUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OriginalSpecializedSOPClassUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::CodingSchemeIdentificationSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::TimezoneOffsetFromUTC>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ContributingEquipmentSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::InstanceNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SOPInstanceStatus>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SOPAuthorizationDateTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SOPAuthorizationComment>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AuthorizationEquipmentCertificationNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::MACParametersSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DigitalSignaturesSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::EncryptedAttributesSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::OriginalAttributesSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::HL7StructuredDocumentReferenceSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::LongitudinalTemporalInformationModified>(source_dataset, instance);
}

//------------------------------------------------------------------------------

void Series::copyGeneralImageModule(const Series::csptr& source, std::size_t instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->getDataSet(instance);
    m_pimpl->copyElement<gdcm::Keywords::InstanceNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PatientOrientation>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ContentDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ContentTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ImageType>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionNumber>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionDate>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::AcquisitionDateTime>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedImageSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DerivationDescription>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::DerivationCodeSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::SourceImageSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ReferencedInstanceSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ImagesInAcquisition>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::ImageComments>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::QualityControlImage>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::BurnedInAnnotation>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RecognizableVisualFeatures>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompression>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompressionRatio>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::LossyImageCompressionMethod>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IconImageSequence>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::PresentationLUTShape>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::IrradiationEventUID>(source_dataset, instance);
    m_pimpl->copyElement<gdcm::Keywords::RealWorldValueMappingSequence>(source_dataset, instance);
}

/// Recursive DES equality check
inline static bool isEqual(const gdcm::DataSet::DataElementSet& des, const gdcm::DataSet::DataElementSet& other_des)
{
    if(des != other_des)
    {
        return false;
    }

    // Check each element, std::set is ordered so we can use iterators
    for(auto it = des.cbegin(), other_it = other_des.cbegin() ;
        it != des.cend() && other_it != other_des.cend() ;
        ++it, ++other_it)
    {
        if(*it != *other_it)
        {
            return false;
        }

        // Check if the element is a sequence
        const auto& sq       = it->GetValueAsSQ();
        const auto& other_sq = other_it->GetValueAsSQ();

        if((sq.GetPointer() != nullptr && other_sq.GetPointer() == nullptr)
           || (sq.GetPointer() == nullptr && other_sq.GetPointer() != nullptr))
        {
            return false;
        }

        // If both are sequences, check each item recursively
        if(sq.GetPointer() != nullptr && other_sq.GetPointer() != nullptr)
        {
            if(sq->GetNumberOfItems() != other_sq->GetNumberOfItems())
            {
                return false;
            }

            for(gdcm::SequenceOfItems::SizeType i = 1, end = sq->GetNumberOfItems() ; i <= end ; ++i)
            {
                const auto& item       = sq->GetItem(i);
                const auto& other_item = other_sq->GetItem(i);

                if(item != other_item)
                {
                    return false;
                }

                const auto& nested_dataset       = item.GetNestedDataSet();
                const auto& other_nested_dataset = other_item.GetNestedDataSet();

                if(!isEqual(nested_dataset.GetDES(), other_nested_dataset.GetDES()))
                {
                    return false;
                }
            }
        }
    }

    return true;
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
        const auto& des       = m_pimpl->m_frame_datasets[i].first.GetDES();
        const auto& other_des = other.m_pimpl->m_frame_datasets[i].first.GetDES();

        // gdcm::DataSet::DataElementSet is a std::set, so we would use operator==, but unfortunately, it is not
        // implemented correctly and we must do it ourselves
        if(!isEqual(des, other_des))
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

std::string Series::getByteValue(dicom::attribute::Keyword tag, std::size_t instance) const
{
    const auto& attribute = dicom::attribute::get(tag);
    return getByteValue(attribute.m_group, attribute.m_element, instance);
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

void Series::setByteValue(
    dicom::attribute::Keyword tag,
    const std::string& value,
    std::size_t instance
)
{
    const auto& attribute = dicom::attribute::get(tag);
    setByteValue(attribute.m_group, attribute.m_element, value, instance);
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

void Series::setStringValue(dicom::attribute::Keyword tag, const std::string& value, std::size_t instance)
{
    const auto& attribute = dicom::attribute::get(tag);
    setStringValue(attribute.m_group, attribute.m_element, value, instance);
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

std::string Series::getStringValue(dicom::attribute::Keyword tag, std::size_t instance) const
{
    const auto& attribute = dicom::attribute::get(tag);
    return getStringValue(attribute.m_group, attribute.m_element, instance);
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

std::vector<std::string> Series::getByteValues(dicom::attribute::Keyword tag, std::size_t instance) const
{
    const auto& attribute = dicom::attribute::get(tag);
    return getByteValues(attribute.m_group, attribute.m_element, instance);
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

void Series::setByteValues(dicom::attribute::Keyword tag, const std::vector<std::string>& values, std::size_t instance)
{
    const auto& attribute = dicom::attribute::get(tag);
    setByteValues(attribute.m_group, attribute.m_element, values, instance);
}

//------------------------------------------------------------------------------

std::optional<std::string> Series::getPrivateValue(std::uint8_t element, std::size_t instance) const
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", element >= 0x10 && element <= 0xFF);

    // Get the tag
    gdcm::Tag tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + element);

    // Get the dataset
    auto& dataset = m_pimpl->getDataSet(instance);

    // Get the value
    return detail::getPrivateStringValue(dataset, tag);
}

//------------------------------------------------------------------------------

void Series::setPrivateValue(const std::optional<std::string>& value, std::uint8_t element, std::size_t instance)
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", element >= 0x10 && element <= 0xFF);

    // Get the tag
    gdcm::Tag tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + element);

    // Get the dataset
    auto& dataset = m_pimpl->getDataSet(instance);

    // Set the value
    detail::setPrivateValue(dataset, tag, value);
}

//------------------------------------------------------------------------------

std::optional<std::string> Series::getMultiFramePrivateValue(
    std::uint8_t element,
    std::size_t frameIndex
) const
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", element >= 0x10 && element <= 0xFF);

    return m_pimpl->getMultiFramePrivateValue<gdcm::Keywords::PerFrameFunctionalGroupsSequence>(
        element,
        element + 0x01,
        frameIndex
    );
}

//------------------------------------------------------------------------------

void Series::setMultiFramePrivateValue(
    const std::optional<std::string>& value,
    std::uint8_t element,
    std::size_t frameIndex
)
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", element >= 0x10 && element <= 0xFF);

    m_pimpl->setMultiFramePrivateValue<gdcm::Keywords::PerFrameFunctionalGroupsSequence>(
        value,
        element,
        element + 0x01,
        frameIndex
    );
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

bool Series::isMultiFrame() const noexcept
{
    switch(getSOPKeyword())
    {
        // Multi-frame SOP Classes
        // Found using dicom_parser.py --mandatory-tags "(0018,9074)"
        case dicom::sop::Keyword::EnhancedCTImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedCTImageStorage:
        case dicom::sop::Keyword::EnhancedMRImageStorage:
        case dicom::sop::Keyword::EnhancedMRColorImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedMRImageStorage:
        case dicom::sop::Keyword::EnhancedUSVolumeStorage:
        case dicom::sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::EnhancedXAImageStorage:
        case dicom::sop::Keyword::EnhancedXRFImageStorage:
        case dicom::sop::Keyword::XRay3DAngiographicImageStorage:
        case dicom::sop::Keyword::XRay3DCraniofacialImageStorage:
        case dicom::sop::Keyword::BreastTomosynthesisImageStorage:
        case dicom::sop::Keyword::BreastProjectionXRayImageStorageForPresentation:
        case dicom::sop::Keyword::BreastProjectionXRayImageStorageForProcessing:
        case dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation:
        case dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing:
        case dicom::sop::Keyword::ParametricMapStorage:
        case dicom::sop::Keyword::SegmentationStorage:
        case dicom::sop::Keyword::OphthalmicTomographyImageStorage:
        case dicom::sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage:
        case dicom::sop::Keyword::VLWholeSlideMicroscopyImageStorage:
        case dicom::sop::Keyword::EnhancedPETImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedPETImageStorage:
            return true;

        // Single-frame SOP Classes or no SOP Class
        default:
            return false;
    }
}

//------------------------------------------------------------------------------

std::size_t Series::numFrames() const noexcept
{
    if(isMultiFrame())
    {
        const auto& numberOfFrames = m_pimpl->getValue<gdcm::Keywords::NumberOfFrames>();
        return std::size_t(numberOfFrames.value_or(0));
    }

    return numInstances();
}

//------------------------------------------------------------------------------

void Series::shrinkFrames(std::size_t size)
{
    if(isMultiFrame())
    {
        m_pimpl->m_frame_datasets.resize(1);
    }
    else if(size < m_pimpl->m_frame_datasets.size())
    {
        m_pimpl->m_frame_datasets.resize(size);
    }

    m_pimpl->shrinkMultiFrame(size);
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

dicom::sop::Keyword Series::getSOPKeyword() const noexcept
{
    if(const auto& sop_class_uid = getSOPClassUID(); !sop_class_uid.empty())
    {
        try
        {
            const auto& sop_class = dicom::sop::get(sop_class_uid);
            return sop_class.m_keyword;
        }
        catch(...)
        {
            if(!sop_class_uid.empty())
            {
                SIGHT_ERROR("Unable to find SOP class name for SOP class UID '" << sop_class_uid << "'.");
            }
            else
            {
                SIGHT_ERROR("SOP class UID is not set.");
            }
        }
    }

    return dicom::sop::Keyword::INVALID;
}

//------------------------------------------------------------------------------

void Series::setSOPKeyword(dicom::sop::Keyword keyword)
{
    const auto& sop = dicom::sop::get(keyword);

    m_pimpl->setValue<gdcm::Keywords::SOPClassUID>(std::string(sop.m_uid));
}

//------------------------------------------------------------------------------

std::string_view Series::getSOPClassName() const noexcept
{
    const auto& sop_keyword = getSOPKeyword();

    if(sop_keyword != dicom::sop::Keyword::INVALID)
    {
        return dicom::sop::get(sop_keyword).m_name;
    }

    return getClassname();
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

std::string Series::getSOPClassUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SOPClassUID>();
}

//------------------------------------------------------------------------------

void Series::setSOPClassUID(const std::string& sopClassUID)
{
    m_pimpl->setValue<gdcm::Keywords::SOPClassUID>(sopClassUID);
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
    if(const auto& sop_class_uid = getSOPClassUID(); !sop_class_uid.empty())
    {
        if(const DicomType result = getDicomType(sop_class_uid); result != DicomType::UNKNOWN)
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
    // cspell:ignore Multiframe Radiofluoroscopic Tomosynthesis Bscan Dermoscopic
    switch(dicom::sop::keyword(sop_class_uid))
    {
        // Found using dicom_parser.py --mandatory-tags "(0062,0002)" "(0066,0002)" "(0066,0011)"
        case dicom::sop::Keyword::GenericImplantTemplateStorage:
        case dicom::sop::Keyword::SegmentationStorage:
        case dicom::sop::Keyword::SurfaceSegmentationStorage:
        case dicom::sop::Keyword::SurfaceScanMeshStorage:
        case dicom::sop::Keyword::SurfaceScanPointCloudStorage:
            return DicomType::MODEL;

        // Found using dicom_parser.py --mandatory-tags "(7FE0,0010)"
        case dicom::sop::Keyword::ComputedRadiographyImageStorage:
        case dicom::sop::Keyword::DigitalXRayImageStorageForPresentation:
        case dicom::sop::Keyword::DigitalXRayImageStorageForProcessing:
        case dicom::sop::Keyword::DigitalMammographyXRayImageStorageForPresentation:
        case dicom::sop::Keyword::DigitalMammographyXRayImageStorageForProcessing:
        case dicom::sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation:
        case dicom::sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing:
        case dicom::sop::Keyword::CTImageStorage:
        case dicom::sop::Keyword::EnhancedCTImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedCTImageStorage:
        case dicom::sop::Keyword::UltrasoundMultiFrameImageStorage:
        case dicom::sop::Keyword::MRImageStorage:
        case dicom::sop::Keyword::EnhancedMRImageStorage:
        case dicom::sop::Keyword::EnhancedMRColorImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedMRImageStorage:
        case dicom::sop::Keyword::UltrasoundImageStorage:
        case dicom::sop::Keyword::EnhancedUSVolumeStorage:
        case dicom::sop::Keyword::SecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage:
        case dicom::sop::Keyword::XRayAngiographicImageStorage:
        case dicom::sop::Keyword::EnhancedXAImageStorage:
        case dicom::sop::Keyword::XRayRadiofluoroscopicImageStorage:
        case dicom::sop::Keyword::EnhancedXRFImageStorage:
        case dicom::sop::Keyword::XRay3DAngiographicImageStorage:
        case dicom::sop::Keyword::XRay3DCraniofacialImageStorage:
        case dicom::sop::Keyword::BreastTomosynthesisImageStorage:
        case dicom::sop::Keyword::BreastProjectionXRayImageStorageForPresentation:
        case dicom::sop::Keyword::BreastProjectionXRayImageStorageForProcessing:
        case dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation:
        case dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing:
        case dicom::sop::Keyword::NuclearMedicineImageStorage:
        case dicom::sop::Keyword::ParametricMapStorage:
        case dicom::sop::Keyword::VLEndoscopicImageStorage:
        case dicom::sop::Keyword::VideoEndoscopicImageStorage:
        case dicom::sop::Keyword::VLMicroscopicImageStorage:
        case dicom::sop::Keyword::VideoMicroscopicImageStorage:
        case dicom::sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage:
        case dicom::sop::Keyword::VLPhotographicImageStorage:
        case dicom::sop::Keyword::VideoPhotographicImageStorage:
        case dicom::sop::Keyword::OphthalmicPhotography8BitImageStorage:
        case dicom::sop::Keyword::OphthalmicPhotography16BitImageStorage:
        case dicom::sop::Keyword::OphthalmicTomographyImageStorage:
        case dicom::sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage:
        case dicom::sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage:
        case dicom::sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage:
        case dicom::sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage:
        case dicom::sop::Keyword::VLWholeSlideMicroscopyImageStorage:
        case dicom::sop::Keyword::DermoscopicPhotographyImageStorage:
        case dicom::sop::Keyword::OphthalmicThicknessMapStorage:
        case dicom::sop::Keyword::CornealTopographyMapStorage:
        case dicom::sop::Keyword::PositronEmissionTomographyImageStorage:
        case dicom::sop::Keyword::EnhancedPETImageStorage:
        case dicom::sop::Keyword::LegacyConvertedEnhancedPETImageStorage:
        case dicom::sop::Keyword::RTImageStorage:
        case dicom::sop::Keyword::RTDoseStorage:
            return DicomType::IMAGE;

        default:
            return DicomType::UNKNOWN;
    }
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
    m_pimpl->setValue<gdcm::Keywords::RescaleIntercept>(rescaleIntercept, instance);
}

//------------------------------------------------------------------------------

std::optional<double> Series::getRescaleSlope(std::size_t instance) const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::RescaleSlope>(instance);
}

//------------------------------------------------------------------------------

void Series::setRescaleSlope(const std::optional<double>& rescaleSlope, std::size_t instance)
{
    m_pimpl->setValue<gdcm::Keywords::RescaleSlope>(rescaleSlope, instance);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImagePositionPatient(std::size_t instance) const
{
    // Assert if the SOP class is not set
    SIGHT_ASSERT(
        "SOP class is not set, please call Series::setSOPClassUID before calling Series::getImagePositionPatient",
        getSOPKeyword() != dicom::sop::Keyword::INVALID
    );

    if(isMultiFrame())
    {
        // If we deal with Enhanced US Volume, we need to look in:
        // {Multi-frame Functional Groups Module}
        // |
        // <Per-frame Functional Groups Sequence Attribute>
        // |
        // -- frame 1
        //    <Plane Position Sequence Attribute>
        //    |
        //    -- <Image Position (Patient) Attribute>
        //
        // -- frame 2
        // ...
        // <shared Functional Groups Sequence Attribute>
        // | ...
        return m_pimpl->getMultiFrameValues<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlanePositionSequence,
            gdcm::Keywords::ImagePositionPatient
        >(instance);
    }

    // Default case use simple ImagePositionPatient tag values.
    return m_pimpl->getValues<gdcm::Keywords::ImagePositionPatient>(instance);
}

//------------------------------------------------------------------------------

void Series::setImagePositionPatient(const std::vector<double>& imagePositionPatient, std::size_t instance)
{
    // Assert if the SOP class is not set
    SIGHT_ASSERT(
        "SOP class is not set, please call Series::setSOPClassUID before calling Series::setImagePositionPatient",
        getSOPKeyword() != dicom::sop::Keyword::INVALID
    );

    if(isMultiFrame())
    {
        // If we deal with Enhanced US Volume, we need to look in:
        // {Multi-frame Functional Groups Module}
        // |
        // <Per-frame Functional Groups Sequence Attribute>
        // |
        // -- frame 1
        //    <Plane Position Sequence Attribute>
        //    |
        //    -- <Image Position (Patient) Attribute>
        //
        // -- frame 2
        // ...
        // <shared Functional Groups Sequence Attribute>
        // | ...
        m_pimpl->setMultiFrameValues<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlanePositionSequence,
            gdcm::Keywords::ImagePositionPatient
        >(imagePositionPatient, instance);
    }
    else
    {
        m_pimpl->setValues<gdcm::Keywords::ImagePositionPatient>(imagePositionPatient, instance);
    }
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImageOrientationPatient(std::size_t instance) const
{
    SIGHT_ASSERT(
        "SOP class is not set, please call Series::setSOPClassUID before calling Series::getImageOrientationPatient",
        getSOPKeyword() != dicom::sop::Keyword::INVALID
    );

    if(isMultiFrame())
    {
        // If we deal with Enhanced US Volume, we need to look in:
        // {Multi-frame Functional Groups Module}
        // |
        // <Per-frame Functional Groups Sequence Attribute>
        // |
        // -- frame 1
        //    <Plane Orientation Sequence Attribute>
        //    |
        //    -- <Image Orientation (Patient) Attribute>
        //
        // -- frame 2
        // ...
        // <shared Functional Groups Sequence Attribute>
        // | ...
        // Assert if the SOP class is not set
        return m_pimpl->getMultiFrameValues<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlaneOrientationSequence,
            gdcm::Keywords::ImageOrientationPatient
        >(instance);
    }

    return m_pimpl->getValues<gdcm::Keywords::ImageOrientationPatient>(instance);
}

//------------------------------------------------------------------------------

void Series::setImageOrientationPatient(const std::vector<double>& imageOrientationPatient, std::size_t instance)
{
    SIGHT_ASSERT(
        "SOP class is not set, please call Series::setSOPClassUID before calling Series::setImageOrientationPatient",
        getSOPKeyword() != dicom::sop::Keyword::INVALID
    );

    if(isMultiFrame())
    {
        // If we deal with Enhanced US Volume, we need to look in:
        // {Multi-frame Functional Groups Module}
        // |
        // <Per-frame Functional Groups Sequence Attribute>
        // |
        // -- frame 1
        //    <Plane Position Sequence Attribute>
        //    |
        //    -- <Image Position (Patient) Attribute>
        //
        // -- frame 2
        // ...
        // <shared Functional Groups Sequence Attribute>
        // | ...
        m_pimpl->setMultiFrameValues<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlaneOrientationSequence,
            gdcm::Keywords::ImageOrientationPatient
        >(imageOrientationPatient, instance);
    }
    else
    {
        m_pimpl->setValues<gdcm::Keywords::ImageOrientationPatient>(imageOrientationPatient, instance);
    }
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImagePositionVolume(std::size_t frameIndex) const
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Plane Position Volume Sequence Attribute>
    //    |
    //    -- <Image Position (Volume) Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...

    // Try with Per-Frame Functional Groups
    return m_pimpl->getMultiFrameValues<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlanePositionVolumeSequence,
        gdcm::Keywords::ImagePositionVolume
    >(frameIndex);
}

//------------------------------------------------------------------------------

void Series::setImagePositionVolume(const std::vector<double>& imagePositionVolume, std::size_t frameIndex)
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Plane Position Volume Sequence Attribute>
    //    |
    //    -- <Image Position (Volume) Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    m_pimpl->setMultiFrameValues<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlanePositionSequence,
        gdcm::Keywords::ImagePositionVolume
    >(imagePositionVolume, frameIndex);
}

//------------------------------------------------------------------------------

std::vector<double> Series::getImageOrientationVolume(std::size_t frameIndex) const
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Plane Orientation Volume Sequence Attribute>
    //    |
    //    -- <Image Orientation (Volume) Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    return m_pimpl->getMultiFrameValues<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlaneOrientationVolumeSequence,
        gdcm::Keywords::ImageOrientationVolume
    >(frameIndex);
}

//------------------------------------------------------------------------------

void Series::setImageOrientationVolume(const std::vector<double>& imageOrientationVolume, std::size_t frameIndex)
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Plane Orientation Volume Sequence Attribute>
    //    |
    //    -- <Image Orientation (Volume) Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    m_pimpl->setMultiFrameValues<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlaneOrientationSequence,
        gdcm::Keywords::ImageOrientationVolume
    >(imageOrientationVolume, frameIndex);
}

//------------------------------------------------------------------------------

std::optional<std::string> Series::getFrameAcquisitionDateTime(std::size_t frameIndex) const
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Acquisition DateTime Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    if(const auto& value = m_pimpl->getMultiFrameValue<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameAcquisitionDateTime
                           >(frameIndex); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void Series::setFrameAcquisitionDateTime(
    const std::optional<std::string>& frameAcquisitionDateTime,
    std::size_t frameIndex
)
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Acquisition DateTime Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    m_pimpl->setMultiFrameValue<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameAcquisitionDateTime
    >(frameAcquisitionDateTime, frameIndex);
}

//------------------------------------------------------------------------------

std::optional<std::chrono::system_clock::time_point> Series::getFrameAcquisitionTimePoint(
    std::size_t frameIndex
) const noexcept
{
    try
    {
        // Get the acquisition date time.
        const auto& optional = getFrameAcquisitionDateTime(frameIndex);

        // Convert from YYYYMMDDHHMMSS.FFFFFF
        /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
        return dateTimeToTimePoint(*optional);
    }
    catch(...)
    {
        return std::nullopt;
    }
}

//------------------------------------------------------------------------------

void Series::setFrameAcquisitionTimePoint(
    const std::optional<std::chrono::system_clock::time_point>& timePoint,
    std::size_t frameIndex
)
{
    if(timePoint)
    {
        // Convert to YYYYMMDDHHMMSS.FFFFFF
        /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
        const auto& dateTime = timePointToDateTime(*timePoint);
        setFrameAcquisitionDateTime(dateTime, frameIndex);
    }
    else
    {
        setFrameAcquisitionDateTime(std::nullopt, frameIndex);
    }
}

//------------------------------------------------------------------------------

std::optional<std::string> Series::getFrameComments(std::size_t frameIndex) const
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Comments Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    if(const auto& value = m_pimpl->getMultiFrameValue<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameComments
                           >(frameIndex); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void Series::setFrameComments(const std::optional<std::string>& frameComments, std::size_t frameIndex)
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Comments>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    m_pimpl->setMultiFrameValue<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameComments
    >(frameComments, frameIndex);
}

//------------------------------------------------------------------------------

std::optional<std::string> Series::getFrameLabel(std::size_t frameIndex) const
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Label Attribute>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    if(const auto& value = m_pimpl->getMultiFrameValue<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameLabel
                           >(frameIndex); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void Series::setFrameLabel(const std::optional<std::string>& frameLabel, std::size_t frameIndex)
{
    // If we deal with Enhanced US Volume, we need to look in:
    // {Multi-frame Functional Groups Module}
    // |
    // <Per-frame Functional Groups Sequence Attribute>
    // |
    // -- frame 1
    //    <Frame Content Sequence Attribute>
    //    |
    //    -- <Frame Label>
    //
    // -- frame 2
    // ...
    // <shared Functional Groups Sequence Attribute>
    // | ...
    m_pimpl->setMultiFrameValue<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameLabel
    >(frameLabel, frameIndex);
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
    /// @todo
    return m_pimpl->getValue<gdcm::Keywords::SliceThickness>();
}

//------------------------------------------------------------------------------

void Series::setSliceThickness(const std::optional<double>& sliceThickness)
{
    /// @todo
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
