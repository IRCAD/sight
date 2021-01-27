/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "guiQt/builder/SlideViewBuilder.hpp"

#include "guiQt/container/QtContainer.hpp"
#include <guiQt/widget/SlideBar.hpp>

#include <gui/registry/macros.hpp>

#include <QWidget>

fwGuiRegisterMacro( ::sight::gui::builder::SlideViewBuilder, ::sight::gui::builder::ISlideViewBuilder::REGISTRY_KEY)

namespace sight::gui
{
namespace builder
{

//-----------------------------------------------------------------------------

SlideViewBuilder::SlideViewBuilder(gui::GuiBaseObject::Key)
{
}

//-----------------------------------------------------------------------------

SlideViewBuilder::~SlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::createContainer( gui::container::fwContainer::sptr parent )
{
    m_parent = guiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    QWidget* qtParent = m_parent->getQtContainer();

    guiQt::widget::SlideBar::HAlignment hAlign             = guiQt::widget::SlideBar::LEFT;
    guiQt::widget::SlideBar::VAlignment vAlign             = guiQt::widget::SlideBar::TOP;
    guiQt::widget::SlideBar::AnimatableAlignment animAlign = guiQt::widget::SlideBar::TOP_ANIMATION;

    switch(m_hAlignment)
    {
        case ISlideViewBuilder::LEFT:
            hAlign = guiQt::widget::SlideBar::LEFT;
            break;
        case ISlideViewBuilder::RIGHT:
            hAlign = guiQt::widget::SlideBar::RIGHT;
            break;
    }

    switch(m_vAlignment)
    {
        case ISlideViewBuilder::TOP:
            vAlign = guiQt::widget::SlideBar::TOP;
            break;
        case ISlideViewBuilder::BOTTOM:
            vAlign = guiQt::widget::SlideBar::BOTTOM;
            break;
    }

    switch(m_animatableAlignment)
    {
        case ISlideViewBuilder::TOP_ANIMATION:
            animAlign = guiQt::widget::SlideBar::TOP_ANIMATION;
            break;
        case ISlideViewBuilder::BOTTOM_ANIMATION:
            animAlign = guiQt::widget::SlideBar::BOTTOM_ANIMATION;
            break;
        case ISlideViewBuilder::LEFT_ANIMATION:
            animAlign = guiQt::widget::SlideBar::LEFT_ANIMATION;
            break;
        case ISlideViewBuilder::RIGHT_ANIMATION:
            animAlign = guiQt::widget::SlideBar::RIGHT_ANIMATION;
            break;
    }

    guiQt::widget::SlideBar* slideBar
        = new guiQt::widget::SlideBar(qtParent, hAlign, vAlign, m_width, m_percentWidth, m_height, m_percentHeight,
                                      m_hOffset, m_percentHOffset, m_vOffset, m_percentVOffset, m_opacity,
                                      m_animatable, animAlign);

    if (!m_styleSheet.empty())
    {
        slideBar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    guiQt::container::QtContainer::sptr qtContainer = guiQt::container::QtContainer::New();
    qtContainer->setQtContainer(slideBar);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::destroyContainer()
{
    SLM_ASSERT("The Container is not initialized", m_container);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder.
} // namespace sight::gui.
