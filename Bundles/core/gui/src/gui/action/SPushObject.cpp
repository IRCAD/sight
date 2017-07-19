/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SPushObject.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <boost/regex.hpp>

namespace gui
{
namespace action
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

static const std::string s_SOURCE_KEY      = "source";
static const std::string s_DESTINATION_KEY = "destination";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SPushObject, ::fwData::Composite );

//------------------------------------------------------------------------------

SPushObject::SPushObject() noexcept
{
    newSlot(s_UPDATE_OBJECTS_SLOT, &SPushObject::updateObjects, this);
}

//------------------------------------------------------------------------------

SPushObject::~SPushObject() noexcept
{
}

//------------------------------------------------------------------------------

void SPushObject::configuring()
{
    this->initialize();

    if(this->isVersion2())
    {
        const ConfigurationType pushCfg = m_configuration->findConfigurationElement("push");

        SLM_ASSERT("<push> tag must have one attribute 'srcKey'.", pushCfg->hasAttribute("srcKey"));
        m_srcKey = pushCfg->getAttributeValue("srcKey");
    }
    else
    {
        std::vector < ConfigurationType > pushConfig = m_configuration->find("push", "", "", 3);
        std::string src, srcUid, srcKey, key;
        ::boost::regex re("(.*)\\[(.*)\\]");
        ::boost::smatch match;
        for( ConfigurationType pushItem :  pushConfig)
        {
            SLM_ASSERT("<push> tag must have one attribute key.", pushItem->hasAttribute("key"));
            key = pushItem->getAttributeValue("key");
            SLM_ASSERT("<push> tag must have one attribute src.", pushItem->hasAttribute("src"));
            src = pushItem->getAttributeValue("src");
            if( ::boost::regex_match(src, match, re) )
            {
                OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
                srcUid.assign(match[1].first, match[1].second);
                srcKey.assign(match[2].first, match[2].second);
                m_key2src[key] = std::make_pair(srcUid, srcKey);
                m_srcMap[srcUid].insert(srcKey);
                SLM_TRACE("Add " + srcUid + "[" + srcKey + "]");
            }
            else
            {
                OSLM_FATAL("The regexp \"" << re << "\" does not match \"" << src << "\"");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SPushObject::starting()
{
    this->actionServiceStarting();
    this->updateObjects();
}

//------------------------------------------------------------------------------

void SPushObject::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SPushObject::updating()
{
    if(this->isVersion2())
    {
        ::fwData::Composite::sptr compositeSrc = this->getInOut< ::fwData::Composite >(s_SOURCE_KEY);
        SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

        ::fwData::Object::sptr obj = compositeSrc->at< ::fwData::Object>(m_srcKey);

        SLM_WARN_IF("'" + m_srcKey + "' not found in composite '" + compositeSrc->getID() + "'", obj == nullptr);
        if (::fwServices::OSR::isRegistered(s_DESTINATION_KEY, ::fwServices::IService::AccessType::OUTPUT,
                                            this->getSptr()))
        {
            this->setOutput(s_DESTINATION_KEY, nullptr);
        }
        else if (obj)
        {
            this->setOutput(s_DESTINATION_KEY, obj);
        }
    }
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
        std::shared_ptr< ::fwDataTools::helper::Composite > compositeHelper( new ::fwDataTools::helper::Composite(
                                                                                 composite ) );

        for(DestKeyMapType::value_type elt :  m_key2src )
        {
            const auto& key    = elt.first;
            const auto& srcUid = elt.second.first;
            const auto& srcKey = elt.second.second;
            SLM_ASSERT( srcUid + " doesn't exist", ::fwTools::fwID::exist(srcUid) );
            ::fwData::Composite::sptr compositeSrc = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(
                                                                                           srcUid ) );
            SLM_ASSERT("fwData::Composite dynamicCast failed for "+srcUid, compositeSrc);

            ::fwData::Composite::const_iterator iter = compositeSrc->find(srcKey);

            SLM_WARN_IF("'" + srcKey + "' not found in composite '" + srcUid + "'", iter == compositeSrc->end());
            if (iter != compositeSrc->end())
            {
                ::fwData::Object::sptr obj = compositeSrc->getContainer()[srcKey];

                if ( composite->find(key) != composite->end() )
                {
                    compositeHelper->remove(key);
                }
                else
                {
                    compositeHelper->add(key, obj);
                }
            }
        }
        // Notification of message
        compositeHelper->notify();
    }
}

//------------------------------------------------------------------------------

void SPushObject::updateObjects()
{
    bool executable = true;
    std::string srcUid;

    if(this->isVersion2())
    {
        ::fwData::Composite::sptr compositeSrc = this->getInOut< ::fwData::Composite >(s_SOURCE_KEY);
        SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

        executable = (compositeSrc->find(m_srcKey) != compositeSrc->end());
        OSLM_TRACE("start check : " << compositeSrc->getID() << "[" << m_srcKey << "] : " <<
                   (compositeSrc->find(m_srcKey) != compositeSrc->end()) );
    }
    else
    {
        for(const SrcKeyMapType::value_type& valElt :  m_srcMap )
        {
            srcUid = valElt.first;

            OSLM_ASSERT( srcUid << " doesn't exist", ::fwTools::fwID::exist(srcUid) );
            ::fwData::Composite::sptr compositeSrc = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(
                                                                                           srcUid ) );
            OSLM_ASSERT("fwData::Composite dynamicCast failed for "<<srcUid, compositeSrc);
            for(SrcKeyMapType::key_type keyElt :  valElt.second )
            {
                executable &= (compositeSrc->find(keyElt) != compositeSrc->end());
                OSLM_TRACE("start check : " << srcUid << "[" << keyElt << "] : " <<
                           (compositeSrc->find(keyElt) != compositeSrc->end()) );
            }
        }
    }
    this->::fwGui::IActionSrv::setIsExecutable( executable );

    if(executable && this->::fwGui::IActionSrv::getIsActive())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SPushObject::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPushObject::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SOURCE_KEY, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT );
    connections.push( s_SOURCE_KEY, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace gui
