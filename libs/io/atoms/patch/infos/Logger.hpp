/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/atoms/config.hpp"

#include <boost/shared_ptr.hpp>

#include <string>
#include <utility>
#include <vector>

namespace sight::io::atoms::patch
{

namespace infos
{

/**
 * @brief Logs events occurring during object patching.
 */
class IO_ATOMS_CLASS_API Logger
{
public:

    typedef std::ostringstream StreamType;
    typedef ::boost::shared_ptr<StreamType> StreamPtrType;

    /// Logger destructor
    IO_ATOMS_API ~Logger();

    /// Error message
    IO_ATOMS_API void error(const std::string& message);

    /// Bad cast message
    IO_ATOMS_API void badCast(const std::string& message);

    /// Out of range message
    IO_ATOMS_API void outOfRange(const std::string& message);

    /// Information message
    IO_ATOMS_API void info(const std::string& message);

    /// New attribute message
    IO_ATOMS_API void addAttribute(const std::string& message);

    /// Erase attribute message
    IO_ATOMS_API void eraseAttribute(const std::string& message);

    /// Replace attribute message
    IO_ATOMS_API void replaceAttribute(const std::string& message);

    //------------------------------------------------------------------------------

    static Logger& getLogger()
    {
        return s_logger;
    }

    IO_ATOMS_API static StreamPtrType getStream();

private:

    /// Logger constructor
    IO_ATOMS_API Logger();

    /// Default instance
    IO_ATOMS_API static Logger s_logger;

    /// Log stream
    IO_ATOMS_API static StreamPtrType s_stream;
};

} //infos

} //fwAtomHelper
