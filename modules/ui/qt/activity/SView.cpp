/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

namespace sight::module::ui::qt
{

namespace activity
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
{
}

//-----------------------------------------------------------------------------

void SView::configuring()
{
    this->::sight::ui::base::view::IActivityView::configuring();

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
    this->::sight::ui::base::IGuiContainer::create();

    auto parentContainer = ::sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    QVBoxLayout* layout = new QVBoxLayout();
    if(m_border >= 0)
    {
        layout->setContentsMargins(m_border, m_border, m_border, m_border);
    }

    QWidget* widget = new QWidget();
    layout->addWidget(widget);

    auto subContainer = ::sight::ui::qt::container::QtContainer::New();

    subContainer->setQtContainer(widget);
    m_wid = this->getID() + "_container";
    ::sight::ui::base::GuiRegistry::registerWIDContainer(m_wid, subContainer);

    parentContainer->setLayout(layout);

    m_configManager = service::IAppConfigManager::New();

    if(!m_mainActivityId.empty())
    {
        data::ActivitySeries::sptr activity = this->createMainActivity();
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

    auto subContainer = ::sight::ui::base::GuiRegistry::getWIDContainer(m_wid);
    ::sight::ui::base::GuiRegistry::unregisterWIDContainer(m_wid);

    subContainer->destroyContainer();

    this->destroy();
}

//------------------------------------------------------------------------------

void SView::updating()
{
}

//------------------------------------------------------------------------------

void SView::launchActivity(data::ActivitySeries::sptr activitySeries)
{
    if(this->validateActivity(activitySeries))
    {
        if(m_configManager->isStarted())
        {
            m_configManager->stopAndDestroy();
        }

        sight::activity::extension::ActivityInfo info;
        info = sight::activity::extension::Activity::getDefault()->getInfo(activitySeries->getActivityConfigId());

        ReplaceMapType replaceMap;
        this->translateParameters(m_parameters, replaceMap);
        this->translateParameters(activitySeries->getData(), info.appConfig.parameters, replaceMap);
        replaceMap["AS_UID"]     = activitySeries->getID();
        replaceMap["WID_PARENT"] = m_wid;
        std::string genericUidAdaptor = service::extension::AppConfig::getUniqueIdentifier(info.appConfig.id);
        replaceMap["GENERIC_UID"] = genericUidAdaptor;
        try
        {
            m_configManager->setConfig(info.appConfig.id, replaceMap);
            m_configManager->launch();

            m_sigActivityLaunched->asyncEmit(activitySeries);
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

} // namespace activity

} // namespace sight::module::ui::qt
