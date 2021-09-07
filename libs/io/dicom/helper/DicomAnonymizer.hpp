/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <io/zip/WriteZipArchive.hpp>

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

class IJob;
class Observer;

}

namespace sight::io::dicom
{

namespace helper
{

/**
 * @brief This class contains helpers to anonymize dicom files on filesystem.
 *        Anonymization is performed according to the DICOM standard - Part 15 - Annex E - Basic Profile
 */
class IO_DICOM_CLASS_API DicomAnonymizer
{
public:

    SIGHT_DECLARE_CLASS(DicomAnonymizer);

    //------------------------------------------------------------------------------

    static sptr New()
    {
        return std::make_shared<DicomAnonymizer>();
    }

    /// Constructor
    IO_DICOM_API DicomAnonymizer();

    /// Destructor
    IO_DICOM_API virtual ~DicomAnonymizer();

    /// Map used to maintain UID consistency
    typedef std::map<std::string, std::string> UIDMap;

    /// Map used to store exception value
    typedef std::map< ::gdcm::Tag, std::string> ExceptionTagMapType;

    /// Anonymize a folder containing Dicom files
    IO_DICOM_API void anonymize(const std::filesystem::path& dirPath);

    IO_DICOM_API void anonymize(std::istream& inputStream, std::ostream& outputStream);

    /// Add an exceptional value for a tag
    IO_DICOM_API void addExceptionTag(uint16_t group, uint16_t element, const std::string& value = "");

    /**
     * @brief Copy a directory recursively.
     * @param input Input file
     * @param output Output file
     */
    IO_DICOM_API static void copyDirectory(
        const std::filesystem::path& input,
        const std::filesystem::path& output
    );

    /// Get job observer
    IO_DICOM_API SPTR(core::jobs::IJob) getJob() const;

    /// Return next file index
    IO_DICOM_API unsigned int getNextIndex();

    /// Reset file index to 0
    IO_DICOM_API void resetIndex();

    /// The removed tag will not be process by anonymization tag
    IO_DICOM_API void removeAnonymizeTag(const ::gdcm::Tag& tag);

    /// Set Reference date for shifting
    IO_DICOM_API void setReferenceDate(const ::boost::gregorian::date& referenceDate);

    /**
     * @brief Add a date tag that must be shifted.
     * The shift is made according to the interval between the date and the reference date.
     * @param dateTag Date tag that must be shifted. (This tag shall be a date: VR shall be DA)
     *
     * @note The shift is done from Jan 1, 1900.
     */
    IO_DICOM_API void addShiftDateTag(const ::gdcm::Tag& tag);

    /**
     * @brief Tells the anonymizer to do not anonymize the given private tag.
     *
     * @param tag private tag to be preserved from anonymisation
     */
    IO_DICOM_API void preservePrivateTag(const ::gdcm::Tag& tag);

    /**
     * @name Access tags according to their associated action code
     * @{ */
    typedef std::set< ::gdcm::Tag> TagContainerType;
    IO_DICOM_API const TagContainerType& getActionCodeDTags();
    IO_DICOM_API const TagContainerType& getActionCodeZTags();
    IO_DICOM_API const TagContainerType& getActionCodeXTags();
    IO_DICOM_API const TagContainerType& getActionCodeKTags();
    IO_DICOM_API const TagContainerType& getActionCodeCTags();
    IO_DICOM_API const TagContainerType& getActionCodeUTags();
/**  @} */

private:

    void anonymizationProcess(const std::filesystem::path& dirPath);

    ///D: replace with a non-zero length value that may be a dummy value and consistent with the VR
    void applyActionCodeD(const ::gdcm::Tag& tag);

    /**
     * Z: replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with
     * the VR
     *
     * Z/D: Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
     *
     * @note This method applies action code D only.
     */
    void applyActionCodeZ(const ::gdcm::Tag& tag);

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
    void applyActionCodeX(const ::gdcm::Tag& tag);

    /// K: keep (unchanged for non-sequence attributes, cleaned for sequences)
    void applyActionCodeK(const ::gdcm::Tag& tag);

    /**
     * C: clean, that is replace with values of similar meaning known not to contain identifying information and
     * consistent with the VR
     */
    void applyActionCodeC(const ::gdcm::Tag& tag);

    /// U: if UID is not empty, replace with a non-zero length UID
    /// that is internally consistent within a set of Instances
    void applyActionCodeU(const ::gdcm::Tag& tag);

    /**
     * Shift date according to the interval between the date and the reference date.
     *
     * @note The shift is done from Jan 1, 1900.
     */
    void applyActionShiftDate(const ::gdcm::Tag& tag);

    /// Generate a value consistent with the VR
    void generateDummyValue(const ::gdcm::Tag& tag);

    /// Anonymizer
    ::gdcm::Anonymizer m_anonymizer;

    /// String filter used to convert DataElement to String
    ::gdcm::StringFilter m_stringFilter;

    /// Public Dicom Dictionary
    const ::gdcm::Dict& m_publicDictionary;

    /// UID Map
    UIDMap m_uidMap;

    /// Exception tag map
    ExceptionTagMapType m_exceptionTagMap;

    /// Number of Tags processed (Without Curve Data, Overlay Comments, Overlay Data and Private attributes)
    static const unsigned int s_NUMBER_OF_TAGS;

    /// Job observer
    SPTR(core::jobs::Observer) m_observer;

    /// Archiving boolean used to compute correct progress when archiving
    bool m_archiving;

    /// Index of anonymizer
    unsigned int m_fileIndex;

    /// Reference date for shifting
    ::boost::gregorian::date m_referenceDate;

    /**
     * @name Tags to be processed with associated action code.
     * Exception tags added through addExceptionTag will be removed.
     * @see addExceptionTag
     * @{ */
    TagContainerType m_actionCodeDTags;
    TagContainerType m_actionCodeZTags;
    TagContainerType m_actionCodeXTags;
    TagContainerType m_actionCodeKTags;
    TagContainerType m_actionCodeCTags;
    TagContainerType m_actionCodeUTags;
    /**  @} */

    /// List of date tags that must be shifted
    TagContainerType m_actionShiftDateTags;

    /// List of private tags to be preserved from anonymisation
    io::dicom::helper::PrivateTagVecType m_privateTags;
};

} // namespace helper

} // namespace sight::io::dicom
