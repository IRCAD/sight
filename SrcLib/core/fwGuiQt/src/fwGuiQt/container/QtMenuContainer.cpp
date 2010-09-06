/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMenu>
#include "fwGuiQt/container/QtMenuContainer.hpp"

namespace fwGuiQt
{
namespace container
{

//-----------------------------------------------------------------------------

QtMenuContainer::QtMenuContainer() throw()
{}

//-----------------------------------------------------------------------------

QtMenuContainer::~QtMenuContainer() throw()
{}

//-----------------------------------------------------------------------------

void QtMenuContainer::clean()
{
    SLM_ASSERT("Sorry, Qt menu not yet initialized, cleaning impossible", m_menu);
}

//-----------------------------------------------------------------------------

void QtMenuContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, Qt menu not yet initialized, cleaning impossible", m_menu);
}

//-----------------------------------------------------------------------------

void QtMenuContainer::setQtMenu(QMenu *menu)
{
    this->m_menu = menu;
}

//-----------------------------------------------------------------------------

QMenu* QtMenuContainer::getQtMenu()
{
    return this->m_menu;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQt
