/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiDroid/SButton.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hxx>

#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <JUIWindow.h>
#include <JUIButton.h>

namespace uiDroid
{

//------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SButton::s_LOAD_CLICKED_SIG      = "loadClicked";
const ::fwCom::Signals::SignalKeyType SButton::s_SHOW_HIDE_CLICKED_SIG = "showHideclicked";

//------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::uiDroid::SButton, ::fwData::Object);

//------------------------------------------------------------------------------

SButton::SButton() throw() :
    m_visible(true)
{
    m_sigLoadCliked     = LoadClikedSignalType::New();
    m_sigShowHideCliked = ShowHideClikedSignalType::New();
    m_signals( s_LOAD_CLICKED_SIG,  m_sigLoadCliked);
    m_signals( s_SHOW_HIDE_CLICKED_SIG,  m_sigShowHideCliked);
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
    int cur_idx              = 0;

    const char *titles[UI_BUTTON_COUNT]                                     = {"load", "show/hide"};
    std::function<void(ndkGui::JUIView *, const int32_t)> button_handlers[] =
    {
        [this](ndkGui::JUIView *button, const int32_t msg)
        {
            if (msg == ndkGui::JUICALLBACK_BUTTON_UP)
            {
                onLoadButtonClicked();
            }
        },
        [this](ndkGui::JUIView *button, const int32_t msg)
        {
            if (msg == ndkGui::JUICALLBACK_BUTTON_UP)
            {
                onShowHideButtonClicked();
            }
        },
    };

    for (cur_idx = 0; cur_idx < UI_BUTTON_COUNT; cur_idx++)
    {
        ndkGui::JUIButton *button = new ndkGui::JUIButton(titles[cur_idx]);
        button->AddRule(ndkGui::LAYOUT_PARAMETER_ALIGN_LEFT,
                        ndkGui::LAYOUT_PARAMETER_TRUE);
        button->AddRule(ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_LEFT,
                        ndkGui::LAYOUT_PARAMETER_TRUE);
        button->AddRule(ndkGui::LAYOUT_ORIENTATION_VERTICAL,
                        ndkGui::LAYOUT_PARAMETER_TRUE);
        button->SetAttribute("MinimumWidth", button_raw_width - MARGIN);
        button->SetAttribute("MinimumHeight", button_height);
        button->SetMargins(MARGIN, MARGIN + cur_idx * button_height, MARGIN, MARGIN);
        button->SetCallback(button_handlers[cur_idx]);
        ndkGui::JUIWindow::GetInstance()->AddView(button);
    }
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

void SButton::onLoadButtonClicked()
{

    m_sigLoadCliked->asyncEmit();
}

//------------------------------------------------------------------------------

void SButton::onShowHideButtonClicked()
{
    m_visible = !m_visible;
    m_sigShowHideCliked->asyncEmit(m_visible);
}
} // namespace uiDroid
