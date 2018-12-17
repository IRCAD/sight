/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "uiVisu/action/CrossTypeAction.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Float.hpp>
#include <fwData/Image.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/assign/list_of.hpp>


namespace uiVisu
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiVisu::action::CrossTypeAction, ::fwData::Image );

static const ::fwCom::Signals::SignalKeyType s_CROSS_TYPE_MODIFIED_SIG = "crossTypeModified";

std::map< std::string, float >
CrossTypeAction::m_scaleConversion
    = ::boost::assign::map_list_of(std::string("full"),1.0)
          (std::string("half"),0.5)
          (std::string("hide"),0.0);

CrossTypeAction::CrossTypeAction() noexcept
{
    m_sigCrossTypeModified = newSignal< CrossTypeModifiedSignalType >(s_CROSS_TYPE_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

CrossTypeAction::~CrossTypeAction() noexcept
{
}

//------------------------------------------------------------------------------

void CrossTypeAction::starting()
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void CrossTypeAction::stopping()
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void CrossTypeAction::configuring()
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::initialize();

    if( this->m_configuration->size() > 0 )
    {
        std::vector < ConfigurationType > vectConfig = this->m_configuration->find("crossType");
        SLM_ASSERT("Missing <crossType> tag!", !vectConfig.empty());
        m_crossType = vectConfig.at(0)->getValue();
        ::boost::algorithm::trim(m_crossType);
        ::boost::algorithm::to_lower(m_crossType);
        SLM_ASSERT("Unknown crossType", m_scaleConversion.find(m_crossType) != m_scaleConversion.end());
    }
}

//------------------------------------------------------------------------------

void CrossTypeAction::updating()
{
    m_sigCrossTypeModified->asyncEmit(m_scaleConversion[m_crossType]);
}

//------------------------------------------------------------------------------

void CrossTypeAction::swapping()
{

}

//------------------------------------------------------------------------------

void CrossTypeAction::info( std::ostream &_sstream )
{
}


//------------------------------------------------------------------------------

} // action
} // uiVisu

