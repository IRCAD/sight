/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
