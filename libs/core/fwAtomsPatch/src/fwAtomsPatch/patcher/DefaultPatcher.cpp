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

#include "fwAtomsPatch/patcher/DefaultPatcher.hpp"

#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/IPatch.hpp"
#include "fwAtomsPatch/ISemanticPatch.hpp"
#include "fwAtomsPatch/IStructuralPatch.hpp"
#include "fwAtomsPatch/patcher/registry/macros.hpp"
#include "fwAtomsPatch/SemanticPatchDB.hpp"
#include "fwAtomsPatch/StructuralPatchDB.hpp"
#include "fwAtomsPatch/types.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"
#include "fwAtomsPatch/VersionsManager.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>

namespace fwAtomsPatch
{

namespace patcher
{

patcherRegisterMacro(::fwAtomsPatch::patcher::DefaultPatcher, "DefaultPatcher");

DefaultPatcher::DefaultPatcher(::fwAtomsPatch::patcher::IPatcher::Key key)
{
}

//----------------------------------------------------------------------------

DefaultPatcher::~DefaultPatcher()
{
}

//----------------------------------------------------------------------------

atoms::Object::sptr DefaultPatcher::transformObject(atoms::Object::sptr object,
                                                    const std::string& context,
                                                    const std::string& currentVersion,
                                                    const std::string& targetVersion)
{
    m_object         = object;
    m_context        = context;
    m_versionsGraph  = ::fwAtomsPatch::VersionsManager::getDefault()->getGraph(context);
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
    atoms::Object::sptr obj = this->processContextualObject(m_object);
    fwAtomsPatchInfoLogMacro("End contextual pass");

    return obj;
}

//----------------------------------------------------------------------------

atoms::Object::sptr DefaultPatcher::processStructuralObject(atoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        atoms::Object::sptr newAtomObject = atoms::Object::New();

        // Cache update
        m_cache[current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID )] = newAtomObject;
        m_newVersions[current]                                    = newAtomObject;

        // Set id
        newAtomObject->setMetaInfo( ::fwAtomsPatch::s_OBJ_ID,
                                    current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

        // Set meta Info
        newAtomObject->setMetaInfos(current->getMetaInfos());

        // Fetch all attributes and affect them in the new object.
        for( atoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            atoms::Base::sptr obj = this->processBase(elem.second);
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

atoms::Object::sptr DefaultPatcher::processContextualObject(atoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        // Cache update
        m_cache[current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID )] = m_newVersions[current];

        // Fetch all attributes and affect them in the new object.
        for( atoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            if ( elem.second )
            {
                if(elem.second->isObject())
                {
                    atoms::Object::sptr obj = atoms::Object::dynamicCast(elem.second);
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
        return m_cache[current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID )];
    }
}

//  ----------------------------------------------------------------------------

atoms::Base::sptr DefaultPatcher::processBase(atoms::Base::sptr base)
{
    atoms::Base::sptr newBase;

    if ( !base )
    {
        return newBase;
    }

    if(base->isObject())
    {
        atoms::Object::sptr obj = atoms::Object::dynamicCast(base);
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
        atoms::Sequence::sptr seq = atoms::Sequence::dynamicCast(base);
        newBase = this->processSequence(seq);
    }
    else if(base->isMapping())
    {
        atoms::Map::sptr map = atoms::Map::dynamicCast(base);
        newBase = this->processMapping(map);
    }
    else if(base->isString())
    {
        newBase = atoms::String::New(base->getString());
    }
    else if(base->isNumeric())
    {
        newBase = atoms::Numeric::New(base->getString());
    }
    else if(base->isBoolean())
    {
        newBase = atoms::Boolean::New(base->getString());
    }
    else if(base->isBlob())
    {
        atoms::Blob::sptr blob = atoms::Blob::dynamicCast(base);
        newBase = atoms::Blob::New(blob->getBufferObject());
    }

    return newBase;
}

//  ----------------------------------------------------------------------------

atoms::Map::sptr DefaultPatcher::processMapping(atoms::Map::sptr map)
{
    atoms::Map::sptr newMap = atoms::Map::New();

    // Fetch all attributes and affect them in the new object
    std::string key;
    atoms::Base::sptr value;

    for( atoms::Map::MapType::value_type elem :  map->getValue() )
    {
        key   = elem.first;
        value = elem.second;

        atoms::Base::sptr obj = this->processBase(value);
        if (this->isKnown(obj))
        {
            newMap->insert( key,  obj);
        }
    }

    return newMap;
}

//----------------------------------------------------------------------------

atoms::Sequence::sptr DefaultPatcher::processSequence(atoms::Sequence::sptr seq)
{
    atoms::Sequence::sptr newSeq = atoms::Sequence::New();

    // Fetch all attributes and affect them in the new object
    for( atoms::Base::sptr elem :  seq->getValue() )
    {
        atoms::Base::sptr obj = this->processBase(elem);
        if (this->isKnown(obj))
        {
            newSeq->push_back(obj);
        }
    }

    return newSeq;
}

//----------------------------------------------------------------------------

atoms::Object::sptr DefaultPatcher::applyStructuralPatch(
    atoms::Object::sptr previous, atoms::Object::sptr current)
{
    if(previous)
    {
        // Get new version of the object.
        const std::string& classname = ::fwAtomsPatch::helper::getClassname(current);
        const std::string& version   = ::fwAtomsPatch::helper::getVersion(current);

        auto [currentInfos, success] = m_versionsGraph->getLinkedVersion(
            m_currentVersion, m_targetVersion,
            std::make_pair(classname, version) );

        if(success)
        {
            // Get patch
            ::fwAtomsPatch::IStructuralPatch::sptr patch = ::fwAtomsPatch::StructuralPatchDB::getDefault()->getPatch(
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
                ::fwAtomsPatch::helper::setClassname(current, currentInfos.first);
                ::fwAtomsPatch::helper::setVersion(current, currentInfos.second);
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

atoms::Object::sptr DefaultPatcher::applyContextualPatch(
    atoms::Object::sptr previous, atoms::Object::sptr current)
{
    if(previous)
    {
        const std::string currentName = m_versionsGraph->getNode(m_currentVersion).getVersionName();
        const std::string targetName  = m_versionsGraph->getNode(m_targetVersion).getVersionName();

        // Get patch
        ::fwAtomsPatch::SemanticPatchDB::sptr contextDB = ::fwAtomsPatch::SemanticPatchDB::getDefault();
        ::fwAtomsPatch::ISemanticPatch::sptr patch      =
            contextDB->getPatch(m_context, currentName, targetName,
                                ::fwAtomsPatch::helper::getClassname(previous),
                                ::fwAtomsPatch::helper::getVersion(previous));

        if(patch)
        {
            fwAtomsPatchInfoLogMacro("Apply contextual patch to transform '"
                                     + ::fwAtomsPatch::helper::getClassname(previous)
                                     + "|" + ::fwAtomsPatch::helper::getVersion(previous) + "' to '"
                                     + ::fwAtomsPatch::helper::getClassname(current)
                                     + "|" + ::fwAtomsPatch::helper::getVersion(current) + "'");

            // Applying a patch on current (the current object is modified)
            patch->apply(previous, current, m_newVersions);
            fwAtomsPatchInfoLogMacro("End contextual patch");
        }
    }

    return current;
}

//  ----------------------------------------------------------------------------

bool DefaultPatcher::isKnown(const atoms::Base::sptr& base)
{
    bool isKnown                             = false;
    VersionsGraph::NodeType target           = m_versionsGraph->getNode(m_targetVersion);
    VersionDescriptor::VersionsType versions = target.getVersions();
    atoms::Object::sptr obj                  = atoms::Object::dynamicCast(base);

    if (!obj)
    {
        isKnown = true;
    }
    else
    {
        std::string classname = obj->getMetaInfo(::fwAtomsPatch::s_OBJ_CLASSNAME);

        if (classname.empty() || versions.find(classname) != versions.end())
        {
            isKnown = true;
        }
    }
    return isKnown;
}

//  ----------------------------------------------------------------------------

} //  namespace patcher

} //  namespace fwAtomsPatch
