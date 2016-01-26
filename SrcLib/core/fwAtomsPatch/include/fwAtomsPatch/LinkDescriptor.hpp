/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_LINKDESCRIPTOR_HPP__
#define __FWATOMSPATCH_LINKDESCRIPTOR_HPP__

#include <map>

#include "fwCore/BaseObject.hpp"
#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

/**
 * @brief Link descriptor used to identify a link between two versions.
 **/
class FWATOMSPATCH_CLASS_API LinkDescriptor
{

public:

    /// Struct used to compare two LinkDescriptor
    struct Compare {
        bool operator() (LinkDescriptor a, LinkDescriptor b) const
        {
            return (a.m_originVersion+a.m_targetVersion) < (b.m_originVersion+b.m_targetVersion);
        }
    };

    /// VersionID used to link type and version
    typedef std::pair< std::string, std::string > VersionIDType;

    /// Links used to link two versions.
    typedef std::map< VersionIDType, VersionIDType > LinksType;

    /// Constructor
    LinkDescriptor();

    /// Constructor
    LinkDescriptor(const std::string& context, const std::string& originVersion, const std::string& targetVersion,
                   const std::string& patcher, LinksType links, int weight = 1);

    /// Destructor
    ~LinkDescriptor();

    /// Returns link weight.
    int getWeight() const
    {
        return m_weight;
    }

    /// Returns context name.
    const std::string& getContext() const
    {
        return m_context;
    }

    /// Returns origin version.
    const std::string& getOriginVersion() const
    {
        return m_originVersion;
    }

    /// Returns target version.
    const std::string& getTargetVersion() const
    {
        return m_targetVersion;
    }

    /// Returns patcher classname.
    const std::string& getPatcher() const
    {
        return m_patcher;
    }

    /// Returns map of links between versions.
    const LinksType& getLinks() const
    {
        return m_links;
    }

private:

    friend class VersionsGraph;

    /// Weight of the link
    int m_weight;

    /// Context in which the link takes place
    std::string m_context;

    /// Origin version
    std::string m_originVersion;

    /// Target version
    std::string m_targetVersion;

    /// Patcher name
    std::string m_patcher;

    /// Links between versions.
    LinksType m_links;
};



} // fwAtomsPatch

#endif /* __FWATOMSPATCH_LINKDESCRIPTOR_HPP__ */
