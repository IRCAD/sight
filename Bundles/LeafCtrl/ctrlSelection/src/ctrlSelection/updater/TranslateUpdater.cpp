/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/updater/TranslateUpdater.hpp"

#include <boost/property_tree/xml_parser.hpp>
namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::TranslateUpdater, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

TranslateUpdater::TranslateUpdater() throw()
{
    //handlingEventOff::fwComEd::CompositeMsg::ADDED_KEYS);
    //handlingEventOff::fwComEd::CompositeMsg::CHANGED_KEYS);
    //handlingEventOff::fwComEd::CompositeMsg::REMOVED_KEYS);
}

//-----------------------------------------------------------------------------

TranslateUpdater::~TranslateUpdater() throw()
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(_msg);
    SLM_ASSERT("Sorry, this service only manage compositeMsg", compositeMsg);

    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
    SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for (   ManagedTranslations::const_iterator it = m_managedTranslations.begin();
            it != m_managedTranslations.end();
            ++it )
    {
        const std::string &fwid     = it->get<0>();
        const std::string &fromKey  = it->get<1>();
        const std::string &toKey    = it->get<2>();

        // Test if we manage this event from this object message uid
        if( obj->getID() == fwid)
        {
            //  test if message correspond to a defined event
            if( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
            {
                ::fwData::Composite::sptr addedFields = compositeMsg->getAddedKeys();
                if (addedFields->find(fromKey) != addedFields->end())
                {
                    // Udpate the composite object referenced by the composite key
                    this->updateComposite(composite, (*addedFields)[fromKey] , toKey , ADD );
                }
            }
            else if (compositeMsg->hasEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS ))
            {
                ::fwData::Composite::sptr swappedFields = compositeMsg->getNewChangedKeys();
                if (swappedFields->find(fromKey) != swappedFields->end())
                {
                    // Udpate the composite object referenced by the composite key
                    this->updateComposite(composite, (*swappedFields)[fromKey] , toKey , SWAP );
                }
            }
            else if( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ))
            {
                ::fwData::Composite::sptr removedFields = compositeMsg->getRemovedKeys();
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
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    BOOST_FOREACH( const ManagedTranslations::value_type & trans, m_managedTranslations )
    {
        const std::string &fwid     = trans.get<0>();
        const std::string &fromKey  = trans.get<1>();
        const std::string &toKey    = trans.get<2>();

        ::fwData::Composite::sptr compositeFrom = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(fwid) );
        if (compositeFrom)
        {
            ::fwData::Composite::const_iterator iter = compositeFrom->find(fromKey);
            if (iter != compositeFrom->end())
            {
                this->updateComposite(composite, iter->second , toKey , ADD_OR_SWAP );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void TranslateUpdater::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void TranslateUpdater::configuring()  throw ( ::fwTools::Failed )
{
    const ::fwServices::IService::ConfigType conf = this->getConfigTree().get_child("service");

    SLM_ASSERT("Problem with configuration for ObjFromMsgUpdaterSrv type, missing element \"translate\"",
               conf.count("translate") > 0);

    OSLM_DEBUG( "nb of translations = " << conf.count("translate") );
    m_managedTranslations.clear();
    BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type &v, conf.equal_range("translate") )
    {
        const ::fwServices::IService::ConfigType &translate = v.second;
        const ::fwServices::IService::ConfigType xmlattr = translate.get_child("<xmlattr>");

        SLM_FATAL_IF( "Sorry, attribute \"fromKey\" is missing", xmlattr.count("fromKey") != 1 );
        SLM_FATAL_IF( "Sorry, attribute \"toKey\" is missing", xmlattr.count("toKey") != 1 );
        SLM_FATAL_IF( "Sorry, attribute \"fromUID\" is missing", xmlattr.count("fromUID") != 1 );

        std::string fromKey = xmlattr.get<std::string>("fromKey");
        std::string toKey   = xmlattr.get<std::string>("toKey");
        std::string fromUID = xmlattr.get<std::string>("fromUID");

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
