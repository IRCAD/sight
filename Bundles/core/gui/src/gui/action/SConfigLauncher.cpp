/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>

#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include "gui/action/SConfigLauncher.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SConfigLauncher, ::fwData::Object );

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() throw()
{
    m_configLauncher = ::fwServices::helper::ConfigLauncher::New();
}

//------------------------------------------------------------------------------

SConfigLauncher::~SConfigLauncher() throw()
{}

//------------------------------------------------------------------------------

void SConfigLauncher::starting() throw(::fwTools::Failed)
{
    this->actionServiceStarting();
    ::fwData::Object::sptr currentObj = this->getObject();
    bool executable = m_configLauncher->isExecutable(currentObj);
    this->setIsExecutable( executable );
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopping() throw(::fwTools::Failed)
{
    bool configIsRunning = this->getIsActive();
    if ( configIsRunning )
    {
        m_configLauncher->stopConfig();
    }
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SConfigLauncher::configuring() throw(fwTools::Failed)
{
    this->initialize();
    m_configLauncher->parseConfig(this->getConfigTree());
}

//-----------------------------------------------------------------------------

void SConfigLauncher::setIsActive(bool isActive)
{
    this->::fwGui::IActionSrv::setIsActive(isActive);
    if ( isActive )
    {
        m_configLauncher->startConfig(this->getSptr());
    }
    else
    {
        m_configLauncher->stopConfig();
    }
}

//------------------------------------------------------------------------------

void SConfigLauncher::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    if ( msg->hasEvent("WINDOW_CLOSED") )
    {
        this->setIsActive( false );
        m_configLauncher->stopConfig();
    }

    ::fwData::Object::sptr currentObj = this->getObject();
    bool executable = m_configLauncher->isExecutable(currentObj);
    this->setIsExecutable( executable );
}

//------------------------------------------------------------------------------

void SConfigLauncher::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

}
}

