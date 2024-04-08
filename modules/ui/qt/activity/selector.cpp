/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

const core::com::signals::key_t selector::ACTIVITY_ID_SELECTED_SIG = "activity_id_selected";
const core::com::signals::key_t selector::LOAD_REQUESTED_SIG       = "load_requested";

//------------------------------------------------------------------------------

selector::selector() noexcept
{
    new_signal<activity_id_selected_signal_t>(ACTIVITY_ID_SELECTED_SIG);
    new_signal<load_requested_signal_t>(LOAD_REQUESTED_SIG);
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
        const service::config_t& config_filter = cfg.get_child("filter");
        SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", config_filter.count("mode") < 2);

        const auto mode = config_filter.get<std::string>("mode");
        SIGHT_ASSERT(
            "'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
            mode == "include" || mode == "exclude"
        );
        m_filter_mode = mode;

        // NOLINTNEXTLINE(bugprone-branch-clone)
        BOOST_FOREACH(const config_t::value_type& v, config_filter.equal_range("id"))
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

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* group_box = new QGroupBox(tr("Activity"));

    auto* scroll_area = new QScrollArea();
    scroll_area->setWidget(group_box);
    scroll_area->setWidgetResizable(true);

    auto* main_layout = new QVBoxLayout();
    main_layout->addWidget(scroll_area);

    m_button_group = new QButtonGroup(group_box);

    auto infos = sight::activity::extension::activity::get_default()->get_infos();
    m_activities_info = this->get_enabled_activities(infos);

    // Add the load button
    sight::activity::extension::activity_info info_load;
    info_load.title = "Load activity";
    info_load.icon  =
        core::runtime::get_module_resource_file_path("sight::module::ui::icons", "load.svg").string();
    info_load.description = "Load a previously saved activity.";

    m_activities_info.insert(m_activities_info.begin(), info_load);

    std::size_t index_button = 0;
    const float rows         = std::sqrt(static_cast<float>(m_activities_info.size()));
    int num_cols             = static_cast<int>(std::ceil(rows));
    const int num_rows       = static_cast<int>(std::floor(rows));
    num_cols = 2 * num_cols + 1;

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    QWidget* const container = qt_container->get_qt_container();
    container->setObjectName(service_id);
    const std::string style_grid("QGridLayout#activities {"
                                 "border-width: 4px;"
                                 "}");
    container->setStyleSheet(QString::fromUtf8(style_grid.c_str()));

    auto* activities_layout = new QGridLayout();
    activities_layout->setRowMinimumHeight(0, 5);
    activities_layout->setRowStretch(0, 2);
    group_box->setLayout(activities_layout);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    std::string style("* {"
                      "padding: 16px;"
                      "text-align:bottom"
                      "}");

    int i = 1;
    int j = 0;

    for(const auto& info : m_activities_info)
    {
        auto* button = new QPushButton(QIcon(info.icon.c_str()), QString::fromStdString(" " + info.title));
        button->setToolTip(QString::fromStdString(info.description));
        button->setIconSize(QSize(80, 80));
        button->setObjectName(service_id + '/' + info.title.c_str());
        button->setFont(font);

        button->setStyleSheet(QString::fromUtf8(style.c_str()));
        m_button_group->addButton(button, static_cast<int>(index_button));

        auto* label = new QLabel(QString::fromStdString(info.description));
        label->setWordWrap(true);

        activities_layout->setColumnMinimumWidth(j, 10);
        activities_layout->setColumnStretch(j, 5);
        activities_layout->addWidget(button, i, j + 1);

        activities_layout->addWidget(label, i + 1, j + 1);
        j += 2;
        if(j == num_cols - 1)
        {
            activities_layout->setColumnMinimumWidth(j, 10);
            activities_layout->setColumnStretch(j, 5);
            i += 3;
            j  = 0;
        }

        activities_layout->setRowMinimumHeight(i + 2, 5);
        activities_layout->setRowStretch(i + 2, 1);

        ++index_button;
    }

    activities_layout->setRowMinimumHeight(num_rows * 3, 5);
    activities_layout->setRowStretch(num_rows * 3, 2);

    qt_container->set_layout(main_layout);

    this->connect(m_button_group, SIGNAL(buttonClicked(int)), SLOT(on_clicked(int)));
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

void selector::on_clicked(int _id)
{
    if(_id == 0)
    {
        auto sig = this->signal<load_requested_signal_t>(LOAD_REQUESTED_SIG);
        sig->async_emit();
    }
    else
    {
        auto sig = this->signal<activity_id_selected_signal_t>(ACTIVITY_ID_SELECTED_SIG);
        sig->async_emit(m_activities_info[static_cast<std::size_t>(_id)].id);
    }
}

//------------------------------------------------------------------------------

selector::activity_infos_t selector::get_enabled_activities(const activity_infos_t& _infos)
{
    activity_infos_t configs;

    if(m_filter_mode == "include" || m_filter_mode == "exclude")
    {
        const bool is_include_mode = m_filter_mode == "include";

        for(const auto& info : _infos)
        {
            auto key_it = std::find(m_keys.begin(), m_keys.end(), info.id);

            if((key_it != m_keys.end() && is_include_mode) || (key_it == m_keys.end() && !is_include_mode))
            {
                configs.push_back(info);
            }
        }
    }
    else
    {
        configs = _infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
