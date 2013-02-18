/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Object.hpp>
#include <fwData/Vector.hpp>
#include <fwData/String.hpp>

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/VectorMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::VectorMsg );

namespace fwComEd
{

std::string VectorMsg::ADDED_OBJECTS = "ADDED_OBJECTS";
std::string VectorMsg::REMOVED_OBJECTS = "REMOVED_OBJECTS";

//-------------------------------------------------------------------------

VectorMsg::VectorMsg(::fwServices::ObjectMsg::Key key)
{
    m_removedObjects    = ::fwData::Vector::New();
    m_addedObjects      = ::fwData::Vector::New();
}

//-------------------------------------------------------------------------

VectorMsg::~VectorMsg() throw()
{}

//-------------------------------------------------------------------------

void VectorMsg::appendAddedObject( ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( ADDED_OBJECTS ) )
    {
        this->addEvent( ADDED_OBJECTS, m_addedObjects );
    }

    SLM_ASSERT("This object is already registered",
               std::find(m_addedObjects->begin(), m_addedObjects->end(), _pNewObject) == m_addedObjects->end() );

    m_addedObjects->getContainer().push_back(_pNewObject);
}

//-----------------------------------------------------------------------------

::fwData::Vector::sptr VectorMsg::getAddedObjects() const
{
    return m_addedObjects;
}

//-----------------------------------------------------------------------------

void VectorMsg::appendRemovedObject( ::fwData::Object::sptr _pOldObject )
{
    if( ! this->hasEvent( REMOVED_OBJECTS ) )
    {
        this->addEvent( REMOVED_OBJECTS, m_removedObjects );
    }

    SLM_ASSERT("This object is already register",
               std::find(m_removedObjects->begin(), m_removedObjects->end(), _pOldObject) == m_removedObjects->end() );

    m_removedObjects->getContainer().push_back( _pOldObject );
}

//-----------------------------------------------------------------------------

::fwData::Vector::sptr VectorMsg::getRemovedObjects() const
{
    return m_removedObjects;
}

//-----------------------------------------------------------------------------

} // namespace op

