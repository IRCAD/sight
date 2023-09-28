/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SLaunchBrowser.hpp"

#include <core/base.hpp>

#include <service/macros.hpp>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

#include <filesystem>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

SLaunchBrowser::SLaunchBrowser() noexcept =
    default;

//------------------------------------------------------------------------------

SLaunchBrowser::~SLaunchBrowser() noexcept =
    default;

//------------------------------------------------------------------------------

void SLaunchBrowser::info(std::ostream& _sstream)
{
    _sstream << "Action for manage url" << std::endl;
}

//-----------------------------------------------------------------------------

void SLaunchBrowser::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->getConfiguration();
    m_url = config.get<std::string>("url", m_url);
}

//------------------------------------------------------------------------------

void SLaunchBrowser::updating()
{
    SIGHT_WARN_IF("URL is empty.", m_url.empty());
    QUrl url(QString::fromStdString(m_url), QUrl::TolerantMode);

    if(url.isRelative()) // no scheme
    {
        std::filesystem::path path(QCoreApplication::applicationDirPath().toStdString());
        path  = path.parent_path(); // install folder path
        path /= url.path().toStdString();

        QString urlStr = QString::fromStdString("file:///" + path.string());
        url = QUrl(urlStr, QUrl::TolerantMode);
    }

    bool isSuccess = QDesktopServices::openUrl(url);
    SIGHT_WARN_IF("Browser wasn't successfully launched.", !isSuccess);
}

//------------------------------------------------------------------------------

void SLaunchBrowser::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SLaunchBrowser::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
