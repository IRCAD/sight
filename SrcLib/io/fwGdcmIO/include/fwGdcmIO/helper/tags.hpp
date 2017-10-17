/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_TAGS_HPP__
#define __FWGDCMIO_HELPER_TAGS_HPP__

#include "fwGdcmIO/config.hpp"

#include <boost/filesystem/path.hpp>

#include <gdcmTag.h>

#include <string>
#include <vector>



namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief Parse group and element strings and return a ::gdcm::Tag instance
 *
 * @param group the group part of the tag
 * @param element the element part of the tag
 *
 * @return a ::gdcm::Tag instance
 */
FWGDCMIO_API ::gdcm::Tag getGdcmTag(const std::string& group, const std::string& element);

/**
 * @brief List of private GDCM tags type
 *
 * @note Do not use ::gdcm::PrivateTag because it casts element part as uint8_t
 * and we may have uint16_t (for instance Philips tag 0x00E1,0x1022).
 * See part 05 of Dicom Standard, section 7.8.1.
 */
typedef std::vector< ::gdcm::Tag > PrivateTagVecType;

/**
 * @brief Load a set of private tags from given CSV file.
 * Each line of the file must be formatted this way : group,element,owner
 * Example :
@verbatim
0x00E1,0x1022,Philips
@endverbatim
 *
 * @pre file must exist
 * @param tagsPath path to CSV file containing private tags
 *
 * @return a list of ::gdcm::PrivateTag instances
 */
FWGDCMIO_API PrivateTagVecType loadPrivateTags(const ::boost::filesystem::path& tagsPath);

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_TAGS_HPP__

