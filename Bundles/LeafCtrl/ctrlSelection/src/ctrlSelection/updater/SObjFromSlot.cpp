/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/updater/SObjFromSlot.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace ctrlSelection
{

namespace updater
{

const ::fwCom::Slots::SlotKeyType SObjFromSlot::s_ADD_OR_SWAP_SLOT       = "addOrSwap";
const ::fwCom::Slots::SlotKeyType SObjFromSlot::s_ADD_SLOT               = "add";
const ::fwCom::Slots::SlotKeyType SObjFromSlot::s_SWAP_OBJ_SLOT          = "swapObj";
const ::fwCom::Slots::SlotKeyType SObjFromSlot::s_REMOVE_SLOT            = "remove";
const ::fwCom::Slots::SlotKeyType SObjFromSlot::s_REMOVE_IF_PRESENT_SLOT = "removeIfPresent";

static const std::string s_OBJECT = "object";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SObjFromSlot, ::fwData::Composite );

//-----------------------------------------------------------------------------

SObjFromSlot::SObjFromSlot() noexcept
{
    m_slots(s_ADD_OR_SWAP_SLOT, &SObjFromSlot::addOrSwap, this)
        (s_ADD_SLOT, &SObjFromSlot::add, this)
        (s_SWAP_OBJ_SLOT, &SObjFromSlot::swap, this)
        (s_REMOVE_SLOT, &SObjFromSlot::remove, this)
        (s_REMOVE_IF_PRESENT_SLOT, &SObjFromSlot::removeIfPresent, this);

    m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SObjFromSlot::~SObjFromSlot() noexcept
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    if(!this->isVersion2())
    {
        ConfigType cfg = m_configuration->findConfigurationElement("compositeKey");
        SLM_ASSERT("Missing element 'compositeKey'", cfg );

        m_compositeKey = cfg->getValue();
        SLM_ASSERT("Missing 'compositeKey' value", !m_compositeKey.empty());
    }
}

//-----------------------------------------------------------------------------

void SObjFromSlot::starting()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::stopping()
{
}


//-----------------------------------------------------------------------------

void SObjFromSlot::updating()
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SObjFromSlot::add(::fwData::Object::sptr obj)
{
    if(this->isVersion2())
    {
        this->setOutput(s_OBJECT, obj);
    }
    else
    {
        ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
        helper.add(m_compositeKey, obj);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SObjFromSlot::addOrSwap(::fwData::Object::sptr obj)
{
    if(this->isVersion2())
    {
        this->setOutput(s_OBJECT, obj);
    }
    else
    {
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
}

//-----------------------------------------------------------------------------

void SObjFromSlot::swap(::fwData::Object::sptr obj)
{
    if(this->isVersion2())
    {
        this->setOutput(s_OBJECT, obj);
    }
    else
    {
        ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
        helper.swap(m_compositeKey, obj);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SObjFromSlot::remove()
{
    if(this->isVersion2())
    {
        this->setOutput(s_OBJECT, nullptr);
    }
    else
    {
        ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
        helper.remove(m_compositeKey);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SObjFromSlot::removeIfPresent()
{
    if(this->isVersion2())
    {
        this->setOutput(s_OBJECT, nullptr);
    }
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
        ::fwDataTools::helper::Composite helper(composite);
        if (composite->find(m_compositeKey) != composite->end())
        {
            helper.remove(m_compositeKey);
        }
        helper.notify();
    }
}

//-----------------------------------------------------------------------------


} // updater
} // ctrlSelection
