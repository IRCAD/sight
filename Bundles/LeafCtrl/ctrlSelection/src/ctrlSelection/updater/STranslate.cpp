/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/updater/STranslate.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>


#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

namespace ctrlSelection
{

namespace updater
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::STranslate,
                         ::fwData::Composite );

//-----------------------------------------------------------------------------

STranslate::STranslate() noexcept
{
    newSlot(s_ADD_OBJECTS_SLOT, &STranslate::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &STranslate::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &STranslate::removeObjects, this);
}

//-----------------------------------------------------------------------------

STranslate::~STranslate() noexcept
{
}

//-----------------------------------------------------------------------------

void STranslate::starting()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    if (!m_sourceCompositeID.empty())
    {
        ::fwData::Composite::sptr source;
        source = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_sourceCompositeID));
        SLM_ASSERT("Source composite is missing '" + m_sourceCompositeID + "'", source);

        m_connections.connect(source, this->getSptr(), this->getObjSrvConnections() );

        for( const ManagedTranslations::value_type& trans :  m_managedTranslations )
        {
            const std::string& fromKey = trans.first;
            const std::string& toKey   = trans.second;

            ::fwData::Composite::const_iterator iter = source->find(fromKey);
            if (iter != source->end())
            {
                this->updateComposite(composite, iter->second, toKey, ADD_OR_SWAP );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void STranslate::stopping()
{
    m_connections.disconnect();
}

//-----------------------------------------------------------------------------

void STranslate::configuring()
{
    const ::fwServices::IService::ConfigType conf = this->getConfigTree().get_child("service");

    if (conf.count("source"))
    {
        const ::fwServices::IService::ConfigType& sourceCfg = conf.get_child("source");
        m_sourceCompositeID = sourceCfg.get_value<std::string>();
        SLM_ASSERT("Source composite is not defined", !m_sourceCompositeID.empty());
    }

    SLM_ASSERT("Problem with configuration for STranslate type, missing element \"translate\"",
               conf.count("translate") > 0);

    OSLM_DEBUG( "nb of translations = " << conf.count("translate") );
    m_managedTranslations.clear();
    BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type &v,  conf.equal_range("translate") )
    {
        const ::fwServices::IService::ConfigType& translate = v.second;
        const ::fwServices::IService::ConfigType xmlattr    = translate.get_child("<xmlattr>");

        SLM_FATAL_IF( "The attribute \"fromKey\" is missing, it represents the key of the object in the source "
                      "composite that will be translated", xmlattr.count("fromKey") != 1 );
        SLM_FATAL_IF("The attribute \"toKey\" is missing, it represents the the key of the object in the current"
                     "composite that will contain the translation", xmlattr.count("toKey") != 1);

        std::string fromKey = xmlattr.get<std::string>("fromKey");
        std::string toKey   = xmlattr.get<std::string>("toKey");

        OSLM_INFO(
            "Manage translation from "<< fromKey << " to "<< toKey <<
            " in my composite.");
        m_managedTranslations[fromKey] = toKey;
    }

}

//-----------------------------------------------------------------------------

void STranslate::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void STranslate::updating()
{
}

//-----------------------------------------------------------------------------

void STranslate::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void STranslate::addObjects(::fwData::Composite::ContainerType objects)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for ( ManagedTranslations::value_type elt : m_managedTranslations )
    {
        const std::string& fromKey = elt.first;
        const std::string& toKey   = elt.second;

        ::fwData::Composite::ContainerType::iterator iter = objects.find(fromKey);
        if (iter != objects.end())
        {
            // Udpate the composite object referenced by the composite key
            this->updateComposite(composite, iter->second, toKey, ADD );
        }
    }
}

//------------------------------------------------------------------------------

void STranslate::changeObjects(::fwData::Composite::ContainerType newObjects,
                               ::fwData::Composite::ContainerType oldObjects)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for ( ManagedTranslations::value_type elt : m_managedTranslations )
    {
        const std::string& fromKey = elt.first;
        const std::string& toKey   = elt.second;

        ::fwData::Composite::ContainerType::iterator iter = newObjects.find(fromKey);
        if (iter != newObjects.end())
        {
            // Udpate the composite object referenced by the composite key
            this->updateComposite(composite, iter->second, toKey, SWAP );
        }
    }
}

//------------------------------------------------------------------------------

void STranslate::removeObjects(::fwData::Composite::ContainerType objects)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for ( ManagedTranslations::value_type elt : m_managedTranslations )
    {
        const std::string& fromKey = elt.first;
        const std::string& toKey   = elt.second;

        ::fwData::Composite::ContainerType::iterator iter = objects.find(fromKey);
        if (iter != objects.end())
        {
            // Udpate the composite object referenced by the composite key
            this->updateComposite(composite, iter->second, toKey, REMOVE );
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType STranslate::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
