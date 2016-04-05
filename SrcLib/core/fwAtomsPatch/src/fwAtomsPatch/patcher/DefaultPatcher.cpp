/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/IPatch.hpp"
#include "fwAtomsPatch/ISemanticPatch.hpp"
#include "fwAtomsPatch/IStructuralPatch.hpp"
#include "fwAtomsPatch/patcher/DefaultPatcher.hpp"
#include "fwAtomsPatch/patcher/registry/macros.hpp"
#include "fwAtomsPatch/SemanticPatchDB.hpp"
#include "fwAtomsPatch/StructuralPatchDB.hpp"
#include "fwAtomsPatch/types.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"
#include "fwAtomsPatch/VersionsManager.hpp"

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>


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

::fwAtoms::Object::sptr DefaultPatcher::transformObject(::fwAtoms::Object::sptr object,
                                                        const std::string &context,
                                                        const std::string &currentVersion,
                                                        const std::string &targetVersion)
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
    m_pass                      = Contextual;
    ::fwAtoms::Object::sptr obj = this->processContextualObject(m_object);
    fwAtomsPatchInfoLogMacro("End contextual pass");

    return obj;
}


//----------------------------------------------------------------------------

::fwAtoms::Object::sptr DefaultPatcher::processStructuralObject(::fwAtoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        ::fwAtoms::Object::sptr newAtomObject = ::fwAtoms::Object::New();

        // Cache update
        m_cache[current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID )] = newAtomObject;
        m_newVersions[current]                                    = newAtomObject;

        // Set id
        newAtomObject->setMetaInfo( ::fwAtomsPatch::s_OBJ_ID,
                                    current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

        // Set meta Info
        newAtomObject->setMetaInfos(current->getMetaInfos());

        // Fetch all attributes and affect them in the new object.
        for( ::fwAtoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            ::fwAtoms::Base::sptr obj = this->processBase(elem.second);
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

::fwAtoms::Object::sptr DefaultPatcher::processContextualObject(::fwAtoms::Object::sptr current)
{
    CacheType::const_iterator cIt = m_cache.find(current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID ));

    // If the object has not been processed yet.
    if(cIt == m_cache.end())
    {
        // Cache update
        m_cache[current->getMetaInfo( ::fwAtomsPatch::s_OBJ_ID )] = m_newVersions[current];

        // Fetch all attributes and affect them in the new object.
        for( ::fwAtoms::Object::AttributesType::value_type elem :  current->getAttributes() )
        {
            if ( elem.second )
            {
                if(elem.second->isObject())
                {
                    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(elem.second);
                    m_newVersions[obj]          = this->processContextualObject(obj);
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

::fwAtoms::Base::sptr DefaultPatcher::processBase(::fwAtoms::Base::sptr base)
{
    ::fwAtoms::Base::sptr newBase;

    if ( !base )
    {
        return newBase;
    }

    if(base->isObject())
    {
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(base);
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
        ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::dynamicCast(base);
        newBase                       = this->processSequence(seq);
    }
    else if(base->isMapping())
    {
        ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast(base);
        newBase                  = this->processMapping(map);
    }
    else if(base->isString())
    {
        newBase = ::fwAtoms::String::New(base->getString());
    }
    else if(base->isNumeric())
    {
        newBase = ::fwAtoms::Numeric::New(base->getString());
    }
    else if(base->isBoolean())
    {
        newBase = ::fwAtoms::Boolean::New(base->getString());
    }
    else if(base->isBlob())
    {
        ::fwAtoms::Blob::sptr blob = ::fwAtoms::Blob::dynamicCast(base);
        newBase                    = ::fwAtoms::Blob::New(blob->getBufferObject());
    }


    return newBase;
}

//  ----------------------------------------------------------------------------

::fwAtoms::Map::sptr DefaultPatcher::processMapping(::fwAtoms::Map::sptr map)
{
    ::fwAtoms::Map::sptr newMap = ::fwAtoms::Map::New();

    // Fetch all attributes and affect them in the new object
    std::string key;
    ::fwAtoms::Base::sptr value;

    for( ::fwAtoms::Map::MapType::value_type elem :  map->getValue() )
    {
        key   = elem.first;
        value = elem.second;

        ::fwAtoms::Base::sptr obj = this->processBase(value);
        if (this->isKnown(obj))
        {
            newMap->insert( key,  obj);
        }
    }

    return newMap;
}

//----------------------------------------------------------------------------

::fwAtoms::Sequence::sptr DefaultPatcher::processSequence(::fwAtoms::Sequence::sptr seq)
{
    ::fwAtoms::Sequence::sptr newSeq = ::fwAtoms::Sequence::New();

    // Fetch all attributes and affect them in the new object
    for( ::fwAtoms::Base::sptr elem :  seq->getValue() )
    {
        ::fwAtoms::Base::sptr obj = this->processBase(elem);
        if (this->isKnown(obj))
        {
            newSeq->push_back(obj);
        }
    }

    return newSeq;
}

//----------------------------------------------------------------------------

::fwAtoms::Object::sptr DefaultPatcher::applyStructuralPatch(
    ::fwAtoms::Object::sptr previous, ::fwAtoms::Object::sptr current)
{
    if(previous)
    {
        // Get new version of the object.
        fwAtomsPatch::LinkDescriptor::VersionIDType currentInfos;
        bool success;
        const std::string& classname = ::fwAtomsPatch::helper::getClassname(current);
        const std::string& version   = ::fwAtomsPatch::helper::getVersion(current);

        ::boost::tie(currentInfos,success) = m_versionsGraph->getLinkedVersion(
            m_currentVersion, m_targetVersion,
            std::make_pair(classname, version) );

        if(success)
        {
            // Get patch
            ::fwAtomsPatch::IStructuralPatch::sptr patch = ::fwAtomsPatch::StructuralPatchDB::getDefault()->getPatch(
                classname, version, currentInfos.first, currentInfos.second);

            if(patch)
            {
                OSLM_TRACE("[SP] " << classname);
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

::fwAtoms::Object::sptr DefaultPatcher::applyContextualPatch(
    ::fwAtoms::Object::sptr previous, ::fwAtoms::Object::sptr current)
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
            OSLM_TRACE("[CP] " << ::fwAtomsPatch::helper::getClassname(current));
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

bool DefaultPatcher::isKnown(const ::fwAtoms::Base::sptr& base)
{
    bool isKnown                             = false;
    VersionsGraph::NodeType target           = m_versionsGraph->getNode(m_targetVersion);
    VersionDescriptor::VersionsType versions = target.getVersions();
    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(base);

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

