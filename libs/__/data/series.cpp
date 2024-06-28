/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "data/image_series.hpp"
#include "data/model_series.hpp"

#include "data/detail/series_impl.hxx"
#include "data/dicom/sop.hpp"
#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/clock.hpp>
#include <core/compare.hpp>

#include <gdcmDict.h>
#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmPrivateTag.h>
#include <gdcmUIDGenerator.h>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <iomanip>
#include <regex>
#include <sstream>
#include <utility>

SIGHT_REGISTER_DATA(sight::data::series)

namespace sight::data
{

namespace
{

constexpr size_t YEAR_LENGTH        = std::string_view("YYYY").length();
constexpr size_t MONTH_LENGTH       = std::string_view("MM").length();
constexpr size_t YEAR_MONTH_LENGTH  = YEAR_LENGTH + MONTH_LENGTH;
constexpr size_t DAY_LENGTH         = std::string_view("DD").length();
constexpr size_t DATE_LENGTH        = YEAR_MONTH_LENGTH + DAY_LENGTH;
constexpr size_t HOUR_LENGTH        = std::string_view("HH").length();
constexpr size_t MINUTE_LENGTH      = std::string_view("MM").length();
constexpr size_t HOUR_MINUTE_LENGTH = HOUR_LENGTH + MINUTE_LENGTH;
constexpr size_t SECOND_LENGTH      = std::string_view("SS").length();
constexpr size_t TIME_LENGTH        = HOUR_MINUTE_LENGTH + SECOND_LENGTH;

// This allows to register private tags in the private dictionary and so to set and get value from them
class gdcm_loader final
{
public:

    gdcm_loader()
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

        // Create a UID generator with the "programmer" root "2.25
        /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_B.2.html
        m_uid_generator = std::make_unique<gdcm::UIDGenerator>();
        m_uid_generator->SetRoot("2.25");
    }

    //------------------------------------------------------------------------------

    std::string generate_uid()
    {
        std::unique_lock lock(m_mutex);
        return m_uid_generator->Generate();
    }

private:

    std::mutex m_mutex;
    std::unique_ptr<gdcm::UIDGenerator> m_uid_generator;
} gdcm_loader;

/// helper function to get the value of a tag as a string like "(0020,0011)", which can be searched on the internet.
inline std::string tag_to_string(const gdcm::Tag& _tag)
{
    std::stringstream ss;
    ss << "("
    << std::hex << std::setfill('0') << std::setw(4) << _tag.GetGroup() << ","
    << std::hex << std::setfill('0') << std::setw(4) << _tag.GetElement() << ")";

    return ss.str();
}

/// Helper function to parse a path with DICOM tags
inline std::filesystem::path parse_path(
    const sight::data::series* const _series,
    const std::string& _path
)
{
    static const std::regex s_DICOM_REGEX(
        R"([(]\s*(?:0x)?([0123456789abcdefABCDEF]{4})\s*,\s*(?:0x)?([0123456789abcdefABCDEF]{4})\s*[)])"
    );

    // Copy the source string
    std::string path(_path);

    for(std::smatch dicom_match ; std::regex_search(path, dicom_match, s_DICOM_REGEX) ; )
    {
        // Parse the group and element
        const auto group   = uint16_t(std::stoi(dicom_match.str(1), nullptr, 16));
        const auto element = uint16_t(std::stoi(dicom_match.str(2), nullptr, 16));

        // Find the value of the corresponding DICOM tag
        auto dicom_value = _series->get_string_value(group, element);

        // Get the attribute
        const auto& attribute = sight::data::dicom::attribute::get(group, element);

        // If the Value Representation is UI, we will shorten it using hash
        if(attribute.m_vr == sight::data::dicom::attribute::VR::UI)
        {
            std::ostringstream hex;
            hex << std::hex << std::hash<std::string> {}(dicom_value);

            dicom_value = hex.str();
        }

        if(dicom_value.empty())
        {
            // This is helpful for debugging
            const std::string message = "DICOM attribute " + std::string(attribute.m_name)
                                        + " has not been found or is empty.";

            SIGHT_ASSERT(message, false);
            SIGHT_ERROR(message);

            dicom_value = "[NA]";
        }

        // Replace the match with the DICOM value
        path = dicom_match.format("$`" + dicom_value + "$'");
    }

    // Filter forbidden characters for paths (Windows and Linux, even if linux allows almost anything)
    std::replace_if(
        path.begin(),
        path.end(),
        [](char _c)
            {
                static constexpr std::string_view s_FORBIDDEN(":?\"<>|$;* \t\n\r");
                return std::isprint(_c) == 0 || s_FORBIDDEN.find(_c) != std::string::npos;
            },
        '_'
    );

    return path;
}

} // namespace

series::series() :
    m_pimpl(std::make_unique<detail::series_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
series::~series() noexcept = default;

//------------------------------------------------------------------------------

void series::new_sop_instance()
{
    // Set series date, time and uid
    set_sop_instance_uid(gdcm_loader.generate_uid());

    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    const auto& date_time = series::time_point_to_date_time(std::chrono::system_clock::now());
    set_instance_creation_date(date_time.substr(0, DATE_LENGTH));
    set_instance_creation_time(date_time.substr(DATE_LENGTH));
}

//------------------------------------------------------------------------------

void series::new_study_instance()
{
    // Set series date, time and uid
    set_study_instance_uid(gdcm_loader.generate_uid());

    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    const auto& date_time = series::time_point_to_date_time(std::chrono::system_clock::now());
    set_study_date(date_time.substr(0, DATE_LENGTH));
    set_study_time(date_time.substr(DATE_LENGTH));
}

//------------------------------------------------------------------------------

void series::new_series_instance()
{
    // Set series date, time and uid
    set_series_instance_uid(gdcm_loader.generate_uid());

    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    const auto& date_time = series::time_point_to_date_time(std::chrono::system_clock::now());
    set_series_date(date_time.substr(0, DATE_LENGTH));
    set_series_time(date_time.substr(DATE_LENGTH));
}

//------------------------------------------------------------------------------

std::chrono::system_clock::time_point series::date_time_to_time_point(const std::string& _date_time)
{
    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html

    std::tm time_info {};

    if(_date_time.length() >= YEAR_LENGTH)
    {
        time_info.tm_year = std::stoi(_date_time.substr(0, YEAR_LENGTH)) - 1900;
    }

    if(_date_time.length() >= YEAR_MONTH_LENGTH)
    {
        time_info.tm_mon = std::stoi(_date_time.substr(YEAR_LENGTH, MONTH_LENGTH)) - 1;
    }

    if(_date_time.length() >= DATE_LENGTH)
    {
        time_info.tm_mday = std::stoi(_date_time.substr(YEAR_MONTH_LENGTH, DAY_LENGTH));
    }
    else
    {
        time_info.tm_mday = 1;
    }

    if(_date_time.length() >= (DATE_LENGTH + HOUR_LENGTH))
    {
        time_info.tm_hour = std::stoi(_date_time.substr(DATE_LENGTH, HOUR_LENGTH));
    }

    if(_date_time.length() >= (DATE_LENGTH + HOUR_MINUTE_LENGTH))
    {
        time_info.tm_min = std::stoi(_date_time.substr((DATE_LENGTH + HOUR_LENGTH), MINUTE_LENGTH));
    }

    if(_date_time.length() >= (DATE_LENGTH + TIME_LENGTH))
    {
        time_info.tm_sec = std::stoi(_date_time.substr(DATE_LENGTH + HOUR_MINUTE_LENGTH, SECOND_LENGTH));
    }

#ifdef _WIN32
    // cspell:ignore mkgmtime
    std::time_t time = _mkgmtime(&time_info);
#else
    std::time_t time = timegm(&time_info);
#endif

    std::chrono::microseconds microseconds {0};

    if(_date_time.length() > (DATE_LENGTH + TIME_LENGTH + 1))
    {
        // Do not forget '.' after the seconds
        auto us = _date_time.substr((DATE_LENGTH + TIME_LENGTH + 1));

        // Fill with trailing 0 to always have microseconds
        us.resize(6, '0');

        microseconds = std::chrono::microseconds(std::stoi(us));
    }

    return std::chrono::system_clock::from_time_t(time) + microseconds;
}

//------------------------------------------------------------------------------

std::string series::time_point_to_date_time(const std::chrono::system_clock::time_point& _time_point)
{
    // DICOM date time format is YYYYMMDDHHMMSS.FFFFFF, but may include null components
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html

    // Get the date
    const auto days_time_point = std::chrono::floor<std::chrono::days>(_time_point);
    const std::chrono::year_month_day year_month_day {days_time_point};

    // Get the time
    const auto microseconds_time_point = std::chrono::floor<std::chrono::microseconds>(_time_point - days_time_point);
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

std::string series::date_to_iso(const std::string& _date)
{
    // DICOM date format is YYYYMMDD, but may include null components
    // ISO 8601 is YYYY-MM-DD
    /// @see @link https://www.iso.org/fr/iso-8601-date-and-time-format.html
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    std::string date;

    // Year
    if(_date.length() >= YEAR_LENGTH)
    {
        date += _date.substr(0, YEAR_LENGTH);
    }
    else
    {
        date += "1900";
    }

    // Month
    if(_date.length() >= YEAR_MONTH_LENGTH)
    {
        date += "-" + _date.substr(YEAR_LENGTH, MONTH_LENGTH);
    }
    else
    {
        date += "-01";
    }

    // Day
    if(_date.length() >= (DATE_LENGTH))
    {
        date += "-" + _date.substr(YEAR_MONTH_LENGTH, DAY_LENGTH);
    }
    else
    {
        date += "-01";
    }

    return date;
}

//------------------------------------------------------------------------------

std::string series::time_to_iso(const std::string& _time)
{
    // DICOM time format is HHMMSS.FFFFFF, but may include null components
    // ISO 8601 is HH:MM:SS.FFF
    /// @see @link https://www.iso.org/fr/iso-8601-date-and-time-format.html
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    std::string time;

    // Hours
    if(_time.length() >= HOUR_LENGTH)
    {
        time += _time.substr(0, HOUR_LENGTH);
    }
    else
    {
        time += "00";
    }

    // Minutes
    if(_time.length() >= HOUR_MINUTE_LENGTH)
    {
        time += ":" + _time.substr(HOUR_LENGTH, MINUTE_LENGTH);
    }
    else
    {
        time += ":00";
    }

    // Seconds
    if(_time.length() >= TIME_LENGTH)
    {
        time += ":" + _time.substr(HOUR_MINUTE_LENGTH, SECOND_LENGTH);
    }
    else
    {
        time += ":00";
    }

    // Milliseconds
    // HHMMSS.yyyyyy is legit, but also HHMMSS.y -> (+1 for `.`, +1 for `y`)
    if(_time.length() >= (TIME_LENGTH + 1 + 1))
    {
        // ISO only allows 3 decimals (millisecond)
        time += _time.substr(6, 4);
    }
    else
    {
        time += ".000";
    }

    return time;
}

//------------------------------------------------------------------------------

void series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !bool(other)
    );

    m_pimpl->m_frame_datasets = other->m_pimpl->m_frame_datasets;

    base_class_t::shallow_copy(_source);
}

//------------------------------------------------------------------------------

void series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !bool(other)
    );

    m_pimpl->copy_frame_datasets(other->m_pimpl->m_frame_datasets);

    base_class_t::deep_copy(_source, _cache);
}

//------------------------------------------------------------------------------

void series::copy_patient_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IssuerOfPatientID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IssuerOfPatientIDQualifiersSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientBirthDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSex>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferencedPatientSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientBirthTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OtherPatientIDs>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OtherPatientIDsSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OtherPatientNames>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::EthnicGroup>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientComments>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSpeciesDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSpeciesCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientBreedDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientBreedCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::BreedRegistrationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ResponsiblePerson>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ResponsiblePersonRole>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ResponsibleOrganization>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientIdentityRemoved>(source_dataset, _instance);
    // cspell:ignore Deidentification
    m_pimpl->copy_element<gdcm::Keywords::DeidentificationMethod>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DeidentificationMethodCodeSequence>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_clinical_trial_subject_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSponsorName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialProtocolID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialProtocolName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSiteID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSiteName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSubjectID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSubjectReadingID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialProtocolEthicsCommitteeApprovalNumber>(
        source_dataset,
        _instance
    );
}

//------------------------------------------------------------------------------

void series::copy_general_study_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::StudyInstanceUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::StudyDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::StudyTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferringPhysicianName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferringPhysicianIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::StudyID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AccessionNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IssuerOfAccessionNumberSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::StudyDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PhysiciansOfRecord>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PhysiciansOfRecordIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::NameOfPhysiciansReadingStudy>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PhysiciansReadingStudyIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RequestingServiceCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferencedStudySequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ProcedureCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReasonForPerformedProcedureCodeSequence>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_patient_study_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::AdmittingDiagnosesDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AdmittingDiagnosesCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientAge>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSize>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientWeight>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSizeCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::Occupation>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AdditionalPatientHistory>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AdmissionID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IssuerOfAdmissionIDSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ServiceEpisodeID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IssuerOfServiceEpisodeIDSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ServiceEpisodeDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientSexNeutered>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_clinical_trial_study_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialTimePointID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialTimePointDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ConsentForClinicalTrialUseSequence>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_general_series_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::Modality>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesInstanceUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::Laterality>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformingPhysicianName>(source_dataset);
    m_pimpl->copy_element<gdcm::Keywords::PerformingPhysicianIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ProtocolName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SeriesDescriptionCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OperatorsName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OperatorIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferencedPerformedProcedureStepSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RelatedSeriesSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::BodyPartExamined>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientPosition>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RequestAttributesSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepStartDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepStartTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepEndDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepEndTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProcedureStepDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PerformedProtocolCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::CommentsOnThePerformedProcedureStep>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AnatomicalOrientationType>(source_dataset, _instance);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copy_element(source_dataset, gdcm::Tag(0x0028, 0x0108), _instance); // SmallestImagePixelValue
    m_pimpl->copy_element(source_dataset, gdcm::Tag(0x0028, 0x0109), _instance); // LargestPixelValueInSeries
}

//------------------------------------------------------------------------------

void series::copy_clinical_trial_series_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialCoordinatingCenterName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSeriesID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ClinicalTrialSeriesDescription>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_general_equipment_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::Manufacturer>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstitutionName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstitutionAddress>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::StationName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstitutionalDepartmentName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ManufacturerModelName>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DeviceSerialNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SoftwareVersions>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::GantryID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SpatialResolution>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DateOfLastCalibration>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::TimeOfLastCalibration>(source_dataset, _instance);

    // GDCM assert for this, when using gdcm::Keywords...
    m_pimpl->copy_element(source_dataset, gdcm::Tag(0x0028, 0x0120), _instance); // PixelPaddingValue
}

//------------------------------------------------------------------------------

void series::copy_frame_of_reference_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::FrameOfReferenceUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PositionReferenceIndicator>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_sop_common_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::SOPClassUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SOPInstanceUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SpecificCharacterSet>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstanceCreationDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstanceCreationTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstanceCreatorUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RelatedGeneralSOPClassUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OriginalSpecializedSOPClassUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::CodingSchemeIdentificationSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::TimezoneOffsetFromUTC>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ContributingEquipmentSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::InstanceNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SOPInstanceStatus>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SOPAuthorizationDateTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SOPAuthorizationComment>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AuthorizationEquipmentCertificationNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::MACParametersSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DigitalSignaturesSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::EncryptedAttributesSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::OriginalAttributesSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::HL7StructuredDocumentReferenceSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::LongitudinalTemporalInformationModified>(source_dataset, _instance);
}

//------------------------------------------------------------------------------

void series::copy_general_image_module(const series::csptr& _source, std::size_t _instance)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = _source->get_data_set(_instance);
    m_pimpl->copy_element<gdcm::Keywords::InstanceNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PatientOrientation>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ContentDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ContentTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ImageType>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AcquisitionNumber>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AcquisitionDate>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AcquisitionTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::AcquisitionDateTime>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferencedImageSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DerivationDescription>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::DerivationCodeSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::SourceImageSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ReferencedInstanceSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ImagesInAcquisition>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::ImageComments>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::QualityControlImage>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::BurnedInAnnotation>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RecognizableVisualFeatures>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::LossyImageCompression>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::LossyImageCompressionRatio>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::LossyImageCompressionMethod>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IconImageSequence>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::PresentationLUTShape>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::IrradiationEventUID>(source_dataset, _instance);
    m_pimpl->copy_element<gdcm::Keywords::RealWorldValueMappingSequence>(source_dataset, _instance);
}

/// Recursive DES equality check
inline static bool is_equal(const gdcm::DataSet::DataElementSet& _des, const gdcm::DataSet::DataElementSet& _other_des)
{
    if(_des != _other_des)
    {
        return false;
    }

    // Check each element, std::set is ordered so we can use iterators
    for(auto it = _des.cbegin(), other_it = _other_des.cbegin() ;
        it != _des.cend() && other_it != _other_des.cend() ;
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

                if(!is_equal(nested_dataset.GetDES(), other_nested_dataset.GetDES()))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool series::operator==(const series& _other) const noexcept
{
    // Check number of frames
    if(m_pimpl->m_frame_datasets.size() != _other.m_pimpl->m_frame_datasets.size())
    {
        return false;
    }

    // Check frames specific attributes
    for(std::size_t i = 0, end = m_pimpl->m_frame_datasets.size() ; i < end ; ++i)
    {
        const auto& des       = m_pimpl->m_frame_datasets[i].first.GetDES();
        const auto& other_des = _other.m_pimpl->m_frame_datasets[i].first.GetDES();

        // gdcm::DataSet::DataElementSet is a std::set, so we would use operator==, but unfortunately, it is not
        // implemented correctly and we must do it ourselves
        if(!is_equal(des, other_des))
        {
            return false;
        }
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool series::operator!=(const series& _other) const noexcept
{
    return !(*this == _other);
}

//-----------------------------------------------------------------------------

std::string series::get_byte_value(std::uint16_t _group, std::uint16_t _element, std::size_t _instance) const
{
    const auto& dataset = m_pimpl->get_data_set(_instance);
    gdcm::Tag tag(_group, _element);

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

std::string series::get_byte_value(dicom::attribute::Keyword _tag, std::size_t _instance) const
{
    const auto& attribute = dicom::attribute::get(_tag);
    return get_byte_value(attribute.m_group, attribute.m_element, _instance);
}

//------------------------------------------------------------------------------

void series::set_byte_value(
    std::uint16_t _group,
    std::uint16_t _element,
    const std::string& _value,
    std::size_t _instance
)
{
    // Get the VR
    const gdcm::Tag tag(_group, _element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Get the padding char.
    const auto [size, fixed, padding] = detail::get_vr_format(vr);

    const auto& padded =
        [&](char _padding_char)
        {
            if((_value.size() % 2) != 0)
            {
                std::string padded_value(_value);
                padded_value.push_back(_padding_char);
                return padded_value;
            }

            return _value;
        }(padding);

    // Create a new data element and assign the buffer from the string
    gdcm::DataElement data_element(tag);
    data_element.SetVR(vr);
    data_element.SetByteValue(padded.c_str(), std::uint32_t(padded.size()));

    // Store back the data element to the data set
    m_pimpl->get_data_set(_instance).Replace(data_element);
}

//------------------------------------------------------------------------------

void series::set_byte_value(
    dicom::attribute::Keyword _tag,
    const std::string& _value,
    std::size_t _instance
)
{
    const auto& attribute = dicom::attribute::get(_tag);
    set_byte_value(attribute.m_group, attribute.m_element, _value, _instance);
}

//------------------------------------------------------------------------------

void series::set_string_value(
    std::uint16_t _group,
    std::uint16_t _element,
    const std::string& _value,
    std::size_t _instance
)
{
    // cspell:ignore stoull
    // Get the VR
    const gdcm::Tag tag(_group, _element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Nothing to do if the VR is an ASCII one
    if(gdcm::VR::IsASCII(vr))
    {
        set_byte_value(_group, _element, _value, _instance);
    }
    else if(vr == gdcm::VR::FL)
    {
        m_pimpl->set_arithmetic_value(tag, vr, std::stof(_value), _instance);
    }
    else if(vr == gdcm::VR::FD)
    {
        m_pimpl->set_arithmetic_value(tag, vr, std::stod(_value), _instance);
    }
    else if(vr == gdcm::VR::US)
    {
        m_pimpl->set_arithmetic_value(tag, vr, static_cast<uint16_t>(std::stoul(_value)), _instance);
    }
    else if(vr == gdcm::VR::UL)
    {
        m_pimpl->set_arithmetic_value(tag, vr, static_cast<uint32_t>(std::stoul(_value)), _instance);
    }
    else if(vr == gdcm::VR::UV)
    {
        m_pimpl->set_arithmetic_value(tag, vr, std::stoull(_value), _instance);
    }
    else if(vr == gdcm::VR::SS)
    {
        m_pimpl->set_arithmetic_value(tag, vr, static_cast<int16_t>(std::stol(_value)), _instance);
    }
    else if(vr == gdcm::VR::SL)
    {
        m_pimpl->set_arithmetic_value(tag, vr, static_cast<int32_t>(std::stol(_value)), _instance);
    }
    else if(vr == gdcm::VR::SV)
    {
        // cspell:ignore stoll
        m_pimpl->set_arithmetic_value(tag, vr, std::stoll(_value), _instance);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(exception("The value at " + tag_to_string(tag) + " cannot be converted from a string."));
    }
}

//------------------------------------------------------------------------------

void series::set_string_value(dicom::attribute::Keyword _tag, const std::string& _value, std::size_t _instance)
{
    const auto& attribute = dicom::attribute::get(_tag);
    set_string_value(attribute.m_group, attribute.m_element, _value, _instance);
}

//------------------------------------------------------------------------------

std::string series::get_string_value(std::uint16_t _group, std::uint16_t _element, std::size_t _instance) const
{
    // Get the VR
    const gdcm::Tag tag(_group, _element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Nothing to do if the VR is an ASCII one
    if(gdcm::VR::IsASCII(vr))
    {
        return get_byte_value(_group, _element, _instance);
    }

    if(vr == gdcm::VR::FL)
    {
        return m_pimpl->get_arithmetic_value<float>(tag, _instance);
    }

    if(vr == gdcm::VR::FD)
    {
        return m_pimpl->get_arithmetic_value<double>(tag, _instance);
    }

    if(vr == gdcm::VR::US)
    {
        return m_pimpl->get_arithmetic_value<std::uint16_t>(tag, _instance);
    }

    if(vr == gdcm::VR::UL)
    {
        return m_pimpl->get_arithmetic_value<std::uint32_t>(tag, _instance);
    }

    if(vr == gdcm::VR::UV)
    {
        return m_pimpl->get_arithmetic_value<std::uint64_t>(tag, _instance);
    }

    if(vr == gdcm::VR::SS)
    {
        return m_pimpl->get_arithmetic_value<std::int16_t>(tag, _instance);
    }

    if(vr == gdcm::VR::SL)
    {
        return m_pimpl->get_arithmetic_value<std::int32_t>(tag, _instance);
    }

    if(vr == gdcm::VR::SV)
    {
        return m_pimpl->get_arithmetic_value<std::int64_t>(tag, _instance);
    }

    SIGHT_THROW_EXCEPTION(exception("The value at " + tag_to_string(tag) + " cannot be converted to a string."));
}

//------------------------------------------------------------------------------

std::string series::get_string_value(dicom::attribute::Keyword _tag, std::size_t _instance) const
{
    const auto& attribute = dicom::attribute::get(_tag);
    return get_string_value(attribute.m_group, attribute.m_element, _instance);
}

//------------------------------------------------------------------------------

std::vector<std::string> series::get_byte_values(
    std::uint16_t _group,
    std::uint16_t _element,
    std::size_t _instance
) const
{
    std::vector<std::string> values;

    const std::string joined = get_byte_value(_group, _element, _instance);
    boost::split(values, joined, boost::is_any_of(detail::BACKSLASH_SEPARATOR));

    return values;
}

//------------------------------------------------------------------------------

std::vector<std::string> series::get_byte_values(dicom::attribute::Keyword _tag, std::size_t _instance) const
{
    const auto& attribute = dicom::attribute::get(_tag);
    return get_byte_values(attribute.m_group, attribute.m_element, _instance);
}

//------------------------------------------------------------------------------

void series::set_byte_values(
    std::uint16_t _group,
    std::uint16_t _element,
    const std::vector<std::string>& _values,
    std::size_t _instance
)
{
    const auto& joined = boost::join(_values, detail::BACKSLASH_SEPARATOR);
    set_byte_value(_group, _element, joined, _instance);
}

//------------------------------------------------------------------------------

void series::set_byte_values(
    dicom::attribute::Keyword _tag,
    const std::vector<std::string>& _values,
    std::size_t _instance
)
{
    const auto& attribute = dicom::attribute::get(_tag);
    set_byte_values(attribute.m_group, attribute.m_element, _values, _instance);
}

//------------------------------------------------------------------------------

std::optional<std::string> series::get_private_value(std::uint8_t _element, std::size_t _instance) const
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

    // Get the tag
    gdcm::Tag tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + _element);

    // Get the dataset
    auto& dataset = m_pimpl->get_data_set(_instance);

    // Get the value
    return detail::get_private_string_value(dataset, tag);
}

//------------------------------------------------------------------------------

void series::set_private_value(const std::optional<std::string>& _value, std::uint8_t _element, std::size_t _instance)
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

    // Get the tag
    gdcm::Tag tag(detail::PRIVATE_GROUP, detail::PRIVATE_DATA_ELEMENT + _element);

    // Get the dataset
    auto& dataset = m_pimpl->get_data_set(_instance);

    // Set the value
    detail::set_private_value(dataset, tag, _value);
}

//------------------------------------------------------------------------------

std::optional<std::string> series::get_multi_frame_private_value(
    std::uint8_t _element,
    std::size_t _frame_index
) const
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

    return m_pimpl->get_multi_frame_private_value<gdcm::Keywords::PerFrameFunctionalGroupsSequence>(
        _element,
        _element + 0x01,
        _frame_index
    );
}

//------------------------------------------------------------------------------

void series::set_multi_frame_private_value(
    const std::optional<std::string>& _value,
    std::uint8_t _element,
    std::size_t _frame_index
)
{
    SIGHT_ASSERT("The private element must be between 0x10 and 0xFF.", _element >= 0x10 && _element <= 0xFF);

    m_pimpl->set_multi_frame_private_value<gdcm::Keywords::PerFrameFunctionalGroupsSequence>(
        _value,
        _element,
        _element + 0x01,
        _frame_index
    );
}

//------------------------------------------------------------------------------

const gdcm::DataSet& series::get_data_set(std::size_t _instance) const
{
    return m_pimpl->get_data_set(_instance);
}

//------------------------------------------------------------------------------

gdcm::DataSet& series::get_data_set(std::size_t _instance)
{
    return m_pimpl->get_data_set(_instance);
}

//------------------------------------------------------------------------------

void series::set_data_set(const gdcm::DataSet& _dataset, std::size_t _instance)
{
    m_pimpl->get_data_set(_instance) = _dataset;
}

//------------------------------------------------------------------------------

std::filesystem::path series::get_file(std::size_t _instance) const
{
    return m_pimpl->get_file(_instance);
}

//------------------------------------------------------------------------------

void series::set_file(const std::filesystem::path& _file, std::size_t _instance)
{
    m_pimpl->get_file(_instance) = _file;
}

//------------------------------------------------------------------------------

std::size_t series::num_instances() const noexcept
{
    return m_pimpl->m_frame_datasets.size();
}

//------------------------------------------------------------------------------

bool series::is_multi_frame() const noexcept
{
    switch(get_sop_keyword())
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

std::size_t series::num_frames() const noexcept
{
    if(is_multi_frame())
    {
        const auto& number_of_frames = m_pimpl->get_value<gdcm::Keywords::NumberOfFrames>();
        return std::size_t(number_of_frames.value_or(0));
    }

    return num_instances();
}

//------------------------------------------------------------------------------

void series::shrink_frames(std::size_t _size)
{
    if(is_multi_frame())
    {
        m_pimpl->m_frame_datasets.resize(1);
    }
    else if(_size < m_pimpl->m_frame_datasets.size())
    {
        m_pimpl->m_frame_datasets.resize(_size);
    }

    m_pimpl->shrink_multi_frame(_size);
}

//------------------------------------------------------------------------------

bool series::sort(const std::vector<std::size_t>& _sorted)
{
    // Some checks to be sure everything is fine
    // If almost no frame are kept, we may want to try to sort the frames differently.
    if(_sorted.size() < 2)
    {
        return false;
    }

    SIGHT_WARN_IF(
        "Some duplicated frames have been dropped during the sorting process.",
        _sorted.size() != m_pimpl->m_frame_datasets.size()
    );

    // Finally, we can sort the frames in the series
    detail::frame_datasets sorted_frame_datasets;

    for(const auto& from : _sorted)
    {
        sorted_frame_datasets.push_back(std::move(m_pimpl->m_frame_datasets[from]));
    }

    m_pimpl->m_frame_datasets = std::move(sorted_frame_datasets);

    return true;
}

//------------------------------------------------------------------------------

dicom::sop::Keyword series::get_sop_keyword() const noexcept
{
    if(const auto& sop_class_uid = get_sop_class_uid(); !sop_class_uid.empty())
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

void series::set_sop_keyword(dicom::sop::Keyword _keyword)
{
    const auto& sop = dicom::sop::get(_keyword);

    m_pimpl->set_value<gdcm::Keywords::SOPClassUID>(std::string(sop.m_uid));
}

//------------------------------------------------------------------------------

std::string_view series::get_sop_class_name() const noexcept
{
    const auto& sop_keyword = get_sop_keyword();

    if(sop_keyword != dicom::sop::Keyword::INVALID)
    {
        return dicom::sop::get(sop_keyword).m_name;
    }

    return get_classname();
}

//------------------------------------------------------------------------------

std::string series::get_sop_instance_uid() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SOPInstanceUID>();
}

//------------------------------------------------------------------------------

void series::set_sop_instance_uid(const std::string& _sop_instance_uid)
{
    m_pimpl->set_value<gdcm::Keywords::SOPInstanceUID>(_sop_instance_uid);
}

//------------------------------------------------------------------------------

std::string series::get_sop_class_uid() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SOPClassUID>();
}

//------------------------------------------------------------------------------

void series::set_sop_class_uid(const std::string& _sop_class_uid)
{
    m_pimpl->set_value<gdcm::Keywords::SOPClassUID>(_sop_class_uid);
}

//------------------------------------------------------------------------------

std::string series::get_specific_character_set() const noexcept
{
    return m_pimpl->get_joined_values<gdcm::Keywords::SpecificCharacterSet>();
}

//------------------------------------------------------------------------------

void series::set_specific_character_set(const std::string& _specific_character_set)
{
    m_pimpl->set_joined_values<gdcm::Keywords::SpecificCharacterSet>(_specific_character_set);
}

//------------------------------------------------------------------------------

std::string series::get_encoding() const noexcept
{
    const auto& specific_character_set = get_specific_character_set();

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

//------------------------------------------------------------------------------

std::string series::get_instance_creation_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::InstanceCreationDate>();
}

//------------------------------------------------------------------------------

void series::set_instance_creation_date(const std::string& _instance_creation_date)
{
    m_pimpl->set_value<gdcm::Keywords::InstanceCreationDate>(_instance_creation_date);
}

//------------------------------------------------------------------------------

std::string series::get_instance_creation_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::InstanceCreationTime>();
}

//------------------------------------------------------------------------------

void series::set_instance_creation_time(const std::string& _instance_creation_time)
{
    m_pimpl->set_value<gdcm::Keywords::InstanceCreationTime>(_instance_creation_time);
}

//-----------------------------------------------------------------------------

std::string series::get_series_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SeriesDate>();
}

//-----------------------------------------------------------------------------

void series::set_series_date(const std::string& _series_date)
{
    m_pimpl->set_value<gdcm::Keywords::SeriesDate>(_series_date);
}

//-----------------------------------------------------------------------------

std::string series::get_series_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SeriesTime>();
}

//-----------------------------------------------------------------------------

void series::set_series_time(const std::string& _series_time)
{
    m_pimpl->set_value<gdcm::Keywords::SeriesTime>(_series_time);
}

//-----------------------------------------------------------------------------

std::string series::get_modality() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::Modality>();
}

//-----------------------------------------------------------------------------

void series::set_modality(const std::string& _modality)
{
    m_pimpl->set_value<gdcm::Keywords::Modality>(_modality);
}

//-----------------------------------------------------------------------------

std::string series::get_series_description() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SeriesDescription>();
}

//-----------------------------------------------------------------------------

void series::set_series_description(const std::string& _series_description)
{
    m_pimpl->set_value<gdcm::Keywords::SeriesDescription>(_series_description);
}

//------------------------------------------------------------------------------

std::vector<std::string> series::get_performing_physician_names() const noexcept
{
    return m_pimpl->get_string_values<gdcm::Keywords::PerformingPhysicianName>();
}

//------------------------------------------------------------------------------

void series::set_performing_physician_names(const std::vector<std::string>& _performing_physician_name)
{
    m_pimpl->set_string_values<gdcm::Keywords::PerformingPhysicianName>(_performing_physician_name);
}

//------------------------------------------------------------------------------

std::string series::get_performing_physician_name() const noexcept
{
    return m_pimpl->get_joined_values<gdcm::Keywords::PerformingPhysicianName>();
}

//------------------------------------------------------------------------------

void series::set_performing_physician_name(const std::string& _performing_physician_name)
{
    return m_pimpl->set_joined_values<gdcm::Keywords::PerformingPhysicianName>(_performing_physician_name);
}

//------------------------------------------------------------------------------

std::string series::get_anatomical_orientation_type() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::AnatomicalOrientationType>();
}

//------------------------------------------------------------------------------

void series::set_anatomical_orientation_type(const std::string& _anatomical_orientation_type)
{
    m_pimpl->set_value<gdcm::Keywords::AnatomicalOrientationType>(_anatomical_orientation_type);
}

//------------------------------------------------------------------------------

std::string series::get_body_part_examined() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::BodyPartExamined>();
}

//------------------------------------------------------------------------------

void series::set_body_part_examined(const std::string& _body_part_examined)
{
    m_pimpl->set_value<gdcm::Keywords::BodyPartExamined>(_body_part_examined);
}

//------------------------------------------------------------------------------

std::string series::get_protocol_name() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ProtocolName>();
}

//------------------------------------------------------------------------------

void series::set_protocol_name(const std::string& _protocol_name)
{
    m_pimpl->set_value<gdcm::Keywords::ProtocolName>(_protocol_name);
}

//------------------------------------------------------------------------------

std::string series::get_patient_position() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientPosition>();
}

//------------------------------------------------------------------------------

std::string series::get_patient_position_string() const noexcept
{
    const auto& patient_position = get_patient_position();

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

void series::set_patient_position(const std::string& _patient_position)
{
    m_pimpl->set_value<gdcm::Keywords::PatientPosition>(_patient_position);
}

//------------------------------------------------------------------------------

std::string series::get_series_instance_uid() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::SeriesInstanceUID>();
}

//-----------------------------------------------------------------------------

void series::set_series_instance_uid(const std::string& _series_instance_uid)
{
    m_pimpl->set_value<gdcm::Keywords::SeriesInstanceUID>(_series_instance_uid);
}

//-----------------------------------------------------------------------------

std::optional<std::int32_t> series::get_series_number() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::SeriesNumber>();
}

//-----------------------------------------------------------------------------

void series::set_series_number(const std::optional<std::int32_t>& _series_number)
{
    m_pimpl->set_value<gdcm::Keywords::SeriesNumber>(_series_number);
}

//-----------------------------------------------------------------------------

std::string series::get_laterality() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::Laterality>();
}

//-----------------------------------------------------------------------------

void series::set_laterality(const std::string& _laterality)
{
    m_pimpl->set_value<gdcm::Keywords::Laterality>(_laterality);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_start_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepStartDate>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_start_date(const std::string& _performed_procedure_step_start_date)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepStartDate>(_performed_procedure_step_start_date);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_start_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepStartTime>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_start_time(const std::string& _performed_procedure_step_start_time)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepStartTime>(_performed_procedure_step_start_time);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_end_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepEndDate>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_end_date(const std::string& _performed_procedure_step_end_date)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepEndDate>(_performed_procedure_step_end_date);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_end_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepEndTime>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_end_time(const std::string& _performed_procedure_step_end_time)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepEndTime>(_performed_procedure_step_end_time);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_id() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepID>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_id(const std::string& _performed_procedure_step_id)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepID>(_performed_procedure_step_id);
}

//------------------------------------------------------------------------------

std::string series::get_performed_procedure_step_description() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PerformedProcedureStepDescription>();
}

//------------------------------------------------------------------------------

void series::set_performed_procedure_step_description(const std::string& _performed_procedure_step_description)
{
    m_pimpl->set_value<gdcm::Keywords::PerformedProcedureStepDescription>(_performed_procedure_step_description);
}

//------------------------------------------------------------------------------

std::string series::get_comments_on_the_performed_procedure_step() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::CommentsOnThePerformedProcedureStep>();
}

//------------------------------------------------------------------------------

void series::set_comments_on_the_performed_procedure_step(const std::string& _comments_on_the_performed_procedure_step)
{
    m_pimpl->set_value<gdcm::Keywords::CommentsOnThePerformedProcedureStep>(_comments_on_the_performed_procedure_step);
}

//------------------------------------------------------------------------------

std::string series::get_institution_name() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::InstitutionName>();
}

//------------------------------------------------------------------------------

void series::set_institution_name(const std::string& _institution_name)
{
    m_pimpl->set_value<gdcm::Keywords::InstitutionName>(_institution_name);
}

//------------------------------------------------------------------------------

std::string series::get_patient_name() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientName>();
}

//------------------------------------------------------------------------------

void series::set_patient_name(const std::string& _patient_name)
{
    m_pimpl->set_value<gdcm::Keywords::PatientName>(_patient_name);
}

//------------------------------------------------------------------------------

std::string series::get_patient_id() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientID>();
}

//------------------------------------------------------------------------------

void series::set_patient_id(const std::string& _patient_id)
{
    m_pimpl->set_value<gdcm::Keywords::PatientID>(_patient_id);
}

//------------------------------------------------------------------------------

std::string series::get_patient_birth_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientBirthDate>();
}

//------------------------------------------------------------------------------

void series::set_patient_birth_date(const std::string& _patient_birth_date)
{
    m_pimpl->set_value<gdcm::Keywords::PatientBirthDate>(_patient_birth_date);
}

//------------------------------------------------------------------------------

std::string series::get_patient_sex() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientSex>();
}

//------------------------------------------------------------------------------

void series::set_patient_sex(const std::string& _patient_sex)
{
    m_pimpl->set_value<gdcm::Keywords::PatientSex>(_patient_sex);
}

//------------------------------------------------------------------------------

std::string series::get_study_date() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::StudyDate>();
}

//------------------------------------------------------------------------------

void series::set_study_date(const std::string& _study_date)
{
    m_pimpl->set_value<gdcm::Keywords::StudyDate>(_study_date);
}

//------------------------------------------------------------------------------

std::string series::get_study_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::StudyTime>();
}

//------------------------------------------------------------------------------

void series::set_study_time(const std::string& _study_time)
{
    m_pimpl->set_value<gdcm::Keywords::StudyTime>(_study_time);
}

//------------------------------------------------------------------------------

std::string series::get_referring_physician_name() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ReferringPhysicianName>();
}

//------------------------------------------------------------------------------

void series::set_referring_physician_name(const std::string& _referring_physician_name)
{
    m_pimpl->set_value<gdcm::Keywords::ReferringPhysicianName>(_referring_physician_name);
}

//------------------------------------------------------------------------------

std::string series::get_study_description() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::StudyDescription>();
}

//------------------------------------------------------------------------------

void series::set_study_description(const std::string& _study_description)
{
    m_pimpl->set_value<gdcm::Keywords::StudyDescription>(_study_description);
}

//------------------------------------------------------------------------------

std::string series::get_study_instance_uid() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::StudyInstanceUID>();
}

//------------------------------------------------------------------------------

void series::set_study_instance_uid(const std::string& _study_instance_uid)
{
    m_pimpl->set_value<gdcm::Keywords::StudyInstanceUID>(_study_instance_uid);
}

//------------------------------------------------------------------------------

std::string series::get_study_id() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::StudyID>();
}

//------------------------------------------------------------------------------

void series::set_study_id(const std::string& _study_id)
{
    m_pimpl->set_value<gdcm::Keywords::StudyID>(_study_id);
}

//------------------------------------------------------------------------------

std::string series::get_patient_age() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::PatientAge>();
}

//------------------------------------------------------------------------------

void series::set_patient_age(const std::string& _patient_age)
{
    m_pimpl->set_value<gdcm::Keywords::PatientAge>(_patient_age);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_patient_size() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::PatientSize>();
}

//------------------------------------------------------------------------------

void series::set_patient_size(const std::optional<double>& _patient_size)
{
    m_pimpl->set_value<gdcm::Keywords::PatientSize>(_patient_size);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_patient_weight() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::PatientWeight>();
}

//------------------------------------------------------------------------------

void series::set_patient_weight(const std::optional<double>& _patient_weight)
{
    m_pimpl->set_value<gdcm::Keywords::PatientWeight>(_patient_weight);
}

//------------------------------------------------------------------------------

series::dicom_t series::get_dicom_type() const noexcept
{
    // First try with dynamic cast
    if(dynamic_cast<const image_series*>(this) != nullptr)
    {
        return dicom_t::image;
    }

    if(dynamic_cast<const model_series*>(this) != nullptr)
    {
        return dicom_t::model;
    }

    if(dynamic_cast<const fiducials_series*>(this) != nullptr)
    {
        return dicom_t::fiducials;
    }

    // Then try with the sop_classUID
    if(const auto& sop_class_uid = get_sop_class_uid(); !sop_class_uid.empty())
    {
        if(const dicom_t result = get_dicom_type(sop_class_uid); result != dicom_t::unknown)
        {
            return result;
        }
    }

    // On last resort, try with semi deprecated GDCM MediaStorage
    /// @note This GDCM class is no more updated and many things are missing: for example, Surface Scan Mesh Storage
    /// (1.2.840.10008.5.1.4.1.1.68.1) and Surface Scan Point Cloud Storage (1.2.840.10008.5.1.4.1.1.68.2) which are
    /// somewhat "models", are unknown to GDCM MediaStorage.
    gdcm::MediaStorage media_storage;
    media_storage.SetFromDataSet(get_data_set());

    if(gdcm::MediaStorage::IsImage(media_storage))
    {
        return dicom_t::image;
    }

    if(media_storage == gdcm::MediaStorage::SurfaceSegmentationStorage)
    {
        return dicom_t::model;
    }

    // If we are here, nothing worked...
    return dicom_t::unknown;
}

//------------------------------------------------------------------------------
inline static series::dicom_t sop_keyword_to_dicom_type(const dicom::sop::Keyword& _keyword)
{
    // cspell:ignore Multiframe Radiofluoroscopic Tomosynthesis Bscan Dermoscopic
    switch(_keyword)
    {
        // Found using dicom_parser.py --mandatory-tags "(0062,0002)" "(0066,0002)" "(0066,0011)"
        case dicom::sop::Keyword::GenericImplantTemplateStorage:
        case dicom::sop::Keyword::SegmentationStorage:
        case dicom::sop::Keyword::SurfaceSegmentationStorage:
        case dicom::sop::Keyword::SurfaceScanMeshStorage:
        case dicom::sop::Keyword::SurfaceScanPointCloudStorage:
            return series::dicom_t::model;

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
        case dicom::sop::Keyword::PhotoacousticImageStorage:
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
            return series::dicom_t::image;

        case dicom::sop::Keyword::SpatialFiducialsStorage:
            return series::dicom_t::fiducials;

        default:
            return series::dicom_t::unknown;
    }
}

//------------------------------------------------------------------------------

series::dicom_t series::get_dicom_type(const std::string& _sop_class_uid) noexcept
{
    return sop_keyword_to_dicom_type(dicom::sop::keyword(_sop_class_uid));
}

//------------------------------------------------------------------------------

std::string series::dicom_types_to_string(series::DicomTypes _types) noexcept
{
    std::string dicom_types;

    if((_types & static_cast<DicomTypes>(dicom_t::image)) == _types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicom_type_to_string(dicom_t::image);
    }

    if((_types & static_cast<DicomTypes>(dicom_t::model)) == _types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicom_type_to_string(dicom_t::model);
    }

    if((_types & static_cast<DicomTypes>(dicom_t::report)) == _types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicom_type_to_string(dicom_t::report);
    }

    if((_types & static_cast<DicomTypes>(dicom_t::fiducials)) == _types)
    {
        if(!dicom_types.empty())
        {
            dicom_types += ", ";
        }

        dicom_types += dicom_type_to_string(dicom_t::fiducials);
    }

    return dicom_types;
}

//------------------------------------------------------------------------------

series::DicomTypes series::string_to_dicom_types(const std::string& _types) noexcept
{
    DicomTypes dicom_types = 0;

    std::vector<std::string> split;
    boost::split(split, _types, boost::is_any_of(","));

    for(const auto& type : split)
    {
        const auto& trimmed = boost::trim_copy(type);

        if(!trimmed.empty())
        {
            if(const auto& dicom_type = string_to_dicom_type(trimmed); dicom_type != dicom_t::unknown)
            {
                dicom_types |= static_cast<DicomTypes>(dicom_type);
            }
        }
    }

    return dicom_types;
}

//------------------------------------------------------------------------------

series::SopKeywords series::dicom_types_to_sops(DicomTypes _types) noexcept
{
    SopKeywords keywords;

    if((_types & std::uint64_t(dicom_t::image)) == std::uint64_t(dicom_t::image))
    {
        keywords.insert(
            {
                // Found using dicom_parser.py --mandatory-tags "(7FE0,0010)"
                dicom::sop::Keyword::ComputedRadiographyImageStorage,
                dicom::sop::Keyword::DigitalXRayImageStorageForPresentation,
                dicom::sop::Keyword::DigitalXRayImageStorageForProcessing,
                dicom::sop::Keyword::DigitalMammographyXRayImageStorageForPresentation,
                dicom::sop::Keyword::DigitalMammographyXRayImageStorageForProcessing,
                dicom::sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation,
                dicom::sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing,
                dicom::sop::Keyword::CTImageStorage,
                dicom::sop::Keyword::EnhancedCTImageStorage,
                dicom::sop::Keyword::LegacyConvertedEnhancedCTImageStorage,
                dicom::sop::Keyword::UltrasoundMultiFrameImageStorage,
                dicom::sop::Keyword::MRImageStorage,
                dicom::sop::Keyword::EnhancedMRImageStorage,
                dicom::sop::Keyword::EnhancedMRColorImageStorage,
                dicom::sop::Keyword::LegacyConvertedEnhancedMRImageStorage,
                dicom::sop::Keyword::UltrasoundImageStorage,
                dicom::sop::Keyword::EnhancedUSVolumeStorage,
                dicom::sop::Keyword::PhotoacousticImageStorage,
                dicom::sop::Keyword::SecondaryCaptureImageStorage,
                dicom::sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage,
                dicom::sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage,
                dicom::sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage,
                dicom::sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage,
                dicom::sop::Keyword::XRayAngiographicImageStorage,
                dicom::sop::Keyword::EnhancedXAImageStorage,
                dicom::sop::Keyword::XRayRadiofluoroscopicImageStorage,
                dicom::sop::Keyword::EnhancedXRFImageStorage,
                dicom::sop::Keyword::XRay3DAngiographicImageStorage,
                dicom::sop::Keyword::XRay3DCraniofacialImageStorage,
                dicom::sop::Keyword::BreastTomosynthesisImageStorage,
                dicom::sop::Keyword::BreastProjectionXRayImageStorageForPresentation,
                dicom::sop::Keyword::BreastProjectionXRayImageStorageForProcessing,
                dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation,
                dicom::sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing,
                dicom::sop::Keyword::NuclearMedicineImageStorage,
                dicom::sop::Keyword::ParametricMapStorage,
                dicom::sop::Keyword::VLEndoscopicImageStorage,
                dicom::sop::Keyword::VideoEndoscopicImageStorage,
                dicom::sop::Keyword::VLMicroscopicImageStorage,
                dicom::sop::Keyword::VideoMicroscopicImageStorage,
                dicom::sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage,
                dicom::sop::Keyword::VLPhotographicImageStorage,
                dicom::sop::Keyword::VideoPhotographicImageStorage,
                dicom::sop::Keyword::OphthalmicPhotography8BitImageStorage,
                dicom::sop::Keyword::OphthalmicPhotography16BitImageStorage,
                dicom::sop::Keyword::OphthalmicTomographyImageStorage,
                dicom::sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,
                dicom::sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage,
                dicom::sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,
                dicom::sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,
                dicom::sop::Keyword::VLWholeSlideMicroscopyImageStorage,
                dicom::sop::Keyword::DermoscopicPhotographyImageStorage,
                dicom::sop::Keyword::OphthalmicThicknessMapStorage,
                dicom::sop::Keyword::CornealTopographyMapStorage,
                dicom::sop::Keyword::PositronEmissionTomographyImageStorage,
                dicom::sop::Keyword::EnhancedPETImageStorage,
                dicom::sop::Keyword::LegacyConvertedEnhancedPETImageStorage,
                dicom::sop::Keyword::RTImageStorage,
                dicom::sop::Keyword::RTDoseStorage
            });
    }

    if((_types & std::uint64_t(dicom_t::model)) == std::uint64_t(dicom_t::model))
    {
        keywords.insert(
            {
                // Found using dicom_parser.py --mandatory-tags "(0062,0002)" "(0066,0002)" "(0066,0011)"
                dicom::sop::Keyword::GenericImplantTemplateStorage,
                dicom::sop::Keyword::SegmentationStorage,
                dicom::sop::Keyword::SurfaceSegmentationStorage,
                dicom::sop::Keyword::SurfaceScanMeshStorage,
                dicom::sop::Keyword::SurfaceScanPointCloudStorage,
            });
    }

    if((_types & std::uint64_t(dicom_t::fiducials)) == std::uint64_t(dicom_t::fiducials))
    {
        keywords.insert(
            {
                dicom::sop::Keyword::SpatialFiducialsStorage
            });
    }

    return keywords;
}

//------------------------------------------------------------------------------

series::DicomTypes series::sops_to_dicom_types(const SopKeywords& _keywords) noexcept
{
    DicomTypes types {static_cast<DicomTypes>(dicom_t::unknown)};

    for(const auto& keyword : _keywords)
    {
        if(const auto& type = sop_keyword_to_dicom_type(keyword); type != dicom_t::unknown)
        {
            types |= static_cast<DicomTypes>(type);
        }
    }

    return types;
}

//------------------------------------------------------------------------------

series::SopKeywords series::string_to_sops(const std::string& _uids) noexcept
{
    SopKeywords sop_keywords;

    std::vector<std::string> split;
    boost::split(split, _uids, boost::is_any_of(","));

    for(const auto& uid : split)
    {
        const auto& trimmed = boost::trim_copy(uid);

        if(!trimmed.empty())
        {
            if(const auto& sop_keyword = dicom::sop::keyword(trimmed); sop_keyword != dicom::sop::Keyword::INVALID)
            {
                sop_keywords.insert(sop_keyword);
            }
        }
    }

    return sop_keywords;
}

//------------------------------------------------------------------------------

std::string series::sops_to_string(const SopKeywords& _keywords) noexcept
{
    std::string sop_keywords;

    for(const auto& keyword : _keywords)
    {
        if(!sop_keywords.empty())
        {
            sop_keywords += ", ";
        }

        sop_keywords += dicom::sop::get(keyword).m_uid;
    }

    return sop_keywords;
}

//------------------------------------------------------------------------------

std::string series::get_acquisition_date(std::size_t _instance) const
{
    return m_pimpl->get_string_value<gdcm::Keywords::AcquisitionDate>(_instance);
}

//------------------------------------------------------------------------------

void series::set_acquisition_date(const std::string& _acquisition_date, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::AcquisitionDate>(_acquisition_date, _instance);
}

//------------------------------------------------------------------------------

std::string series::get_acquisition_time(std::size_t _instance) const
{
    return m_pimpl->get_string_value<gdcm::Keywords::AcquisitionTime>(_instance);
}

//------------------------------------------------------------------------------

void series::set_acquisition_time(const std::string& _acquisition_time, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::AcquisitionTime>(_acquisition_time, _instance);
}

//------------------------------------------------------------------------------

std::string series::get_content_time(std::size_t _instance) const
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContentTime>(_instance);
}

//------------------------------------------------------------------------------

void series::set_content_time(const std::string& _content_time, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::ContentTime>(_content_time, _instance);
}

//------------------------------------------------------------------------------

std::optional<std::int32_t> series::get_instance_number(std::size_t _instance) const
{
    return m_pimpl->get_value<gdcm::Keywords::InstanceNumber>(_instance);
}

//------------------------------------------------------------------------------

void series::set_instance_number(const std::optional<std::int32_t>& _instance_number, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::InstanceNumber>(_instance_number, _instance);
}

//------------------------------------------------------------------------------

std::optional<std::int32_t> series::get_acquisition_number(std::size_t _instance) const
{
    return m_pimpl->get_value<gdcm::Keywords::AcquisitionNumber>(_instance);
}

//------------------------------------------------------------------------------

void series::set_acquisition_number(std::optional<std::int32_t> _acquisition_number, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::AcquisitionNumber>(_acquisition_number, _instance);
}

//------------------------------------------------------------------------------

std::string series::get_contrast_bolus_agent() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContrastBolusAgent>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_agent(const std::string& _contrast_bolus_agent)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusAgent>(_contrast_bolus_agent);
}

//------------------------------------------------------------------------------

std::string series::get_contrast_bolus_route() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContrastBolusRoute>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_route(const std::string& _contrast_bolus_route)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusRoute>(_contrast_bolus_route);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_contrast_bolus_volume() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::ContrastBolusVolume>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_volume(const std::optional<double>& _contrast_bolus_volume)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusVolume>(_contrast_bolus_volume);
}

//------------------------------------------------------------------------------

std::string series::get_contrast_bolus_start_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContrastBolusStartTime>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_start_time(const std::string& _contrast_bolus_start_time)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusStartTime>(_contrast_bolus_start_time);
}

//------------------------------------------------------------------------------

std::string series::get_contrast_bolus_stop_time() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContrastBolusStopTime>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_stop_time(const std::string& _contrast_bolus_stop_time)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusStopTime>(_contrast_bolus_stop_time);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_contrast_bolus_total_dose() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::ContrastBolusTotalDose>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_total_dose(const std::optional<double>& _contrast_bolus_total_dose)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusTotalDose>(_contrast_bolus_total_dose);
}

//------------------------------------------------------------------------------

std::vector<double> series::get_contrast_flow_rates() const noexcept
{
    return m_pimpl->get_values<gdcm::Keywords::ContrastFlowRate>().value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

std::string series::get_contrast_flow_rate() const noexcept
{
    return m_pimpl->get_joined_values<gdcm::Keywords::ContrastFlowRate>();
}

//------------------------------------------------------------------------------

void series::set_contrast_flow_rates(const std::vector<double>& _contrast_flow_rates)
{
    m_pimpl->set_values<gdcm::Keywords::ContrastFlowRate>(_contrast_flow_rates);
}

//------------------------------------------------------------------------------

void series::set_contrast_flow_rate(const std::string& _contrast_flow_rates)
{
    m_pimpl->set_joined_values<gdcm::Keywords::ContrastFlowRate>(_contrast_flow_rates);
}

//------------------------------------------------------------------------------

std::vector<double> series::get_contrast_flow_durations() const noexcept
{
    return m_pimpl->get_values<gdcm::Keywords::ContrastFlowDuration>().value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

std::string series::get_contrast_flow_duration() const noexcept
{
    return m_pimpl->get_joined_values<gdcm::Keywords::ContrastFlowDuration>();
}

//------------------------------------------------------------------------------

void series::set_contrast_flow_durations(const std::vector<double>& _contrast_flow_durations)
{
    m_pimpl->set_values<gdcm::Keywords::ContrastFlowDuration>(_contrast_flow_durations);
}

//------------------------------------------------------------------------------

void series::set_contrast_flow_duration(const std::string& _contrast_flow_durations)
{
    m_pimpl->set_joined_values<gdcm::Keywords::ContrastFlowDuration>(_contrast_flow_durations);
}

//------------------------------------------------------------------------------

std::string series::get_contrast_bolus_ingredient() const noexcept
{
    return m_pimpl->get_string_value<gdcm::Keywords::ContrastBolusIngredient>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_ingredient(const std::string& _contrast_bolus_ingredient)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusIngredient>(_contrast_bolus_ingredient);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_contrast_bolus_ingredient_concentration() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::ContrastBolusIngredientConcentration>();
}

//------------------------------------------------------------------------------

void series::set_contrast_bolus_ingredient_concentration(
    const std::optional<double>& _contrast_bolus_ingredient_concentration
)
{
    m_pimpl->set_value<gdcm::Keywords::ContrastBolusIngredientConcentration>(_contrast_bolus_ingredient_concentration);
}

//------------------------------------------------------------------------------

std::vector<double> series::window_center() const noexcept
{
    return m_pimpl->get_values<gdcm::Keywords::WindowCenter>().value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

void series::set_window_center(const std::vector<double>& _window_centers)
{
    m_pimpl->set_values<gdcm::Keywords::WindowCenter>(_window_centers);
}

//------------------------------------------------------------------------------

std::vector<double> series::window_width() const noexcept
{
    return m_pimpl->get_values<gdcm::Keywords::WindowWidth>().value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

void series::set_window_width(const std::vector<double>& _window_widths)
{
    m_pimpl->set_values<gdcm::Keywords::WindowWidth>(_window_widths);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_rescale_intercept(std::size_t _instance) const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::RescaleIntercept>(_instance);
}

//------------------------------------------------------------------------------

void series::set_rescale_intercept(const std::optional<double>& _rescale_intercept, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::RescaleIntercept>(_rescale_intercept, _instance);
}

//------------------------------------------------------------------------------

std::optional<double> series::get_rescale_slope(std::size_t _instance) const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::RescaleSlope>(_instance);
}

//------------------------------------------------------------------------------

void series::set_rescale_slope(const std::optional<double>& _rescale_slope, std::size_t _instance)
{
    m_pimpl->set_value<gdcm::Keywords::RescaleSlope>(_rescale_slope, _instance);
}

//------------------------------------------------------------------------------

std::vector<double> series::get_image_position_patient(std::size_t _instance) const
{
    // Assert if the SOP class is not set
    SIGHT_ASSERT(
        "SOP class is not set, please call series::set_sop_class_uid before calling series::get_image_position_patient",
        get_sop_keyword() != dicom::sop::Keyword::INVALID
    );

    if(is_multi_frame())
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
        return m_pimpl->get_multi_frame_values<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlanePositionSequence,
            gdcm::Keywords::ImagePositionPatient
        >(_instance);
    }

    // Default case use simple ImagePositionPatient tag values.
    return m_pimpl->get_values<gdcm::Keywords::ImagePositionPatient>(_instance).value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

void series::set_image_position_patient(const std::vector<double>& _image_position_patient, std::size_t _instance)
{
    // Assert if the SOP class is not set
    SIGHT_ASSERT(
        "SOP class is not set, please call series::set_sop_class_uid before calling series::set_image_position_patient",
        get_sop_keyword() != dicom::sop::Keyword::INVALID
    );

    if(is_multi_frame())
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
        m_pimpl->set_multi_frame_values<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlanePositionSequence,
            gdcm::Keywords::ImagePositionPatient
        >(_image_position_patient, _instance);
    }
    else
    {
        m_pimpl->set_values<gdcm::Keywords::ImagePositionPatient>(_image_position_patient, _instance);
    }
}

//------------------------------------------------------------------------------

std::vector<double> series::get_image_orientation_patient(std::size_t _instance) const
{
    SIGHT_ASSERT(
        "SOP class is not set, please call series::set_sop_class_uid before calling series::get_image_orientation_patient",
        get_sop_keyword() != dicom::sop::Keyword::INVALID
    );

    if(is_multi_frame())
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
        return m_pimpl->get_multi_frame_values<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlaneOrientationSequence,
            gdcm::Keywords::ImageOrientationPatient
        >(_instance);
    }

    return m_pimpl->get_values<gdcm::Keywords::ImageOrientationPatient>(_instance).value_or(std::vector<double> {});
}

//------------------------------------------------------------------------------

void series::set_image_orientation_patient(const std::vector<double>& _image_orientation_patient, std::size_t _instance)
{
    SIGHT_ASSERT(
        "SOP class is not set, please call series::set_sop_class_uid before calling series::set_image_orientation_patient",
        get_sop_keyword() != dicom::sop::Keyword::INVALID
    );

    if(is_multi_frame())
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
        m_pimpl->set_multi_frame_values<
            gdcm::Keywords::PerFrameFunctionalGroupsSequence,
            gdcm::Keywords::PlaneOrientationSequence,
            gdcm::Keywords::ImageOrientationPatient
        >(_image_orientation_patient, _instance);
    }
    else
    {
        m_pimpl->set_values<gdcm::Keywords::ImageOrientationPatient>(_image_orientation_patient, _instance);
    }
}

//------------------------------------------------------------------------------

std::vector<double> series::get_image_position_volume(std::size_t _frame_index) const
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
    return m_pimpl->get_multi_frame_values<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlanePositionVolumeSequence,
        gdcm::Keywords::ImagePositionVolume
    >(_frame_index);
}

//------------------------------------------------------------------------------

void series::set_image_position_volume(const std::vector<double>& _image_position_volume, std::size_t _frame_index)
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
    m_pimpl->set_multi_frame_values<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlanePositionSequence,
        gdcm::Keywords::ImagePositionVolume
    >(_image_position_volume, _frame_index);
}

//------------------------------------------------------------------------------

std::vector<double> series::get_image_orientation_volume(std::size_t _frame_index) const
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
    return m_pimpl->get_multi_frame_values<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlaneOrientationVolumeSequence,
        gdcm::Keywords::ImageOrientationVolume
    >(_frame_index);
}

//------------------------------------------------------------------------------

void series::set_image_orientation_volume(
    const std::vector<double>& _image_orientation_volume,
    std::size_t _frame_index
)
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
    m_pimpl->set_multi_frame_values<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::PlaneOrientationSequence,
        gdcm::Keywords::ImageOrientationVolume
    >(_image_orientation_volume, _frame_index);
}

//------------------------------------------------------------------------------

std::optional<std::string> series::get_frame_acquisition_date_time(std::size_t _frame_index) const
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
    if(const auto& value = m_pimpl->get_multi_frame_value<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameAcquisitionDateTime
                           >(_frame_index); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void series::set_frame_acquisition_date_time(
    const std::optional<std::string>& _frame_acquisition_date_time,
    std::size_t _frame_index
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
    m_pimpl->set_multi_frame_value<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameAcquisitionDateTime
    >(_frame_acquisition_date_time, _frame_index);
}

//------------------------------------------------------------------------------

std::optional<std::chrono::system_clock::time_point> series::get_frame_acquisition_time_point(
    std::size_t _frame_index
) const noexcept
{
    try
    {
        // Get the acquisition date time.
        // Need to ensure that the value is set, as it might not.
        if(const auto& optional = get_frame_acquisition_date_time(_frame_index); optional.has_value())
        {
            // Convert from YYYYMMDDHHMMSS.FFFFFF
            /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
            return date_time_to_time_point(*optional);
        }

        return std::nullopt;
    }
    catch(...)
    {
        return std::nullopt;
    }
}

//------------------------------------------------------------------------------

void series::set_frame_acquisition_time_point(
    sight::core::clock::type _time_point,
    std::size_t _frame_index
)
{
    const auto tp = std::chrono::system_clock::time_point()
                    + std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::duration<double, std::milli>(_time_point)
                    );

    // Convert type to time point
    // Convert to YYYYMMDDHHMMSS.FFFFFF
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
    const auto& date_time = time_point_to_date_time(tp);
    set_frame_acquisition_date_time(date_time, _frame_index);
}

//------------------------------------------------------------------------------

void series::set_frame_acquisition_time_point(
    const std::optional<std::chrono::system_clock::time_point>& _time_point,
    std::size_t _frame_index
)
{
    if(_time_point)
    {
        // Convert to YYYYMMDDHHMMSS.FFFFFF
        /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
        const auto& date_time = time_point_to_date_time(*_time_point);
        set_frame_acquisition_date_time(date_time, _frame_index);
    }
    else
    {
        set_frame_acquisition_date_time(std::nullopt, _frame_index);
    }
}

//------------------------------------------------------------------------------

std::optional<std::string> series::get_frame_comments(std::size_t _frame_index) const
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
    if(const auto& value = m_pimpl->get_multi_frame_value<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameComments
                           >(_frame_index); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void series::set_frame_comments(const std::optional<std::string>& _frame_comments, std::size_t _frame_index)
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
    m_pimpl->set_multi_frame_value<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameComments
    >(_frame_comments, _frame_index);
}

//------------------------------------------------------------------------------

std::optional<std::string> series::get_frame_label(std::size_t _frame_index) const
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
    if(const auto& value = m_pimpl->get_multi_frame_value<
           gdcm::Keywords::PerFrameFunctionalGroupsSequence,
           gdcm::Keywords::FrameContentSequence,
           gdcm::Keywords::FrameLabel
                           >(_frame_index); value)
    {
        return detail::shrink(value->Trim());
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void series::set_frame_label(const std::optional<std::string>& _frame_label, std::size_t _frame_index)
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
    m_pimpl->set_multi_frame_value<
        gdcm::Keywords::PerFrameFunctionalGroupsSequence,
        gdcm::Keywords::FrameContentSequence,
        gdcm::Keywords::FrameLabel
    >(_frame_label, _frame_index);
}

//------------------------------------------------------------------------------

std::optional<sight::data::matrix4> series::get_image_transform_patient(std::size_t _instance) const
{
    const auto position    = this->get_image_position_patient(_instance);
    const auto orientation = this->get_image_orientation_patient(_instance);

    if(position.size() != 3 || orientation.size() != 6)
    {
        return std::nullopt;
    }

    const glm::dvec3 x(orientation[0], orientation[1], orientation[2]);
    const glm::dvec3 y(orientation[3], orientation[4], orientation[5]);
    const glm::dvec3 z = glm::cross(x, y);

    return std::make_optional<sight::data::matrix4>(
        {
            orientation[0], orientation[3], z[0], position[0],
            orientation[1], orientation[4], z[1], position[1],
            orientation[2], orientation[5], z[2], position[2],
            0., 0., 0., 1.
        });
}

//------------------------------------------------------------------------------

bool series::check_image_transform_patient_validity(const sight::data::matrix4& _transform)
{
    [[maybe_unused]] const glm::dvec3 x(_transform[0], _transform[4], _transform[8]);
    [[maybe_unused]] const glm::dvec3 y(_transform[1], _transform[5], _transform[9]);
    [[maybe_unused]] const glm::dvec3 z(_transform[2], _transform[6], _transform[10]);
    [[maybe_unused]] const glm::dvec3 computed_z = glm::cross(x, y);

    return glm::all(glm::epsilonEqual(computed_z, z, 1e-2));
}

//------------------------------------------------------------------------------

void series::set_image_transform_patient(const sight::data::matrix4& _transform, std::size_t _instance)
{
    this->set_image_position_patient({_transform[3], _transform[7], _transform[11]}, _instance);

    SIGHT_ASSERT("Unexpected orientation vector size", series::check_image_transform_patient_validity(_transform));

    this->set_image_orientation_patient(
        {_transform[0], _transform[4], _transform[8], _transform[1], _transform[5], _transform[9]},
        _instance
    );
}

//------------------------------------------------------------------------------

std::optional<double> series::get_slice_thickness() const noexcept
{
    /// @todo
    return m_pimpl->get_value<gdcm::Keywords::SliceThickness>();
}

//------------------------------------------------------------------------------

void series::set_slice_thickness(const std::optional<double>& _slice_thickness)
{
    /// @todo
    m_pimpl->set_value<gdcm::Keywords::SliceThickness>(_slice_thickness);
}

//------------------------------------------------------------------------------

std::optional<std::uint16_t> series::get_rows() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::Rows>();
}

//------------------------------------------------------------------------------

void series::set_rows(const std::optional<std::uint16_t>& _rows)
{
    m_pimpl->set_value<gdcm::Keywords::Rows>(_rows);
}

//------------------------------------------------------------------------------

std::optional<std::uint16_t> series::get_columns() const noexcept
{
    return m_pimpl->get_value<gdcm::Keywords::Columns>();
}

//------------------------------------------------------------------------------

void series::set_columns(const std::optional<std::uint16_t>& _columns)
{
    m_pimpl->set_value<gdcm::Keywords::Columns>(_columns);
}

//------------------------------------------------------------------------------

std::filesystem::path series::file_name(
    const std::optional<std::string>& _suffix
) const
{
    // File name. DICOM value can be specified with the syntax "(group,element)"
    // "<PatientID>.<Modality>.<SeriesDate><SeriesTime>.<SeriesInstanceUID>.dcm"
    static constexpr std::string_view s_FILE_NAME {"(0010,0020).(0008,0060).(0008,0021)(0008,0031).(0020,000E)"};

    std::string file_name {s_FILE_NAME};

    if(_suffix && !_suffix->empty())
    {
        file_name += *_suffix;
    }

    return sight::data::parse_path(this, file_name);
}

//------------------------------------------------------------------------------

std::filesystem::path series::folder(
    const std::optional<std::filesystem::path>& _root
) const
{
    // Sub-folder to save the images. DICOM value can be specified with the syntax "(group,element)"
    // "<PatientID>/<StudyDate><StudyTime>"
    static const std::string s_FOLDER {"(0010,0020)/(0008,0020)(0008,0030)"};

    std::filesystem::path folder {sight::data::parse_path(this, s_FOLDER)};

    if(_root && !_root->empty())
    {
        folder = *_root / folder;
    }

    return std::filesystem::weakly_canonical(folder);
}

//------------------------------------------------------------------------------

std::filesystem::path series::file_path(
    const std::optional<std::filesystem::path>& _root,
    const std::optional<std::string>& _suffix
) const
{
    return std::filesystem::weakly_canonical(folder(_root) / file_name(_suffix));
}

} // namespace sight::data
