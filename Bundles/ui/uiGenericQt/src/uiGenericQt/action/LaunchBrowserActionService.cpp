/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "uiGenericQt/action/LaunchBrowserActionService.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

namespace uiGenericQt
{

namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGenericQt::action::LaunchBrowserActionService, ::fwData::Object );

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

} // namespace uiGenericQt
