/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiDroid/SSlider.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

namespace uiDroid
{

//------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SSlider::s_VALUE_CHANGED_SIG = "valueChanged";

//------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::uiDroid::SSlider, ::fwData::Object);

//------------------------------------------------------------------------------

SSlider::SSlider() throw()
{
    m_sigValueChanged = ValueChangedSignalType::New();
    m_signals( s_VALUE_CHANGED_SIG,  m_sigValueChanged);
}

//------------------------------------------------------------------------------

SSlider::~SSlider() throw()
{
}

//------------------------------------------------------------------------------

void SSlider::configuring() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr cfg;
    cfg = m_configuration->findConfigurationElement("max");
    std::istringstream(cfg->getValue()) >> m_max;

    cfg = m_configuration->findConfigurationElement("value");
    std::istringstream(cfg->getValue()) >> m_value;
}
//------------------------------------------------------------------------------

void SSlider::starting() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_manager = new ::arGuiDroid::ViewManager();
    m_manager->createSlider(m_max);
}

//------------------------------------------------------------------------------

void SSlider::stopping() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SSlider::updating() throw (fwTools::Failed)
{
    m_manager->update();
}

//------------------------------------------------------------------------------
} // namespace uiDroid
