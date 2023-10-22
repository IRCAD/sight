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

#include "ui/qt/layout/line.hpp"

#include "ui/qt/app.hpp"

#include <core/base.hpp>

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QStyle>

// cspell: ignore QWIDGETSIZE_MAX

SIGHT_REGISTER_GUI(sight::ui::qt::layout::line, sight::ui::layout::line::REGISTRY_KEY)

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void line::createLayout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parentContainer);
    const QString q_id = QString::fromStdString(_id);
    m_parentContainer->getQtContainer()->setObjectName(q_id);

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    Orientation orientation = this->getOrientation();

    if(orientation == VERTICAL)
    {
        layout->setDirection(QBoxLayout::TopToBottom);
    }

    const std::list<ViewInfo>& views = this->getViewsInfo();
    for(std::size_t idx = 0 ; const ViewInfo& view_info : views)
    {
        if(view_info.m_isSpacer)
        {
            layout->addStretch();
        }
        else
        {
            QWidget* panel    = nullptr;
            int left_border   = 0;
            int top_border    = 0;
            int right_border  = 0;
            int bottom_border = 0;

            if(view_info.m_spacing != -1)
            {
                layout->setSpacing(view_info.m_spacing);
            }

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

            if(view_info.m_caption.first)
            {
                auto* groupbox = new QGroupBox();
                groupbox->setObjectName(q_id + '/' + view_info.m_caption.second.c_str());
                groupbox->setTitle(QString::fromStdString(view_info.m_caption.second));
                panel          = groupbox;
                left_border   += groupbox->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
                top_border    += groupbox->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
                right_border  += groupbox->style()->pixelMetric(QStyle::PM_LayoutRightMargin);
                bottom_border += groupbox->style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
            }
            else
            {
                panel = new QWidget();
                panel->setObjectName(q_id + '/' + QString("%1").arg(idx));
            }

            panel->setMinimumSize(std::max(view_info.m_minSize.first, 0), std::max(view_info.m_minSize.second, 0));
            panel->setMaximumSize(
                std::min(view_info.m_maxSize.first, QWIDGETSIZE_MAX),
                std::min(view_info.m_maxSize.second, QWIDGETSIZE_MAX)
            );
            panel->setContentsMargins(left_border, top_border, right_border, bottom_border);
            if(!view_info.m_toolTip.empty())
            {
                panel->setToolTip(QString::fromStdString(view_info.m_toolTip));
            }

            ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
            sub_container->setQtContainer(panel);
            m_subViews.push_back(sub_container);

            if(!view_info.m_qssKey.empty())
            {
                panel->setProperty("class", QString::fromStdString(view_info.m_qssKey));
            }

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

            if(view_info.m_useScrollBar)
            {
                auto* scroll_area = new QScrollArea();
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

                layout->addWidget(scroll_area);
                layout->setStretchFactor(scroll_area, view_info.m_proportion);
            }
            else
            {
                layout->addWidget(panel);
                layout->setStretchFactor(panel, view_info.m_proportion);
            }

            if(!view_info.m_visible)
            {
                sub_container->setVisible(false);
            }
        }

        idx++;
    }
}

//-----------------------------------------------------------------------------

void line::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
