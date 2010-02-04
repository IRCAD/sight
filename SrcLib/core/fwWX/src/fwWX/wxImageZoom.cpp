/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/wxImageZoom.hpp"

#include <sstream>

namespace fwWX
{

void wxImageZoom::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
    	m_dragStartPos = event.GetPosition() ;
    	m_state = 1 ;
    }
    else if ( event.LeftUp() )
    {
    	if ( m_state == 1 )
		{
    		m_Pos = m_Pos + event.GetPosition() - m_dragStartPos ;
    		m_state = 0 ;
			Refresh(true) ;
			Update() ;
		}
    }

}

/*
void wxImageZoom::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
    	m_dragStartPos = event.GetPosition() ;
    }
    else if (event.LeftUp() )
    {
    	if (m_dragImage)
		{
    		m_Pos = m_Pos + event.GetPosition() - m_dragStartPos ;
			m_dragImage->Hide() ;
			m_dragImage->EndDrag();
	    	delete m_dragImage;
			m_dragImage = NULL;

			Refresh(true);
			Update();
		}



    }
    else if (event.Dragging() )
	{
    	if (m_dragImage)
    	{
    		delete m_dragImage ;
			m_dragImage = new wxDragImage( m_bmp ) ;
			m_dragImage->Show();
			m_dragImage->BeginDrag(  ( m_dragStartPos - m_Pos ) , this) ;

			m_dragImage->Move(event.GetPosition());
			m_dragImage->Show();

		}
    	else
		{
			m_dragImage = new wxDragImage( m_bmp ) ;
		}
	}

}
*/

wxImageZoom::wxImageZoom(wxWindow* parent, ::boost::filesystem::path _path_png )
:wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxMAXIMIZE_BOX )
{


	m_compteur = 0 ;
	//m_txt_ctrl_etat = new wxTextCtrl(this, -3, _T("evt detecte"),	wxPoint( 0,50 ),wxDefaultSize) ;
	std::string path = _path_png.native_directory_string() ;
	wxString wx_path = wxString::FromAscii(path.data()) ;
	m_img = wxImage( wx_path, wxBITMAP_TYPE_PNG) ;
	m_bmp = wxBitmap( m_img ) ;

	m_Pos = wxPoint( 20, 20) ;
	//m_txt1 = new wxStaticText(this, wxID_ANY, _T("txt1"), wxPoint(10,10 ), wxSize(80,25)) ;
	this->SetScrollbars(100,100,100,100) ;
	this->EnableScrolling(true, true) ;

	this->Connect(wxEVT_PAINT, wxPaintEventHandler(wxImageZoom::OnPaint)) ;
	//this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxImageZoom::OnMouseUp)) ;
	//this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(wxImageZoom::OnMouseDown)) ;
	this->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxImageZoom::OnMouseWheel)) ;
	//dragImage

	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxImageZoom::OnMouseEvent)) ;
	this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(wxImageZoom::OnMouseEvent)) ;
	this->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxImageZoom::OnMouseEvent)) ;
	this->Connect(wxEVT_MOTION, wxMouseEventHandler(wxImageZoom::OnMouseEvent)) ;

}

void wxImageZoom::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this) ;
	PrepareDC( dc );
	dc.DrawBitmap(m_bmp, m_Pos.x , m_Pos.y, false) ;
}

void wxImageZoom::Scale()
{
	wxImage img_tmp = m_img ;
	int x = img_tmp.GetWidth() ;
	int y = img_tmp.GetHeight() ;
	float scale = 1+ (float)((float)m_compteur/(float)10) ;
	img_tmp.Rescale( x*scale  ,	y*scale )  ; //wxIMAGE_QUALITY_HIGH
	m_bmp = wxBitmap( img_tmp ) ;

	this->SetScrollbars( (m_bmp.GetWidth()/100)+1,(m_bmp.GetHeight()/100)+1,100,100 ) ;
	this->Refresh() ;
}

void wxImageZoom::OnMouseUp(wxMouseEvent& event)
{
	m_compteur++ ;
	Scale() ;
}

void wxImageZoom::OnMouseDown(wxMouseEvent& event)
{
	m_compteur-- ;
	Scale() ;
}

void wxImageZoom::OnMouseWheel(wxMouseEvent& event)
{
	if ( event.GetWheelRotation()>0 )
	{
		m_compteur++ ;
	}
	else
	{
		m_compteur-- ;
	}
	Scale() ;
}


} //end namespace fwWX

