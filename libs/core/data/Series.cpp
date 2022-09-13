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
#include "data/Series.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmDataSet.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>

SIGHT_REGISTER_DATA(sight::data::Series)

namespace sight::data
{

static constexpr char SPACE_PADDING_CHAR  = ' ';
static constexpr char NULL_PADDING_CHAR   = '\0';
static constexpr auto BACKSLASH_SEPARATOR = "\\";

/// Returns the maximum or fixed size of a Value Representation and its padding character.
/// @note the data come from https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_6.2.html
/// @param[in] vr_type The Value Representation to get the size of.
static constexpr std::tuple<std::size_t, bool, char> getVRFormat(gdcm::VR::VRType vr_type)
{
    switch(vr_type)
    {
        case gdcm::VR::AE:
            return {16LL, false, NULL_PADDING_CHAR};

        case gdcm::VR::AS:
        case gdcm::VR::AT:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::CS:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::DA:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::DS:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::DT:
            return {26LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::FL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::FD:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::IS:
            return {12LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::LO:
            return {64LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::LT:
            return {10240LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OB:
            return {(1LL << 32) - 2, false, NULL_PADDING_CHAR};

        case gdcm::VR::OD:
            return {(1LL << 32) - 8, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OF:
        case gdcm::VR::OL:
            return {(1LL << 32) - 4, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OV:
            return {(1LL << 32) - 8, false, SPACE_PADDING_CHAR};

        case gdcm::VR::OW:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};

        case gdcm::VR::PN:
            return {64 * 5LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SH:
            return {16LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::SQ:
            return {0LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SS:
            return {2LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::ST:
            return {1024LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::SV:
            return {8LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::TM:
            return {14LL, false, SPACE_PADDING_CHAR};

        case gdcm::VR::UC:
        case gdcm::VR::UN:
        case gdcm::VR::UR:
        case gdcm::VR::UT:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};

        case gdcm::VR::UI:
            return {64LL, false, NULL_PADDING_CHAR};

        case gdcm::VR::UL:
            return {4LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::US:
            return {2LL, true, SPACE_PADDING_CHAR};

        case gdcm::VR::UV:
            return {8LL, true, SPACE_PADDING_CHAR};

        default:
            return {(1LL << 32) - 2, false, SPACE_PADDING_CHAR};
    }
}

/// Returns a string from a number to be stored in a Integer String or Decimal String. The precision is set
/// coordinately to the VR.
/// @param value The value to be converted.
/// @param vr_type The Value Representation of the value.
/// @return The string representation of the value.
template<typename V>
static inline std::string toString(const V& value, gdcm::VR::VRType vr_type)
{
    const auto [size, fixed, padding] = getVRFormat(vr_type);

    std::ostringstream oss;

    if(fixed)
    {
        oss << std::setprecision(int(size) - 1) << std::showpoint << std::fixed << value;
    }
    else
    {
        oss << std::setprecision(int(size) - 1) << std::noshowpoint << std::defaultfloat << value;
    }

    return oss.str();
}

/// Remove the trailing padding \0 characters from a string.
/// @param[in] source The string to be trimmed.
/// @return The trimmed string.
static inline std::string shrink(const std::string& source)
{
    std::string result(source);
    result.erase(result.find_last_not_of(NULL_PADDING_CHAR) + 1);
    return result;
}

/// Private Series implementation
class Series::SeriesImpl final
{
public:

    /// Delete default constructors and assignment operators
    /// @{
    SeriesImpl(const SeriesImpl&)            = delete;
    SeriesImpl(SeriesImpl&&)                 = delete;
    SeriesImpl& operator=(const SeriesImpl&) = delete;
    SeriesImpl& operator=(SeriesImpl&&)      = delete;
    /// @}

    /// Constructor
    inline explicit SeriesImpl(Series* const series) noexcept :
        m_series(series)
    {
    }

    /// Default destructor
    inline ~SeriesImpl() noexcept = default;

    /// Retrieve a DICOM tag value. If the tag is not found, an null optional is returned.
    template<typename A>
    [[nodiscard]] constexpr std::optional<typename A::ArrayType> getValue() const noexcept
    {
        // Unfortunately with GDCM, all non pure string attributes (Integer String, Decimal String, ...) will always
        // be found with a random value, despite the underlying string is "". For example, if the attribute
        // "SeriesNumber" is a "" string, gdcm::Attribute<NULL_CHAR20, NULL_CHAR11>::GetNumberOfValues() will return 1
        // (as 1 is anyway the minimum multiplicity), but with a random integer/decimal value, and there is no way to
        // know if the value is "" (which is valid, it means "unknown") or not.
        //
        // Therefore, we need to access to the underlying data to know if the attribute is empty or not, which is not
        // very efficient or elegant.
        if(m_dataset.FindDataElement(A::GetTag()))
        {
            if(const auto& data_element = m_dataset.GetDataElement(A::GetTag()); !data_element.IsEmpty())
            {
                if(const auto& byte_value = data_element.GetByteValue();
                   byte_value != nullptr && byte_value->GetPointer() != nullptr && byte_value->GetLength() > 0)
                {
                    // Now, we know that we have a non empty value, so we can safely return it.
                    A attribute {};

                    attribute.SetFromDataSet(m_dataset);
                    return attribute.GetValue();
                }
            }
        }

        return std::nullopt;
    }

    /// Retrieve a string DICOM tag value. If the tag is not found, an empty string is returned.
    template<typename A>
    [[nodiscard]] inline std::string getStringValue() const noexcept
    {
        const auto& value = getValue<A>();

        if(value)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                return shrink(value->Trim());
            }
            else
            {
                // Convert the value to a string
                return toString(*value, A::GetVR());
            }
        }

        // Returns an empty string if the value is not found
        return {};
    }

    /// Retrieve multi-value DICOM tag. If the tag is not found, an empty vector is returned.
    template<typename A>
    [[nodiscard]] constexpr std::vector<typename A::ArrayType> getValues() const noexcept
    {
        A attribute;
        attribute.SetFromDataSet(m_dataset);

        const auto count = attribute.GetNumberOfValues();

        if(count > 0)
        {
            auto values_pointer = attribute.GetValues();

            // Pointer can be treated as iterator ;)
            return std::vector<typename A::ArrayType>(
                values_pointer,
                values_pointer + count
            );
        }

        return std::vector<typename A::ArrayType>();
    }

    /// Retrieve multi-value DICOM tag as a single joined string. Use '\' as separator.
    template<typename A>
    [[nodiscard]] inline std::string getJoinedValues() const noexcept
    {
        return boost::join(getStringValues<A>(), BACKSLASH_SEPARATOR);
    }

    /// Retrieve a multi-value string DICOM tag. If the tag is not found, an empty vector is returned.
    template<typename A>
    [[nodiscard]] inline std::vector<std::string> getStringValues() const noexcept
    {
        A attribute;
        attribute.SetFromDataSet(m_dataset);

        const auto values = attribute.GetValues();

        std::vector<std::string> vector;

        for(std::size_t i = 0 ; i < attribute.GetNumberOfValues() ; ++i)
        {
            if constexpr(std::is_base_of_v<std::string, typename A::ArrayType>)
            {
                // Use trimmed string, as we don't care about DICOM string padding with space
                vector.push_back(shrink(values[i].Trim()));
            }
            else
            {
                vector.push_back(toString(values[i], A::GetVR()));
            }
        }

        return vector;
    }

    /// Set a DICOM tag value. If the value is null, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValue(const std::optional<typename A::ArrayType>& value)
    {
        if(value)
        {
            A attribute {};
            attribute.SetValue(*value);
            m_dataset.Replace(attribute.GetAsDataElement());
        }
        else
        {
            // We need to put an "empty" value. Since GDCM doesn't allow us to do it with non string values, we need to
            // hack the system. Sorry GDCM, but an Integer String definitively could be "unknown" and an empty string
            // **IS** valid..
            m_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
    }

    /// Set a string DICOM tag value. If the value is null, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setStringValue(const std::string& value)
    {
        if(value.empty())
        {
            // Force a real emtpy value..
            m_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;

            if constexpr(std::is_floating_point_v<typename A::ArrayType>)
            {
                attribute.SetValue(A::ArrayType(std::stod(value)));
            }
            else if constexpr(std::is_integral_v<typename A::ArrayType>)
            {
                // cspell: ignore stoll
                attribute.SetValue(A::ArrayType(std::stoll(value)));
            }
            else
            {
                // Maybe it works...
                attribute.SetValue(value);
            }

            m_dataset.Replace(attribute.GetAsDataElement());
        }
    }

    /// Set a multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setValues(const std::vector<typename A::ArrayType>& values)
    {
        if(values.empty())
        {
            // Force a real emtpy value..
            m_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
        }
        else
        {
            A attribute;
            attribute.SetValues(values.data(), std::uint32_t(values.size()), true);
            m_dataset.Replace(attribute.GetAsDataElement());
        }
    }

    /// Set a multi-value DICOM tag from a single string. Use '\' as separator.
    template<typename A>
    inline void setJoinedValues(const std::string& values)
    {
        // Split the original string
        std::vector<std::string> split;
        boost::split(split, values, boost::is_any_of(BACKSLASH_SEPARATOR));

        setStringValues<A>(split);
    }

    /// Set a string multi-value DICOM tag. If the vector is empty, the tag is replaced by and empty element.
    template<typename A>
    constexpr void setStringValues(const std::vector<std::string>& values)
    {
        if(!values.empty())
        {
            std::vector<typename A::ArrayType> array;
            array.reserve(values.size());

            for(const auto& value : values)
            {
                /// Yes this could happen, which means empty attribute
                if(!value.empty())
                {
                    if constexpr(std::is_floating_point_v<typename A::ArrayType>)
                    {
                        array.push_back(typename A::ArrayType(std::stod(value)));
                    }
                    else if constexpr(std::is_integral_v<typename A::ArrayType>)
                    {
                        // cspell: ignore stoll
                        array.push_back(typename A::ArrayType(std::stoll(value)));
                    }
                    else
                    {
                        // Maybe it works...
                        array.push_back(value);
                    }
                }
            }

            if(!array.empty())
            {
                A attribute;
                attribute.SetValues(array.data(), std::uint32_t(array.size()), true);
                m_dataset.Replace(attribute.GetAsDataElement());

                // Exit here...
                return;
            }
        }

        // Default: force a real emtpy value..
        m_dataset.Replace(gdcm::DataElement(A::GetTag(), 0, A::GetVR()));
    }

    //------------------------------------------------------------------------------

    template<typename A>
    inline void copyElement(const gdcm::DataSet& source)
    {
        copyElement(source, A::GetTag());
    }

    //------------------------------------------------------------------------------

    inline void copyElement(const gdcm::DataSet& source, const gdcm::Tag& tag)
    {
        if(source.FindDataElement(tag))
        {
            // Make an explicit copy
            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            gdcm::DataElement data_element(source.GetDataElement(tag));

            // Replace the element with the copy
            m_dataset.Replace(data_element);
        }
        else
        {
            // Tag not found, remove it
            m_dataset.Remove(tag);
        }
    }

    /// Pointer to the public class
    Series* const m_series {nullptr};

    /// Dicom data set instance
    gdcm::DataSet m_dataset;
};

Series::Series(Object::Key /*unused*/) :
    m_pimpl(std::make_unique<SeriesImpl>(this))
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

    m_pimpl->m_dataset = other->m_pimpl->m_dataset;

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

    m_pimpl->m_dataset = other->m_pimpl->m_dataset;

    BaseClass::deepCopy(source, cache);
}

//------------------------------------------------------------------------------

void Series::copyPatientModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
    m_pimpl->copyElement<gdcm::Keywords::FrameOfReferenceUID>(source_dataset);
    m_pimpl->copyElement<gdcm::Keywords::PositionReferenceIndicator>(source_dataset);
}

//------------------------------------------------------------------------------

void Series::copySOPCommonModule(const Series::csptr& source)
{
    // Unfortunately and contrary to DCMTK, GDCM does not provide a way to copy a module.
    // We have to copy each element, one bye one. This is a bit inelegant, but it is the only way.
    /// @note list of tag borrowed from DCMTK modhelp.cc
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    const auto& source_dataset = source->m_pimpl->m_dataset;
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
    if(m_pimpl->m_dataset.GetDES() != other.m_pimpl->m_dataset.GetDES())
    {
        return false;
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

std::string Series::getByteValue(std::uint16_t group, std::uint16_t element) const noexcept
{
    gdcm::Tag tag(group, element);
    if(m_pimpl->m_dataset.FindDataElement(tag))
    {
        if(const auto& data_element = m_pimpl->m_dataset.GetDataElement(tag); !data_element.IsEmpty())
        {
            if(const auto& byte_value = data_element.GetByteValue();
               byte_value != nullptr && byte_value->GetPointer() != nullptr)
            {
                return shrink(gdcm::String<>(byte_value->GetPointer(), byte_value->GetLength()).Trim());
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

void Series::setByteValue(std::uint16_t group, std::uint16_t element, const std::string& value)
{
    // Get the VR
    const gdcm::Tag tag(group, element);
    const gdcm::VR vr(gdcm::GetVRFromTag(tag));

    // Ensure the string is valid.
    const auto [size, fixed, padding] = getVRFormat(vr);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "The length of the value '"
            + value
            + "' is incorrect: "
            + std::to_string(value.size())
            + (fixed ? " != " : " > ") + std::to_string(size)
        ),
        (fixed && value.size() != size) || (!fixed && value.size() > size)
    );

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
    m_pimpl->m_dataset.Replace(data_element);
}

//------------------------------------------------------------------------------

std::vector<std::string> Series::getByteValues(std::uint16_t group, std::uint16_t element) const noexcept
{
    std::vector<std::string> values;

    const std::string joined = getByteValue(group, element);
    boost::split(values, joined, boost::is_any_of(BACKSLASH_SEPARATOR));

    return values;
}

//------------------------------------------------------------------------------

void Series::setByteValues(std::uint16_t group, std::uint16_t element, const std::vector<std::string>& values)
{
    const auto& joined = boost::join(values, BACKSLASH_SEPARATOR);
    setByteValue(group, element, joined);
}

//------------------------------------------------------------------------------

const gdcm::DataSet& Series::getDataSet() const noexcept
{
    return m_pimpl->m_dataset;
}

//------------------------------------------------------------------------------

gdcm::DataSet& Series::getDataSet() noexcept
{
    return m_pimpl->m_dataset;
}

//------------------------------------------------------------------------------

void Series::setDataSet(const gdcm::DataSet& dataset) noexcept
{
    m_pimpl->m_dataset = dataset;
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

std::string Series::getSOPInstanceUID() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::SOPInstanceUID>();
}

//------------------------------------------------------------------------------

void Series::setSOPInstanceUID(const std::string& sopInstanceUID)
{
    m_pimpl->setValue<gdcm::Keywords::SOPInstanceUID>(sopInstanceUID);
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

std::string ImageSeries::getAcquisitionDate() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::AcquisitionDate>();
}

//------------------------------------------------------------------------------

void ImageSeries::setAcquisitionDate(const std::string& acquisitionDate)
{
    m_pimpl->setValue<gdcm::Keywords::AcquisitionDate>(acquisitionDate);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getAcquisitionTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::AcquisitionTime>();
}

//------------------------------------------------------------------------------

void ImageSeries::setAcquisitionTime(const std::string& acquisitionTime)
{
    m_pimpl->setValue<gdcm::Keywords::AcquisitionTime>(acquisitionTime);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastBolusAgent() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusAgent>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusAgent(const std::string& contrastBolusAgent)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusAgent>(contrastBolusAgent);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastBolusRoute() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusRoute>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusRoute(const std::string& contrastBolusRoute)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusRoute>(contrastBolusRoute);
}

//------------------------------------------------------------------------------

std::optional<double> ImageSeries::getContrastBolusVolume() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusVolume>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusVolume(const std::optional<double>& contrastBolusVolume)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusVolume>(contrastBolusVolume);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastBolusStartTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusStartTime>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusStartTime(const std::string& contrastBolusStartTime)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusStartTime>(contrastBolusStartTime);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastBolusStopTime() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusStopTime>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusStopTime(const std::string& contrastBolusStopTime)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusStopTime>(contrastBolusStopTime);
}

//------------------------------------------------------------------------------

std::optional<double> ImageSeries::getContrastBolusTotalDose() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusTotalDose>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusTotalDose(const std::optional<double>& contrastBolusTotalDose)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusTotalDose>(contrastBolusTotalDose);
}

//------------------------------------------------------------------------------

std::vector<double> ImageSeries::getContrastFlowRates() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::ContrastFlowRate>();
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastFlowRate() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::ContrastFlowRate>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastFlowRates(const std::vector<double>& contrastFlowRates)
{
    m_pimpl->setValues<gdcm::Keywords::ContrastFlowRate>(contrastFlowRates);
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastFlowRate(const std::string& contrastFlowRates)
{
    m_pimpl->setJoinedValues<gdcm::Keywords::ContrastFlowRate>(contrastFlowRates);
}

//------------------------------------------------------------------------------

std::vector<double> ImageSeries::getContrastFlowDurations() const noexcept
{
    return m_pimpl->getValues<gdcm::Keywords::ContrastFlowDuration>();
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastFlowDuration() const noexcept
{
    return m_pimpl->getJoinedValues<gdcm::Keywords::ContrastFlowDuration>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastFlowDurations(const std::vector<double>& contrastFlowDurations)
{
    m_pimpl->setValues<gdcm::Keywords::ContrastFlowDuration>(contrastFlowDurations);
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastFlowDuration(const std::string& contrastFlowDurations)
{
    m_pimpl->setJoinedValues<gdcm::Keywords::ContrastFlowDuration>(contrastFlowDurations);
}

//------------------------------------------------------------------------------

std::string ImageSeries::getContrastBolusIngredient() const noexcept
{
    return m_pimpl->getStringValue<gdcm::Keywords::ContrastBolusIngredient>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusIngredient(const std::string& contrastBolusIngredient)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredient>(contrastBolusIngredient);
}

//------------------------------------------------------------------------------

std::optional<double> ImageSeries::getContrastBolusIngredientConcentration() const noexcept
{
    return m_pimpl->getValue<gdcm::Keywords::ContrastBolusIngredientConcentration>();
}

//------------------------------------------------------------------------------

void ImageSeries::setContrastBolusIngredientConcentration(
    const std::optional<double>& contrastBolusIngredientConcentration
)
{
    m_pimpl->setValue<gdcm::Keywords::ContrastBolusIngredientConcentration>(contrastBolusIngredientConcentration);
}

} // namespace sight::data
