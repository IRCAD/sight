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

#include "io/atoms/patch/patcher/DefaultPatcher.hpp"

#include "io/atoms/patch/helper/functions.hpp"
#include "io/atoms/patch/infos/log.hpp"
#include "io/atoms/patch/IPatch.hpp"
#include "io/atoms/patch/ISemanticPatch.hpp"
#include "io/atoms/patch/IStructuralPatch.hpp"
#include "io/atoms/patch/patcher/registry/macros.hpp"
#include "io/atoms/patch/SemanticPatchDB.hpp"
#include "io/atoms/patch/StructuralPatchDB.hpp"
#include "io/atoms/patch/types.hpp"
#include "io/atoms/patch/VersionDescriptor.hpp"
#include "io/atoms/patch/VersionsManager.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>

namespace sight::io::atoms::patch
{

namespace patcher
{

patcherRegisterMacro(io::atoms::patch::patcher::DefaultPatcher, "DefaultPatcher");

DefaultPatcher::DefaultPatcher(io::atoms::patch::patcher::IPatcher::Key key)
{
}

//----------------------------------------------------------------------------

DefaultPatcher::~DefaultPatcher()
{
}

//----------------------------------------------------------------------------

sight::atoms::Object::sptr DefaultPatcher::transformObject(sight::atoms::Object::sptr object,
                                                           const std::string& context,
                                                           const std::string& currentVersion,
                                                           const std::string& targetVersion)
{
    m_object         = object;
    m_context        = context;
    m_versionsGraph  = io::atoms::patch::VersionsManager::getDefault()->getGraph(context);
    m_currentVersion = m_versionsGraph->getNode(currentVersion);
    m_targetVersion  = m_versionsGraph->getNode(targetVersion);

    fwAtomsPatchInfoLogMacro("Transform from version '"
                             + m_versionsGraph->getNode(m_currentVersion).getVersionName() + "' to '"
                             + m_versionsGraph->getNode(m_targetVersion).getVersionName() + "' in context '"
                             + m_context + "'");
    fwAtomsPatchInfoLogMacro("Begin structural pass");

    // Structural
    m_newVersions.clear();
    m_cache.clear();
    m_pass = Structural;
    this->processStructuralObject(m_object);
    fwAtomsPatchInfoLogMacro("End structural pass");

    // Contextual
    fwAtomsPatchInfoLogMacro("Begin contextual pass");
    m_cache.clear();
    m_pass = Contextual;
    sight::atoms::Object::sptr obj = this->processContextualObject(m_object);
    fwAtomsPatchInfoLogMacro("End contextual pass");

    return obj;
}

//----------------------------------------------------------------------------

sight::atoms::Object::sptr DefaultPatcher::processStructuralObject(sight::atoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( io::atoms::patch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        sight::atoms::Object::sptr newAtomObject = sight::atoms::Object::New();

        // Cache update
        m_cache[current->getMetaInfo( io::atoms::patch::s_OBJ_ID )] = newAtomObject;
        m_newVersions[current]                                      = newAtomObject;

        // Set id
        newAtomObject->setMetaInfo( io::atoms::patch::s_OBJ_ID,
                                    current->getMetaInfo( io::atoms::patch::s_OBJ_ID ));

        // Set meta Info
        newAtomObject->setMetaInfos(current->getMetaInfos());

        // Fetch all attributes and affect them in the new object.
        for( sight::atoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            sight::atoms::Base::sptr obj = this->processBase(elem.second);
            if(this->isKnown(obj))
            {
                newAtomObject->setAttribute(elem.first, obj);
            }
        }

        return this->applyStructuralPatch(current, newAtomObject);
    }
    // If the object has already been processed.
    else
    {
        return cIt->second;
    }
}

//----------------------------------------------------------------------------

sight::atoms::Object::sptr DefaultPatcher::processContextualObject(sight::atoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( io::atoms::patch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        // Cache update
        m_cache[current->getMetaInfo( io::atoms::patch::s_OBJ_ID )] = m_newVersions[current];

        // Fetch all attributes and affect them in the new object.
        for( sight::atoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            if ( elem.second )
            {
                if(elem.second->isObject())
                {
                    sight::atoms::Object::sptr obj = sight::atoms::Object::dynamicCast(elem.second);
                    m_newVersions[obj] = this->processContextualObject(obj);
                }
                else
                {
                    this->processBase(elem.second);
                }
            }
        }

        return this->applyContextualPatch(current, m_newVersions[current]);
    }
    // If the object has already been processed.
    else
    {
        return m_cache[current->getMetaInfo( io::atoms::patch::s_OBJ_ID )];
    }
}

//  ----------------------------------------------------------------------------

sight::atoms::Base::sptr DefaultPatcher::processBase(sight::atoms::Base::sptr base)
{
    sight::atoms::Base::sptr newBase;

    if ( !base )
    {
        return newBase;
    }

    if(base->isObject())
    {
        sight::atoms::Object::sptr obj = sight::atoms::Object::dynamicCast(base);
        if(m_pass == Structural)
        {
            newBase = this->processStructuralObject(obj);
        }
        else
        {
            newBase = this->processContextualObject(obj);
        }
    }
    else if(base->isSequence())
    {
        sight::atoms::Sequence::sptr seq = sight::atoms::Sequence::dynamicCast(base);
        newBase = this->processSequence(seq);
    }
    else if(base->isMapping())
    {
        sight::atoms::Map::sptr map = sight::atoms::Map::dynamicCast(base);
        newBase = this->processMapping(map);
    }
    else if(base->isString())
    {
        newBase = sight::atoms::String::New(base->getString());
    }
    else if(base->isNumeric())
    {
        newBase = sight::atoms::Numeric::New(base->getString());
    }
    else if(base->isBoolean())
    {
        newBase = sight::atoms::Boolean::New(base->getString());
    }
    else if(base->isBlob())
    {
        sight::atoms::Blob::sptr blob = sight::atoms::Blob::dynamicCast(base);
        newBase = sight::atoms::Blob::New(blob->getBufferObject());
    }

    return newBase;
}

//  ----------------------------------------------------------------------------

sight::atoms::Map::sptr DefaultPatcher::processMapping(sight::atoms::Map::sptr map)
{
    sight::atoms::Map::sptr newMap = sight::atoms::Map::New();

    // Fetch all attributes and affect them in the new object
    std::string key;
    sight::atoms::Base::sptr value;

    for( sight::atoms::Map::MapType::value_type elem :  map->getValue() )
    {
        key   = elem.first;
        value = elem.second;

        sight::atoms::Base::sptr obj = this->processBase(value);
        if (this->isKnown(obj))
        {
            newMap->insert( key,  obj);
        }
    }

    return newMap;
}

//----------------------------------------------------------------------------

sight::atoms::Sequence::sptr DefaultPatcher::processSequence(sight::atoms::Sequence::sptr seq)
{
    sight::atoms::Sequence::sptr newSeq = sight::atoms::Sequence::New();

    // Fetch all attributes and affect them in the new object
    for( sight::atoms::Base::sptr elem :  seq->getValue() )
    {
        sight::atoms::Base::sptr obj = this->processBase(elem);
        if (this->isKnown(obj))
        {
            newSeq->push_back(obj);
        }
    }

    return newSeq;
}

//----------------------------------------------------------------------------

sight::atoms::Object::sptr DefaultPatcher::applyStructuralPatch(
    sight::atoms::Object::sptr previous, sight::atoms::Object::sptr current)
{
    if(previous)
    {
        // Get new version of the object.
        const std::string& classname = io::atoms::patch::helper::getClassname(current);
        const std::string& version   = io::atoms::patch::helper::getVersion(current);

        auto [currentInfos, success] = m_versionsGraph->getLinkedVersion(
            m_currentVersion, m_targetVersion,
            std::make_pair(classname, version) );

        if(success)
        {
            // Get patch
            io::atoms::patch::IStructuralPatch::sptr patch =
                io::atoms::patch::StructuralPatchDB::getDefault()->getPatch(
                    classname, version, currentInfos.first, currentInfos.second);

            if(patch)
            {
                fwAtomsPatchInfoLogMacro("Apply structural patch to transform '" + classname
                                         + "|" + version + "' to '"
                                         + currentInfos.first + "|" + currentInfos.second + "'");
                // Applying a patch on current (the current object is modified)
                patch->apply(previous, current, m_newVersions);
                fwAtomsPatchInfoLogMacro("End structural patch");
            }
            else
            {
                fwAtomsPatchInfoLogMacro("Move object '" + classname + "|" + version + "' to '"
                                         + currentInfos.first + "|" + currentInfos.second + "'");
                io::atoms::patch::helper::setClassname(current, currentInfos.first);
                io::atoms::patch::helper::setVersion(current, currentInfos.second);
            }
        }
        else
        {
            fwAtomsPatchErrorLogMacro("No linked version found for object '" + classname
                                      + "' with version '" + version + "'");
        }
    }

    return current;
}

//----------------------------------------------------------------------------

sight::atoms::Object::sptr DefaultPatcher::applyContextualPatch(
    sight::atoms::Object::sptr previous, sight::atoms::Object::sptr current)
{
    if(previous)
    {
        const std::string currentName = m_versionsGraph->getNode(m_currentVersion).getVersionName();
        const std::string targetName  = m_versionsGraph->getNode(m_targetVersion).getVersionName();

        // Get patch
        io::atoms::patch::SemanticPatchDB::sptr contextDB = io::atoms::patch::SemanticPatchDB::getDefault();
        io::atoms::patch::ISemanticPatch::sptr patch      =
            contextDB->getPatch(m_context, currentName, targetName,
                                io::atoms::patch::helper::getClassname(previous),
                                io::atoms::patch::helper::getVersion(previous));

        if(patch)
        {
            fwAtomsPatchInfoLogMacro("Apply contextual patch to transform '"
                                     + io::atoms::patch::helper::getClassname(previous)
                                     + "|" + io::atoms::patch::helper::getVersion(previous) + "' to '"
                                     + io::atoms::patch::helper::getClassname(current)
                                     + "|" + io::atoms::patch::helper::getVersion(current) + "'");

            // Applying a patch on current (the current object is modified)
            patch->apply(previous, current, m_newVersions);
            fwAtomsPatchInfoLogMacro("End contextual patch");
        }
    }

    return current;
}

//  ----------------------------------------------------------------------------

bool DefaultPatcher::isKnown(const sight::atoms::Base::sptr& base)
{
    bool isKnown                             = false;
    VersionsGraph::NodeType target           = m_versionsGraph->getNode(m_targetVersion);
    VersionDescriptor::VersionsType versions = target.getVersions();
    sight::atoms::Object::sptr obj           = sight::atoms::Object::dynamicCast(base);

    if (!obj)
    {
        isKnown = true;
    }
    else
    {
        std::string classname = obj->getMetaInfo(io::atoms::patch::s_OBJ_CLASSNAME);

        if (classname.empty() || versions.find(classname) != versions.end())
        {
            isKnown = true;
        }
    }
    return isKnown;
}

//  ----------------------------------------------------------------------------

} //  namespace patcher

} //  namespace sight::io::atoms::patch
