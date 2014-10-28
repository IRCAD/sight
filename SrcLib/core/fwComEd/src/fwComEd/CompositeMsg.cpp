/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/CompositeMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::CompositeMsg );

namespace fwComEd
{

std::string CompositeMsg::ADDED_KEYS = "ADDED_KEYS";
std::string CompositeMsg::REMOVED_KEYS = "REMOVED_KEYS";
std::string CompositeMsg::CHANGED_KEYS = "CHANGED_KEYS";

//-------------------------------------------------------------------------

CompositeMsg::CompositeMsg(::fwServices::ObjectMsg::Key key)
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

void CompositeMsg::appendAddedKey( std::string _compositeKey, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( ADDED_KEYS ) )
    {
        this->addEvent( ADDED_KEYS, m_addedKeys );
    }

    SLM_ASSERT("This composite key is already register", m_addedKeys->find(_compositeKey) == m_addedKeys->end() );

    m_addedKeys->getContainer()[ _compositeKey ] = _pNewObject;
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

