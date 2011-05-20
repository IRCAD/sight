/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QUrl>
#include <QDesktopServices>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include "uiGenericQt/action/LaunchBrowserActionService.hpp"

namespace uiGeneric
{

namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiGeneric::action::LaunchBrowserActionService , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

LaunchBrowserActionService::LaunchBrowserActionService( ) throw()
{
    m_url = "";
}

//------------------------------------------------------------------------------

LaunchBrowserActionService::~LaunchBrowserActionService() throw()
{}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::info(std::ostream &_sstream )
{
    _sstream << "Action for manage url" << std::endl;
}

//-----------------------------------------------------------------------------

void LaunchBrowserActionService::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->::fwGui::IActionSrv::initialize();

    std::vector < Configuration > urlConfig = m_configuration->find("url");
    if(!urlConfig.empty())
    {
        m_url = urlConfig.at(0)->getValue();
    }
}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    SLM_WARN_IF("URL is empty.", m_url.empty());
    bool isSuccess = QDesktopServices::openUrl(QUrl(QString::fromStdString(m_url), QUrl::TolerantMode));
    SLM_WARN_IF("Browser wasn't successfully launched.", !isSuccess);
}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}
//------------------------------------------------------------------------------

} // namespace action

} // namespace uiGeneric
