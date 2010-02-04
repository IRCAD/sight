/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/DefaultMenu.hpp"


REGISTER_SERVICE( ::gui::aspect::IMenu , ::gui::aspect::DefaultMenu , ::fwTools::Object );

namespace gui
{

namespace aspect
{

DefaultMenu::DefaultMenu() throw()
{}

DefaultMenu::~DefaultMenu() throw()
{}

}

}
