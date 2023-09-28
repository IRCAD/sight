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

#include "modules/ui/qt/plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/profile.hpp>

#include <service/macros.hpp>

#include <ui/__/macros.hpp>
#include <ui/qt/App.hpp>
#include <ui/qt/dialog/input.hpp>
#include <ui/qt/dialog/location.hpp>
#include <ui/qt/dialog/logger.hpp>
#include <ui/qt/dialog/message.hpp>
#include <ui/qt/dialog/notification.hpp>
#include <ui/qt/dialog/progress.hpp>
#include <ui/qt/dialog/pulse_progress.hpp>
#include <ui/qt/dialog/selector.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <QFile>
#include <QResource>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include <functional>

// Register all dialog implementation when we do use qt
namespace base_dialog = sight::ui::dialog;

SIGHT_REGISTER_GUI(sight::ui::qt::dialog::input, base_dialog::input_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::location, base_dialog::location_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::logger, base_dialog::logger_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::message, base_dialog::message_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::notification, base_dialog::notification_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::progress, base_dialog::progress_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::pulse_progress, base_dialog::pulse_progress_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::selector, base_dialog::selector_base::REGISTRY_KEY);

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::ui::qt::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
    core::runtime::profile::sptr profile = core::runtime::get_current_profile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->get_raw_arg_count();
    char** argv = profile->get_raw_params();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QApplication>(new sight::ui::qt::App(argc, argv, true));
        };

    auto workerQt = sight::ui::qt::getQtWorker(argc, argv, callback, profile->name(), profile->get_version());
    core::thread::set_default_worker(workerQt);

    workerQt->post([this](auto&& ...){loadStyleSheet();});

    core::runtime::get_current_profile()->set_run_callback(run);
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
    core::thread::reset_default_worker();
}

//-----------------------------------------------------------------------------

int plugin::run() noexcept
{
    auto workerQt = core::thread::get_default_worker();
    workerQt->get_future().wait(); // This is required to start WorkerQt loop

    int result = std::any_cast<int>(workerQt->get_future().get());

    return result;
}

//-----------------------------------------------------------------------------

void plugin::loadStyleSheet()
{
    if(QCoreApplication::instance() != nullptr)
    {
        if(this->get_module()->has_parameter("resource"))
        {
            const std::string resourceFile = this->get_module()->get_parameter_value("resource");
            const auto path                = core::runtime::get_module_resource_file_path(resourceFile);

            [[maybe_unused]] const bool resourceLoaded = QResource::registerResource(path.string().c_str());
            SIGHT_ASSERT("Cannot load resources '" + resourceFile + "'.", resourceLoaded);
        }

        if(this->get_module()->has_parameter("style"))
        {
            const std::string style = this->get_module()->get_parameter_value("style");
            qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
        }

        QString touchFriendlyStyle;
        if(this->get_module()->get_parameter_value("touch_friendly") == "true")
        {
            const std::filesystem::path touchFriendlyStylePath = core::runtime::get_module_resource_file_path(
                "sight::module::ui::qt/touch-friendly.qss"
            );
            {
                QFile data(QString::fromStdString(touchFriendlyStylePath.string()));
                if(data.open(QFile::ReadOnly))
                {
                    touchFriendlyStyle = QTextStream(&data).readAll();
                }
            }
        }

        QString appStyle;
        if(this->get_module()->has_parameter("stylesheet"))
        {
            const std::string stylesheetValue = this->get_module()->get_parameter_value("stylesheet");
            const std::filesystem::path path  = core::runtime::get_module_resource_file_path(stylesheetValue);
            {
                QFile data(QString::fromStdString(path.string()));
                if(data.open(QFile::ReadOnly))
                {
                    appStyle = QTextStream(&data).readAll();
                }
            }
        }

        QString styleResult = appStyle + touchFriendlyStyle;
        if(!styleResult.isEmpty())
        {
            qApp->setStyleSheet(styleResult);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
