/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiGenericQt/action/LaunchBrowserActionService.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace uiGeneric
{

namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGeneric::action::LaunchBrowserActionService, ::fwData::Object );

//------------------------------------------------------------------------------

LaunchBrowserActionService::LaunchBrowserActionService( ) noexcept
{
    m_url = "";
}

//------------------------------------------------------------------------------

LaunchBrowserActionService::~LaunchBrowserActionService() noexcept
{
}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::info(std::ostream& _sstream )
{
    _sstream << "Action for manage url" << std::endl;
}

//-----------------------------------------------------------------------------

void LaunchBrowserActionService::configuring()
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

void LaunchBrowserActionService::updating()
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

void LaunchBrowserActionService::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void LaunchBrowserActionService::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}
//------------------------------------------------------------------------------

} // namespace action

} // namespace uiGeneric
