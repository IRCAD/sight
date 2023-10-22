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

#include "dynamic_view.hpp"

#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/tools/date_and_time.hpp>

#include <service/extension/config.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>

#include <QBoxLayout>
#include <QTabWidget>
#include <QtGui>

#include <regex>

namespace sight::module::ui::qt::activity
{

static const core::com::slots::key_t CREATE_TAB_SLOT = "createTab";

static const core::com::signals::key_t ACTIVITY_SELECTED_SLOT = "activitySelected";
static const core::com::signals::key_t NOTHING_SELECTED_SLOT  = "nothingSelected";

//------------------------------------------------------------------------------

dynamic_view::dynamic_view() noexcept
{
    new_slot(CREATE_TAB_SLOT, &dynamic_view::createTab, this);

    m_sigActivitySelected = new_signal<activity_selected_signal_t>(ACTIVITY_SELECTED_SLOT);
    m_sigNothingSelected  = new_signal<nothing_selected_signal_t>(NOTHING_SELECTED_SLOT);
}

//------------------------------------------------------------------------------

dynamic_view::~dynamic_view() noexcept =
    default;

//------------------------------------------------------------------------------

void dynamic_view::configuring()
{
    this->sight::ui::activity_view::configuring();

    const auto& config = this->get_config();

    m_mainActivityClosable = config.get<bool>("mainActivity.<xmlattr>.closable", m_mainActivityClosable);
    m_documentMode         = config.get<bool>("config.<xmlattr>.document", m_documentMode);
}

//------------------------------------------------------------------------------

void dynamic_view::starting()
{
    this->sight::ui::service::create();

    auto parent_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(m_documentMode);
    m_tabWidget->setMovable(true);

    QObject::connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabSignal(int)));
    QObject::connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int)));

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget(m_tabWidget);

    parent_container->setLayout(layout);

    m_currentWidget = nullptr;

    if(!m_mainActivityId.empty())
    {
        this->buildMainActivity();
    }
}

//------------------------------------------------------------------------------

void dynamic_view::stopping()
{
    while(m_tabWidget->count() != 0)
    {
        this->closeTab(0, true);
    }

    m_tabWidget->clear();

    this->destroy();
    m_tabWidget = nullptr;
}

//------------------------------------------------------------------------------

void dynamic_view::updating()
{
}

//------------------------------------------------------------------------------

void dynamic_view::launchActivity(data::activity::sptr _activity)
{
    if(this->validateActivity(_activity))
    {
        dynamic_viewInfo view_info = this->createViewInfo(_activity);
        view_info.closable = true;

        this->launchTab(view_info);
    }
}

//------------------------------------------------------------------------------

void dynamic_view::createTab(sight::activity::message _info)
{
    dynamic_viewInfo view_info;
    view_info.title          = _info.getTitle();
    view_info.tabID          = _info.getTabID();
    view_info.closable       = _info.isClosable();
    view_info.icon           = _info.getIconPath();
    view_info.tooltip        = _info.getToolTip();
    view_info.viewConfigID   = _info.getAppConfigID();
    view_info.replacementMap = _info.getReplacementMap();
    view_info.activity       = _info.getActivity();

    this->launchTab(view_info);
}

//------------------------------------------------------------------------------

void dynamic_view::launchTab(dynamic_viewInfo& _info)
{
    static int count = 0;
    auto iter        = std::find(m_activityIds.begin(), m_activityIds.end(), _info.activity->get_id());

    if(iter != m_activityIds.end())
    {
        sight::ui::dialog::message::show(
            "Launch Activity",
            "The current activity is already launched. \n"
            "It cannot be launched twice.",
            sight::ui::dialog::message::WARNING
        );
        return;
    }

    if(m_titleToCount.find(_info.title) != m_titleToCount.end())
    {
        m_titleToCount[_info.title]++;
    }
    else
    {
        m_titleToCount[_info.title] = 1;
    }

    QString final_title = QString("%1 %2").arg(_info.title.c_str(), "(%1)").arg(m_titleToCount[_info.title]);
    _info.wid = QString("dynamic_view-%1").arg(count++).toStdString();

    auto sub_container = sight::ui::qt::container::widget::make();
    auto* widget       = new QWidget(m_tabWidget);
    sub_container->setQtContainer(widget);
    sight::ui::registry::register_wid_container(_info.wid, sub_container);

    _info.replacementMap["WID_PARENT"]  = _info.wid;
    _info.replacementMap["GENERIC_UID"] = sight::app::extension::config::getUniqueIdentifier(_info.viewConfigID);

    auto helper = sight::app::config_manager::make();

    try
    {
        helper->setConfig(_info.viewConfigID, _info.replacementMap);
        if(!m_dynamicConfigStartStop)
        {
            helper->launch();
        }
        else
        {
            helper->create();
        }
    }
    catch(std::exception& e)
    {
        sight::ui::dialog::message::show(
            "Activity launch failed",
            e.what(),
            sight::ui::dialog::message::CRITICAL
        );
        SIGHT_ERROR(e.what());
        return;
    }

    _info.container = sub_container;
    _info.helper    = helper;
    m_activityIds.insert(_info.activity->get_id());

    m_dynamicInfoMap[widget] = _info;
    m_tabIDList.insert(_info.tabID);

    int index = m_tabWidget->addTab(widget, final_title);
    if(!_info.tooltip.empty())
    {
        m_tabWidget->setTabToolTip(index, QString::fromStdString(_info.tooltip));
    }

    if(!_info.icon.empty())
    {
        m_tabWidget->setTabIcon(index, QIcon(QString::fromStdString(_info.icon)));
    }

    m_tabWidget->setCurrentWidget(widget);
}

//------------------------------------------------------------------------------

void dynamic_view::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void dynamic_view::closeTabSignal(int _index)
{
    closeTab(_index, false);
}

//------------------------------------------------------------------------------

void dynamic_view::closeTab(int _index, bool _force_close)
{
    QWidget* widget = m_tabWidget->widget(_index);

    SIGHT_ASSERT("Widget is not in dynamicInfoMap", m_dynamicInfoMap.find(widget) != m_dynamicInfoMap.end());
    dynamic_viewInfo info = m_dynamicInfoMap[widget];
    if(info.closable || _force_close)
    {
        m_tabIDList.erase(info.tabID);
        if(!m_dynamicConfigStartStop)
        {
            info.helper->stopAndDestroy();
        }
        else
        {
            if(info.helper->started())
            {
                info.helper->stop();
            }

            info.helper->destroy();
        }

        info.helper.reset();

        //Remove tab first, to avoid tab beeing removed by container->destroy
        m_currentWidget = nullptr;
        m_tabWidget->removeTab(_index);

        sight::ui::registry::unregister_wid_container(info.wid);

        info.container->destroyContainer();
        info.container.reset();
        m_dynamicInfoMap.erase(widget);
        m_activityIds.erase(info.activity->get_id());
    }
    else
    {
        sight::ui::dialog::message::show(
            "Close tab",
            "The tab " + info.title + " can not be closed.",
            sight::ui::dialog::message::INFO
        );
    }
}

//------------------------------------------------------------------------------

void dynamic_view::changedTab(int _index)
{
    QWidget* widget = m_tabWidget->widget(_index);

    if(m_dynamicConfigStartStop && widget != m_currentWidget)
    {
        if(m_currentWidget != nullptr)
        {
            dynamic_viewInfo oldinfo = m_dynamicInfoMap[m_currentWidget];
            oldinfo.helper->stop();
        }

        if(widget != nullptr)
        {
            dynamic_viewInfo newinfo = m_dynamicInfoMap[widget];
            if(!newinfo.helper->started())
            {
                newinfo.helper->start();
                newinfo.helper->update();
            }
        }
    }

    m_currentWidget = widget;

    if(_index >= 0)
    {
        dynamic_viewInfo info = m_dynamicInfoMap[widget];
        m_sigActivitySelected->async_emit(info.activity);
    }
    else
    {
        m_sigNothingSelected->async_emit();
    }
}

//------------------------------------------------------------------------------

void dynamic_view::buildMainActivity()
{
    auto activity = this->createMainActivity();

    if(activity)
    {
        dynamic_viewInfo view_info;
        view_info          = this->createViewInfo(activity);
        view_info.closable = m_mainActivityClosable;

        this->launchTab(view_info);
    }
}

//------------------------------------------------------------------------------

dynamic_view::dynamic_viewInfo dynamic_view::createViewInfo(data::activity::sptr _activity)
{
    auto [info, replacementMap] = sight::activity::extension::activity::getDefault()->getInfoAndReplacementMap(
        *_activity,
        m_parameters
    );

    dynamic_viewInfo view_info;
    view_info.title          = info.title;
    view_info.icon           = info.icon;
    view_info.tooltip        = info.tabInfo.empty() ? info.title : info.tabInfo;
    view_info.viewConfigID   = info.appConfig.id;
    view_info.activity       = _activity;
    view_info.replacementMap = replacementMap;

    return view_info;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
