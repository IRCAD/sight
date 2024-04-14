/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "ui/qt/builder/slideview.hpp"

#include "ui/qt/container/widget.hpp"

#include <ui/__/macros.hpp>
#include <ui/qt/widget/slide_bar.hpp>

#include <QWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::builder::slideview, sight::ui::builder::slideview::REGISTRY_KEY)

namespace sight::ui::qt::builder
{

//-----------------------------------------------------------------------------

void slideview::create_container(ui::container::widget::sptr _parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    QWidget* qt_parent = m_parent->get_qt_container();

    ui::qt::widget::slide_bar::h_alignment h_align             = ui::qt::widget::slide_bar::left;
    ui::qt::widget::slide_bar::v_alignment v_align             = ui::qt::widget::slide_bar::top;
    ui::qt::widget::slide_bar::animatable_alignment anim_align = ui::qt::widget::slide_bar::top_animation;

    switch(m_h_alignment)
    {
        case slideview::left:
            h_align = ui::qt::widget::slide_bar::left;
            break;

        case slideview::right:
            h_align = ui::qt::widget::slide_bar::right;
            break;
    }

    switch(m_v_alignment)
    {
        case slideview::top:
            v_align = ui::qt::widget::slide_bar::top;
            break;

        case slideview::bottom:
            v_align = ui::qt::widget::slide_bar::bottom;
            break;
    }

    switch(m_animatable_alignment)
    {
        case slideview::top_animation:
            anim_align = ui::qt::widget::slide_bar::top_animation;
            break;

        case slideview::bottom_animation:
            anim_align = ui::qt::widget::slide_bar::bottom_animation;
            break;

        case slideview::left_animation:
            anim_align = ui::qt::widget::slide_bar::left_animation;
            break;

        case slideview::right_animation:
            anim_align = ui::qt::widget::slide_bar::right_animation;
            break;
    }

    auto* slide_bar =
        new ui::qt::widget::slide_bar(
            qt_parent,
            h_align,
            v_align,
            m_width,
            m_percent_width,
            m_height,
            m_percent_height,
            m_h_offset,
            m_percent_h_offset,
            m_v_offset,
            m_percent_v_offset,
            m_opacity,
            m_animatable,
            anim_align
        );

    if(!m_style_sheet.empty())
    {
        slide_bar->setStyleSheet(QString::fromStdString(m_style_sheet));
    }

    ui::qt::container::widget::sptr qt_container = ui::qt::container::widget::make();
    qt_container->set_qt_container(slide_bar);

    m_container = qt_container;
}

//-----------------------------------------------------------------------------

void slideview::destroy_container()
{
    SIGHT_ASSERT("The Container is not initialized", m_container);

    m_container->destroy_container();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
