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

#include <fwAtoms/Object.hpp>

#include "fwAtomsPatch/exceptions/ImpossibleConversion.hpp"
#include "fwAtomsPatch/exceptions/MissingInformation.hpp"
#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/patcher/IPatcher.hpp"
#include "fwAtomsPatch/PatchingManager.hpp"
#include "fwAtomsPatch/VersionsGraph.hpp"
#include "fwAtomsPatch/VersionsManager.hpp"

namespace fwAtomsPatch
{

PatchingManager::PatchingManager(::fwAtoms::Object::sptr object)
    : m_object(object)
{
}

// ----------------------------------------------------------------------------

PatchingManager::~PatchingManager()
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr PatchingManager::transformTo(const std::string& newVersion)
{
    const std::string& context        = m_object->getMetaInfo("context");
    const std::string& currentVersion = m_object->getMetaInfo("version_name");

    if(currentVersion == newVersion)
    {
        OSLM_WARN("Trying to patch an object with the same version (" << currentVersion << ").");
        return m_object;
    }

    FW_RAISE_EXCEPTION_IF(
        ::fwAtomsPatch::exceptions::MissingInformation("Context information is missing."),
        context.empty());

    FW_RAISE_EXCEPTION_IF(
        ::fwAtomsPatch::exceptions::MissingInformation("Version information is missing."),
        currentVersion.empty());


    ::fwAtomsPatch::VersionsGraph::sptr versionsGraph;
    versionsGraph = ::fwAtomsPatch::VersionsManager::getDefault()->getGraph(context);

    FW_RAISE_EXCEPTION_IF( ::fwAtomsPatch::exceptions::ImpossibleConversion(
                               "There is no way to go from version '" + currentVersion + "' to version '" +
                               newVersion + "' for context '" + context +"'."), !versionsGraph);


    ::fwAtomsPatch::VersionsGraph::VersionSeriesType series
        = versionsGraph->shortestPath(currentVersion, newVersion);

    FW_RAISE_EXCEPTION_IF( ::fwAtomsPatch::exceptions::ImpossibleConversion(
                               "There is no way to go from version '" + currentVersion + "' to version '" +
                               newVersion + "' for context '" + context +"'."),
                           series.empty());

    ::fwAtomsPatch::VersionsGraph::NodeIDType currentVersionNode = versionsGraph->getNode(currentVersion);

    ::fwAtomsPatch::patcher::IPatcher::sptr patcher;
    std::string currentName, targetName;

    SLM_TRACE("[PATCHING] Starting...");

    for(::fwAtomsPatch::VersionsGraph::VersionSeriesType::value_type elt :  series)
    {
        ::fwAtomsPatch::VersionsGraph::NodeIDType targetVersionNode = elt;

        //Retrieve versions names
        currentName = versionsGraph->getNode(currentVersionNode).getVersionName();
        targetName  = versionsGraph->getNode(targetVersionNode).getVersionName();

        //Retrieve link
        const ::fwAtomsPatch::LinkDescriptor& link
            = versionsGraph->getEdge(currentVersionNode, targetVersionNode);

        //Retrieve patcher
        patcher = ::fwAtomsPatch::patcher::factory::New(link.getPatcher());
        OSLM_ASSERT("There is no patcher called \"" << link.getPatcher() << "\".", patcher);

        OSLM_TRACE("[PATCHING] '" << currentName << "' -> '" << targetName << "'.");
        fwAtomsPatchInfoLogMacro("Begin patcher '" + link.getPatcher() + "'");

        //Patching
        m_object = patcher->transformObject(m_object, context, currentName, targetName);
        fwAtomsPatchInfoLogMacro("End patcher '" + link.getPatcher() + "'");

        currentVersionNode = targetVersionNode;
    }

    m_object->setMetaInfo("version_name", newVersion);

    return m_object;
}

} //namespace fwAtomsPatch
