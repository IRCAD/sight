/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QWidget>
#include <QList>
#include <QLayout>

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
{
    SLM_ASSERT( "Error during destruction : The qt container included in this class is still allocated, please call destroyContainer() before.", m_container == 0 );
}

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
    m_container->layout()->deleteLater();
    m_container->setLayout(0);
}

//-----------------------------------------------------------------------------

void QtContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, QWidget not yet initialized", m_container);

    if (! m_container->children().empty())
    {
        QList<QObject *> list = m_container->children();
        QLayout* layout = qobject_cast <QLayout*> (list.at(0));
        OSLM_ASSERT("Container must only contain one layout.", list.count() == 1 && layout);
        OSLM_ASSERT("layout must be empty ( " << layout->count() << " children).", layout->isEmpty ());
    }

//    OSLM_ASSERT("Container must be empty ( " << m_container->children().count() << " children).", m_container->children().empty());

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
