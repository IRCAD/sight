/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/container/QtContainer.hpp"

#include <QDockWidget>
#include <QLayout>
#include <QList>
#include <QMetaObject>
#include <QWidget>

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtContainer::QtContainer(::fwGui::GuiBaseObject::Key key) noexcept :
    m_container(0)
{
}

//-----------------------------------------------------------------------------

QtContainer::~QtContainer() noexcept
{
    SLM_ASSERT(
        "Error during destruction : The qt container included in this class is still allocated, please call destroyContainer() before.",
        m_container == nullptr );
}

//-----------------------------------------------------------------------------

void QtContainer::setLayout( QLayout* const layout )
{
    SLM_ASSERT("The container must be initialized before invoking setLayout().", m_container);

    // Recursively delete all children
    QLayout* oldLayout = m_container->layout();

    if(nullptr != oldLayout)
    {
        this->clean();

        // Since we will set a new layout, delete the old one as stated in Qt documentation
        delete oldLayout;
        oldLayout = nullptr;
    }

    // Assign the new layout manager
    m_container->setLayout(layout);
}

//-----------------------------------------------------------------------------

void QtContainer::clean()
{
    SLM_ASSERT("The container must be initialized before invoking clean().", m_container);

    // Recursively delete all children
    QLayout* oldLayout = m_container->layout();

    if(nullptr != oldLayout)
    {
        // This block layouting when there is a lot of child
        m_container->setUpdatesEnabled(false);

        for( QLayoutItem* child = oldLayout->takeAt(0); nullptr != child; child = oldLayout->takeAt(0))
        {
            delete child;
        }

        // Restore update
        m_container->setUpdatesEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void QtContainer::destroyContainer()
{
    SLM_ASSERT("The container must be initialized before invoking destroyContainer().", m_container);

    if(m_container)
    {
        delete m_container;
        m_container.clear();
    }
}

//-----------------------------------------------------------------------------

void QtContainer::setQtContainer(QWidget* container)
{
    m_container = container;
}

//-----------------------------------------------------------------------------

QWidget* QtContainer::getQtContainer()
{
    return m_container;
}

//-----------------------------------------------------------------------------

bool QtContainer::isShownOnScreen()
{
    SLM_ASSERT("The container must be initialized before invoking isShownOnScreen().", m_container);
    return m_container->isVisible();
}

//-----------------------------------------------------------------------------

void QtContainer::setVisible(bool isVisible)
{
    SLM_ASSERT("The container must be initialized before invoking setVisible().", m_container);

    QWidget* parent   = m_container->parentWidget();
    QDockWidget* dock = qobject_cast<QDockWidget*>(parent);

    if(dock && dock->isVisible() != isVisible)
    {
        dock->setVisible(isVisible);
    }

    if(m_container->isVisible() != isVisible)
    {
        m_container->setVisible(isVisible);
    }
}

//-----------------------------------------------------------------------------

void QtContainer::setEnabled(bool isEnabled)
{
    SLM_ASSERT("The container must be initialized before invoking setEnabled().", m_container);

    QWidget* parent   = m_container->parentWidget();
    QDockWidget* dock = qobject_cast<QDockWidget*>(parent);

    if(dock && dock->isEnabled() != isEnabled)
    {
        dock->setEnabled(isEnabled);
    }

    if(m_container->isEnabled() != isEnabled)
    {
        m_container->setEnabled(isEnabled);
    }
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
