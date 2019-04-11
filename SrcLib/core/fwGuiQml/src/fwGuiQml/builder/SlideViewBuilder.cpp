/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "fwGuiQml/builder/SlideViewBuilder.hpp"

#include "fwGuiQml/container/QmlContainer.hpp"
#include <fwGuiQml/widget/SlideBar.hpp>

#include <fwGui/registry/macros.hpp>

fwGuiRegisterMacro( ::fwGui::builder::SlideViewBuilder, ::fwGui::builder::ISlideViewBuilder::REGISTRY_KEY);

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

SlideViewBuilder::SlideViewBuilder(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

SlideViewBuilder::~SlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQml::container::QmlContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QmlContainer", m_parent);

//    QWidget* qmlParent = m_parent->getQmlContainer();

    ::fwGuiQml::widget::SlideBar::Aligment align = ::fwGuiQml::widget::SlideBar::LEFT;
    switch (m_aligment)
    {
        case ISlideViewBuilder::LEFT:
            align = ::fwGuiQml::widget::SlideBar::LEFT;
            break;
        case ISlideViewBuilder::RIGHT:
            align = ::fwGuiQml::widget::SlideBar::RIGHT;
            break;
        case ISlideViewBuilder::TOP:
            align = ::fwGuiQml::widget::SlideBar::TOP;
            break;
        case ISlideViewBuilder::BOTTOM:
            align = ::fwGuiQml::widget::SlideBar::BOTTOM;
            break;
        default:
            SLM_ASSERT("Alignement not managed", false);
            break;
    }

//    ::fwGuiQml::widget::SlideBar* slideBar = new ::fwGuiQml::widget::SlideBar(qmlParent, align, m_size, m_opacity);
    if (!m_styleSheet.empty())
    {
//        slideBar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    ::fwGuiQml::container::QmlContainer::sptr qmlContainer = ::fwGuiQml::container::QmlContainer::New();
//    qmlContainer->setQmlContainer(slideBar);

    m_container = qmlContainer;
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::destroyContainer()
{
    SLM_ASSERT("The Container is not initialized", m_container);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui
