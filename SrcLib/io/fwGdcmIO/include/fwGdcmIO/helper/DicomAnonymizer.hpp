/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMANONYMIZER_HPP__
#define __FWGDCMIO_HELPER_DICOMANONYMIZER_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwZip/WriteZipArchive.hpp>

#include <gdcmAnonymizer.h>
#include <gdcmDicts.h>
#include <gdcmStringFilter.h>

#include <boost/filesystem/path.hpp>

#include <map>
#include <set>
#include <string>
#include <iostream>

namespace fwJobs
{
class IJob;
class Observer;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief   This class contains helpers to anonymize dicom files on filesystem.
 *          Anonymization is performed according to the DICOM standard - Part 15 - Annex E - Basic Profile
 */
class FWGDCMIO_CLASS_API DicomAnonymizer
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomAnonymizer), (()), new DicomAnonymizer );

    /// Constructor
    FWGDCMIO_API DicomAnonymizer();

    /// Destructor
    FWGDCMIO_API virtual ~DicomAnonymizer();

    /// Map used to maintain UID consistency
    typedef std::map< std::string, std::string > UIDMap;

    /// Map used to store exception value
    typedef std::map< ::gdcm::Tag, std::string > ExceptionTagMapType;

    /// Anonymize a folder containing Dicom files
    FWGDCMIO_API void anonymize(const ::boost::filesystem::path& dirPath);

    FWGDCMIO_API void anonymize(std::istream& inputStream, std::ostream& outputStream);

    /// Add an exceptional value for a tag
    FWGDCMIO_API void addExceptionTag(uint16_t group, uint16_t element, const std::string& value = "");

    /**
     * @brief Copy a directory recursively.
     * @param input Input file
     * @param output Output file
     */
    FWGDCMIO_API static void copyDirectory(::boost::filesystem::path input, ::boost::filesystem::path output);

    /// Get job observer
    FWGDCMIO_API SPTR(::fwJobs::IJob) getJob() const;

    /// Return next file index
    FWGDCMIO_API unsigned int getNextIndex();

    /// Reset file index to 0
    FWGDCMIO_API void resetIndex();

    /// The removed tag will not be process by anonimization tag
    FWGDCMIO_API void removeAnonymizeTag(const ::gdcm::Tag& tag);

private:

    void anonymizationProcess(const ::boost::filesystem::path& dirPath);

    ///D : replace with a non-zero length value that may be a dummy value and consistent with the VR
    void applyActionCodeD(const ::gdcm::Tag& tag);

    /**
     * Z : replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with
     * the VR
     *
     * Z/D : Z unless D is required to maintain IOD conformance (Type 2 versus Type 1)
     *
     * @note This method applies action code D only.
     */
    void applyActionCodeZ(const ::gdcm::Tag& tag);

    /**
     * X : remove tag
     * X/Z : X unless Z is required to maintain IOD conformance (Type 3 versus Type 2)
     * X/D : X unless D is required to maintain IOD conformance (Type 3 versus Type 1)
     * X/Z/D : X unless Z or D is required to maintain IOD conformance (Type 3 versus Type 2 versus Type 1)
     *
     * X/Z/U* : X unless Z or replacement of contained instance UIDs (U) is required to maintain IOD conformance
     * (Type 3 versus Type 2 versus Type 1 sequences containing UID references)
     *
     * @note This method applies action code X only.
     */
    void applyActionCodeX(const ::gdcm::Tag& tag);

    /// K : keep (unchanged for non-sequence attributes, cleaned for sequences)
    void applyActionCodeK(const ::gdcm::Tag& tag);

    /**
     * C : clean, that is replace with values of similar meaning known not to contain identifying information and
     * consistent with the VR
     */
    void applyActionCodeC(const ::gdcm::Tag& tag);

    /// U : replace with a non-zero length UID that is internally consistent within a set of Instances
    void applyActionCodeU(const ::gdcm::Tag& tag);

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
    SPTR(::fwJobs::Observer) m_observer;

    /// Archiving boolean used to compute correct progress when archiving
    bool m_archiving;

    /// Index of anonymizer
    unsigned int m_fileIndex;

    /**
     * @name Default tags to be processed with associated action code.
     * @{ */
    typedef std::set< ::gdcm::Tag > TagContainerType;

    static const TagContainerType s_ACTION_CODE_D_TAGS;
    static const TagContainerType s_ACTION_CODE_Z_TAGS;
    static const TagContainerType s_ACTION_CODE_X_TAGS;
    static const TagContainerType s_ACTION_CODE_K_TAGS;
    static const TagContainerType s_ACTION_CODE_C_TAGS;
    static const TagContainerType s_ACTION_CODE_U_TAGS;
    /**  @} */

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
};

} // namespace helper
} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_DICOMANONYMIZER_HPP__*/
