/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/Vector.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Vector.hpp>

namespace fwDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

Vector::Vector( ::fwData::Vector::wptr _vector ) :
    m_vector( _vector )
{
}

//-----------------------------------------------------------------------------

Vector::~Vector()
{
    if(!m_addedObjects.empty() || !m_removedObjects.empty())
    {
        notify();
    }
}

//-----------------------------------------------------------------------------

void Vector::add( ::fwData::Object::sptr _newObject )
{
    ::fwData::Vector::sptr vector = m_vector.lock();
    OSLM_ASSERT( "The object " << _newObject->getID() << " must not exist in vector.",
                 std::find(vector->begin(), vector->end(), _newObject) == vector->end());

    // Modify vector
    vector->getContainer().push_back(_newObject );

    m_addedObjects.push_back(_newObject);
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

    m_removedObjects.push_back(_oldObject);

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

void Vector::notify()
{
    if ( !m_removedObjects.empty() )
    {
        auto sig = m_vector.lock()->signal< ::fwData::Vector::RemovedObjectsSignalType >(
            ::fwData::Vector::s_REMOVED_OBJECTS_SIG);
        sig->asyncEmit(m_removedObjects);
    }
    if ( !m_addedObjects.empty() )
    {
        auto sig = m_vector.lock()->signal< ::fwData::Vector::AddedObjectsSignalType >(
            ::fwData::Vector::s_ADDED_OBJECTS_SIG);
        sig->asyncEmit(m_addedObjects);
    }
    OSLM_INFO_IF("No changes were found on the vector '" + m_vector.lock()->getID() + "', nothing to notify.",
                 m_addedObjects.empty() && m_removedObjects.empty());

    m_removedObjects.clear();
    m_addedObjects.clear();
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwDataTools
