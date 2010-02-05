/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IMAGEURLPANEL_HPP_
#define IMAGEURLPANEL_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace widgets
{

/**
 * @brief	This class is used to show an image file or image url
 * @class	ImageURLPanel.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @note 	This class is not commented, because it is not used in the open framework tutorial.
 * @todo 	ImageURLPanel is not commented.
 */
class FWWX_CLASS_API ImageURLPanel : public wxPanel
{

public:

	FWWX_API ImageURLPanel(wxWindow *parent, const wxBitmap& bitmap);
	FWWX_API void OnPaint(wxPaintEvent& WXUNUSED(event));
	FWWX_API void OnWeb(wxMouseEvent& WXUNUSED(event));
	FWWX_API void OnFocus(wxMouseEvent& WXUNUSED(event));
	FWWX_API void OnEnter(wxMouseEvent& WXUNUSED(event));
	FWWX_API void OnLeave(wxMouseEvent& WXUNUSED(event));

private:

	wxBitmap m_bitmap;

    DECLARE_EVENT_TABLE()
};
}
}
#endif /*IMAGEURLPANEL_HPP_*/
