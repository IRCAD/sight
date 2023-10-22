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

void toolbox::createLayout(ui::container::widget::sptr _parent, const std::string& _id)
{
    const QString q_id                               = QString::fromStdString(_id);
    ui::qt::container::widget::sptr parent_container =
        std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    parent_container->getQtContainer()->setObjectName(q_id);

    auto* layout = new QVBoxLayout();
    parent_container->setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    auto* sv      = new QScrollArea();
    auto* toolbox = new ui::qt::widget::QfwToolBox(sv);
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);
    layout->addWidget(sv);

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

        auto* panel = new QWidget();
        panel->setObjectName(q_id + '/' + view_info.m_caption.c_str());
        panel->setMinimumSize(std::max(view_info.m_minSize.first, 0), std::max(view_info.m_minSize.second, 0));
        panel->setMaximumSize(
            std::min(view_info.m_maxSize.first, QWIDGETSIZE_MAX),
            std::min(view_info.m_maxSize.second, QWIDGETSIZE_MAX)
        );
        panel->setContentsMargins(left_border, top_border, right_border, bottom_border);
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
            panel->setStyleSheet(style + qApp->styleSheet());
        }

        ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
        sub_container->setQtContainer(panel);
        m_subViews.push_back(sub_container);

        int index = 0;
        if(view_info.m_useScrollBar)
        {
            auto* scroll_area = new QScrollArea(toolbox);
            scroll_area->setWidget(panel);
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

            index = toolbox->addItem(scroll_area, QString::fromStdString(view_info.m_caption));
        }
        else
        {
            index = toolbox->addItem(panel, QString::fromStdString(view_info.m_caption));
        }

        if(view_info.m_expanded)
        {
            toolbox->expandItem(index);
        }

        if(!view_info.m_visible)
        {
            sub_container->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------

void toolbox::destroyLayout()
{
    this->destroySubViews();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
