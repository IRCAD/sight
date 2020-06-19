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

#ifndef __FWATOMSPATCH_VERSIONDESCRIPTOR_HPP__
#define __FWATOMSPATCH_VERSIONDESCRIPTOR_HPP__

#include <map>
#include <string>

#include <fwCore/BaseObject.hpp>

#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

/**
 * @brief Version descriptor used to identify a version.
 */
class FWATOMSPATCH_CLASS_API VersionDescriptor
{

public:

    /**
     * @brief Struct used to compare two VersionDescriptor
     */
    struct Compare {
        bool operator() (VersionDescriptor a, VersionDescriptor b) const
        {
            return a.getVersionName() < b.getVersionName();
        }
    };

    /**
     * @brief VersionIDs used to link type and version
     */
    typedef std::map< std::string, std::string > VersionsType;

    /**
     * @brief Constructor
     */
    VersionDescriptor();

    /**
     * @brief Constructor
     */
    VersionDescriptor(const std::string& context, const std::string& versionName, const VersionsType& versions);

    /**
     * @brief Destructor
     */
    ~VersionDescriptor();

    /// Returns context name.
    const std::string& getContext() const
    {
        return m_context;
    }

    /// Returns version name.
    const std::string& getVersionName() const
    {
        return m_versionName;
    }

    /// Returns versions.
    const VersionsType& getVersions() const
    {
        return m_versions;
    }

private:

    /// Context name.
    std::string m_context;

    /// Version name.
    std::string m_versionName;

    /// Versions.
    VersionsType m_versions;
};

} // fwAtomsPatch

#endif /* __FWATOMSPATCH_VERSIONDESCRIPTOR_HPP__ */
