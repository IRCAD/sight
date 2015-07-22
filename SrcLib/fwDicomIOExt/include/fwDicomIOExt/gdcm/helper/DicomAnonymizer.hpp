/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_GDCM_HELPER_DICOMANONYMIZER_HPP__
#define __FWDICOMIOEXT_GDCM_HELPER_DICOMANONYMIZER_HPP__

#include <map>
#include <string>

#include <boost/filesystem/path.hpp>

#include <gdcmAnonymizer.h>
#include <gdcmDicts.h>
#include <gdcmStringFilter.h>

#include <fwTools/ProgressAdviser.hpp>

#include <fwZip/WriteZipArchive.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace gdcm
{
namespace helper
{

/**
 * @brief   This class contains helpers to anonymize dicom files on filesystem.
 *          Anonymization is performed according to the DICOM standard - Part 15 - Annex E - Basic Profile
 * @class   DicomAnonymizer
 * @date    2013.
 */
class FWDICOMIOEXT_CLASS_API DicomAnonymizer : public ::fwTools::ProgressAdviser
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomAnonymizer), (()), new DicomAnonymizer );

    /// Constructor
    FWDICOMIOEXT_API DicomAnonymizer();

    /// Destructor
    FWDICOMIOEXT_API virtual ~DicomAnonymizer();

    /// Map used to maintain UID consistency
    typedef std::map< std::string, std::string > UIDMap;

    /// Map used to store exception value
    typedef std::map< ::gdcm::Tag, std::string > ExceptionTagMap;

    /// Anonymize a folder containing Dicom files
    FWDICOMIOEXT_API void anonymize(const ::boost::filesystem::path &dirPath);

    /// Anonymize a folder containing Dicom files and write them in a zip file
    FWDICOMIOEXT_API void anonymizeAndZip(const ::boost::filesystem::path &dirPath,
                                          ::fwZip::IWriteArchive::sptr archive);

    /// Add an exceptional value for a tag
    FWDICOMIOEXT_API void addExceptionTag(uint16_t group, uint16_t element, const std::string &value);

    /**
     * @brief Copy a directory recursively.
     * @param input Input file
     * @param output Output file
     */
    FWDICOMIOEXT_API static void copyDirectory(::boost::filesystem::path input, ::boost::filesystem::path output);

private:
    /// Process a tag of a Dicom file
    void processTag(const ::gdcm::Tag& tag, const std::string &actionCode);

    /// Apply Action Code D
    void applyActionCodeD(const ::gdcm::Tag& tag);

    /// Apply Action Code Z
    void applyActionCodeZ(const ::gdcm::Tag& tag);

    /// Apply Action Code X
    void applyActionCodeX(const ::gdcm::Tag& tag);

    /// Apply Action Code K
    void applyActionCodeK(const ::gdcm::Tag& tag);

    /// Apply Action Code C
    void applyActionCodeC(const ::gdcm::Tag& tag);

    /// Apply Action Code U
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

    /// Tag Index
    unsigned int m_tagIndex;

    /// Total Number of Tags (Number of Tags x Number of files)
    unsigned int m_totalNumberOfTags;

    /// Exception tag map
    ExceptionTagMap m_exceptionTagMap;

    /// Number of Tags processed (Without Curve Data, Overlay Comments, Overlay Data and Private attributes)
    static const unsigned int s_NUMBER_OF_TAGS;
};

} // namespace helper
} // namespace gdcm
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_GDCM_HELPER_DICOMANONYMIZER_HPP__*/
