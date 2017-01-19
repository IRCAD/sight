/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
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

const ::fwServices::IService::KeyType s_COMPOSITE_INOUT = "composite";
const ::fwServices::IService::KeyType s_OBJECT_INOUT    = "object";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SObjFromUid, ::fwData::Composite );

// -----------------------------------------------------------------------------

SObjFromUid::SObjFromUid() throw()
{
    m_slots(s_ADD_OR_SWAP_SLOT, &SObjFromUid::addOrSwap, this)
        (s_ADD_SLOT, &SObjFromUid::add, this)
        (s_SWAP_OBJ_SLOT, &SObjFromUid::swap, this)
        (s_REMOVE_SLOT, &SObjFromUid::remove, this)
        (s_REMOVE_IF_PRESENT_SLOT, &SObjFromUid::removeIfPresent, this);

    m_slots.setWorker(m_associatedWorker);
}

// -----------------------------------------------------------------------------

SObjFromUid::~SObjFromUid() throw()
{
}

// -----------------------------------------------------------------------------

void SObjFromUid::configuring()  throw ( ::fwTools::Failed )
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    if (!this->isVersion2())
    {
        ConfigType objCfg = m_configuration->findConfigurationElement("uid");
        SLM_ASSERT("Missing element 'uid'", objCfg );

        m_objectUid = objCfg->getValue();

        SLM_ASSERT("Missing 'uid' value", !m_objectUid.empty());
    }

    ConfigType cfg = m_configuration->findConfigurationElement("compositeKey");

    SLM_ASSERT("Missing element 'compositeKey'", cfg );

    m_compositeKey = cfg->getValue();

    SLM_ASSERT("Missing 'compositeKey' value", !m_compositeKey.empty());
}

// -----------------------------------------------------------------------------

void SObjFromUid::starting()  throw ( ::fwTools::Failed )
{
}

// -----------------------------------------------------------------------------

void SObjFromUid::stopping()  throw ( ::fwTools::Failed )
{
}

// -----------------------------------------------------------------------------

void SObjFromUid::updating() throw ( ::fwTools::Failed )
{
}

// -----------------------------------------------------------------------------

void SObjFromUid::info( std::ostream& _sstream )
{
}

// -----------------------------------------------------------------------------

void SObjFromUid::add()
{
    ::fwData::Composite::sptr composite;
    ::fwData::Object::sptr obj;
    if (this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
        SLM_ASSERT("Object '" + s_COMPOSITE_INOUT + "' is missing.", composite);
        obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
        obj       = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    }
    ::fwDataTools::helper::Composite helper( composite );
    helper.add(m_compositeKey, obj);
    helper.notify();
}

// -----------------------------------------------------------------------------

void SObjFromUid::addOrSwap()
{
    ::fwData::Composite::sptr composite;
    ::fwData::Object::sptr obj;
    if (this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
        SLM_ASSERT("Object '" + s_COMPOSITE_INOUT + "' is missing.", composite);
        obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
        obj       = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    }
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

// -----------------------------------------------------------------------------

void SObjFromUid::swap()
{
    ::fwData::Composite::sptr composite;
    ::fwData::Object::sptr obj;
    if (this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
        SLM_ASSERT("Object '" + s_COMPOSITE_INOUT + "' is missing.", composite);
        obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
        obj       = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(m_objectUid));
    }
    ::fwDataTools::helper::Composite helper( composite );
    helper.swap(m_compositeKey, obj);
    helper.notify();
}

// -----------------------------------------------------------------------------

void SObjFromUid::remove()
{
    ::fwData::Composite::sptr composite;
    if (this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
        SLM_ASSERT("Object '" + s_COMPOSITE_INOUT + "' is missing.", composite);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
    }
    ::fwDataTools::helper::Composite helper( composite );
    helper.remove(m_compositeKey);
    helper.notify();
}

// -----------------------------------------------------------------------------

void SObjFromUid::removeIfPresent()
{
    ::fwData::Composite::sptr composite;
    if (this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
        SLM_ASSERT("Object '" + s_COMPOSITE_INOUT + "' is missing.", composite);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
    }
    if (composite->find(m_compositeKey) != composite->end())
    {
        ::fwDataTools::helper::Composite helper(composite);
        helper.remove(m_compositeKey);
        helper.notify();
    }
}

// -----------------------------------------------------------------------------

} // updater
} // ctrlSelection
