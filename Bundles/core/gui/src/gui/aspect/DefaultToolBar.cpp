/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include "gui/aspect/IToolBar.hpp"
#include "gui/aspect/DefaultToolBar.hpp"

REGISTER_SERVICE( ::gui::aspect::IToolBar , ::gui::aspect::DefaultToolBar , ::fwTools::Object ) ;

namespace gui
{

namespace aspect
{

DefaultToolBar::DefaultToolBar() throw()
{}

DefaultToolBar::~DefaultToolBar() throw()
{}

}

}
