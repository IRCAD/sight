/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "SView.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/GuiRegistry.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::activity
{

const core::com::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const std::string s_BORDER_CONFIG = "border";

//------------------------------------------------------------------------------

SView::SView()
{
    m_sigActivityLaunched = newSignal<ActivityLaunchedSignalType>(s_ACTIVITY_LAUNCHED_SIG);
}

//------------------------------------------------------------------------------

SView::~SView()
= default;

//-----------------------------------------------------------------------------

void SView::configuring()
{
    this->sight::ui::base::view::IActivityView::configuring();

    const ConfigType configType = this->getConfigTree();
    const auto config           = configType.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_border = config->get<int>(s_BORDER_CONFIG, m_border);
    }
}

//------------------------------------------------------------------------------

void SView::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    auto parentContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    parentContainer->getQtContainer()->setObjectName(serviceID);

    auto* layout = new QVBoxLayout();
    if(m_border >= 0)
    {
        layout->setContentsMargins(m_border, m_border, m_border, m_border);
    }

    auto* widget = new QWidget();
    layout->addWidget(widget);
    widget->setObjectName(serviceID + "/container");

    auto subContainer = sight::ui::qt::container::QtContainer::New();

    subContainer->setQtContainer(widget);
    m_wid = this->getID() + "_container";
    sight::ui::base::GuiRegistry::registerWIDContainer(m_wid, subContainer);

    parentContainer->setLayout(layout);

    m_configManager = service::AppConfigManager::New();

    if(!m_mainActivityId.empty())
    {
        data::Activity::sptr activity = this->createMainActivity();
        if(activity)
        {
            this->launchActivity(activity);
        }
    }
}

//------------------------------------------------------------------------------

void SView::stopping()
{
    if(m_configManager && m_configManager->isStarted())
    {
        m_configManager->stopAndDestroy();
    }

    auto subContainer = sight::ui::base::GuiRegistry::getWIDContainer(m_wid);
    sight::ui::base::GuiRegistry::unregisterWIDContainer(m_wid);

    subContainer->destroyContainer();

    this->destroy();
}

//------------------------------------------------------------------------------

void SView::updating()
{
}

//------------------------------------------------------------------------------

void SView::launchActivity(data::Activity::sptr activity)
{
    if(this->validateActivity(activity))
    {
        if(m_configManager->isStarted())
        {
            m_configManager->stopAndDestroy();
        }

        auto [info, replacementMap] = sight::activity::extension::Activity::getDefault()->getInfoAndReplacementMap(
            *activity,
            m_parameters
        );

        replacementMap["WID_PARENT"]  = m_wid;
        replacementMap["GENERIC_UID"] = service::extension::AppConfig::getUniqueIdentifier(info.appConfig.id);

        try
        {
            m_configManager->setConfig(info.appConfig.id, replacementMap);
            m_configManager->launch();

            m_sigActivityLaunched->asyncEmit(activity);
        }
        catch(std::exception& e)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Activity launch failed",
                e.what(),
                sight::ui::base::dialog::IMessageDialog::CRITICAL
            );
            SIGHT_ERROR(e.what());
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
