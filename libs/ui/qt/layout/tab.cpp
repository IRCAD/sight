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

void tab::createLayout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parentContainer);
    m_parentContainer->getQtContainer()->setObjectName(QString::fromStdString(_id));

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_parentContainer->setLayout(layout);

    m_tabWidget = new QTabWidget();
    layout->addWidget(m_tabWidget);

    const std::list<ViewInfo>& views = this->getViewsInfo();

    for(const ViewInfo& view_info : views)
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
            left_border   = view_info.m_leftBorder;
            top_border    = view_info.m_topBorder;
            right_border  = view_info.m_rightBorder;
            bottom_border = view_info.m_bottomBorder;
        }

        auto* const widget        = new QWidget(m_tabWidget);
        const int min_width_size  = std::max(view_info.m_minSize.first, 0);
        const int min_height_size = std::max(view_info.m_minSize.second, 0);
        widget->setMinimumSize(min_width_size, min_height_size);
        widget->setContentsMargins(left_border, top_border, right_border, bottom_border);

        if(!view_info.m_backgroundColor.empty())
        {
            std::array<std::uint8_t, 4> rgba {};
            data::tools::color::hexaStringToRGBA(view_info.m_backgroundColor, rgba);
            std::stringstream ss;
            ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
            << static_cast<std::int16_t>(rgba[1]) << ','
            << static_cast<std::int16_t>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            widget->setStyleSheet(style + qApp->styleSheet());
        }

        ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
        sub_container->setQtContainer(widget);
        m_subViews.push_back(sub_container);

        int idx = 0;
        if(view_info.m_useScrollBar)
        {
            auto* const scroll_area = new QScrollArea(m_tabWidget);
            scroll_area->setWidget(widget);
            scroll_area->setWidgetResizable(true);
            if(!view_info.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexaStringToRGBA(view_info.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                scroll_area->setStyleSheet(style + qApp->styleSheet());
            }

            idx = m_tabWidget->addTab(scroll_area, QString::fromStdString(view_info.m_caption));
        }
        else
        {
            idx = m_tabWidget->addTab(widget, QString::fromStdString(view_info.m_caption));
        }

        if(view_info.m_isSelect)
        {
            m_tabWidget->setCurrentIndex(idx);
        }
    }
}

//-----------------------------------------------------------------------------

void tab::destroyLayout()
{
    this->destroySubViews();
    m_tabWidget->clear();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
