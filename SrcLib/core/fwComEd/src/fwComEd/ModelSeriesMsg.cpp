/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/ModelSeriesMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::ModelSeriesMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ModelSeriesMsg::ADD_RECONSTRUCTION          = "ADD_RECONSTRUCTION";
std::string ModelSeriesMsg::REMOVED_RECONSTRUCTIONS     = "REMOVED_RECONSTRUCTIONS";
std::string ModelSeriesMsg::SHOW_RECONSTRUCTIONS        = "ShowReconstructions";
std::string ModelSeriesMsg::VISIBILITY                  = "VISIBILITY";
std::string ModelSeriesMsg::NEW_RECONSTRUCTION_SELECTED = "NEW_RECONSTRUCTION_SELECTED";

//-----------------------------------------------------------------------------

ModelSeriesMsg::ModelSeriesMsg(::fwServices::ObjectMsg::Key key)
{
}

//-----------------------------------------------------------------------------

ModelSeriesMsg::~ModelSeriesMsg() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

