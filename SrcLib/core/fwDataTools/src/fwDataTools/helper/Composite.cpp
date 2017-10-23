/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/Composite.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Composite.hpp>

namespace fwDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

Composite::Composite( ::fwData::Composite::wptr _composite ) :
    m_composite( _composite )
{
}

//-----------------------------------------------------------------------------

Composite::~Composite()
{
}

//-----------------------------------------------------------------------------

void Composite::add( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                   "object to be added.",
                   m_composite.lock()->find(_compositeKey) != m_composite.lock()->end() );

    // Modify composite
    m_composite.lock()->getContainer()[ _compositeKey ] = _newObject;

    m_addedObjects[_compositeKey] = _newObject;

}

//-----------------------------------------------------------------------------

void Composite::remove( std::string _compositeKey )
{
    OSLM_FATAL_IF( "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                   "object to be removed.",
                   m_composite.lock()->find(_compositeKey) == m_composite.lock()->end() );

    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getContainer()[ _compositeKey ];

    // Modify composite
    m_composite.lock()->getContainer().erase( _compositeKey );

    m_removedObjects[_compositeKey] = objBackup;

}

//-----------------------------------------------------------------------------

void Composite::clear()
{
    ::fwData::Composite::sptr composite = m_composite.lock();
    std::vector<std::string> vectKey;
    std::transform( composite->begin(), composite->end(),
                    std::back_inserter(vectKey),
                    std::bind(&::fwData::Composite::value_type::first, std::placeholders::_1) );

    for(std::string key :  vectKey)
    {
        this->remove(key);
    }
}

//-----------------------------------------------------------------------------

void Composite::swap( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                   "object to be swapped.",
                   m_composite.lock()->find(_compositeKey) == m_composite.lock()->end() );

    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getContainer()[ _compositeKey ];

    if( objBackup != _newObject )
    {
        // Modify composite
        m_composite.lock()->getContainer()[ _compositeKey ] = _newObject;

        m_newChangedObjects[_compositeKey] = _newObject;
        m_oldChangedObjects[_compositeKey] = objBackup;
    }
    else
    {
        OSLM_INFO(
            "Cannot swap this object ( "<< _compositeKey <<
            " ) in composite because it is the same object. Do nothing (not notification)");
    }
}

//-----------------------------------------------------------------------------

void Composite::notify()
{
    if ( !m_removedObjects.empty() )
    {
        auto sig = m_composite.lock()->signal< ::fwData::Composite::RemovedObjectsSignalType >(
            ::fwData::Composite::s_REMOVED_OBJECTS_SIG);

        sig->asyncEmit(m_removedObjects);
    }
    if ( !m_newChangedObjects.empty() && !m_oldChangedObjects.empty() )
    {
        auto sig = m_composite.lock()->signal< ::fwData::Composite::ChangedObjectsSignalType >(
            ::fwData::Composite::s_CHANGED_OBJECTS_SIG);

        sig->asyncEmit(m_newChangedObjects, m_oldChangedObjects);
    }
    if ( !m_addedObjects.empty() )
    {
        auto sig = m_composite.lock()->signal< ::fwData::Composite::AddedObjectsSignalType >(
            ::fwData::Composite::s_ADDED_OBJECTS_SIG);

        sig->asyncEmit(m_addedObjects);
    }
    OSLM_INFO_IF("No changes were found on the composite '" + m_composite.lock()->getID() + "', nothing to notify.",
                 m_addedObjects.empty() && m_newChangedObjects.empty() && m_removedObjects.empty());
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwDataTools
