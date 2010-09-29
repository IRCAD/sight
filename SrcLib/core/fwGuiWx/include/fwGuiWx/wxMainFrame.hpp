/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_WXDMAINFRAME_HPP_
#define FWGUIWX_WXDMAINFRAME_HPP_

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{

/**
 * @class   wxMainFrame.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
* @todo wxMainFrame is not commented.
 */
class FWGUIWX_CLASS_API wxMainFrame : public wxFrame

{
public:

    FWGUIWX_API wxMainFrame(wxWindow *parent, wxWindowID  id,
            const wxString &title, const wxPoint &pos=wxDefaultPosition,
            const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE,
            const wxString &name=wxFrameNameStr);

    FWGUIWX_API virtual ~wxMainFrame();
    FWGUIWX_API virtual bool Destroy();

    /// The path in the user configuration for wxFrame settings
    static const wxString CONFIG_PATH;
    /// The location configuration key
    static const wxString  LOCATION_CONFIG_KEY;

private:

    void readConfig();
    void writeConfig();

};

}//namespace fwGuiWx

#endif /*FWGUIWX_WXDMAINFRAME_HPP_*/
