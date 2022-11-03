/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/ui/qt/Plugin.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <service/macros.hpp>

#include <ui/base/registry/macros.hpp>
#include <ui/qt/App.hpp>
#include <ui/qt/dialog/InputDialog.hpp>
#include <ui/qt/dialog/LocationDialog.hpp>
#include <ui/qt/dialog/LoggerDialog.hpp>
#include <ui/qt/dialog/MessageDialog.hpp>
#include <ui/qt/dialog/MultiSelectorDialog.hpp>
#include <ui/qt/dialog/NotificationDialog.hpp>
#include <ui/qt/dialog/ProgressDialog.hpp>
#include <ui/qt/dialog/PulseProgressDialog.hpp>
#include <ui/qt/dialog/SelectorDialog.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <QFile>
#include <QResource>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include <functional>

// Register all dialog implementation when we do use qt
namespace base_dialog = sight::ui::base::dialog;

SIGHT_REGISTER_GUI(sight::ui::qt::dialog::InputDialog, base_dialog::IInputDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::LocationDialog, base_dialog::ILocationDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::LoggerDialog, base_dialog::ILoggerDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::MessageDialog, base_dialog::IMessageDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::MultiSelectorDialog, base_dialog::IMultiSelectorDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::NotificationDialog, base_dialog::INotificationDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::ProgressDialog, base_dialog::IProgressDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::PulseProgressDialog, base_dialog::IPulseProgressDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::qt::dialog::SelectorDialog, base_dialog::ISelectorDialog::REGISTRY_KEY);

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::ui::qt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void Plugin::start()
{
    core::runtime::Profile::sptr profile = core::runtime::getCurrentProfile();
    SIGHT_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QApplication>(new sight::ui::qt::App(argc, argv, true));
        };

    auto workerQt = sight::ui::qt::getQtWorker(argc, argv, callback, profile->getName(), profile->getVersion());
    core::thread::setDefaultWorker(workerQt);

    workerQt->post([this](auto&& ...){loadStyleSheet();});

    core::runtime::getCurrentProfile()->setRunCallback(run);
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    core::thread::resetDefaultWorker();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    auto workerQt = core::thread::getDefaultWorker();
    workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    int result = std::any_cast<int>(workerQt->getFuture().get());

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if(QCoreApplication::instance() != nullptr)
    {
        if(this->getModule()->hasParameter("resource"))
        {
            const std::string resourceFile = this->getModule()->getParameterValue("resource");
            const auto path                = core::runtime::getModuleResourceFilePath(resourceFile);

            [[maybe_unused]] const bool resourceLoaded = QResource::registerResource(path.string().c_str());
            SIGHT_ASSERT("Cannot load resources '" + resourceFile + "'.", resourceLoaded);
        }

        if(this->getModule()->hasParameter("style"))
        {
            const std::string style = this->getModule()->getParameterValue("style");
            qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
        }

        QString touchFriendlyStyle;
        if(this->getModule()->getParameterValue("touch_friendly") == "true")
        {
            const std::filesystem::path touchFriendlyStylePath = core::runtime::getModuleResourceFilePath(
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
        if(this->getModule()->hasParameter("stylesheet"))
        {
            const std::string stylesheetValue = this->getModule()->getParameterValue("stylesheet");
            const std::filesystem::path path  = core::runtime::getModuleResourceFilePath(stylesheetValue);
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
