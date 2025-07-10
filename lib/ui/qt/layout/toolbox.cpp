/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "ui/qt/layout/toolbox.hpp"

#include "ui/qt/app.hpp"
#include "ui/qt/container/widget.hpp"

#include <core/base.hpp>

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

// cspell: ignore QWIDGETSIZE_MAX

SIGHT_REGISTER_GUI(sight::ui::qt::layout::toolbox, sight::ui::layout::toolbox::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void toolbox::create_layout(ui::container::widget::sptr _parent, const std::string& _id)
{
    const QString q_id                               = QString::fromStdString(_id);
    ui::qt::container::widget::sptr parent_container =
        std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    parent_container->get_qt_container()->setObjectName(q_id);

    auto* layout = new QVBoxLayout();
    parent_container->set_layout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    auto* sv      = new QScrollArea();
    auto* toolbox = new ui::qt::widget::toolbox(sv, this->get_icon_color().c_str());
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);
    layout->addWidget(sv);

    const std::list<view_info>& views = this->get_views_info();
    for(const view_info& view_info : views)
    {
        int left_border   = 0;
        int top_border    = 0;
        int right_border  = 0;
        int bottom_border = 0;
        if(view_info.m_border != 0)
        {
            left_border = top_border = right_border = bottom_border = view_info.m_border;
        }
        else
        {
            left_border   = view_info.m_left_border;
            top_border    = view_info.m_top_border;
            right_border  = view_info.m_right_border;
            bottom_border = view_info.m_bottom_border;
        }

        auto* panel = new QWidget();
        panel->setObjectName(q_id + '/' + view_info.m_caption.c_str());
        panel->setMinimumSize(std::max(view_info.m_min_size.first, 0), std::max(view_info.m_min_size.second, 0));
        panel->setMaximumSize(
            std::min(view_info.m_max_size.first, QWIDGETSIZE_MAX),
            std::min(view_info.m_max_size.second, QWIDGETSIZE_MAX)
        );
        panel->setContentsMargins(left_border, top_border, right_border, bottom_border);
        if(!view_info.m_background_color.empty())
        {
            std::array<std::uint8_t, 4> rgba {};
            data::tools::color::hexa_string_to_rgba(view_info.m_background_color, rgba);
            std::stringstream ss;
            ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
            << static_cast<std::int16_t>(rgba[1]) << ','
            << static_cast<std::int16_t>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            panel->setStyleSheet(style + qApp->styleSheet());
        }

        if(!view_info.m_qss_key.empty())
        {
            panel->setProperty("class", QString::fromStdString(view_info.m_qss_key));
        }

        ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
        sub_container->set_qt_container(panel);
        m_sub_views.push_back(sub_container);

        int index = 0;
        if(view_info.m_use_scroll_bar)
        {
            auto* scroll_area = new QScrollArea(toolbox);
            scroll_area->setWidget(panel);
            scroll_area->setWidgetResizable(true);
            if(!view_info.m_background_color.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexa_string_to_rgba(view_info.m_background_color, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                scroll_area->setStyleSheet(style + qApp->styleSheet());
            }

            index = toolbox->add_item(scroll_area, QString::fromStdString(view_info.m_caption));
        }
        else
        {
            index = toolbox->add_item(panel, QString::fromStdString(view_info.m_caption));
        }

        if(view_info.m_expanded)
        {
            toolbox->expand_item(index);
        }

        if(!view_info.m_visible)
        {
            sub_container->set_visible(false);
        }
    }
}

//-----------------------------------------------------------------------------

void toolbox::destroy_layout()
{
    this->destroy_sub_views();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
