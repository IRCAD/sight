/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include "fwGuiQt/container/QtToolBarContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtToolBarContainer::QtToolBarContainer() throw()
{}

//-----------------------------------------------------------------------------

QtToolBarContainer::~QtToolBarContainer() throw()
{}

//-----------------------------------------------------------------------------

void QtToolBarContainer::clean()
{
    SLM_ASSERT("Sorry, Qt toolBar not yet initialized, cleaning impossible", m_toolBar);
    m_toolBar->clear();

    QList<QAction *> listAction = m_toolBar->findChildren<QAction *>();
    foreach (QAction *a, listAction)
    {
        a->setParent(0);
        a->deleteLater();
    }

    QList<QMenu *> listMenu = m_toolBar->findChildren<QMenu *>();
    foreach (QMenu *a, listMenu)
    {
        a->setParent(0);
        a->deleteLater();
    }
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, Qt toolBar not yet initialized, cleaning impossible", m_toolBar);

    QList<QAction *> allActions = m_toolBar->findChildren<QAction *>();
    OSLM_ASSERT("ToolBar container must be empty ( " << allActions.count() << " actions).", allActions.empty());

    QList<QMenu *> allMenus = m_toolBar->findChildren<QMenu *>();
    OSLM_ASSERT("ToolBar container must be empty ( " << allMenus.count() << " menus).", allMenus.empty());

    m_toolBar->deleteLater();
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::setQtToolBar(QToolBar *toolBar)
{
    this->m_toolBar = toolBar;
}

//-----------------------------------------------------------------------------

QToolBar* QtToolBarContainer::getQtToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
