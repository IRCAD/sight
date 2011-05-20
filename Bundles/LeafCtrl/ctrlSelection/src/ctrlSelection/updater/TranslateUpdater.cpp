/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/updater/TranslateUpdater.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::TranslateUpdater, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

TranslateUpdater::TranslateUpdater() throw()
{
    addNewHandledEvent(::fwComEd::CompositeMsg::ADDED_FIELDS);
    addNewHandledEvent(::fwComEd::CompositeMsg::SWAPPED_FIELDS);
    addNewHandledEvent(::fwComEd::CompositeMsg::REMOVED_FIELDS);
}

//-----------------------------------------------------------------------------

TranslateUpdater::~TranslateUpdater() throw()
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(_msg);
    SLM_ASSERT("Sorry, this service only manage compositeMsg", compositeMsg);

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for (   ManagedTranslations::iterator it = m_managedTranslations.begin();
            it != m_managedTranslations.end();
            ++it )
    {
        std::string uuid     = it->get<0>();
        std::string fromKey  = it->get<1>();
        std::string toKey    = it->get<2>();

        ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
        SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");

        // Test if we manage this event from this object message uid
        if( obj->getID() == uuid)
        {
            //  test if message correspond to a defined event
            if( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
            {
                ::fwData::Composite::sptr addedFields = compositeMsg->getAddedFields();
                if (addedFields->find(fromKey) != addedFields->end())
                {
                    // Udpate the composite object referenced by the composite key
                    this->updateComposite(composite, (*addedFields)[fromKey] , toKey , ADD );
                }
            }
            else if (compositeMsg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS ))
            {
                ::fwData::Composite::sptr swappedFields = compositeMsg->getSwappedNewFields();
                if (swappedFields->find(fromKey) != swappedFields->end())
                {
                    // Udpate the composite object referenced by the composite key
                    this->updateComposite(composite, (*swappedFields)[fromKey] , toKey , SWAP );
                }
            }
            else if( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS ))
            {
                ::fwData::Composite::sptr removedFields = compositeMsg->getRemovedFields();
                if (removedFields->find(fromKey) != removedFields->end())
                {
                    // Udpate the composite object referenced by the composite key
                    this->updateComposite(composite, ::fwData::Object::sptr() , toKey , REMOVE );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void TranslateUpdater::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void TranslateUpdater::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleTranslations = m_configuration->findAllConfigurationElement("translate");

    SLM_ASSERT("Problem with configuration for ObjFromMsgUpdaterSrv type, missing element \"translate\"", handleTranslations.size() != 0 );
    OSLM_DEBUG( "handleEvents.size() = " << handleTranslations.size() );
    m_managedTranslations.clear();
    for(    ::fwRuntime::ConfigurationElementContainer::Iterator item = handleTranslations.begin();
            item != handleTranslations.end();
            ++item )
    {
        SLM_FATAL_IF( "Sorry, attribute \"fromKey\" is missing", !(*item)->hasAttribute("fromKey") );
        std::string fromKey =  (*item)->getExistingAttributeValue("fromKey");

        SLM_FATAL_IF( "Sorry, attribute \"toKey\" is missing", !(*item)->hasAttribute("toKey") );
        std::string toKey =  (*item)->getExistingAttributeValue("toKey");

        SLM_FATAL_IF( "Sorry, attribute \"fromUID\" is missing", !(*item)->hasAttribute("fromUID") );
        std::string fromUID =  (*item)->getExistingAttributeValue("fromUID");

        OSLM_INFO( "Manage translation from this object "<< fromUID <<", from "<< fromKey << " to "<< toKey <<" in my composite.");
        ::boost::tuple< std::string, std::string, std::string > managedTranslation (fromUID, fromKey, toKey);
        m_managedTranslations.push_back( managedTranslation );
    }
}

//-----------------------------------------------------------------------------

void TranslateUpdater::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
