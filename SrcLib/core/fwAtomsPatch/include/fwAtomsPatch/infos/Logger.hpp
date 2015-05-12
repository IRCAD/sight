/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_INFOS_LOGGER_HPP__
#define __FWATOMSPATCH_INFOS_LOGGER_HPP__

#include <string>
#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
namespace infos
{

/**
 * @class Logger
 * @brief Logs events occurring during object patching.
 */
class FWATOMSPATCH_CLASS_API Logger
{
public:

    typedef std::ostringstream StreamType;
    typedef ::boost::shared_ptr< StreamType  > StreamPtrType;

    /// Logger destructor
    FWATOMSPATCH_API ~Logger();

    /// Error message
    FWATOMSPATCH_API void error(const std::string& message);

    /// Bad cast message
    FWATOMSPATCH_API void badCast(const std::string& message);

    /// Out of range message
    FWATOMSPATCH_API void outOfRange(const std::string& message);

    /// Information message
    FWATOMSPATCH_API void info(const std::string& message);

    /// New attribute message
    FWATOMSPATCH_API void addAttribute(const std::string& message);

    /// Erase attribute message
    FWATOMSPATCH_API void eraseAttribute(const std::string& message);

    /// Replace attribute message
    FWATOMSPATCH_API void replaceAttribute(const std::string& message);

    static Logger &getLogger()
    {
        return s_logger;
    }

    FWATOMSPATCH_API static StreamPtrType getStream();

private:

    /// Logger constructor
    FWATOMSPATCH_API Logger();

    /// Default instance
    FWATOMSPATCH_API static Logger s_logger;

    /// Log stream
    FWATOMSPATCH_API static StreamPtrType s_stream;
};

} //infos
} //fwAtomHelper

#endif /* __FWATOMSPATCH_INFOS_LOGGER_HPP__ */
