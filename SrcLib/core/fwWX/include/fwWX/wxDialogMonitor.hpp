/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_WXDIALOGMONITOR_HPP_
#define FWWX_WXDIALOGMONITOR_HPP_

#include "fwWX/config.hpp"
#include "fwWX/wxMonitor.hpp"

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/minifram.h>

#include <fwServices/monitor/Monitor.hpp>

namespace fwWX
{
/**
 * @brief   This class returns a wxMiniFrame witch contain a wxMonitor(Display Services).
 * @class   wxDialogMonitor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
* @todo wxDialogMonitor is not commented.
 */
class FWWX_CLASS_API wxDialogMonitor : public wxMiniFrame

{
public:
    /**
    * @brief    Constructor
    */
    FWWX_API wxDialogMonitor(wxWindow* parent, fwServices::monitor::map_object_servicesNames map_string, wxString title = _("Objects and Services Database")) ;

    FWWX_API wxDialogMonitor(wxWindow* parent, fwServices::monitor::string_map map_string, wxString title = _("Objects and Services Database")) ;

    void FWWX_API setOnSelectionCallback( wxMonitorCallback::ptr callback);

private:
    wxMonitor *m_monitor;

};

}//namespace fwWX

#endif /*WXDIALOGMONITOR_HPP_*/
