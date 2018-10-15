/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SSlotCaller.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/helper.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <boost/regex.hpp>

namespace gui
{
namespace action
{

static const ::fwServices::IService::KeyType s_SLOT_KEY        = "slot";
static const ::fwServices::IService::KeyType s_SLOTS_KEY       = "slots";
static const ::fwServices::IService::KeyType s_SYNCED_CALL_KEY = "sync";

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SSlotCaller );

//-----------------------------------------------------------------------------

SSlotCaller::SSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

SSlotCaller::~SSlotCaller() noexcept
{
}

//-----------------------------------------------------------------------------

void SSlotCaller::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SSlotCaller::stopping()
{
    SLM_TRACE_FUNC();

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SSlotCaller::info(std::ostream& _sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSlotCaller::updating()
{
    SLM_TRACE_FUNC();

    if(this->confirmAction())
    {
        for(const SlotInfoType& info :  m_slotInfos)
        {
            const HasSlotIDType& HasSlotId             = info.first;
            const ::fwCom::Slots::SlotKeyType& slotKey = info.second;

            if (::fwTools::fwID::exist(HasSlotId))
            {
                const ::fwTools::Object::sptr obj       = ::fwTools::fwID::getObject(HasSlotId);
                const ::fwCom::HasSlots::csptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

                const ::fwCom::SlotBase::csptr slot = hasSlots->slot(slotKey);

                if(m_synchronized)
                {
                    slot->run();
                }
                else
                {
                    slot->asyncRun();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSlotCaller::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();

    ConfigurationType syncCfg = m_configuration->findConfigurationElement(s_SYNCED_CALL_KEY);

    m_synchronized = syncCfg && syncCfg->getValue() == "true";

    OSLM_ASSERT("Missing slots configuration element in " << this->getID(),
                m_configuration->hasConfigurationElement(s_SLOTS_KEY));
    ConfigurationType cfg = m_configuration->findConfigurationElement(s_SLOTS_KEY);

    ::fwRuntime::ConfigurationElementContainer slotCfgs = cfg->findAllConfigurationElement(s_SLOT_KEY);

    ::boost::regex re("(.*)/(.*)");
    ::boost::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem :   slotCfgs.getElements())
    {
        src = elem->getValue();
        if( ::boost::regex_match(src, match, re) )
        {
            OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT("Missing hasSlotsId attribute", !uid.empty());
            SLM_ASSERT("Missing slotKey attribute", !key.empty());

            m_slotInfos.push_back( std::make_pair(uid, key) );
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace gui
