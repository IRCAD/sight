/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/builder/ContainerBuilder.hpp"

#include "fwGuiQt/container/QtContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

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
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::New();
    QWidget* widget = new QWidget();
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

void ContainerBuilder::setParent(::fwGui::container::fwContainer::sptr parent)
{
    SLM_ASSERT("The QtContainer is not yet initialized, cleaning is thus impossible", m_container);
    ::fwGuiQt::container::QtContainer::sptr parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", parentContainer);
    ::fwGuiQt::container::QtContainer::sptr container = ::fwGuiQt::container::QtContainer::dynamicCast(m_container);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", container);

    QWidget* qtContainer = container->getQtContainer();
    SLM_ASSERT("The QtContainer is not yet initialized", qtContainer);
    QWidget* qtParent = parentContainer->getQtContainer();
    SLM_ASSERT("The parent's QtContainer is not yet initialized", qtParent);

    if(qtParent != m_parent->getQtContainer() )
    {
        if(!qtParent->layout())
        {
            SLM_TRACE("New parent container hasn't layout");
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
} // namespace fwGui

