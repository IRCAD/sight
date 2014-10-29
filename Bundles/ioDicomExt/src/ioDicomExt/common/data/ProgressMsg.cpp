/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomExt/common/data/ProgressMsg.hpp"

namespace ioDicomExt
{

namespace common
{

namespace data
{

//------------------------------------------------------------------------------

const std::string ProgressMsg::s_START_PROGRESS_BAR = "START_PROGRESS_BAR";
const std::string ProgressMsg::s_STOP_PROGRESS_BAR = "STOP_PROGRESS_BAR";
const std::string ProgressMsg::s_UPDATE_PROGRESS_BAR = "UPDATE_PROGRESS_BAR";

//------------------------------------------------------------------------------

ProgressMsg::ProgressMsg(::fwServices::ObjectMsg::Key key) throw() :
        m_barId("defaultProgressBar"),
        m_message(""),
        m_percentage(0)
{
}
//------------------------------------------------------------------------------

ProgressMsg::~ProgressMsg() throw()
{
}

//------------------------------------------------------------------------------

} // namespace data
} // namespace common
} // namespace ioDicomExt
