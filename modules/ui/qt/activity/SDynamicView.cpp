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

#include "SDynamicView.hpp"

#include <activity/IActivityValidator.hpp>
#include <activity/IValidator.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/dateAndTime.hpp>

#include <service/extension/AppConfig.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/GuiRegistry.hpp>

#include <QBoxLayout>
#include <QTabWidget>
#include <QtGui>

#include <regex>

namespace sight::module::ui::qt::activity
{

static const core::com::Slots::SlotKeyType s_CREATE_TAB_SLOT = "createTab";

static const core::com::Signals::SignalKeyType s_ACTIVITY_SELECTED_SLOT = "activitySelected";
static const core::com::Signals::SignalKeyType s_NOTHING_SELECTED_SLOT  = "nothingSelected";

//------------------------------------------------------------------------------

SDynamicView::SDynamicView() noexcept
{
    newSlot(s_CREATE_TAB_SLOT, &SDynamicView::createTab, this);

    m_sigActivitySelected = newSignal<ActivitySelectedSignalType>(s_ACTIVITY_SELECTED_SLOT);
    m_sigNothingSelected  = newSignal<NothingSelectedSignalType>(s_NOTHING_SELECTED_SLOT);
}

//------------------------------------------------------------------------------

SDynamicView::~SDynamicView() noexcept =
    default;

//------------------------------------------------------------------------------

void SDynamicView::configuring()
{
    this->sight::ui::base::view::IActivityView::configuring();

    const auto& config = this->getConfigTree();

    m_mainActivityClosable = config.get<bool>("mainActivity.<xmlattr>.closable", m_mainActivityClosable);
    m_documentMode         = config.get<bool>("config.<xmlattr>.document", m_documentMode);
}

//------------------------------------------------------------------------------

void SDynamicView::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto parentContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(m_documentMode);
    m_tabWidget->setMovable(true);

    QObject::connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabSignal(int)));
    QObject::connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int)));

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget(m_tabWidget);

    parentContainer->setLayout(layout);

    m_currentWidget = nullptr;

    if(!m_mainActivityId.empty())
    {
        this->buildMainActivity();
    }
}

//------------------------------------------------------------------------------

void SDynamicView::stopping()
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

void SDynamicView::updating()
{
}

//------------------------------------------------------------------------------

void SDynamicView::launchActivity(data::Activity::sptr activity)
{
    if(this->validateActivity(activity))
    {
        SDynamicViewInfo viewInfo = this->createViewInfo(activity);
        viewInfo.closable = true;

        this->launchTab(viewInfo);
    }
}

//------------------------------------------------------------------------------

void SDynamicView::createTab(sight::activity::ActivityMsg info)
{
    SDynamicViewInfo viewInfo;
    viewInfo.title          = info.getTitle();
    viewInfo.tabID          = info.getTabID();
    viewInfo.closable       = info.isClosable();
    viewInfo.icon           = info.getIconPath();
    viewInfo.tooltip        = info.getToolTip();
    viewInfo.viewConfigID   = info.getAppConfigID();
    viewInfo.replacementMap = info.getReplacementMap();
    viewInfo.activity       = info.getActivity();

    this->launchTab(viewInfo);
}

//------------------------------------------------------------------------------

void SDynamicView::launchTab(SDynamicViewInfo& info)
{
    static int count = 0;
    auto iter        = std::find(m_activityIds.begin(), m_activityIds.end(), info.activity->getID());

    if(iter != m_activityIds.end())
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Launch Activity",
            "The current activity is already launched. \n"
            "It cannot be launched twice.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        return;
    }

    if(m_titleToCount.find(info.title) != m_titleToCount.end())
    {
        m_titleToCount[info.title]++;
    }
    else
    {
        m_titleToCount[info.title] = 1;
    }

    QString finalTitle = QString("%1 %2").arg(info.title.c_str(), "(%1)").arg(m_titleToCount[info.title]);
    info.wid = QString("SDynamicView-%1").arg(count++).toStdString();

    auto subContainer = sight::ui::qt::container::QtContainer::New();
    auto* widget      = new QWidget(m_tabWidget);
    subContainer->setQtContainer(widget);
    sight::ui::base::GuiRegistry::registerWIDContainer(info.wid, subContainer);

    info.replacementMap["WID_PARENT"]  = info.wid;
    info.replacementMap["GENERIC_UID"] = service::extension::AppConfig::getUniqueIdentifier(info.viewConfigID);

    auto helper = service::AppConfigManager::New();

    try
    {
        helper->setConfig(info.viewConfigID, info.replacementMap);
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
        sight::ui::base::dialog::MessageDialog::show(
            "Activity launch failed",
            e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
        SIGHT_ERROR(e.what());
        return;
    }

    info.container = subContainer;
    info.helper    = helper;
    m_activityIds.insert(info.activity->getID());

    m_dynamicInfoMap[widget] = info;
    m_tabIDList.insert(info.tabID);

    int index = m_tabWidget->addTab(widget, finalTitle);
    if(!info.tooltip.empty())
    {
        m_tabWidget->setTabToolTip(index, QString::fromStdString(info.tooltip));
    }

    if(!info.icon.empty())
    {
        m_tabWidget->setTabIcon(index, QIcon(QString::fromStdString(info.icon)));
    }

    m_tabWidget->setCurrentWidget(widget);
}

//------------------------------------------------------------------------------

void SDynamicView::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void SDynamicView::closeTabSignal(int index)
{
    closeTab(index, false);
}

//------------------------------------------------------------------------------

void SDynamicView::closeTab(int index, bool forceClose)
{
    QWidget* widget = m_tabWidget->widget(index);

    SIGHT_ASSERT("Widget is not in dynamicInfoMap", m_dynamicInfoMap.find(widget) != m_dynamicInfoMap.end());
    SDynamicViewInfo info = m_dynamicInfoMap[widget];
    if(info.closable || forceClose)
    {
        m_tabIDList.erase(info.tabID);
        if(!m_dynamicConfigStartStop)
        {
            info.helper->stopAndDestroy();
        }
        else
        {
            if(info.helper->isStarted())
            {
                info.helper->stop();
            }

            info.helper->destroy();
        }

        info.helper.reset();

        //Remove tab first, to avoid tab beeing removed by container->destroy
        m_currentWidget = nullptr;
        m_tabWidget->removeTab(index);

        sight::ui::base::GuiRegistry::unregisterWIDContainer(info.wid);

        info.container->destroyContainer();
        info.container.reset();
        m_dynamicInfoMap.erase(widget);
        m_activityIds.erase(info.activity->getID());
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Close tab",
            "The tab " + info.title + " can not be closed.",
            sight::ui::base::dialog::IMessageDialog::INFO
        );
    }
}

//------------------------------------------------------------------------------

void SDynamicView::changedTab(int index)
{
    QWidget* widget = m_tabWidget->widget(index);

    if(m_dynamicConfigStartStop && widget != m_currentWidget)
    {
        if(m_currentWidget != nullptr)
        {
            SDynamicViewInfo oldinfo = m_dynamicInfoMap[m_currentWidget];
            oldinfo.helper->stop();
        }

        if(widget != nullptr)
        {
            SDynamicViewInfo newinfo = m_dynamicInfoMap[widget];
            if(!newinfo.helper->isStarted())
            {
                newinfo.helper->start();
                newinfo.helper->update();
            }
        }
    }

    m_currentWidget = widget;

    if(index >= 0)
    {
        SDynamicViewInfo info = m_dynamicInfoMap[widget];
        m_sigActivitySelected->asyncEmit(info.activity);
    }
    else
    {
        m_sigNothingSelected->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SDynamicView::buildMainActivity()
{
    auto activity = this->createMainActivity();

    if(activity)
    {
        SDynamicViewInfo viewInfo;
        viewInfo          = this->createViewInfo(activity);
        viewInfo.closable = m_mainActivityClosable;

        this->launchTab(viewInfo);
    }
}

//------------------------------------------------------------------------------

SDynamicView::SDynamicViewInfo SDynamicView::createViewInfo(data::Activity::sptr activity)
{
    auto [info, replacementMap] = sight::activity::extension::Activity::getDefault()->getInfoAndReplacementMap(
        *activity,
        m_parameters
    );

    SDynamicViewInfo viewInfo;
    viewInfo.title          = info.title;
    viewInfo.icon           = info.icon;
    viewInfo.tooltip        = info.tabInfo.empty() ? info.title : info.tabInfo;
    viewInfo.viewConfigID   = info.appConfig.id;
    viewInfo.activity       = activity;
    viewInfo.replacementMap = replacementMap;

    return viewInfo;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
