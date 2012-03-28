/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/spyLog.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include "fwComEd/CompositeMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::CompositeMsg, ::fwComEd::CompositeMsg );

namespace fwComEd
{

std::string CompositeMsg::MODIFIED_KEYS = "MODIFIED_KEYS";

std::string CompositeMsg::ADDED_KEYS = "ADDED_KEYS";
std::string CompositeMsg::REMOVED_KEYS = "REMOVED_KEYS";
std::string CompositeMsg::CHANGED_KEYS = "CHANGED_KEYS";

//-------------------------------------------------------------------------

CompositeMsg::CompositeMsg() throw()
{
    m_removedKeys    = ::fwData::Composite::New();
    m_addedKeys      = ::fwData::Composite::New();
    m_oldChangedKeys = ::fwData::Composite::New();
    m_newChangedKeys = ::fwData::Composite::New();
}

//-------------------------------------------------------------------------

CompositeMsg::~CompositeMsg() throw()
{}

//-------------------------------------------------------------------------

void CompositeMsg::addModifiedKeysEvent( const std::vector< std::string > & _modifiedKeys )
{
    m_modifiedKeys = _modifiedKeys;
    this->addEvent(MODIFIED_KEYS);
}

//-------------------------------------------------------------------------

void CompositeMsg::addModifiedKeyEvent( std::string _modifiedKey )
{
    if( ! this->hasEvent( MODIFIED_KEYS ) )
    {
        this->addEvent( MODIFIED_KEYS );
    }
    m_modifiedKeys.push_back(_modifiedKey);
}

//-------------------------------------------------------------------------

void CompositeMsg::addModifiedKeysEvent( const std::vector< std::string > & _modifiedKeys, std::vector< ::fwData::Object::sptr > _oldObjects )
{
    m_modifiedKeys = _modifiedKeys;
    m_modifiedObjects = _oldObjects;
    this->addEvent(MODIFIED_KEYS);
}

//-------------------------------------------------------------------------

std::vector< std::string > CompositeMsg::getModifiedKeys() const
{
    SLM_ASSERT("sorry, CompositeMsg does not contained MODIFIED_KEYS event", this->hasEvent(MODIFIED_KEYS));
    return m_modifiedKeys;
}

//-------------------------------------------------------------------------

void CompositeMsg::appendAddedKey( std::string _compositeKey, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( ADDED_KEYS ) )
    {
        this->addEvent( ADDED_KEYS, m_addedKeys );
    }

    SLM_ASSERT("This composite key is already register", m_addedKeys->find(_compositeKey) == m_addedKeys->end() );

    m_addedKeys->getContainer()[ _compositeKey ] = _pNewObject;
    addModifiedKeyEvent( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getAddedKeys() const
{
    return m_addedKeys;
}

//-----------------------------------------------------------------------------

void CompositeMsg::appendRemovedKey( std::string _compositeKey, ::fwData::Object::sptr _pOldObject )
{
    if( ! this->hasEvent( REMOVED_KEYS ) )
    {
        this->addEvent( REMOVED_KEYS, m_removedKeys );
    }

    SLM_ASSERT("This composite key is already register", m_removedKeys->find(_compositeKey) == m_removedKeys->end() );

    m_removedKeys->getContainer()[ _compositeKey ] = _pOldObject;
    addModifiedKeyEvent( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getRemovedKeys() const
{
    return m_removedKeys;
}

//-----------------------------------------------------------------------------

void CompositeMsg::appendChangedKey( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( CHANGED_KEYS ) )
    {
        this->addEvent( CHANGED_KEYS, m_oldChangedKeys );
    }

    SLM_ASSERT("This composite key is already register", m_oldChangedKeys->find(_compositeKey) == m_oldChangedKeys->end() );

    m_oldChangedKeys->getContainer()[ _compositeKey ] = _pOldObject;
    m_newChangedKeys->getContainer()[ _compositeKey ] = _pNewObject;

    addModifiedKeyEvent( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getOldChangedKeys() const
{
    return m_oldChangedKeys;
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getNewChangedKeys() const
{
    return m_newChangedKeys;
}

//-----------------------------------------------------------------------------

} // namespace op

