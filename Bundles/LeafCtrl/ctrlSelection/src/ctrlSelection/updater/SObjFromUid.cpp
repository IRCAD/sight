/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/updater/SObjFromUid.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwServices/macros.hpp>


namespace ctrlSelection
{

namespace updater
{

const ::fwCom::Slots::SlotKeyType SObjFromUid::s_ADD_OR_SWAP_SLOT       = "addOrSwap";
const ::fwCom::Slots::SlotKeyType SObjFromUid::s_ADD_SLOT               = "add";
const ::fwCom::Slots::SlotKeyType SObjFromUid::s_SWAP_OBJ_SLOT          = "swapObj";
const ::fwCom::Slots::SlotKeyType SObjFromUid::s_REMOVE_SLOT            = "remove";
const ::fwCom::Slots::SlotKeyType SObjFromUid::s_REMOVE_IF_PRESENT_SLOT = "removeIfPresent";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SObjFromUid, ::fwData::Composite );

//-----------------------------------------------------------------------------

SObjFromUid::SObjFromUid() throw()
{
    m_slots(s_ADD_OR_SWAP_SLOT, &SObjFromUid::addOrSwap, this)
        (s_ADD_SLOT, &SObjFromUid::add, this)
        (s_SWAP_OBJ_SLOT, &SObjFromUid::swap, this)
        (s_REMOVE_SLOT, &SObjFromUid::remove, this)
        (s_REMOVE_IF_PRESENT_SLOT, &SObjFromUid::removeIfPresent, this);

    m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SObjFromUid::~SObjFromUid() throw()
{
}

//-----------------------------------------------------------------------------

void SObjFromUid::configuring()  throw ( ::fwTools::Failed )
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType objCfg = m_configuration->findConfigurationElement("uid");
    SLM_ASSERT("Missing element 'uid'", objCfg );

    m_objectUid = objCfg->getValue();

    SLM_ASSERT("Missing 'uid' value", !m_objectUid.empty());


    ConfigType cfg = m_configuration->findConfigurationElement("compositeKey");

    SLM_ASSERT("Missing element 'compositeKey'", cfg );

    m_compositeKey = cfg->getValue();

    SLM_ASSERT("Missing 'compositeKey' value", !m_compositeKey.empty());
}

//-----------------------------------------------------------------------------

void SObjFromUid::starting()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SObjFromUid::stopping()  throw ( ::fwTools::Failed )
{
}


//-----------------------------------------------------------------------------

void SObjFromUid::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SObjFromUid::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SObjFromUid::add()
{
    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
    helper.add(m_compositeKey, obj);
    helper.notify();
}

//-----------------------------------------------------------------------------

void SObjFromUid::addOrSwap()
{
    ::fwData::Object::sptr obj          = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwDataTools::helper::Composite helper(composite);
    if (composite->find(m_compositeKey) == composite->end())
    {
        helper.add(m_compositeKey, obj);
    }
    else
    {
        helper.swap(m_compositeKey, obj);
    }
    helper.notify();
}

//-----------------------------------------------------------------------------

void SObjFromUid::swap()
{
    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
    helper.swap(m_compositeKey, obj);
    helper.notify();
}

//-----------------------------------------------------------------------------

void SObjFromUid::remove()
{
    ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
    helper.remove(m_compositeKey);
    helper.notify();
}

//-----------------------------------------------------------------------------

void SObjFromUid::removeIfPresent()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwDataTools::helper::Composite helper(composite);
    if (composite->find(m_compositeKey) != composite->end())
    {
        helper.remove(m_compositeKey);
    }
    helper.notify();
}

//-----------------------------------------------------------------------------


} // updater
} // ctrlSelection
