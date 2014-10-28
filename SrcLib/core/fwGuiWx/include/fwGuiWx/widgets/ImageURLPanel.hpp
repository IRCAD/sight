/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IMAGEURLPANEL_HPP_
#define IMAGEURLPANEL_HPP_

#include <wx/wx.h>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{

namespace widgets
{

/**
 * @brief   This class is used to show an image file or image url
 * @class   ImageURLPanel
 * 
 * @date    2009.
 * @note    This class is not commented, because it is not used in the open framework tutorial.
 * @todo    ImageURLPanel is not commented.
 */
class FWGUIWX_CLASS_API ImageURLPanel : public wxPanel
{

public:

    FWGUIWX_API ImageURLPanel(wxWindow *parent, const wxBitmap& bitmap);
    FWGUIWX_API void OnPaint(wxPaintEvent& WXUNUSED(event));
    FWGUIWX_API void OnWeb(wxMouseEvent& WXUNUSED(event));
    FWGUIWX_API void OnFocus(wxMouseEvent& WXUNUSED(event));
    FWGUIWX_API void OnEnter(wxMouseEvent& WXUNUSED(event));
    FWGUIWX_API void OnLeave(wxMouseEvent& WXUNUSED(event));

private:

    wxBitmap m_bitmap;

    DECLARE_EVENT_TABLE()
};
}
}
#endif /*IMAGEURLPANEL_HPP_*/
