/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/PushObjectSrv.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Composite.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>

#include <boost/regex.hpp>

namespace gui
{
namespace action
{


static const ::fwCom::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::PushObjectSrv, ::fwData::Composite );

//------------------------------------------------------------------------------

PushObjectSrv::PushObjectSrv() throw()
{
    newSlot(s_UPDATE_OBJECTS_SLOT, &PushObjectSrv::updateObjects, this);
}

//------------------------------------------------------------------------------

PushObjectSrv::~PushObjectSrv() throw()
{
}

//------------------------------------------------------------------------------

void PushObjectSrv::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
    bool executable = true;
    std::string src_uid;
    for(const SrcKeyMapType::value_type& valElt :  m_srcMap )
    {
        src_uid = valElt.first;
        SLM_TRACE("start check : " + src_uid );
        OSLM_ASSERT( src_uid << " doesn't exist", ::fwTools::fwID::exist(src_uid) );
        ::fwData::Composite::sptr composite_src = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(
                                                                                        src_uid ) );
        OSLM_ASSERT("fwData::Composite dynamicCast failed for "<<src_uid, composite_src);
        for(SrcKeyMapType::key_type keyElt :  valElt.second )
        {
            executable &= (composite_src->find(keyElt)!= composite_src->end());
            OSLM_TRACE("start check : " << src_uid << "[" << keyElt << "] : " <<
                       (composite_src->find(keyElt)!= composite_src->end()) );
        }
    }
    this->::fwGui::IActionSrv::setIsExecutable( executable );
}

//------------------------------------------------------------------------------

void PushObjectSrv::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void PushObjectSrv::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    std::vector < ConfigurationType > pushConfig = m_configuration->find("push","","",3);
    std::string src, src_uid, src_key, key;
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
            src_uid.assign(match[1].first, match[1].second);
            src_key.assign(match[2].first, match[2].second);
            m_key2src[key] = std::make_pair(src_uid, src_key);
            m_srcMap[src_uid].insert(src_key);
            SLM_TRACE("Add " + src_uid + "[" + src_key + "]");
        }
        else
        {
            OSLM_FATAL("The regexp \"" << re << "\" does not match \"" << src << "\"");
        }
    }
}

//------------------------------------------------------------------------------

void PushObjectSrv::updating() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    std::shared_ptr< ::fwComEd::helper::Composite > compositeHelper ( new ::fwComEd::helper::Composite( composite ) );

    std::string src_uid("");
    std::string src_key("");
    std::string key("");
    for(DestKeyMapType::value_type elt :  m_key2src )
    {
        key     = elt.first;
        src_uid = elt.second.first;
        src_key = elt.second.second;
        SLM_ASSERT( src_uid + " doesn't exist", ::fwTools::fwID::exist(src_uid) );
        ::fwData::Composite::sptr composite_src = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(
                                                                                        src_uid ) );
        SLM_ASSERT("fwData::Composite dynamicCast failed for "+src_uid, composite_src);


        ::fwData::Composite::const_iterator iter = composite_src->find(src_key);

        SLM_WARN_IF("'" + src_key + "' not found in composite '" + src_uid + "'",iter == composite_src->end());
        if (iter != composite_src->end())
        {
            ::fwData::Object::sptr obj = composite_src->getContainer()[src_key];

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

//------------------------------------------------------------------------------

void PushObjectSrv::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void PushObjectSrv::updateObjects()
{
    bool executable = !m_srcMap.empty();

    std::string src_uid;
    std::string src_key;
    std::string key;
    for(const SrcKeyMapType::value_type& valElt :  m_srcMap )
    {
        src_uid = valElt.first;
        SLM_ASSERT( src_uid + " doesn't exist", ::fwTools::fwID::exist(src_uid) );
        ::fwData::Composite::sptr composite_src = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject(
                                                                                        src_uid ) );
        SLM_ASSERT("fwData::Composite dynamicCast failed for "+ src_uid, composite_src);

        for(SrcKeyMapType::key_type keyElt :  valElt.second )
        {
            executable &= (composite_src->find(keyElt)!= composite_src->end());
        }
    }

    this->setIsExecutable( executable );

    //TODO managed active mode (objects already present in target composite)
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType PushObjectSrv::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

}
}

