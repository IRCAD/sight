/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

void line::create_layout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parent_container);

    const QString q_id = QString::fromStdString(_id);
    m_parent_container->get_qt_container()->setObjectName(q_id);

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_parent_container->set_layout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    if(auto orientation = this->get_orientation(); orientation == vertical)
    {
        layout->setDirection(QBoxLayout::TopToBottom);
    }

    if(const auto spacing = this->get_spacing(); spacing != -1)
    {
        layout->setSpacing(spacing);
    }

    for(std::size_t idx = 0 ; const view_info& view_info : this->get_views_info())
    {
        if(view_info.m_is_spacer)
        {
            layout->addStretch();
        }
        else
        {
            // Set the spacing
            /// @note this is cumbersome as child views will override the layout spacing, and the last view will win.
            /// @todo remove this as this is not a good practice.
            if(view_info.m_spacing != -1)
            {
                FW_DEPRECATED_MSG("Setting spacing on views is deprecated.", "26.0");
                layout->setSpacing(view_info.m_spacing);
            }

            // top_panel is the final layer, exposed to children. Create it first.
            /// @note its layout may be created by children. Settings margins, spacing on it will have no effects.
            auto* const top_panel = new QWidget();

            // mid_panel widget is the layer just below the panel. It could be a groupbox or a simple widget.
            // It is the one that will manage margins.
            QWidget* mid_panel = nullptr;

            // Compute margins
            int left_margin   = view_info.m_left_border;
            int top_margin    = view_info.m_top_border;
            int right_margin  = view_info.m_right_border;
            int bottom_margin = view_info.m_bottom_border;

            if(view_info.m_border != 0)
            {
                left_margin   = view_info.m_border;
                top_margin    = view_info.m_border;
                right_margin  = view_info.m_border;
                bottom_margin = view_info.m_border;
            }

            // If there is a "caption", create a groupbox
            if(view_info.m_caption.first)
            {
                const QString& title = QString::fromStdString(view_info.m_caption.second);
                auto* const groupbox = new QGroupBox();
                groupbox->setObjectName(q_id + '/' + title);
                groupbox->setTitle(title);
                mid_panel = groupbox;

                // Groupbox margins should not go under defaults, or title may be cropped
                const auto* const style = groupbox->style();
                left_margin   = std::max(left_margin, style->pixelMetric(QStyle::PM_LayoutLeftMargin));
                top_margin    = std::max(top_margin, style->pixelMetric(QStyle::PM_LayoutTopMargin));
                right_margin  = std::max(right_margin, style->pixelMetric(QStyle::PM_LayoutRightMargin));
                bottom_margin = std::max(bottom_margin, style->pixelMetric(QStyle::PM_LayoutBottomMargin));
            }
            else
            {
                mid_panel = new QWidget();
                mid_panel->setObjectName(q_id + '/' + QString::number(idx));
            }

            // Add the top_panel to the mid_panel layout
            auto* const mid_layout = new QBoxLayout(QBoxLayout::LeftToRight);
            mid_layout->addWidget(top_panel);
            mid_panel->setLayout(mid_layout);

            // Set mid_panel margins
            mid_layout->setContentsMargins(
                left_margin,
                top_margin,
                right_margin,
                bottom_margin
            );

            // bottom_panel is the layer just below the mid_panel. It could be mid_panel or a scroll area.
            QWidget* bottom_panel = mid_panel;

            if(view_info.m_use_scroll_bar)
            {
                auto* const scroll_area = new QScrollArea();
                scroll_area->setWidget(mid_panel);
                scroll_area->setWidgetResizable(true);
                bottom_panel = scroll_area;
            }

            // Set top_panel tool tip
            if(!view_info.m_tool_tip.empty())
            {
                const auto& tool_tip = QString::fromStdString(view_info.m_tool_tip);

                top_panel->setToolTip(tool_tip);
                mid_panel->setToolTip(tool_tip);

                if(mid_panel != bottom_panel)
                {
                    bottom_panel->setToolTip(tool_tip);
                }
            }

            // Setting the class for the bottom_panel (QSS is forwarded to children)
            if(!view_info.m_qss_key.empty())
            {
                bottom_panel->setProperty("class", QString::fromStdString(view_info.m_qss_key));
            }

            // Manage background color (QSS is forwarded to children)
            if(!view_info.m_background_color.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexa_string_to_rgba(view_info.m_background_color, rgba);

                std::stringstream ss;
                ss << "QWidget { background-color: rgba("
                << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";

                const QString style = QString::fromStdString(ss.str()) + qApp->styleSheet();

                bottom_panel->setStyleSheet(style);
            }

            // Set minimum/maximum size
            bottom_panel->setMinimumSize(
                std::max(view_info.m_min_size.first, bottom_panel->minimumWidth()),
                std::max(view_info.m_min_size.second, bottom_panel->minimumHeight())
            );

            bottom_panel->setMaximumSize(
                std::min(view_info.m_max_size.first, QWIDGETSIZE_MAX),
                std::min(view_info.m_max_size.second, QWIDGETSIZE_MAX)
            );

            // Add the bottom_panel to the layout
            layout->addWidget(bottom_panel);
            layout->setStretchFactor(bottom_panel, view_info.m_proportion);

            // Create the sub view
            auto sub_view = ui::qt::container::widget::make();
            sub_view->set_qt_container(top_panel, bottom_panel);
            m_sub_views.push_back(sub_view);

            // Manage container visibility
            if(!view_info.m_visible)
            {
                sub_view->set_visible(false);
            }
        }

        idx++;
    }
}

//-----------------------------------------------------------------------------

void line::destroy_layout()
{
    this->destroy_sub_views();
    m_parent_container->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
