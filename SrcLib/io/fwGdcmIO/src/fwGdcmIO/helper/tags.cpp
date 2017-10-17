/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/tags.hpp"
#include "fwGdcmIO/helper/CsvIO.hpp"
#include "fwGdcmIO/exception/InvalidTag.hpp"

#include <fwCore/exceptionmacros.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <fstream>

namespace fwGdcmIO
{

namespace helper
{

//------------------------------------------------------------------------------

::gdcm::Tag getGdcmTag(const std::string& group, const std::string& element)
{
    SLM_ASSERT("Group and element can not be empty", !group.empty() && !element.empty());

    typedef uint16_t DestType;
    DestType groupDest;
    DestType elementDest;

    try
    {
        unsigned long groupL = std::stoul(group, nullptr, 16);
        unsigned long elementL = std::stoul(element, nullptr, 16);

        groupDest = ::boost::numeric_cast< DestType >(groupL);
        elementDest = ::boost::numeric_cast< DestType >(elementL);
    }
    catch(std::out_of_range& e)
    {
        FW_RAISE_EXCEPTION(::fwGdcmIO::exception::InvalidTag(
                    "Unable to read DICOM tag from '" + group + "," + element + "' : " + e.what()));
    }
    catch(std::invalid_argument& e)
    {
        FW_RAISE_EXCEPTION(::fwGdcmIO::exception::InvalidTag(
                    "Unable to read DICOM tag from '" + group + "," + element + "' : " + e.what()));
    }
    catch(::boost::bad_numeric_cast& e)
    {
        FW_RAISE_EXCEPTION(::fwGdcmIO::exception::InvalidTag(
                    "Unable to read DICOM tag from '" + group + "," + element + "' : " + e.what()));
    }

    return ::gdcm::Tag(groupDest, elementDest);
}

//------------------------------------------------------------------------------

PrivateTagVecType loadPrivateTags(const ::boost::filesystem::path& tagsPath)
{
    SLM_ASSERT("File '" + tagsPath.string() + "' must exists",
            ::boost::filesystem::exists(tagsPath) && ::boost::filesystem::is_regular_file(tagsPath));

    PrivateTagVecType privateTags;
    auto csvStream = std::ifstream(tagsPath.string());
    ::fwGdcmIO::helper::CsvIO reader(csvStream);
    ::fwGdcmIO::helper::CsvIO::TokenContainerType tag = reader.getLine();

    while(!tag.empty())
    {
        OSLM_WARN_IF("Unxpected token count : " << tag.size() << " (3 expected : group, element, manufacturer)",
                tag.size() != 3);
        FW_RAISE_IF("Unable to read private tag file", tag.size() < 2);

        privateTags.push_back(::fwGdcmIO::helper::getGdcmTag(tag[0], tag[1]));
        tag = reader.getLine();
    }

    return privateTags;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO

