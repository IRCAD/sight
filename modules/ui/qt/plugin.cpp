/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include <ui/qt/app.hpp>
#include <ui/qt/dialog/input.hpp>
#include <ui/qt/dialog/location.hpp>
#include <ui/qt/dialog/logger.hpp>
#include <ui/qt/dialog/message.hpp>
#include <ui/qt/dialog/notification.hpp>
#include <ui/qt/dialog/progress.hpp>
#include <ui/qt/dialog/pulse_progress.hpp>
#include <ui/qt/dialog/selector.hpp>
#include <ui/qt/worker_qt.hpp>

#include <QFile>
#include <QPainter>
#include <QProxyStyle>
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

/// This class is used to provide a proper disabled icon, especially when the icon is full white.
class proxy_style final : public QProxyStyle
{
public:

    using QProxyStyle::QProxyStyle;

    //------------------------------------------------------------------------------

    QPixmap generatedIconPixmap(QIcon::Mode _icon_mode, const QPixmap& _pixmap, const QStyleOption* _option) const final
    {
        if(_icon_mode == QIcon::Disabled)
        {
            // Copy the original pixmap
            QPixmap disabled_pixmap = _pixmap;

            // Create a QPainter and fill the pixmap with a semi-transparent white
            QPainter painter(&disabled_pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(disabled_pixmap.rect(), QColor(255, 255, 255, 25));
            painter.end();

            return disabled_pixmap;
        }

        return QProxyStyle::generatedIconPixmap(_icon_mode, _pixmap, _option);
    }
};

//-----------------------------------------------------------------------------

void plugin::start()
{
    core::runtime::profile::sptr profile = core::runtime::get_current_profile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->get_raw_arg_count();
    char** argv = profile->get_raw_params();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& _argc, char** _argv)
        {
            return QSharedPointer<QApplication>(new sight::ui::qt::app(_argc, _argv, true));
        };

    auto worker_qt = sight::ui::qt::get_qt_worker(argc, argv, callback, profile->name(), profile->version());
    core::thread::set_default_worker(worker_qt);

    worker_qt->post([this](auto&& ...){load_style_sheet();});

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
    auto worker_qt = core::thread::get_default_worker();
    worker_qt->get_future().wait(); // This is required to start worker_qt loop

    int result = std::any_cast<int>(worker_qt->get_future().get());

    return result;
}

//-----------------------------------------------------------------------------

void plugin::load_style_sheet()
{
    if(qApp != nullptr)
    {
        if(this->get_module()->has_parameter("resource"))
        {
            const std::string resource_file = this->get_module()->get_parameter_value("resource");
            const auto path                 = core::runtime::get_module_resource_file_path(resource_file);

            [[maybe_unused]] const bool resource_loaded = QResource::registerResource(path.string().c_str());
            SIGHT_ASSERT("Cannot load resources '" + resource_file + "'.", resource_loaded);
        }

        // Also apply our proxy style to override default disabled icon look
        if(this->get_module()->has_parameter("style"))
        {
            const std::string style = this->get_module()->get_parameter_value("style");
            qApp->setStyle(new proxy_style(QStyleFactory::create(QString::fromStdString(style))));
        }
        else
        {
            qApp->setStyle(new proxy_style);
        }

        QString touch_friendly_style;
        if(this->get_module()->get_parameter_value("touch_friendly") == "true")
        {
            const std::filesystem::path touch_friendly_style_path = core::runtime::get_module_resource_file_path(
                "sight::module::ui::qt/touch-friendly.qss"
            );
            {
                QFile data(QString::fromStdString(touch_friendly_style_path.string()));
                if(data.open(QFile::ReadOnly))
                {
                    touch_friendly_style = QTextStream(&data).readAll();
                }
            }
        }

        QString app_style;
        if(this->get_module()->has_parameter("stylesheet"))
        {
            const std::string stylesheet_value = this->get_module()->get_parameter_value("stylesheet");
            const std::filesystem::path path   = core::runtime::get_module_resource_file_path(stylesheet_value);
            {
                QFile data(QString::fromStdString(path.string()));
                if(data.open(QFile::ReadOnly))
                {
                    app_style = QTextStream(&data).readAll();
                }
            }
        }

        QString style_result = app_style + touch_friendly_style;
        if(!style_result.isEmpty())
        {
            qApp->setStyleSheet(style_result);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
