/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "fwGuiQt/builder/SlideViewBuilder.hpp"

#include "fwGuiQt/container/QtContainer.hpp"
#include <fwGuiQt/widget/SlideBar.hpp>

#include <fwGui/registry/macros.hpp>

#include <QWidget>

fwGuiRegisterMacro( ::fwGui::builder::SlideViewBuilder, ::fwGui::builder::ISlideViewBuilder::REGISTRY_KEY)

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

SlideViewBuilder::SlideViewBuilder(::fwGui::GuiBaseObject::Key)
{
}

//-----------------------------------------------------------------------------

SlideViewBuilder::~SlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    QWidget* qtParent = m_parent->getQtContainer();

    ::fwGuiQt::widget::SlideBar::HAlignment hAlign             = ::fwGuiQt::widget::SlideBar::LEFT;
    ::fwGuiQt::widget::SlideBar::VAlignment vAlign             = ::fwGuiQt::widget::SlideBar::TOP;
    ::fwGuiQt::widget::SlideBar::AnimatableAlignment animAlign = ::fwGuiQt::widget::SlideBar::TOP_ANIMATION;

    switch(m_hAlignment)
    {
        case ISlideViewBuilder::LEFT:
            hAlign = ::fwGuiQt::widget::SlideBar::LEFT;
            break;
        case ISlideViewBuilder::RIGHT:
            hAlign = ::fwGuiQt::widget::SlideBar::RIGHT;
            break;
    }

    switch(m_vAlignment)
    {
        case ISlideViewBuilder::TOP:
            vAlign = ::fwGuiQt::widget::SlideBar::TOP;
            break;
        case ISlideViewBuilder::BOTTOM:
            vAlign = ::fwGuiQt::widget::SlideBar::BOTTOM;
            break;
    }

    switch(m_animatableAlignment)
    {
        case ISlideViewBuilder::TOP_ANIMATION:
            animAlign = ::fwGuiQt::widget::SlideBar::TOP_ANIMATION;
            break;
        case ISlideViewBuilder::BOTTOM_ANIMATION:
            animAlign = ::fwGuiQt::widget::SlideBar::BOTTOM_ANIMATION;
            break;
        case ISlideViewBuilder::LEFT_ANIMATION:
            animAlign = ::fwGuiQt::widget::SlideBar::LEFT_ANIMATION;
            break;
        case ISlideViewBuilder::RIGHT_ANIMATION:
            animAlign = ::fwGuiQt::widget::SlideBar::RIGHT_ANIMATION;
            break;
    }

    ::fwGuiQt::widget::SlideBar* slideBar
        = new ::fwGuiQt::widget::SlideBar(qtParent, hAlign, vAlign, m_width, m_percentWidth, m_height, m_percentHeight,
                                          m_hOffset, m_percentHOffset, m_vOffset, m_percentVOffset, m_opacity,
                                          m_animatable, animAlign);

    if (!m_styleSheet.empty())
    {
        slideBar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::New();
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
} // namespace fwGui.
