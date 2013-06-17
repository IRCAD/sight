/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        bool operator() (VersionDescriptor a, VersionDescriptor b) const {
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
    const std::string& getContext() const { return m_context; }

    /// Returns version name.
    const std::string& getVersionName() const { return m_versionName; }

    /// Returns versions.
    const VersionsType& getVersions() const { return m_versions; }

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
