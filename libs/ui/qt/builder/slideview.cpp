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
#include <ui/qt/widget/SlideBar.hpp>

#include <QWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::builder::slideview, sight::ui::builder::slideview::REGISTRY_KEY)

namespace sight::ui::qt::builder
{

//-----------------------------------------------------------------------------

void slideview::createContainer(ui::container::widget::sptr parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    QWidget* qtParent = m_parent->getQtContainer();

    ui::qt::widget::SlideBar::HAlignment hAlign             = ui::qt::widget::SlideBar::LEFT;
    ui::qt::widget::SlideBar::VAlignment vAlign             = ui::qt::widget::SlideBar::TOP;
    ui::qt::widget::SlideBar::AnimatableAlignment animAlign = ui::qt::widget::SlideBar::TOP_ANIMATION;

    switch(m_hAlignment)
    {
        case slideview::LEFT:
            hAlign = ui::qt::widget::SlideBar::LEFT;
            break;

        case slideview::RIGHT:
            hAlign = ui::qt::widget::SlideBar::RIGHT;
            break;
    }

    switch(m_vAlignment)
    {
        case slideview::TOP:
            vAlign = ui::qt::widget::SlideBar::TOP;
            break;

        case slideview::BOTTOM:
            vAlign = ui::qt::widget::SlideBar::BOTTOM;
            break;
    }

    switch(m_animatableAlignment)
    {
        case slideview::TOP_ANIMATION:
            animAlign = ui::qt::widget::SlideBar::TOP_ANIMATION;
            break;

        case slideview::BOTTOM_ANIMATION:
            animAlign = ui::qt::widget::SlideBar::BOTTOM_ANIMATION;
            break;

        case slideview::LEFT_ANIMATION:
            animAlign = ui::qt::widget::SlideBar::LEFT_ANIMATION;
            break;

        case slideview::RIGHT_ANIMATION:
            animAlign = ui::qt::widget::SlideBar::RIGHT_ANIMATION;
            break;
    }

    auto* slideBar =
        new ui::qt::widget::SlideBar(
            qtParent,
            hAlign,
            vAlign,
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
            animAlign
        );

    if(!m_styleSheet.empty())
    {
        slideBar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    ui::qt::container::widget::sptr qtContainer = ui::qt::container::widget::make();
    qtContainer->setQtContainer(slideBar);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void slideview::destroyContainer()
{
    SIGHT_ASSERT("The Container is not initialized", m_container);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
