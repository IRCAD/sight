/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/HistogramMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::HistogramMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string HistogramMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

HistogramMsg::HistogramMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

HistogramMsg::~HistogramMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

