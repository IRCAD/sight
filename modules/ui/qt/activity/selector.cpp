/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "selector.hpp"

#include <activity/builder/base.hpp>
#include <activity/validator/base.hpp>

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/activity.hpp>
#include <data/composite.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/foreach.hpp>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(sight::activity::extension::activity_info)

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

const core::com::signals::key_t selector::ACTIVITY_ID_SELECTED_SIG = "activityIDSelected";
const core::com::signals::key_t selector::LOAD_REQUESTED_SIG       = "loadRequested";

//------------------------------------------------------------------------------

selector::selector() noexcept
{
    new_signal<ActivityIDSelectedSignalType>(ACTIVITY_ID_SELECTED_SIG);
    new_signal<LoadRequestedSignalType>(LOAD_REQUESTED_SIG);
}

//------------------------------------------------------------------------------

selector::~selector() noexcept =
    default;

//------------------------------------------------------------------------------

void selector::configuring()
{
    sight::ui::service::initialize();

    const auto cfg = this->get_config();

    if(cfg.count("filter") == 1)
    {
        const service::config_t& configFilter = cfg.get_child("filter");
        SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

        const auto mode = configFilter.get<std::string>("mode");
        SIGHT_ASSERT(
            "'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
            mode == "include" || mode == "exclude"
        );
        m_filterMode = mode;

        // NOLINTNEXTLINE(bugprone-branch-clone)
        BOOST_FOREACH(const config_t::value_type& v, configFilter.equal_range("id"))
        {
            m_keys.push_back(v.second.get<std::string>(""));
        }
    }

    SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", cfg.count("filter") < 2);
}

//------------------------------------------------------------------------------

void selector::starting()
{
    sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* groupBox = new QGroupBox(tr("Activity"));

    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(groupBox);
    scrollArea->setWidgetResizable(true);

    auto* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea);

    m_buttonGroup = new QButtonGroup(groupBox);

    auto infos = sight::activity::extension::activity::getDefault()->getInfos();
    m_activitiesInfo = this->getEnabledActivities(infos);

    // Add the load button
    sight::activity::extension::activity_info infoLoad;
    infoLoad.title = "Load activity";
    infoLoad.icon  =
        core::runtime::get_module_resource_file_path("sight::module::ui::icons", "LoadActivity.svg").string();
    infoLoad.description = "Load a previously saved activity.";

    m_activitiesInfo.insert(m_activitiesInfo.begin(), infoLoad);

    std::size_t indexButton = 0;
    const float rows        = std::sqrt(static_cast<float>(m_activitiesInfo.size()));
    int numCols             = static_cast<int>(std::ceil(rows));
    const int numRows       = static_cast<int>(std::floor(rows));
    numCols = 2 * numCols + 1;

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QWidget* const container = qtContainer->getQtContainer();
    container->setObjectName(serviceID);
    const std::string styleGrid("QGridLayout#activities {"
                                "border-width: 4px;"
                                "}");
    container->setStyleSheet(QString::fromUtf8(styleGrid.c_str()));

    auto* activitiesLayout = new QGridLayout();
    activitiesLayout->setRowMinimumHeight(0, 5);
    activitiesLayout->setRowStretch(0, 2);
    groupBox->setLayout(activitiesLayout);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    std::string style("* {"
                      "padding: 16px;"
                      "text-align:bottom"
                      "}");

    int i = 1;
    int j = 0;

    for(const auto& info : m_activitiesInfo)
    {
        auto* button = new QPushButton(QIcon(info.icon.c_str()), QString::fromStdString(" " + info.title));
        button->setToolTip(QString::fromStdString(info.description));
        button->setIconSize(QSize(80, 80));
        button->setObjectName(serviceID + '/' + info.title.c_str());
        button->setFont(font);

        button->setStyleSheet(QString::fromUtf8(style.c_str()));
        m_buttonGroup->addButton(button, static_cast<int>(indexButton));

        auto* label = new QLabel(QString::fromStdString(info.description));
        label->setWordWrap(true);

        activitiesLayout->setColumnMinimumWidth(j, 10);
        activitiesLayout->setColumnStretch(j, 5);
        activitiesLayout->addWidget(button, i, j + 1);

        activitiesLayout->addWidget(label, i + 1, j + 1);
        j += 2;
        if(j == numCols - 1)
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

void selector::stopping()
{
    this->disconnect();
    this->destroy();
}

//------------------------------------------------------------------------------

void selector::updating()
{
}

//------------------------------------------------------------------------------

void selector::onClicked(int id)
{
    if(id == 0)
    {
        auto sig = this->signal<LoadRequestedSignalType>(LOAD_REQUESTED_SIG);
        sig->async_emit();
    }
    else
    {
        auto sig = this->signal<ActivityIDSelectedSignalType>(ACTIVITY_ID_SELECTED_SIG);
        sig->async_emit(m_activitiesInfo[static_cast<std::size_t>(id)].id);
    }
}

//------------------------------------------------------------------------------

selector::activity_infos_t selector::getEnabledActivities(const activity_infos_t& infos)
{
    activity_infos_t configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool isIncludeMode = m_filterMode == "include";

        for(const auto& info : infos)
        {
            auto keyIt = std::find(m_keys.begin(), m_keys.end(), info.id);

            if((keyIt != m_keys.end() && isIncludeMode) || (keyIt == m_keys.end() && !isIncludeMode))
            {
                configs.push_back(info);
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

} // namespace sight::module::ui::qt::activity
