/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_WXDMAINFRAME_HPP_
#define FWWX_WXDMAINFRAME_HPP_

#include "fwWX/config.hpp"
#include "fwWX/wxMonitor.hpp"

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>


namespace fwWX
{

/**
 * @brief   This class returns a wxMiniFrame witch contain a wxMonitor(Display Services).
 * @class   wxMainFrame.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
* @todo wxMainFrame is not commented.
 */
class FWWX_CLASS_API wxMainFrame : public wxFrame

{
public:

    FWWX_API wxMainFrame(wxWindow *parent, wxWindowID  id,
            const wxString &title, const wxPoint &pos=wxDefaultPosition,
            const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE,
            const wxString &name=wxFrameNameStr);

    FWWX_API virtual ~wxMainFrame();
    FWWX_API virtual bool Destroy();

    /// The path in the user configuration for wxFrame settings
    static const wxString CONFIG_PATH;
    /// The location configuration key
    static const wxString  LOCATION_CONFIG_KEY;

private:

    void readConfig();
    void writeConfig();

};

}//namespace fwWX

#endif /*FWWX_WXDMAINFRAME_HPP_*/
