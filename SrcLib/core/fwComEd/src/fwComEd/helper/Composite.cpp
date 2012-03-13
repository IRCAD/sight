#include <fwData/Composite.hpp>

#include <fwServices/IEditionService.hpp>

#include "fwComEd/helper/Composite.hpp"

namespace fwComEd
{
namespace helper
{

//-----------------------------------------------------------------------------

Composite::Composite( ::fwData::Composite::wptr _composite )
    :   m_compositeMsg ( new ::fwComEd::CompositeMsg() ),
        m_composite ( _composite )
{}

//-----------------------------------------------------------------------------

Composite::~Composite()
{}

//-----------------------------------------------------------------------------

void Composite::add( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must not exist in composite." , m_composite.lock()->getRefMap().find(_compositeKey) != m_composite.lock()->getRefMap().end() );

    // Modify composite
    m_composite.lock()->getRefMap()[ _compositeKey ] = _newObject;

    // Modify message
    m_compositeMsg->addAddedField( _compositeKey, _newObject );

}

//-----------------------------------------------------------------------------

void Composite::remove( std::string _compositeKey )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must exist in composite." , m_composite.lock()->getRefMap().find(_compositeKey) == m_composite.lock()->getRefMap().end() );

    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getRefMap()[ _compositeKey ];

    // Modify composite
    m_composite.lock()->getRefMap().erase( _compositeKey );

    // Modify message
    m_compositeMsg->addRemovedField( _compositeKey, objBackup );

}

//-----------------------------------------------------------------------------

void Composite::swap( std::string _compositeKey, ::fwData::Object::sptr _newObject )
{
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must exist in composite." , m_composite.lock()->getRefMap().find(_compositeKey) == m_composite.lock()->getRefMap().end() );


    // Get old object
    ::fwData::Object::sptr objBackup = m_composite.lock()->getRefMap()[ _compositeKey ];

    if( objBackup != _newObject )
    {
        // Modify composite
        m_composite.lock()->getRefMap()[ _compositeKey ] = _newObject;

        // Modify message
        m_compositeMsg->addSwappedField( _compositeKey, objBackup, _newObject );
    }
    else
    {
        OSLM_INFO("Cannot swap this object ( "<< _compositeKey <<" ) in composite because it is the same object. Do nothing (not notification)");
    }
}

//-----------------------------------------------------------------------------

void Composite::notify( ::fwServices::IService::sptr _serviceSource )
{
    if ( m_compositeMsg->getEventIds().size() > 0 )
    {
        ::fwServices::IEditionService::notify( _serviceSource, m_composite.lock(), m_compositeMsg , ::fwServices::ComChannelService::NOTIFY_SOURCE );
    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.", m_compositeMsg->getEventIds().size() == 0);
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwComEd
