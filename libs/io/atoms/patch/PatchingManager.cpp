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

#include "io/atoms/patch/PatchingManager.hpp"

#include "io/atoms/patch/exceptions/ImpossibleConversion.hpp"
#include "io/atoms/patch/exceptions/MissingInformation.hpp"
#include "io/atoms/patch/infos/log.hpp"
#include "io/atoms/patch/patcher/IPatcher.hpp"
#include "io/atoms/patch/VersionsGraph.hpp"
#include "io/atoms/patch/VersionsManager.hpp"

#include <atoms/Object.hpp>

namespace sight::io::atoms::patch
{

PatchingManager::PatchingManager(sight::atoms::Object::sptr object) :
    m_object(object)
{
}

// ----------------------------------------------------------------------------

PatchingManager::~PatchingManager()
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr PatchingManager::transformTo(const std::string& newVersion)
{
    const std::string& context        = m_object->getMetaInfo("context");
    const std::string& currentVersion = m_object->getMetaInfo("version_name");

    if(currentVersion == newVersion)
    {
        SLM_WARN("Trying to patch an object with the same version (" << currentVersion << ").");
        return m_object;
    }

    FW_RAISE_EXCEPTION_IF(
        io::atoms::patch::exceptions::MissingInformation("Context information is missing."),
        context.empty());

    FW_RAISE_EXCEPTION_IF(
        io::atoms::patch::exceptions::MissingInformation("Version information is missing."),
        currentVersion.empty());

    io::atoms::patch::VersionsGraph::sptr versionsGraph;
    versionsGraph = io::atoms::patch::VersionsManager::getDefault()->getGraph(context);

    FW_RAISE_EXCEPTION_IF( io::atoms::patch::exceptions::ImpossibleConversion(
                               "There is no way to go from version '" + currentVersion + "' to version '" +
                               newVersion + "' for context '" + context +"'."), !versionsGraph);

    io::atoms::patch::VersionsGraph::VersionSeriesType series
        = versionsGraph->shortestPath(currentVersion, newVersion);

    FW_RAISE_EXCEPTION_IF( io::atoms::patch::exceptions::ImpossibleConversion(
                               "There is no way to go from version '" + currentVersion + "' to version '" +
                               newVersion + "' for context '" + context +"'."),
                           series.empty());

    io::atoms::patch::VersionsGraph::NodeIDType currentVersionNode = versionsGraph->getNode(currentVersion);

    io::atoms::patch::patcher::IPatcher::sptr patcher;
    std::string currentName, targetName;

    for(io::atoms::patch::VersionsGraph::VersionSeriesType::value_type elt :  series)
    {
        io::atoms::patch::VersionsGraph::NodeIDType targetVersionNode = elt;

        //Retrieve versions names
        currentName = versionsGraph->getNode(currentVersionNode).getVersionName();
        targetName  = versionsGraph->getNode(targetVersionNode).getVersionName();

        //Retrieve link
        const io::atoms::patch::LinkDescriptor& link
            = versionsGraph->getEdge(currentVersionNode, targetVersionNode);

        //Retrieve patcher
        patcher = io::atoms::patch::patcher::factory::New(link.getPatcher());
        SLM_ASSERT("There is no patcher called \"" << link.getPatcher() << "\".", patcher);

        fwAtomsPatchInfoLogMacro("Begin patcher '" + link.getPatcher() + "'");

        //Patching
        m_object = patcher->transformObject(m_object, context, currentName, targetName);
        fwAtomsPatchInfoLogMacro("End patcher '" + link.getPatcher() + "'");

        currentVersionNode = targetVersionNode;
    }

    m_object->setMetaInfo("version_name", newVersion);

    return m_object;
}

} //namespace sight::io::atoms::patch
