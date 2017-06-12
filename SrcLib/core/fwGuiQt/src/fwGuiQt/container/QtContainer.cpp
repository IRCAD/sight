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

QtContainer::QtContainer(::fwGui::GuiBaseObject::Key key) throw() :
    m_container(0)
{
}

//-----------------------------------------------------------------------------

QtContainer::~QtContainer() throw()
{
    SLM_ASSERT(
        "Error during destruction : The qt container included in this class is still allocated, please call destroyContainer() before.",
        m_container == 0 );
}

//-----------------------------------------------------------------------------

void QtContainer::setLayout( QLayout* const layout )
{
    SLM_ASSERT("The QWidget is not yet initialized, setting a layout is thus impossible", m_container);

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
    SLM_ASSERT("The QWidget is not yet initialized, cleaning is thus impossible", m_container);

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

        // Just to be sure there are no widget child that were not attached to a layout
        //qDeleteAll(m_container->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

        // Restore update
        m_container->setUpdatesEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void QtContainer::destroyContainer()
{
    SLM_ASSERT("The QWidget is not yet initialized", m_container);
    if(m_container)
    {
        m_container->deleteLater();
        m_container = NULL;
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
    SLM_ASSERT("The QtContainer is not yet initialized, cleaning is thus impossible", m_container);
    return m_container->isVisible();
}

//-----------------------------------------------------------------------------

void QtContainer::setVisible(bool isVisible)
{
    SLM_ASSERT("The QtContainer is not yet initialized, cleaning is thus impossible", m_container);
    QWidget* parent   = m_container->parentWidget();
    QDockWidget* dock = qobject_cast<QDockWidget*>(parent);
    if(dock)
    {
        dock->setVisible(isVisible);
    }
    m_container->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void QtContainer::setEnabled(bool isEnabled)
{
    SLM_ASSERT("The QtContainer is not yet initialized, cleaning is thus impossible", m_container);
    QWidget* parent   = m_container->parentWidget();
    QDockWidget* dock = qobject_cast<QDockWidget*>(parent);
    if(dock)
    {
        dock->setEnabled(isEnabled);
    }
    m_container->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
