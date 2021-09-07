/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/atoms/patch/VersionsManager.hpp"

#include "io/atoms/patch/exceptions/BadExtension.hpp"
#include "io/atoms/patch/exceptions/MissingInformation.hpp"
#include "io/atoms/patch/types.hpp"

#include <core/exceptionmacros.hpp>
#include <core/spyLog.hpp>

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <camp/class.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::atoms::patch
{

//------------------------------------------------------------------------------

std::string getValue(
    const ::boost::property_tree::ptree& node,
    const std::string& name,
    const std::filesystem::path& filePath
)
{
    std::string value;
    try
    {
        value = node.get<std::string>(name);
    }
    catch(::boost::property_tree::ptree_bad_path&)
    {
        SIGHT_THROW_EXCEPTION(
            io::atoms::patch::exceptions::MissingInformation(
                name + " information are missing in '" + filePath.string() + "'."
            )
        );
    }

    return value;
}

SPTR(VersionsManager) VersionsManager::s_default = std::make_shared<VersionsManager>();

// ----------------------------------------------------------------------------

VersionsManager::VersionsManager()
{
}

// ----------------------------------------------------------------------------

VersionsManager::~VersionsManager()
{
}

// ----------------------------------------------------------------------------

void VersionsManager::buildVersionTable(const std::string& dirPath)
{
    core::mt::WriteLock lock(m_versionMutex);
    for(std::filesystem::recursive_directory_iterator end, dir(dirPath) ; dir != end ; ++dir)
    {
        if(!std::filesystem::is_directory(*dir)
           && dir->path().extension() == ".versions")
        {
            m_versionTable.push_back((*dir).path());
        }
    }
}

// ----------------------------------------------------------------------------

void VersionsManager::buildLinkTable(const std::string& dirPath)
{
    core::mt::WriteLock lock(m_linkMutex);
    for(std::filesystem::recursive_directory_iterator end, dir(dirPath) ;
        dir != end ; ++dir)
    {
        if(!std::filesystem::is_directory(*dir)
           && dir->path().extension() == ".graphlink")
        {
            m_linkTable.push_back((*dir).path());
        }
    }
}

// ----------------------------------------------------------------------------

void VersionsManager::generateNewFile(
    const std::filesystem::path& filePath,
    const std::string& context,
    const std::string& versionName
)
{
    SIGHT_THROW_EXCEPTION_IF(
        io::atoms::patch::exceptions::BadExtension(".versions file required"),
        filePath.extension() != ".versions"
    );

    namespace pt = ::boost::property_tree;
    std::size_t classCount = ::camp::classCount();
    pt::ptree root;

    root.put("context", context);
    root.put("version_name", versionName);

    pt::ptree versions;
    for(int i = 0 ; i < classCount ; ++i)
    {
        const ::camp::Class& metaclass = ::camp::classByIndex(i);
        const std::string& className   = metaclass.name();

        if(metaclass.hasTag(io::atoms::patch::s_OBJ_VERSION))
        {
            const ::camp::Value& value = metaclass.tag(io::atoms::patch::s_OBJ_VERSION);
            versions.put(className, value.to<std::string>());
        }
    }

    root.push_back(pt::ptree::value_type("versions", pt::ptree(versions)));
    pt::json_parser::write_json(filePath.string(), root);
}

// ----------------------------------------------------------------------------

io::atoms::patch::VersionDescriptor VersionsManager::getVersion(const std::filesystem::path& filePath)
{
    SIGHT_THROW_EXCEPTION_IF(
        io::atoms::patch::exceptions::BadExtension(".versions file required"),
        filePath.extension() != ".versions"
    );

    namespace pt = ::boost::property_tree;
    pt::ptree root;

    io::atoms::patch::VersionDescriptor::VersionsType versionids;

    std::ifstream file(filePath.string().c_str());
    std::istream input(file.rdbuf());
    pt::json_parser::read_json(input, root);
    const std::string& context     = getValue(root, "context", filePath);
    const std::string& versionName = getValue(root, "version_name", filePath);

    for(pt::ptree::value_type& node : root.get_child("versions"))
    {
        versionids[node.first] = std::string(node.second.data().c_str());
    }

    file.close();

    return io::atoms::patch::VersionDescriptor(context, versionName, versionids);
}

// ----------------------------------------------------------------------------

io::atoms::patch::LinkDescriptor VersionsManager::getLink(const std::filesystem::path& filePath)
{
    SIGHT_THROW_EXCEPTION_IF(
        io::atoms::patch::exceptions::BadExtension(".graphlink file required"),
        filePath.extension() != ".graphlink"
    );

    namespace pt = ::boost::property_tree;
    typedef std::vector<std::pair<std::string, std::string> > LinkType;

    LinkType link;
    io::atoms::patch::LinkDescriptor::LinksType links;
    pt::ptree root;

    std::ifstream file(filePath.string().c_str());
    std::istream input(file.rdbuf());
    pt::json_parser::read_json(input, root);

    const std::string& context       = getValue(root, "context", filePath);
    const std::string& originVersion = getValue(root, "origin_version", filePath);
    const std::string& targetVersion = getValue(root, "target_version", filePath);

    const std::string& patcher = root.get("patcher", "DefaultPatcher");

    for(pt::ptree::value_type& child : root.get_child("links"))
    {
        for(pt::ptree::value_type& node : (child.second).get_child(""))
        {
            link.push_back(std::make_pair(node.first, node.second.data()));
        }

        SIGHT_THROW_EXCEPTION_IF(
            io::atoms::patch::exceptions::MissingInformation(
                "A link should contain an origin version and a target version."
            ),
            link.size() != 2
        );

        links[link[0]] = link[1];

        // Clear the vector
        LinkType().swap(link);
    }

    file.close();

    return io::atoms::patch::LinkDescriptor(context, originVersion, targetVersion, patcher, links);
}

// ----------------------------------------------------------------------------

void VersionsManager::generateVersionsGraph()
{
    {
        core::mt::ReadLock versionLock(m_versionMutex);

        //For every versions
        for(VersionsManager::ListPathType::value_type elt : m_versionTable)
        {
            io::atoms::patch::VersionDescriptor version = VersionsManager::getVersion(elt);

            if(m_versionsGraphMap.find(version.getContext()) == m_versionsGraphMap.end())
            {
                m_versionsGraphMap[version.getContext()] = VersionsGraph::New();
            }

            m_versionsGraphMap[version.getContext()]->addNode(version);
        }
    }

    {
        core::mt::ReadLock linkLock(m_linkMutex);

        //For every links
        for(VersionsManager::ListPathType::value_type elt : m_linkTable)
        {
            io::atoms::patch::LinkDescriptor link = VersionsManager::getLink(elt);

            SIGHT_ASSERT(
                "There is no graph created for the context \"" << link.getContext() << "\".",
                m_versionsGraphMap.find(link.getContext()) != m_versionsGraphMap.end()
            );

            m_versionsGraphMap[link.getContext()]->addEdge(link);
        }
    }
}

// ----------------------------------------------------------------------------

VersionsGraph::sptr VersionsManager::getGraph(const std::string& context)
{
    VersionsGraph::sptr vg;

    core::mt::ReadToWriteLock lock(m_graphMutex);
    if(m_versionsGraphMap.empty())
    {
        core::mt::UpgradeToWriteLock writeLock(lock);
        VersionsManager::generateVersionsGraph();
    }

    VersionsGraphMapType::iterator elem = m_versionsGraphMap.find(context);
    if(elem != m_versionsGraphMap.end())
    {
        vg = elem->second;
    }

    return vg;
}

} // namespace sight::io::atoms::patch
