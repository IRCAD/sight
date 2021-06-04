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

#include "io/session/config.hpp"

#include <core/location/ILocation.hpp>

#include <filesystem>

namespace sight::io::session
{

namespace detail
{

/// Interface to define a location that holds what is needed to de/serialize a session.
class IO_SESSION_CLASS_API ISession : public core::location::ILocation
{
public:

    SIGHT_DECLARE_CLASS(ISession, core::location::ILocation);

    /// String serialization function
    inline std::string toString() const override;

protected:

    /// Constructor
    IO_SESSION_API ISession() = default;

    /// Destructor
    IO_SESSION_API virtual ~ISession() = default;

    /// Return the default index file path used to store the objects tree
    inline virtual std::filesystem::path getIndexFilePath() const;
};

//------------------------------------------------------------------------------

inline std::string ISession::toString() const
{
    return core::location::ILocation::toString();
}

//------------------------------------------------------------------------------

inline std::filesystem::path ISession::getIndexFilePath() const
{
    return "/index.json";
}

} // namespace detail

} // namespace sight::io::session
