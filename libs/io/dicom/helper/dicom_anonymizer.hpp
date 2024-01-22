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

#pragma once

#include "io/dicom/config.hpp"
#include "io/dicom/helper/tags.hpp"

#include <core/macros.hpp>

#include <boost/date_time/gregorian/gregorian_types.hpp>

#include <gdcmAnonymizer.h>
#include <gdcmDicts.h>
#include <gdcmStringFilter.h>

#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <string>

namespace sight::core::jobs
{

class base;
class observer;

} // namespace sight::core::jobs

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to anonymize dicom files on filesystem.
 *        Anonymization is performed according to the DICOM standard - Part 15 - Annex E - Basic Profile
 */
class IO_DICOM_CLASS_API dicom_anonymizer
{
public:

    SIGHT_DECLARE_CLASS(dicom_anonymizer);

    /// Constructor
    IO_DICOM_API dicom_anonymizer();

    /// Destructor
    IO_DICOM_API virtual ~dicom_anonymizer();

    /// Map used to maintain UID consistency
    using uid_map = std::map<std::string, std::string>;

    /// Map used to store exception value
    using exception_tag_map_t = std::map<gdcm::Tag, std::string>;

    /// Anonymize a folder containing Dicom files
    IO_DICOM_API void anonymize(const std::filesystem::path& _dir_path);

    IO_DICOM_API void anonymize(std::istream& _input_stream, std::ostream& _output_stream);

    /// Add an exceptional value for a tag
    IO_DICOM_API void add_exception_tag(uint16_t _group, uint16_t _element, const std::string& _value = "");

    /**
     * @brief Copy a directory recursively.
     * @param _input Input file
     * @param _output Output file
     */
    IO_DICOM_API static void copy_directory(
        const std::filesystem::path& _input,
        const std::filesystem::path& _output
    );

    /// Get job observer
    [[nodiscard]] IO_DICOM_API SPTR(core::jobs::base) get_job() const;

    /// Return next file index
    IO_DICOM_API unsigned int get_next_index();

    /// Reset file index to 0
    IO_DICOM_API void reset_index();

    /// The removed tag will not be process by anonymization tag
    IO_DICOM_API void remove_anonymize_tag(const gdcm::Tag& _tag);

    /// Set Reference date for shifting
    IO_DICOM_API void set_reference_date(const boost::gregorian::date& _reference_date);

    /**
     * @brief Add a date tag that must be shifted.
     * The shift is made according to the interval between the date and the reference date.
     * @param _tag Date tag that must be shifted. (This tag shall be a date: VR shall be DA)
     *
     * @note The shift is done from Jan 1, 1900.
     */
    IO_DICOM_API void add_shift_date_tag(const gdcm::Tag& _tag);

    /**
     * @brief Tells the anonymizer to do not anonymize the given private tag.
     *
     * @param _tag private tag to be preserved from anonymization
     */
    IO_DICOM_API void preserve_private_tag(const gdcm::Tag& _tag);

    /**
     * @name Access tags according to their associated action code
     * @{ */
    using tag_container_t = std::set<gdcm::Tag>;
    IO_DICOM_API const tag_container_t& get_action_code_d_tags();
    IO_DICOM_API const tag_container_t& get_action_code_z_tags();
    IO_DICOM_API const tag_container_t& get_action_code_x_tags();
    IO_DICOM_API const tag_container_t& get_action_code_k_tags();
    IO_DICOM_API const tag_container_t& get_action_code_c_tags();
    IO_DICOM_API const tag_container_t& get_action_code_u_tags();
/**  @} */

private:

    void anonymization_process(const std::filesystem::path& _dir_path);

    ///D: replace with a non-zero length value that may be a dummy value and consistent with the VR
    void apply_action_code_d(const gdcm::Tag& _tag);

    /**
     * Z: replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with
     * the VR
     *
     * Z/D: Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
     *
     * @note This method applies action code D only.
     */
    void apply_action_code_z(const gdcm::Tag& _tag);

    /**
     * X: remove tag
     * X/Z: X unless Z is required to maintain IOD conformance (Type 3 versus Type 2)
     * X/D: X unless D is required to maintain IOD conformance (Type 3 versus Type 1)
     * X/Z/D: X unless Z or D is required to maintain IOD conformance (Type 3 versus Type 2 versus Type 1)
     *
     * X/Z/U*: X unless Z or replacement of contained instance UIDs (U) is required to maintain IOD conformance
     * (Type 3 versus Type 2 versus Type 1 sequences containing UID references)
     *
     * @note This method applies action code X only.
     */
    void apply_action_code_x(const gdcm::Tag& _tag);

    /// K: keep (unchanged for non-sequence attributes, cleaned for sequences)
    void apply_action_code_k(const gdcm::Tag& _tag);

    /**
     * C: clean, that is replace with values of similar meaning known not to contain identifying information and
     * consistent with the VR
     */
    static void apply_action_code_c(const gdcm::Tag& _tag);

    /// U: if UID is not empty, replace with a non-zero length UID
    /// that is internally consistent within a set of Instances
    void apply_action_code_u(const gdcm::Tag& _tag);

    /**
     * Shift date according to the interval between the date and the reference date.
     *
     * @note The shift is done from Jan 1, 1900.
     */
    void apply_action_shift_date(const gdcm::Tag& _tag);

    /// Generate a value consistent with the VR
    void generate_dummy_value(const gdcm::Tag& _tag);

    /// Anonymizer
    gdcm::Anonymizer m_anonymizer;

    /// String filter used to convert DataElement to String
    gdcm::StringFilter m_string_filter;

    /// Public Dicom Dictionary
    const gdcm::Dict& m_public_dictionary;

    /// UID Map
    uid_map m_uid_map;

    /// Exception tag map
    exception_tag_map_t m_exception_tag_map;

    /// Number of Tags processed (Without Curve Data, Overlay Comments, Overlay Data and Private attributes)
    static const unsigned int NUMBER_OF_TAGS;

    /// Job observer
    SPTR(core::jobs::observer) m_observer;

    /// Archiving boolean used to compute correct progress when archiving
    bool m_archiving {false};

    /// Index of anonymizer
    unsigned int m_file_index {0};

    /// Reference date for shifting
    boost::gregorian::date m_reference_date;

    /**
     * @name Tags to be processed with associated action code.
     * Exception tags added through addExceptionTag will be removed.
     * @see addExceptionTag
     * @{ */
    tag_container_t m_action_code_d_tags;
    tag_container_t m_action_code_z_tags;
    tag_container_t m_action_code_x_tags;
    tag_container_t m_action_code_k_tags;
    tag_container_t m_action_code_c_tags;
    tag_container_t m_action_code_u_tags;
    /**  @} */

    /// List of date tags that must be shifted
    tag_container_t m_action_shift_date_tags;

    /// List of private tags to be preserved from anonymisation
    io::dicom::helper::private_tag_vec_t m_private_tags;
};

} // namespace sight::io::dicom::helper
