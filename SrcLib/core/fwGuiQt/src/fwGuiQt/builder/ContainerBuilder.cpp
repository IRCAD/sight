/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/builder/ContainerBuilder.hpp"



REGISTER_BINDING( ::fwGui::builder::IContainerBuilder,
        ::fwGui::builder::ContainerBuilder,
        ::fwGui::builder::IContainerBuilder::RegistryKeyType,
        ::fwGui::builder::IContainerBuilder::REGISTRY_KEY );


namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

ContainerBuilder::ContainerBuilder()
{}

//-----------------------------------------------------------------------------

ContainerBuilder::~ContainerBuilder()
{}

//-----------------------------------------------------------------------------

void ContainerBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("Sorry, the parent container is not a QtContainer", m_parent);

    ::fwGuiQt::container::QtContainer::NewSptr qtContainer;
    QWidget *qtParent = m_parent->getQtContainer();
    QWidget *widget = new QWidget();
    qtContainer->setQtContainer(widget);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    SLM_ASSERT("Sorry, the parent container has already a layout", !qtParent->layout());
    qtParent->setLayout(layout);
    layout->addWidget(widget);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void ContainerBuilder::destroyContainer()
{
    SLM_ASSERT("Sorry, Container not initialized", m_container);
    SLM_ASSERT("Sorry, the parent container is not a QtContainer", m_parent);

    m_container->destroyContainer();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void ContainerBuilder::setParent(::fwGui::container::fwContainer::sptr parent)
{
    SLM_ASSERT("Sorry, QtContainer not yet initialized, cleaning impossible", m_container);
    ::fwGuiQt::container::QtContainer::sptr parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", parentContainer);
    ::fwGuiQt::container::QtContainer::sptr container = ::fwGuiQt::container::QtContainer::dynamicCast(m_container);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", container);

    QWidget *qtContainer = container->getQtContainer();
    SLM_ASSERT("Sorry, QtContainer not yet initialized", qtContainer);
    QWidget *qtParent = parentContainer->getQtContainer();
    SLM_ASSERT("Sorry, parent QtContainer not yet initialized", qtParent);

    if(qtParent != m_parent->getQtContainer() )
    {
        if(!qtParent->layout())
        {
            SLM_TRACE("New parent container hasn't layout");
            QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
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



