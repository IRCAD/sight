/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwRuntime/Version.hpp"

#include "fwRuntime/RuntimeException.hpp"

#include <boost/lexical_cast.hpp>

#include <cassert>
#include <regex>
#include <sstream>

namespace fwRuntime
{

Version::Version() :
    m_major(-1),
    m_minor(-1),
    m_patch(-1),
    m_defined(false)
{
}

//------------------------------------------------------------------------------

Version::Version(const std::string& version) :
    m_major(-1),
    m_minor(-1),
    m_patch(-1),
    m_defined(false)
{
    if( version.empty() == false )
    {
        std::regex regex("(\\d+)([-.](\\d+)){0,1}([-.](\\d+)){0,1}");
        std::smatch what;
        if( std::regex_match(version, what, regex) )
        {
            const std::string& major = what[1].str();
            const std::string& minor = what[3].str();
            const std::string& patch = what[5].str();

            assert(!major.empty());
            if(!major.empty())
            {
                m_major = ::boost::lexical_cast<int>( major );
            }

            assert(!minor.empty());
            if(!minor.empty())
            {
                m_minor = ::boost::lexical_cast<int>( minor );
            }

            if(!patch.empty())
            {
                m_patch = ::boost::lexical_cast<int>( patch );
            }

            m_defined = true;
        }
        else
        {
            throw RuntimeException(version + " : invalid version.");
        }
    }
}

//------------------------------------------------------------------------------

Version::Version(const int major, const int minor, const int patch) :
    m_major(major),
    m_minor(minor),
    m_patch(patch),
    m_defined(true)
{
}

//------------------------------------------------------------------------------

const std::string Version::string() const
{
    std::ostringstream os;
    os << *this;
    return os.str();
}

//------------------------------------------------------------------------------

bool Version::operator==(const Version& version) const
{
    if( m_defined && version.m_defined )
    {
        return m_major == version.m_major && m_minor == version.m_minor && m_patch == version.m_patch;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Version& version)
{
    if(version.m_defined)
    {
        auto& res = os << version.m_major << "." << version.m_minor;

        if(version.m_patch != -1)
        {
            res << "-" << version.m_patch;
        }

        return res;
    }
    else
    {
        return os << "version-not-defined";
    }
}

} // namespace fwRuntime
