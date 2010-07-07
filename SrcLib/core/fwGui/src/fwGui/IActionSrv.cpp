/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IActionSrv.hpp"

namespace fwGui
{

IActionSrv::IActionSrv()
{}

//-----------------------------------------------------------------------------

IActionSrv::~IActionSrv()
{}

//-----------------------------------------------------------------------------

void IActionSrv::initialize()
{
    m_registrar = ::fwGui::registrar::ActionRegistrar::NewSptr(this->getUUID());
}


//-----------------------------------------------------------------------------

}
