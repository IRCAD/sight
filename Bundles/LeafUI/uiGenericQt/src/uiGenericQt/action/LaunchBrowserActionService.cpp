/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>

#include "uiGenericQt/action/LaunchBrowserActionService.hpp"

namespace uiGeneric
{

namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGeneric::action::LaunchBrowserActionService, ::fwData::Object );

//------------------------------------------------------------------------------

LaunchBrowserActionService::LaunchBrowserActionService( ) throw()
{
    m_url = "";
}

//------------------------------------------------------------------------------

LaunchBrowserActionService::~LaunchBrowserActionService() throw()
{
}

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
    QUrl url(QString::fromStdString(m_url), QUrl::TolerantMode);

    if(url.isRelative()) // no scheme
    {
        ::boost::filesystem::path path(QCoreApplication::applicationDirPath().toStdString());
        path  = path.parent_path(); // install folder path
        path /= url.path().toStdString();

        QString urlStr = QString::fromStdString("file:///" + path.string());
        url = QUrl(urlStr, QUrl::TolerantMode);
    }

    bool isSuccess = QDesktopServices::openUrl(url);
    SLM_WARN_IF("Browser wasn't successfully launched.", !isSuccess);
}

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
