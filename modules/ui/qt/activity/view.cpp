/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "view.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <service/extension/config.hpp>
#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::activity
{

const core::com::signals::key_t ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const std::string s_BORDER_CONFIG = "border";

//------------------------------------------------------------------------------

view::view() :
    m_sigActivityLaunched(new_signal<ActivityLaunchedSignalType>(ACTIVITY_LAUNCHED_SIG))
{
}

//------------------------------------------------------------------------------

view::~view()
= default;

//-----------------------------------------------------------------------------

void view::configuring()
{
    this->sight::ui::view::IActivityView::configuring();

    const config_t config_t = this->get_config();
    const auto config       = config_t.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_border = config->get<int>(s_BORDER_CONFIG, m_border);
    }
}

//------------------------------------------------------------------------------

void view::starting()
{
    this->sight::ui::service::create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto parentContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    parentContainer->getQtContainer()->setObjectName(serviceID);

    auto* layout = new QVBoxLayout();
    if(m_border >= 0)
    {
        layout->setContentsMargins(m_border, m_border, m_border, m_border);
    }

    auto* widget = new QWidget();
    layout->addWidget(widget);
    widget->setObjectName(serviceID + "/container");

    auto subContainer = sight::ui::qt::container::widget::make();

    subContainer->setQtContainer(widget);
    m_wid = this->get_id() + "_container";
    sight::ui::registry::registerWIDContainer(m_wid, subContainer);

    parentContainer->setLayout(layout);

    m_configManager = sight::app::config_manager::make();

    if(!m_mainActivityId.empty())
    {
        data::activity::sptr activity = this->createMainActivity();
        if(activity)
        {
            this->launchActivity(activity);
        }
    }
}

//------------------------------------------------------------------------------

void view::stopping()
{
    if(m_configManager && m_configManager->started())
    {
        m_configManager->stopAndDestroy();
    }

    auto subContainer = sight::ui::registry::getWIDContainer(m_wid);
    sight::ui::registry::unregisterWIDContainer(m_wid);

    subContainer->destroyContainer();

    this->destroy();
}

//------------------------------------------------------------------------------

void view::updating()
{
}

//------------------------------------------------------------------------------

void view::launchActivity(data::activity::sptr activity)
{
    if(this->validateActivity(activity))
    {
        if(m_configManager->started())
        {
            m_configManager->stopAndDestroy();
        }

        auto [info, replacementMap] = sight::activity::extension::activity::getDefault()->getInfoAndReplacementMap(
            *activity,
            m_parameters
        );

        replacementMap["WID_PARENT"]  = m_wid;
        replacementMap["GENERIC_UID"] = sight::app::extension::config::getUniqueIdentifier(info.appConfig.id);

        try
        {
            m_configManager->setConfig(info.appConfig.id, replacementMap);
            m_configManager->launch();

            m_sigActivityLaunched->async_emit(activity);
        }
        catch(std::exception& e)
        {
            sight::ui::dialog::message::show(
                "Activity launch failed",
                e.what(),
                sight::ui::dialog::message::CRITICAL
            );
            SIGHT_ERROR(e.what());
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
