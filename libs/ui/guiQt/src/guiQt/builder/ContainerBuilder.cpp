/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "guiQt/builder/ContainerBuilder.hpp"

#include "guiQt/container/QtContainer.hpp"

#include <gui/registry/macros.hpp>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

fwGuiRegisterMacro( ::sight::gui::builder::ContainerBuilder, ::sight::gui::builder::IContainerBuilder::REGISTRY_KEY);

namespace sight::gui
{
namespace builder
{

//-----------------------------------------------------------------------------

ContainerBuilder::ContainerBuilder(gui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ContainerBuilder::~ContainerBuilder()
{
}

//-----------------------------------------------------------------------------

void ContainerBuilder::createContainer( gui::container::fwContainer::sptr parent )
{
    m_parent = guiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    guiQt::container::QtContainer::sptr qtContainer = guiQt::container::QtContainer::New();
    QWidget* widget                                 = new QWidget();
    qtContainer->setQtContainer(widget);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget);

    m_parent->setLayout(layout);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void ContainerBuilder::destroyContainer()
{
    SLM_ASSERT("The Container is not initialized", m_container);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

void ContainerBuilder::setParent(gui::container::fwContainer::sptr parent)
{
    SLM_ASSERT("The QtContainer is not yet initialized, cleaning is thus impossible", m_container);
    guiQt::container::QtContainer::sptr parentContainer = guiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", parentContainer);
    guiQt::container::QtContainer::sptr container = guiQt::container::QtContainer::dynamicCast(m_container);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", container);

    QWidget* qtContainer = container->getQtContainer();
    SLM_ASSERT("The QtContainer is not yet initialized", qtContainer);
    QWidget* qtParent = parentContainer->getQtContainer();
    SLM_ASSERT("The parent's QtContainer is not yet initialized", qtParent);

    if(qtParent != m_parent->getQtContainer() )
    {
        if(!qtParent->layout())
        {
            QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
            layout->setContentsMargins(0, 0, 0, 0);
            qtParent->setLayout(layout);
        }
        qtParent->layout()->addWidget(qtContainer);
        qtContainer->update();

        m_parent = parentContainer;
    }
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace sight::gui
