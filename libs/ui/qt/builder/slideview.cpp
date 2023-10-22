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

void slideview::createContainer(ui::container::widget::sptr _parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    QWidget* qt_parent = m_parent->getQtContainer();

    ui::qt::widget::slide_bar::HAlignment h_align             = ui::qt::widget::slide_bar::LEFT;
    ui::qt::widget::slide_bar::VAlignment v_align             = ui::qt::widget::slide_bar::TOP;
    ui::qt::widget::slide_bar::AnimatableAlignment anim_align = ui::qt::widget::slide_bar::TOP_ANIMATION;

    switch(m_hAlignment)
    {
        case slideview::LEFT:
            h_align = ui::qt::widget::slide_bar::LEFT;
            break;

        case slideview::RIGHT:
            h_align = ui::qt::widget::slide_bar::RIGHT;
            break;
    }

    switch(m_vAlignment)
    {
        case slideview::TOP:
            v_align = ui::qt::widget::slide_bar::TOP;
            break;

        case slideview::BOTTOM:
            v_align = ui::qt::widget::slide_bar::BOTTOM;
            break;
    }

    switch(m_animatableAlignment)
    {
        case slideview::TOP_ANIMATION:
            anim_align = ui::qt::widget::slide_bar::TOP_ANIMATION;
            break;

        case slideview::BOTTOM_ANIMATION:
            anim_align = ui::qt::widget::slide_bar::BOTTOM_ANIMATION;
            break;

        case slideview::LEFT_ANIMATION:
            anim_align = ui::qt::widget::slide_bar::LEFT_ANIMATION;
            break;

        case slideview::RIGHT_ANIMATION:
            anim_align = ui::qt::widget::slide_bar::RIGHT_ANIMATION;
            break;
    }

    auto* slide_bar =
        new ui::qt::widget::slide_bar(
            qt_parent,
            h_align,
            v_align,
            m_width,
            m_percentWidth,
            m_height,
            m_percentHeight,
            m_hOffset,
            m_percentHOffset,
            m_vOffset,
            m_percentVOffset,
            m_opacity,
            m_animatable,
            anim_align
        );

    if(!m_styleSheet.empty())
    {
        slide_bar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    ui::qt::container::widget::sptr qt_container = ui::qt::container::widget::make();
    qt_container->setQtContainer(slide_bar);

    m_container = qt_container;
}

//-----------------------------------------------------------------------------

void slideview::destroyContainer()
{
    SIGHT_ASSERT("The Container is not initialized", m_container);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
