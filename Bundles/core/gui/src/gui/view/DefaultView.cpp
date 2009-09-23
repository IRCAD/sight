/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwData/Object.hpp>

#include "gui/view/DefaultView.hpp"

namespace gui
{
namespace view
{

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::DefaultView , ::fwTools::Object ) ;

DefaultView::DefaultView() throw()
{}

DefaultView::~DefaultView() throw()
{}


}
}

