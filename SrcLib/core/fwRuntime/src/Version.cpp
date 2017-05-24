/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/Version.hpp"

#include "fwRuntime/RuntimeException.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <cassert>
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
        ::boost::regex regex("(\\d+)([-.](\\d+)){0,1}([-.](\\d+)){0,1}");
        ::boost::smatch what;
        if( ::boost::regex_match(version, what, regex) )
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
        auto& res = os << version.m_major << "-" << version.m_minor;

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
