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

#include "ui/qt/layout/tab.hpp"

#include "ui/qt/app.hpp"

#include <core/base.hpp>

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QTabWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::tab, sight::ui::layout::tab::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void tab::create_layout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parent_container);
    m_parent_container->get_qt_container()->setObjectName(QString::fromStdString(_id));

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_parent_container->set_layout(layout);

    m_tab_widget = new QTabWidget();
    layout->addWidget(m_tab_widget);

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

        auto* const widget        = new QWidget(m_tab_widget);
        const int min_width_size  = std::max(view_info.m_min_size.first, 0);
        const int min_height_size = std::max(view_info.m_min_size.second, 0);
        widget->setMinimumSize(min_width_size, min_height_size);
        widget->setContentsMargins(left_border, top_border, right_border, bottom_border);

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
            widget->setStyleSheet(style + qApp->styleSheet());
        }

        if(!view_info.m_qss_key.empty())
        {
            widget->setProperty("class", QString::fromStdString(view_info.m_qss_key));
        }

        ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
        sub_container->set_qt_container(widget);
        m_sub_views.push_back(sub_container);

        int idx = 0;
        if(view_info.m_use_scroll_bar)
        {
            auto* const scroll_area = new QScrollArea(m_tab_widget);
            scroll_area->setWidget(widget);
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

            idx = m_tab_widget->addTab(scroll_area, QString::fromStdString(view_info.m_caption));
        }
        else
        {
            idx = m_tab_widget->addTab(widget, QString::fromStdString(view_info.m_caption));
        }

        if(view_info.m_is_select)
        {
            m_tab_widget->setCurrentIndex(idx);
        }
    }
}

//-----------------------------------------------------------------------------

void tab::destroy_layout()
{
    this->destroy_sub_views();
    m_tab_widget->clear();
    m_parent_container->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
