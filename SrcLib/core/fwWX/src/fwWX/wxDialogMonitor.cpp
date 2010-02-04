/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/wxDialogMonitor.hpp"
#include "fwWX/wxMonitor.hpp"

namespace fwWX
{

wxDialogMonitor::wxDialogMonitor(wxWindow* parent, fwServices::monitor::map_object_servicesNames map_string , wxString title )
: wxMiniFrame(parent, wxID_ANY, title , wxDefaultPosition, wxSize(600,700), wxDEFAULT_FRAME_STYLE)
{
	m_monitor = new wxMonitor(this, map_string ) ;
}

wxDialogMonitor::wxDialogMonitor(wxWindow* parent, fwServices::monitor::string_map map_string , wxString title )
: wxMiniFrame(parent, wxID_ANY, title , wxDefaultPosition, wxSize(600,700), wxDEFAULT_FRAME_STYLE)
{
	m_monitor = new wxMonitor(this, map_string ) ;
}

void wxDialogMonitor::setOnSelectionCallback( wxMonitorCallback::ptr callback)
{
    assert(m_monitor);
    m_monitor->setOnSelectionCallback(callback);
}

}//namespace fwWX

