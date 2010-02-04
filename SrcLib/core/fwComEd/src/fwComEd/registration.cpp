/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwData/ProcessObject.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>

#include "fwComEd/ProcessObjectEditor.hpp"
#include "fwComEd/CompositeEditor.hpp"


namespace fwComEd
{

//// Only processobject editor is specialized
REGISTER_SERVICE( fwServices::IEditionService, fwComEd::ProcessObjectEditor, ::fwData::ProcessObject ) ;
REGISTER_SERVICE( fwServices::IEditionService, fwComEd::CompositeEditor, ::fwData::Composite         ) ;




}
