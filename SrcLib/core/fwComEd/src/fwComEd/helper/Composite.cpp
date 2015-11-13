/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/helper/Composite.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/IService.hpp>

#include <functional>

namespace fwComEd
{
namespace helper
{

//-----------------------------------------------------------------------------

Composite::Composite( ::fwData::Composite::wptr _composite )
    :   m_compositeMsg ( ::fwComEd::CompositeMsg::New() ),
      m_composite ( _composite )
{
}

//-----------------------------------------------------------------------------

Composite::~Composite()
{
}

//-----------------------------------------------------------------------------

void Composite::add( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must not exist in composite.",
                   m_composite.lock()->find(_compositeKey) != m_composite.lock()->end() );

    // Modify composite
    m_composite.lock()->getContainer()[ _compositeKey ] = _newObject;

    // Modify message
    m_compositeMsg->appendAddedKey( _compositeKey, _newObject );

}

//-----------------------------------------------------------------------------

void Composite::remove( std::string _compositeKey )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must exist in composite.",
                   m_composite.lock()->find(_compositeKey) == m_composite.lock()->end() );

    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getContainer()[ _compositeKey ];

    // Modify composite
    m_composite.lock()->getContainer().erase( _compositeKey );

    // Modify message
    m_compositeMsg->appendRemovedKey( _compositeKey, objBackup );

}

//-----------------------------------------------------------------------------

void Composite::clear()
{
    ::fwData::Composite::sptr composite = m_composite.lock();
    std::vector<std::string> vectKey;
    std::transform( composite->begin(), composite->end(),
                    std::back_inserter(vectKey),
                    std::bind(&::fwData::Composite::value_type::first, std::placeholders::_1) );

    BOOST_FOREACH(std::string key, vectKey)
    {
        this->remove(key);
    }
}

//-----------------------------------------------------------------------------

void Composite::swap( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must exist in composite.",
                   m_composite.lock()->find(_compositeKey) == m_composite.lock()->end() );


    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getContainer()[ _compositeKey ];

    if( objBackup != _newObject )
    {
        // Modify composite
        m_composite.lock()->getContainer()[ _compositeKey ] = _newObject;

        // Modify message
        m_compositeMsg->appendChangedKey( _compositeKey, objBackup, _newObject );
    }
    else
    {
        OSLM_INFO(
            "Cannot swap this object ( "<< _compositeKey <<
            " ) in composite because it is the same object. Do nothing (not notification)");
    }
}

//-----------------------------------------------------------------------------

void Composite::notify( ::fwServices::IService::sptr serviceSource, bool _allowLoops )
{
    if ( m_compositeMsg->getEventIds().size() > 0 )
    {
        m_compositeMsg->setSource( serviceSource );
        m_compositeMsg->setSubject( m_composite.lock() );
        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = m_composite.lock()->signal< ::fwData::Object::ObjectModifiedSignalType >(
            ::fwData::Object::s_OBJECT_MODIFIED_SIG);

        if(_allowLoops)
        {
            sig->asyncEmit(m_compositeMsg);
        }
        else
        {
            ::fwServices::IService::ReceiveSlotType::sptr slot;
            slot = serviceSource->slot< ::fwServices::IService::ReceiveSlotType >(
                ::fwServices::IService::s_RECEIVE_SLOT );
            ::fwCom::Connection::Blocker block(sig->getConnection(slot));
            sig->asyncEmit(m_compositeMsg);
        }

    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.",
                m_compositeMsg->getEventIds().size() == 0);
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwComEd
