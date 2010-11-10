/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QToolBar>
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
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, Qt toolBar not yet initialized, cleaning impossible", m_toolBar);
    OSLM_ASSERT("Container must be empty ( " << m_toolBar->children().count() << " children).", m_toolBar->children().empty());
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
