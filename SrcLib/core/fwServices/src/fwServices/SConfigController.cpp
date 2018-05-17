/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/SConfigController.hpp"

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

namespace fwServices
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::fwServices::SConfigController );

//------------------------------------------------------------------------------

SConfigController::SConfigController() noexcept
{
    m_configLauncher = ::boost::make_unique< ::fwServices::helper::ConfigLauncher>();
}

//------------------------------------------------------------------------------

SConfigController::~SConfigController() noexcept
{
}

//------------------------------------------------------------------------------

void SConfigController::starting()
{
    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::stopping()
{
    m_configLauncher->stopConfig();
}

//------------------------------------------------------------------------------

void SConfigController::swapping()
{
    m_configLauncher->stopConfig();
    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::configuring()
{
    m_configLauncher->parseConfig(this->getConfigTree(), this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::updating()
{
}

//------------------------------------------------------------------------------

void SConfigController::info( std::ostream& )
{
}

//------------------------------------------------------------------------------

}
