/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wxhtml.h>

#include <fwCore/base.hpp>

#include "fwWX/widgets/ImageURLPanel.hpp"

namespace fwWX
{

namespace widgets
{

//------------------------------------------------------------------------------

ImageURLPanel::ImageURLPanel(wxWindow *parent, const wxBitmap& bitmap) : wxPanel(parent, wxID_ANY), m_bitmap(bitmap)
{
	SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());
	//this->SetBackgroundColour(*wxRED);
}

void ImageURLPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc( this );
	dc.DrawBitmap( m_bitmap, 0, 0, true /* use mask */ );
}

void ImageURLPanel::OnWeb(wxMouseEvent& WXUNUSED(event))
{
	wxLaunchDefaultBrowser(_T("http://www.ircad.fr/"));
}

void ImageURLPanel::OnFocus(wxMouseEvent& WXUNUSED(event))
{
	wxLaunchDefaultBrowser(_T("http://www.ircad.fr/"));
}

void ImageURLPanel::OnEnter(wxMouseEvent& WXUNUSED(event))
{
	SLM_TRACE("ImageURLPanel::OnEnter");
	SetCursor(wxCURSOR_HAND);
}

void ImageURLPanel::OnLeave(wxMouseEvent& WXUNUSED(event))
{
	SLM_TRACE("ImageURLPanel::OnLeave");
	SetCursor(wxCURSOR_ARROW);
}

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ImageURLPanel, wxPanel)
    EVT_PAINT(ImageURLPanel::OnPaint)
    EVT_LEFT_DCLICK(ImageURLPanel::OnWeb)
	EVT_ENTER_WINDOW(ImageURLPanel::OnEnter)
	EVT_LEAVE_WINDOW(ImageURLPanel::OnLeave)
END_EVENT_TABLE()

//------------------------------------------------------------------------------

} // widgets
} // fwWX
