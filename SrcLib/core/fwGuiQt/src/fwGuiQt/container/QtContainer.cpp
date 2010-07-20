/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QWidget>
#include <QList>

#include "fwGuiQt/container/QtContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtContainer::QtContainer() throw() : m_container(0)
{}

//-----------------------------------------------------------------------------

QtContainer::~QtContainer() throw()
{}

//-----------------------------------------------------------------------------

void QtContainer::clean()
{
    SLM_ASSERT("Sorry, QWidget not yet initialized, cleaning impossible", m_container);

    QObjectList list = m_container->children();
    QWidget *w = NULL;

    foreach (QObject *o, list)
    {
        w = qobject_cast<QWidget*>(o);
        if (w)
        {
            w->hide();
            w->setParent(0);
            w->deleteLater();
        }
    }

}

//-----------------------------------------------------------------------------

void QtContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, QWidget not yet initialized", m_container);

    m_container->deleteLater();
    m_container = NULL;
}

//-----------------------------------------------------------------------------

void QtContainer::setQtContainer(QWidget *container)
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
    SLM_ASSERT("Sorry, wxWindows not yet initialized, cleaning impossible", m_container);
    return m_container->isVisible();
}

} // namespace container
} // namespace fwGuiQt
