/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>

#include <fwData/Vector.hpp>

#include <fwServices/IEditionService.hpp>

#include "fwComEd/helper/Vector.hpp"

namespace fwComEd
{
namespace helper
{

//-----------------------------------------------------------------------------

Vector::Vector( ::fwData::Vector::wptr _vector )
    :   m_vectorMsg ( ::fwComEd::VectorMsg::New() ),
      m_vector ( _vector )
{
}

//-----------------------------------------------------------------------------

Vector::~Vector()
{
}

//-----------------------------------------------------------------------------

void Vector::add( ::fwData::Object::sptr _newObject )
{
    ::fwData::Vector::sptr vector = m_vector.lock();
    OSLM_ASSERT( "The object " << _newObject->getID() << " must not exist in vector.",
                 std::find(vector->begin(), vector->end(), _newObject) == vector->end());

    // Modify vector
    vector->getContainer().push_back(_newObject );

    // Modify message
    m_vectorMsg->appendAddedObject( _newObject );

}

//-----------------------------------------------------------------------------

void Vector::remove( ::fwData::Object::sptr _oldObject )
{
    ::fwData::Vector::sptr vector   = m_vector.lock();
    ::fwData::Vector::iterator iter = std::find(vector->begin(), vector->end(), _oldObject);
    OSLM_ASSERT( "The object " << _oldObject->getID() << " must exist in vector.",
                 iter != vector->end());

    // Modify vector
    vector->getContainer().erase( iter );

    // Modify message
    m_vectorMsg->appendRemovedObject( _oldObject );

}

//-----------------------------------------------------------------------------

void Vector::clear()
{
    ::fwData::Vector::sptr vector = m_vector.lock();

    while (!vector->empty())
    {
        this->remove(vector->front());
    }
}

//-----------------------------------------------------------------------------

void Vector::notify( ::fwServices::IService::sptr _serviceSource, bool notifySource )
{
    if ( m_vectorMsg->getEventIds().size() > 0 )
    {
        ::fwServices::IEditionService::notify( _serviceSource, m_vector.lock(), m_vectorMsg, notifySource );
    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.",
                m_vectorMsg->getEventIds().size() == 0);
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwComEd
