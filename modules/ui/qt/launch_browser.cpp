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

#include "launch_browser.hpp"

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

launch_browser::launch_browser() noexcept =
    default;

//------------------------------------------------------------------------------

launch_browser::~launch_browser() noexcept =
    default;

//------------------------------------------------------------------------------

void launch_browser::info(std::ostream& _sstream)
{
    _sstream << "Action for manage url" << std::endl;
}

//-----------------------------------------------------------------------------

void launch_browser::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->get_config();
    m_url = config.get<std::string>("url", m_url);
}

//------------------------------------------------------------------------------

void launch_browser::updating()
{
    SIGHT_WARN_IF("URL is empty.", m_url.empty());
    QUrl url(QString::fromStdString(m_url), QUrl::TolerantMode);

    if(url.isRelative()) // no scheme
    {
        std::filesystem::path path(QCoreApplication::applicationDirPath().toStdString());
        path  = path.parent_path(); // install folder path
        path /= url.path().toStdString();

        QString url_str = QString::fromStdString("file:///" + path.string());
        url = QUrl(url_str, QUrl::TolerantMode);
    }

    bool is_success = QDesktopServices::openUrl(url);
    SIGHT_WARN_IF("Browser wasn't successfully launched.", !is_success);
}

//------------------------------------------------------------------------------

void launch_browser::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void launch_browser::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
