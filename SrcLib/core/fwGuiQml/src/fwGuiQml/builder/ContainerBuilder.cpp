/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/builder/ContainerBuilder.hpp"

#include "fwGuiQml/container/QmlContainer.hpp"

#include <fwGui/registry/macros.hpp>

fwGuiRegisterMacro(::fwGui::builder::ContainerBuilder, ::fwGui::builder::IContainerBuilder::REGISTRY_KEY);

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

ContainerBuilder::ContainerBuilder(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ContainerBuilder::~ContainerBuilder()
{
}

//-----------------------------------------------------------------------------

void ContainerBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQml::container::QmlContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QmlContainer", m_parent);

    ::fwGuiQml::container::QmlContainer::sptr qmlContainer = ::fwGuiQml::container::QmlContainer::New();
//    QWidget* widget = new QWidget();
//    qmlContainer->setQmlContainer(widget);

//    QVBoxLayout* layout = new QVBoxLayout();
//    layout->setContentsMargins(0, 0, 0, 0);
//    layout->addWidget(widget);

//    m_parent->setLayout(layout);

    m_container = qmlContainer;
}

//-----------------------------------------------------------------------------

void ContainerBuilder::destroyContainer()
{
    SLM_ASSERT("The Container is not initialized", m_container);
    SLM_ASSERT("The parent container is not a QmlContainer", m_parent);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

void ContainerBuilder::setParent(::fwGui::container::fwContainer::sptr parent)
{
    SLM_ASSERT("The QmlContainer is not yet initialized, cleaning is thus impossible", m_container);
    ::fwGuiQml::container::QmlContainer::sptr parentContainer =
        ::fwGuiQml::container::QmlContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QmlContainer failed", parentContainer);
    ::fwGuiQml::container::QmlContainer::sptr container = ::fwGuiQml::container::QmlContainer::dynamicCast(m_container);
    SLM_ASSERT("dynamicCast fwContainer to QmlContainer failed", container);

//    QWidget* qmlContainer = container->getQmlContainer();
//    SLM_ASSERT("The QmlContainer is not yet initialized", qmlContainer);
//    QWidget* qmlParent = parentContainer->getQmlContainer();
//    SLM_ASSERT("The parent's QmlContainer is not yet initialized", qmlParent);

//    if(qmlParent != m_parent->getQmlContainer() )
//    {
//        if(!qmlParent->layout())
//        {
//            SLM_TRACE("New parent container hasn't layout");
//            QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
//            layout->setContentsMargins(0, 0, 0, 0);
//            qmlParent->setLayout(layout);
//        }
//        qmlParent->layout()->addWidget(qmlContainer);
//        qmlContainer->update();

//        m_parent = parentContainer;
//    }
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui
