/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiDroid/SButton.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

namespace uiDroid
{

//------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SButton::s_CLICKED_SIG = "clicked";

//------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::uiDroid::SButton, ::fwData::Object);

//------------------------------------------------------------------------------

SButton::SButton() throw()
{
    m_sigClicked = ClickedSignalType::New();
    m_signals( s_CLICKED_SIG,  m_sigClicked);
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

    m_manager = new ::arGuiDroid::ViewManager();
    m_manager->createButton(m_label);
}

//------------------------------------------------------------------------------

void SButton::stopping() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SButton::updating() throw (fwTools::Failed)
{
    m_manager->update();
}

//------------------------------------------------------------------------------
} // namespace uiDroid
