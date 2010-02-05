/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/////////////////////////////////////////////////////////////////////////////
// Name:        colourpicker.h
// Purpose:     wxColourPicker header
// Author:      Juan Antonio Ortega
// Created:     2006/06/17
// RCS-ID:      $Id: colourpicker.h,v 1.1 2006/06/27 18:10:54 ja_ortega Exp $
// Copyright:   (c) 2006 Juan Antonio Ortega
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _COLOURPICKER_H_
#define _COLOURPICKER_H_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/popupwin.h>

#include "fwWX/config.hpp"

#define SQUARE_WIDTH    12
#define SQUARE_SEPARATION   6
#define ROWS    5
#define COLUMNS 8

class wxColourPanel;


/**
 * @brief       This class provides a widgets which allows the user to pick a colour from a palette.
 * @class       WindowingParameterEditor.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo This class has to be removed, because it is available in the new wxWidgets release.
 *
 * This class provides a widgets which allows
 * the user to pick a colour from a palette. You can customize several
 * parameters of the palette: thw width of the square colours, the separation
 * between them, the number of rows and the number of columns. In order to
 * change this settings, open wx/colourpicker.h and change the defines that
 * are at the top of the file.
 */
class FWWX_CLASS_API wxColourPicker : public wxBitmapButton {

    wxColourPanel *m_cp;

    wxColour m_colour;

    wxBitmap CreateColourBitmap(const wxColour& c, const wxSize& size, bool enabled);

public:

    /**
     * wxColourPicker Constructor
     * @param parent The parent window
     * @param id The widget's id
     */
        FWWX_API wxColourPicker(wxWindow *parent, int id);

    // Get-Set methods
    /**
     * Sets the colour that is being displayed as selected colour in
     * the picker button. This method sends an event whenever it is called.
     * @param colour The colour to show in the picker
     */
        FWWX_API void SetColour(wxColour& colour);

    /**
     * Return the selected colour
     * @return The selected colour
     */
        FWWX_API wxColour GetColour();

    // Overriden methods
        FWWX_API virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    // Events
        FWWX_API void OnPopup(wxCommandEvent& WXUNUSED(e));

    DECLARE_EVENT_TABLE()
};

/**
 * wxColourPickerEvent class. This event is fired each time the user changes
 * the selected colour in the control. If the user chooses the same colour,
 * then no event is sent. In order to catch the event, you can use the
 * EVT_COLOUR_CHANGED(id, func) macro. This event is also fired when you
 * programatically change the selected colour.
 */
class FWWX_CLASS_API wxColourPickerEvent : public wxCommandEvent {
    wxColour m_colour;
public:
        FWWX_API wxColourPickerEvent(int id, wxColour& colur);

    /**
     * Returns the new selected colour
     * @return The selected colour
     */
        FWWX_API wxColour GetColour();
        FWWX_API wxEvent* Clone() const;
};

//----------------------------------------------------
//  Internal classes
//----------------------------------------------------

class wxColourElement{
public:
    wxColour m_colour;
    wxString m_name;
};

class wxColourPanel : public wxPopupTransientWindow{
    wxBitmap m_back;
    wxColour m_selectedColour;
    wxColour m_overColour;
    wxColourPicker *m_picker;
    bool m_overMore;
    bool m_isFlat, m_osHasDropShadow;
    static const wxColourElement ms_colorTable[ROWS][COLUMNS];

    void GetColourFromPoint(int x, int y, wxColour& colour, bool& more, wxString& name);
    void PaintSquare(wxImage& image, wxImage& maskImage, int x, int y, wxColour col);
public:
    wxColourPanel(wxWindow *parent);

    // Get-Set methods
    void SetColour(wxColour& colour);
    wxColour GetColour();

    // Overriden methods
    virtual void Popup(wxWindow *focus, wxColourPicker *picker);
    virtual void Dismiss();

    // Events
    void OnMouseMotion(wxMouseEvent& e);
    void OnMouseDown(wxMouseEvent& e);
    void OnMouseLeave(wxMouseEvent& e);
    void OnPaint(wxPaintEvent& e);

    DECLARE_EVENT_TABLE()
};

DECLARE_EVENT_TYPE(wxEVT_COMMAND_COLOUR_CHANGED, -1)

typedef void (wxEvtHandler::*wxColourPickerEventFunction)(wxColourPickerEvent&);
#define EVT_COLOUR_CHANGED(id,func)    \
wx__DECLARE_EVT1(wxEVT_COMMAND_COLOUR_CHANGED, id, (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxColourPickerEventFunction, &func))

#endif
