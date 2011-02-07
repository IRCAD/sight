/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMenuBar>

#include "fwGuiQt/container/QtMenuBarContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtMenuBarContainer::QtMenuBarContainer() throw() : m_menuBar(0)
{}

//-----------------------------------------------------------------------------

QtMenuBarContainer::~QtMenuBarContainer() throw()
{}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::clean()
{
    SLM_ASSERT("Sorry, Qt MenuBar not yet initialized, cleaning impossible", m_menuBar);

    m_menuBar->clear();

    QList<QMenu *> list = m_menuBar->findChildren<QMenu *>();
    foreach (QMenu *m, list)
    {
        m->setParent(0);
        m->deleteLater();
    }
}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, Qt MenuBar not yet initialized", m_menuBar);

    QList<QMenu *> allMenu = m_menuBar->findChildren<QMenu *>();
    OSLM_ASSERT("MenuBar container must be empty ( " << allMenu.count() << " menus).", allMenu.empty());

    m_menuBar->hide();
    m_menuBar->setParent(0);
    m_menuBar->deleteLater();
}

//-----------------------------------------------------------------------------

void QtMenuBarContainer::setQtMenuBar(QMenuBar *menuBar)
{
    this->m_menuBar = menuBar;
}

//-----------------------------------------------------------------------------

QMenuBar* QtMenuBarContainer::getQtMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
