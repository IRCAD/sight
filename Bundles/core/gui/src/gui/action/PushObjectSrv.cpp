/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/regex.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

#include "gui/action/PushObjectSrv.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::PushObjectSrv, ::fwData::Composite );

//------------------------------------------------------------------------------

PushObjectSrv::PushObjectSrv() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_KEYS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS );
}

//------------------------------------------------------------------------------

PushObjectSrv::~PushObjectSrv() throw()
{}

//------------------------------------------------------------------------------

void PushObjectSrv::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
//    bool executable = true;
//    std::string src_uid;
//    BOOST_FOREACH(SrcKeyMapType::value_type valElt, m_srcMap )
//    {
//        src_uid = valElt.first;
//        OSLM_ASSERT( src_uid << " doesn't exist", ::fwTools::fwID::exist(src_uid) );
//        ::fwData::Composite::sptr composite_src = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( src_uid ) );
//        OSLM_ASSERT("fwData::Composite dynamicCast failed for "<<src_uid, composite_src);
//        BOOST_FOREACH(SrcKeyMapType::key_type keyElt, valElt.second )
//        {
//            executable &= (composite_src->find(keyElt)!= composite_src->end());
//        }
//    }
//    this->::fwGui::IActionSrv::setIsExecutable( executable );
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

    std::vector < ConfigurationType > pushConfig = m_configuration->find("push");
    std::string src, src_uid, src_key, key;
    ::boost::regex re("(.*)\\[(.*)\\]");
    ::boost::smatch match;
    BOOST_FOREACH( ConfigurationType pushItem, pushConfig)
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
    ::boost::shared_ptr< ::fwComEd::helper::Composite > compositeHelper ( new ::fwComEd::helper::Composite( composite ) );

    std::string src_uid("");
    std::string src_key("");
    std::string key("");
    BOOST_FOREACH(DestKeyMapType::value_type elt, m_key2src )
    {
        key = elt.first;
        src_uid = elt.second.first;
        src_key = elt.second.second;
        OSLM_ASSERT( src_uid << " doesn't exist", ::fwTools::fwID::exist(src_uid) );
        ::fwData::Composite::sptr composite_src = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( src_uid ) );
        OSLM_ASSERT("fwData::Composite dynamicCast failed for "<<src_uid, composite_src);
        OSLM_ASSERT(src_key << " not found in composite "<<src_uid, composite_src->find(src_key) != composite_src->end());
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
    // Notification of message
    compositeHelper->notify( this->getSptr() );
}

//------------------------------------------------------------------------------

void PushObjectSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    //TODO
}

//------------------------------------------------------------------------------

void PushObjectSrv::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

}
}

