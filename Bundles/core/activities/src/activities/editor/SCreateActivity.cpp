/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "activities/editor/SCreateActivity.hpp"

#ifdef KEEP_OLD_SERVICE

#include "fwServices/IService.hpp"

#include <fwActivities/IBuilder.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Convert.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(::fwActivities::registry::ActivityInfo)

#endif

namespace activities
{
namespace editor
{

#ifdef KEEP_OLD_SERVICE

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::activities::editor::SCreateActivity );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SCreateActivity::s_ACTIVITY_ID_SELECTED_SIG = "activityIDSelected";
const ::fwCom::Signals::SignalKeyType SCreateActivity::s_LOAD_REQUESTED_SIG       = "loadRequested";

//------------------------------------------------------------------------------

#endif

SCreateActivity::SCreateActivity() noexcept
{
#ifndef KEEP_OLD_SERVICE
    SLM_FATAL("Do not use activities::SCreateActivity with Qml");
#else
    FW_DEPRECATED("::activities::editor::SCreateActivity", "::uiActivitiesQt::editor::SCreateActivity", "21.0");
    newSignal< ActivityIDSelectedSignalType >(s_ACTIVITY_ID_SELECTED_SIG);
    newSignal< LoadRequestedSignalType >(s_LOAD_REQUESTED_SIG);
#endif
}

//------------------------------------------------------------------------------

SCreateActivity::~SCreateActivity() noexcept
{
}

#ifdef KEEP_OLD_SERVICE

//------------------------------------------------------------------------------

void SCreateActivity::configuring()
{
    fwGui::IGuiContainerSrv::initialize();

    const auto cfg = this->getConfigTree();

    if(cfg.count("filter") == 1 )
    {
        const ::fwServices::IService::ConfigType& configFilter = cfg.get_child("filter");
        SLM_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

        const std::string mode = configFilter.get< std::string >("mode");
        SLM_ASSERT("'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                   mode == "include" || mode == "exclude");
        m_filterMode = mode;

        BOOST_FOREACH( const ConfigType::value_type& v, configFilter.equal_range("id") )
        {
            m_keys.push_back(v.second.get<std::string>(""));
        }
    }
    SLM_ASSERT("A maximum of 1 <filter> tag is allowed", cfg.count("filter") < 2);
}

//------------------------------------------------------------------------------

void SCreateActivity::starting()
{
    fwGui::IGuiContainerSrv::create();

    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QGroupBox* groupBox = new QGroupBox(tr("Activities") );

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(groupBox);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea);

    m_buttonGroup = new QButtonGroup(groupBox);

    ActivityInfoContainer infos = ::fwActivities::registry::Activities::getDefault()->getInfos();
    m_activitiesInfo = this->getEnabledActivities(infos);

    // Add the load button
    ::fwActivities::registry::ActivityInfo infoLoad;
    infoLoad.title       = "Load activity";
    infoLoad.icon        = ::fwRuntime::getBundleResourceFilePath("media", "icons/LoadActivity.svg").string();
    infoLoad.description = "Load a previously saved activity.";

    m_activitiesInfo.insert(m_activitiesInfo.begin(), infoLoad);

    size_t indexButton = 0;
    size_t numCols     = static_cast<size_t>(std::ceil(std::sqrt(static_cast<float>(m_activitiesInfo.size()))));
    int numRows        = static_cast<int>(std::floor(std::sqrt(static_cast<float>(m_activitiesInfo.size()))));
    numCols = numCols + numCols + 1;

    QWidget* const container = qtContainer->getQtContainer();
    container->setObjectName("activities");
    std::string styleGrid("QGridLayout#activities {"
                          "border-width: 4px;"
                          "}");
    container->setStyleSheet(QString::fromUtf8(styleGrid.c_str()));

    QGridLayout* activitiesLayout = new QGridLayout();
    activitiesLayout->setRowMinimumHeight(0, 5);
    activitiesLayout->setRowStretch(0, 2);
    groupBox->setLayout(activitiesLayout);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    std::string style("QPushButton#activityButton {"
                      "padding: 16px;"
                      "text-align:bottom"
                      "}");

    int i = 1;
    int j = 0;

    for(const ::fwActivities::registry::ActivityInfo& info :  m_activitiesInfo)
    {
        QPushButton* button = new QPushButton(QIcon(info.icon.c_str()), QString::fromStdString(" " + info.title));
        button->setToolTip(QString::fromStdString(info.description));
        button->setIconSize(QSize(80, 80));
        button->setObjectName("activityButton");
        button->setFont(font);

        button->setStyleSheet(QString::fromUtf8(style.c_str()));
        m_buttonGroup->addButton(button, static_cast<int>(indexButton));

        QLabel* label = new QLabel(QString::fromStdString(info.description));
        label->setWordWrap(true);

        activitiesLayout->setColumnMinimumWidth(j, 10);
        activitiesLayout->setColumnStretch(j, 5);
        activitiesLayout->addWidget(button, i, j + 1);

        activitiesLayout->addWidget(label, i + 1, j + 1);
        j += 2;
        if(j == static_cast<int>(numCols) - 1 )
        {
            activitiesLayout->setColumnMinimumWidth(j, 10);
            activitiesLayout->setColumnStretch(j, 5);
            i += 3;
            j  = 0;
        }

        activitiesLayout->setRowMinimumHeight(i + 2, 5);
        activitiesLayout->setRowStretch(i + 2, 1);

        ++indexButton;
    }

    activitiesLayout->setRowMinimumHeight(numRows * 3, 5);
    activitiesLayout->setRowStretch(numRows * 3, 2);

    qtContainer->setLayout(mainLayout);

    this->connect(m_buttonGroup, SIGNAL(buttonClicked(int)), SLOT(onClicked(int)));
}

//------------------------------------------------------------------------------

void SCreateActivity::stopping()
{
    this->disconnect();
    this->destroy();
}

//------------------------------------------------------------------------------

void SCreateActivity::updating()
{
}

//------------------------------------------------------------------------------

void SCreateActivity::onClicked(int id)
{
    if(id == 0)
    {
        auto sig = this->signal<LoadRequestedSignalType>(s_LOAD_REQUESTED_SIG);
        sig->asyncEmit();
    }
    else
    {
        auto sig = this->signal<ActivityIDSelectedSignalType>(s_ACTIVITY_ID_SELECTED_SIG);
        sig->asyncEmit(m_activitiesInfo[ static_cast<size_t>(id)].id);
    }
}

//------------------------------------------------------------------------------

SCreateActivity::ActivityInfoContainer SCreateActivity::getEnabledActivities(const ActivityInfoContainer& infos)
{
    ActivityInfoContainer configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool isIncludeMode = m_filterMode == "include";

        for(ActivityInfoContainer::const_iterator iter = infos.begin(); iter != infos.end(); ++iter)
        {
            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), iter->id);

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                configs.push_back(*iter);
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                configs.push_back(*iter);
            }
        }
    }
    else
    {
        configs = infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

#endif

} // namespace editor
} // namespace activities
