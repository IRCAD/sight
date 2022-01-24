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
#include <core/runtime/operations.hpp>
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
namespace idialog = sight::ui::base::dialog;

fwGuiRegisterMacro(sight::ui::qt::dialog::InputDialog, idialog::IInputDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::LocationDialog, idialog::ILocationDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::LoggerDialog, idialog::ILoggerDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::MessageDialog, idialog::IMessageDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::MultiSelectorDialog, idialog::IMultiSelectorDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::NotificationDialog, idialog::INotificationDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::ProgressDialog, idialog::IProgressDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::PulseProgressDialog, idialog::IPulseProgressDialog::REGISTRY_KEY);
fwGuiRegisterMacro(sight::ui::qt::dialog::SelectorDialog, idialog::ISelectorDialog::REGISTRY_KEY);

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::ui::qt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

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

    workerQt->post(std::bind(&Plugin::loadStyleSheet, this));

    core::runtime::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    core::thread::resetDefaultWorker();
}

//-----------------------------------------------------------------------------

void setup()
{
    core::runtime::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    auto workerQt = core::thread::getDefaultWorker();
    workerQt->post(std::bind(&setup));
    workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    core::runtime::getCurrentProfile()->cleanup();
    int result = std::any_cast<int>(workerQt->getFuture().get());

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if(this->getModule()->hasParameter("resource"))
    {
        const std::string resourceFile = this->getModule()->getParameterValue("resource");
        const auto path                = core::runtime::getModuleResourceFilePath(resourceFile);

        const bool resourceLoaded = QResource::registerResource(path.string().c_str());
        SIGHT_ASSERT("Cannot load resources '" + resourceFile + "'.", resourceLoaded);
    }

    if(this->getModule()->hasParameter("style"))
    {
        const std::string style = this->getModule()->getParameterValue("style");
        qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
    }

    if(this->getModule()->hasParameter("stylesheet"))
    {
        const std::string stylesheetFile = this->getModule()->getParameterValue("stylesheet");
        const auto path                  = core::runtime::getModuleResourceFilePath(stylesheetFile);

        QFile data(QString::fromStdString(path.string()));
        QString style;
        if(data.open(QFile::ReadOnly))
        {
            QTextStream styleIn(&data);
            style = styleIn.readAll();
            data.close();
            qApp->setStyleSheet(style);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
