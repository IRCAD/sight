/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

QtToolBarContainer::QtToolBarContainer(::fwGui::GuiBaseObject::Key key) noexcept
{
}

//-----------------------------------------------------------------------------

QtToolBarContainer::~QtToolBarContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::clean()
{
    SLM_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    m_toolBar->clear();
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::destroyContainer()
{
    SLM_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    delete m_toolBar;
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
