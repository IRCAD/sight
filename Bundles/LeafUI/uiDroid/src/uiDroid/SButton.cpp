/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiDroid/SButton.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <JUIWindow.h>
#include <JUIButton.h>

namespace uiDroid
{

//------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::uiDroid::SButton, ::fwData::Object);

//------------------------------------------------------------------------------

SButton::SButton() throw() :
    m_boolState(true)
{
}

//------------------------------------------------------------------------------

SButton::~SButton() throw()
{
}

//------------------------------------------------------------------------------

void SButton::configuring() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr cfg;
    cfg     = m_configuration->findConfigurationElement("label");
    m_label = cfg->getValue();


    ::fwRuntime::ConfigurationElement::sptr slotCfg = m_configuration->findConfigurationElement("slot");

    std::string src, uid, key;

    src = slotCfg->getValue();
    size_t pos = src.find("/");
    if( std::string::npos != pos)
    {
        uid.assign(src, 0,pos);
        key.assign(src,pos +1,src.size() - pos);

        SLM_ASSERT("Missing hasSlotsId attribute", !uid.empty());
        SLM_ASSERT("Missing slotKey attribute", !key.empty());

        m_slotPair = std::make_pair(uid, key);
    }
}
//------------------------------------------------------------------------------

void SButton::starting() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    android_app* app = profile->getApp();

    const int32_t MARGIN = 40;

    // Using ndkGUi, a support library, to create and bind game management UIs
    int32_t win_width  = ANativeWindow_getWidth(app->window);
    int32_t win_height = ANativeWindow_getHeight(app->window);

    if (win_height <= 0 || win_width <= 0)
    {
        LOGE("Failed to get native window size");
        return;
    }
    if (win_height > win_width)
    {
        int32_t tmp = win_width;
        win_width  = win_height;
        win_height = tmp;
    }

    int32_t button_raw_width = win_width / 8;  // we have 2 buttons
    int32_t button_height    = win_height / 8;
    static int cur_idx       = 0;


    ndkGui::JUIButton *button = new ndkGui::JUIButton(m_label.c_str());
    button->AddRule(ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_LEFT,
                    ndkGui::LAYOUT_PARAMETER_TRUE);
    button->AddRule(ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_TOP,
                    ndkGui::LAYOUT_PARAMETER_TRUE);
    button->SetAttribute("MinimumWidth", button_raw_width - MARGIN);
    button->SetAttribute("MinimumHeight", button_height);
    button->SetMargins(MARGIN, MARGIN + cur_idx * button_height, MARGIN, MARGIN);


    std::string HasSlotId = m_slotPair.first;
    ::fwCom::Slots::SlotKeyType slotKey = m_slotPair.second;

    SLM_ASSERT(" the slot  "<<HasSlotId<<" not found",::fwTools::fwID::exist(HasSlotId));
    ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(HasSlotId);
    ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
    SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

    m_slot = hasSlots->slot(slotKey);

    std::function<void(ndkGui::JUIView *, const int32_t)> button_handlers =
        [this](ndkGui::JUIView *button, const int32_t msg)
        {
            if (msg == ndkGui::JUICALLBACK_BUTTON_UP)
            {

                onClicked();
            }
        };

    button->SetCallback(button_handlers);

    ndkGui::JUIWindow::GetInstance()->AddView(button);
    cur_idx++;

}

//------------------------------------------------------------------------------

void SButton::stopping() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SButton::updating() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SButton::onClicked()
{
    m_boolState = !m_boolState;
    m_slot->asyncRun(m_boolState); // defautl param is allowed
}

} // namespace uiDroid

