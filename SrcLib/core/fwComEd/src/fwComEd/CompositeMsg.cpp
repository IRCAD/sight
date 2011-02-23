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

std::string CompositeMsg::MODIFIED_FIELDS = "MODIFIED_FIELDS";

std::string CompositeMsg::ADDED_FIELDS = "ADDED_FIELDS";
std::string CompositeMsg::REMOVED_FIELDS = "REMOVED_FIELDS";
std::string CompositeMsg::SWAPPED_FIELDS = "SWAPPED_FIELDS";

//-------------------------------------------------------------------------

CompositeMsg::CompositeMsg() throw()
: m_removedFields ( new ::fwData::Composite() ),
m_addedFields ( new ::fwData::Composite() ),
m_swappedOldFields ( new ::fwData::Composite() ),
m_swappedNewFields ( new ::fwData::Composite() )
{}

//-------------------------------------------------------------------------

CompositeMsg::~CompositeMsg() throw()
{}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields )
{
    m_modifiedFields = _modifiedFields;
    this->addEvent(MODIFIED_FIELDS);

}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedField( std::string _modifiedField )
{
    if( ! this->hasEvent( MODIFIED_FIELDS ) )
    {
        this->addEvent( MODIFIED_FIELDS );
    }

    m_modifiedFields.push_back(_modifiedField);
}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields, std::vector< ::fwData::Object::sptr > _oldObjects )
{
    m_modifiedFields = _modifiedFields;
    m_modifiedObjects = _oldObjects;
    this->addEvent(MODIFIED_FIELDS);
}

//-------------------------------------------------------------------------

std::vector< std::string > CompositeMsg::getEventModifiedFields() const
{
    SLM_ASSERT("sorry, CompositeMsg does not contained MODIFIED_FIELDS event", this->hasEvent(MODIFIED_FIELDS));

    return m_modifiedFields;
}

//-------------------------------------------------------------------------

void CompositeMsg::addAddedField( std::string _compositeKey, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( ADDED_FIELDS ) )
    {
        this->addEvent( ADDED_FIELDS, m_addedFields );
    }

    SLM_ASSERT("This composite key is already register", m_addedFields->getRefMap().find(_compositeKey) == m_addedFields->getRefMap().end() );

    m_addedFields->getRefMap()[ _compositeKey ] = _pNewObject;
    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getAddedFields() const
{
    return m_addedFields;
}

//-----------------------------------------------------------------------------

void CompositeMsg::addRemovedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject )
{
    if( ! this->hasEvent( REMOVED_FIELDS ) )
    {
        this->addEvent( REMOVED_FIELDS, m_removedFields );
    }

    SLM_ASSERT("This composite key is already register", m_removedFields->getRefMap().find(_compositeKey) == m_removedFields->getRefMap().end() );

    m_removedFields->getRefMap()[ _compositeKey ] = _pOldObject;
    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getRemovedFields() const
{
    return m_removedFields;
}

//-----------------------------------------------------------------------------

void CompositeMsg::addSwappedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( SWAPPED_FIELDS ) )
    {
        this->addEvent( SWAPPED_FIELDS, m_swappedOldFields );
    }

    SLM_ASSERT("This composite key is already register", m_swappedOldFields->getRefMap().find(_compositeKey) == m_swappedOldFields->getRefMap().end() );

    m_swappedOldFields->getRefMap()[ _compositeKey ] = _pOldObject;
    m_swappedNewFields->getRefMap()[ _compositeKey ] = _pNewObject;

    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getSwappedOldFields() const
{
    return m_swappedOldFields;
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getSwappedNewFields() const
{
    return m_swappedNewFields;
}

//-----------------------------------------------------------------------------

} // namespace op

