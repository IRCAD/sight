/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/////////////////////////////////////////////////////////////////////////////
// Name:        colourpicker.cpp
// Purpose:     wxColourPicker source
// Author:      Juan Antonio Ortega
// Created:     2006/06/17
// RCS-ID:      $Id: colourpicker.cpp,v 1.1 2006/06/27 18:10:54 ja_ortega Exp $
// Copyright:   (c) 2006 Juan Antonio Ortega
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <wx/colordlg.h>

#include <fwWX/widgets/colourpicker.hpp>

#ifdef __WXMSW__
    #include <wx/msw/wrapwin.h>
    #ifndef SPI_GETFLATMENU
        #define SPI_GETFLATMENU 0x1022
    #endif
    #ifndef CS_DROPSHADOW
        #define CS_DROPSHADOW   0x00020000
    #endif
    #ifndef SPI_GETDROPSHADOW
        #define SPI_GETDROPSHADOW   0x1024
    #endif
#endif

DEFINE_EVENT_TYPE(wxEVT_COMMAND_COLOUR_CHANGED)

const wxColourElement wxColourPanel::ms_colorTable[ROWS][COLUMNS] = {
    {{wxColour(0, 0, 0), _("Black")},
     {wxColour(153, 51, 0), _("Brown")},
     {wxColour(51, 51, 0), _("Olive drab")},
     {wxColour(0, 51, 0), _("Dark green")},
     {wxColour(0, 51, 102), _("Dark teal")},
     {wxColour(0, 0, 128), _("Dark blue")},
     {wxColour(51, 51, 153), _("Indigo")},
     {wxColour(51, 51, 51), _("Grey 80%")}},

    {{wxColour(128, 0, 0), _("Dark red")},
     {wxColour(255, 102, 0), _("Orange")},
     {wxColour(128, 128, 0), _("Dark yellow")},
     {wxColour(0, 128, 0), _("Green")},
     {wxColour(0, 128, 128), _("Teal")},
     {wxColour(0, 0, 255), _("Blue")},
     {wxColour(102, 102, 153), _("Blue-gray")},
     {wxColour(128, 128, 128), _("Grey 50%")}},

    {{wxColour(255, 0, 0), _("Red")},
     {wxColour(255, 153, 0), _("Light orange")},
     {wxColour(153, 204, 0), _("Lime green")},
     {wxColour(51, 153, 102), _("Sea green")},
     {wxColour(51, 204, 204), _("Aqua")},
     {wxColour(51, 102, 255), _("Light blue")},
     {wxColour(128, 0, 128), _("Violet")},
     {wxColour(152, 153, 153), _("Grey 40%")}},

    {{wxColour(255, 0, 255), _("Magenta")},
     {wxColour(255, 204, 0), _("Gold")},
     {wxColour(255, 255, 0), _("Yellow")},
     {wxColour(0, 255, 0), _("Bright green")},
     {wxColour(0, 255, 255), _("Turquoise")},
     {wxColour(0, 204, 255), _("Sky blue")},
     {wxColour(153, 51, 102), _("Plum")},
     {wxColour(192, 192, 192), _("Grey 25%")}},

    {{wxColour(255, 153, 204), _("Light pink")},
     {wxColour(255, 205, 153), _("Sand")},
     {wxColour(255, 255, 153), _("Light yellow")},
     {wxColour(204, 255, 204), _("Light green")},
     {wxColour(204, 255, 255), _("Light turquoise")},
     {wxColour(153, 204, 255), _("Powder blue")},
     {wxColour(204, 153, 255), _("Lavender")},
     {wxColour(255, 255, 255), _("White")}}
};

//----------------------------------------------------------------
// Class wxColourPicker
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(wxColourPicker, wxBitmapButton)
    EVT_BUTTON(-1, wxColourPicker::OnPopup)
END_EVENT_TABLE()

wxColourPicker::wxColourPicker(wxWindow *parent, int id) : wxBitmapButton(parent, id, CreateColourBitmap(*wxBLACK, wxSize(20, 20), true)){
    m_colour = *wxWHITE;
    m_cp = new wxColourPanel(parent);
    int width = (SQUARE_SEPARATION + SQUARE_WIDTH) * COLUMNS - SQUARE_SEPARATION;
    int height = (SQUARE_SEPARATION + SQUARE_WIDTH) * ROWS + SQUARE_SEPARATION;
    m_cp->SetClientSize(width + SQUARE_SEPARATION * 2, height + 23 + 6);
}

// Get-Set methods

void wxColourPicker::SetColour(wxColour& colour){
    if (m_colour != colour){
        m_colour = colour;
        SetBitmapLabel(CreateColourBitmap(m_colour, GetSize(), true));

        wxColourPickerEvent event(GetId(), m_colour);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
}

wxColour wxColourPicker::GetColour(){
    return m_colour;
}

// Overriden methods

void wxColourPicker::DoSetSize(int x, int y, int width, int height, int sizeFlags){
    wxBitmapButton::DoSetSize(x, y, width, height, sizeFlags);
    SetBitmapLabel(CreateColourBitmap(m_colour, GetSize(), true));
    SetBitmapDisabled(CreateColourBitmap(m_colour, GetSize(), false));
}

// Events

void wxColourPicker::OnPopup(wxCommandEvent& WXUNUSED(e)){
#ifdef __WXMSW__
//    ::PostMessage((HWND) GetHandle(), BM_SETSTATE, TRUE, 0);
#endif
    int x = 0;
    int y = GetSize().y;
    ClientToScreen(&x, &y);
    m_cp->SetColour(m_colour);
    m_cp->SetSize(x, y, -1, -1, wxSIZE_USE_EXISTING);
    m_cp->Popup(::wxGetTopLevelParent(this), this);
}

// Private methods

wxBitmap wxColourPicker::CreateColourBitmap(const wxColour& c, const wxSize& size, bool enabled){
    int x, y, i;
    wxImage image;
    wxColour dark = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    wxColour light = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);

    image.Create(size.x - 10, size.y - 10);

    wxColour maskColour = wxColour(255, 0, 255);
    if (c == maskColour) maskColour = wxColour(192, 192, 192);

    for (x = 0; x < size.x - 10; x++)
        for (y = 0; y < size.y - 10; y++)
            image.SetRGB(x, y, maskColour.Red(), maskColour.Green(), maskColour.Blue());

    if (enabled)
        for (x = 0; x < size.x - 10 - 12; x++)
            for (y = 0; y < size.y - 10; y++){
                wxColour pixcol = c;
                if (x == 0 || x == size.x - 23 || y == 0 || y == size.y - 11)
                    pixcol = *wxBLACK;
                image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
            }

    if (size.x - 19 >= 0){
        for (y = 0; y < size.y - 10; y++){
            if (enabled) image.SetRGB(size.x - 18, y, light.Red(), light.Green(), light.Blue());
            image.SetRGB(size.x - 19, y, dark.Red(), dark.Green(), dark.Blue());
        }
    }

    if (size.x - 16 >= 0){
        wxColour arrowColour = enabled ? *wxBLACK : wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        int width = 5;
        int x = size.x - 16;
        int y = (size.y - 10 - 4) / 2;
        while (width > 0){
            for (i = 0; i < width; i++)
                image.SetRGB(x + i, y, arrowColour.Red(), arrowColour.Green(), arrowColour.Blue());
            width -= 2;
            x++;
            y++;
        }
    }

    wxBitmap res = wxBitmap(image);
    res.SetMask(new wxMask(res, maskColour));
    return res;
}

//----------------------------------------------------------------
// Class wxColourPanel
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(wxColourPanel, wxPopupTransientWindow)
    EVT_PAINT(wxColourPanel::OnPaint)
    EVT_MOTION(wxColourPanel::OnMouseMotion)
    EVT_LEAVE_WINDOW(wxColourPanel::OnMouseLeave)
    EVT_LEFT_UP(wxColourPanel::OnMouseDown)
END_EVENT_TABLE()

wxColourPanel::wxColourPanel(wxWindow *parent) : wxPopupTransientWindow(parent){
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    int x, y;
    wxImage maskImage;
    wxImage image;
    int width = (SQUARE_SEPARATION + SQUARE_WIDTH) * COLUMNS - SQUARE_SEPARATION;
    int height = (SQUARE_SEPARATION + SQUARE_WIDTH) * ROWS - SQUARE_SEPARATION;
    image.Create(width, height);
    maskImage.Create(width, height);

    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            image.SetRGB(x, y, 255, 255, 255);

    int squareStart = SQUARE_WIDTH + SQUARE_SEPARATION;
    for (x = 0; x < COLUMNS; x++)
        for (y = 0; y < ROWS; y++){
            PaintSquare(image, maskImage, x * squareStart, y * squareStart, ms_colorTable[y][x].m_colour);
        }
    m_back = wxBitmap(image);
    m_back.SetMask(new wxMask(wxBitmap(maskImage, 1)));

    m_isFlat = false;
    m_osHasDropShadow = false;
#ifdef __WXMSW__
    int majorVersion, minorVersion;
    int osVersion = ::wxGetOsVersion(&majorVersion, &minorVersion);
    bool isXp = osVersion == wxOS_WINDOWS_NT && (majorVersion > 5 || (majorVersion == 5 && minorVersion >= 1));
    if (isXp){
//        BOOL flatMenu;
//        ::SystemParametersInfo(SPI_GETFLATMENU, 0, &flatMenu, FALSE);
//        m_isFlat = flatMenu != 0;
//        BOOL dropShadow;
//        ::SystemParametersInfo(SPI_GETDROPSHADOW, 0, &dropShadow, FALSE);
//        m_osHasDropShadow = dropShadow != 0;
    }
#endif
    if (!m_isFlat) SetWindowStyle(wxDOUBLE_BORDER);
}

// Get-Set methods

void wxColourPanel::SetColour(wxColour& colour){
    m_selectedColour = colour;
}

wxColour wxColourPanel::GetColour(){
    return m_selectedColour;
}

// Overriden methods

void wxColourPanel::Popup(wxWindow *focus, wxColourPicker *picker){
#ifdef __WXMSW__
    if (m_osHasDropShadow){
//        DWORD style = GetClassLong((HWND) GetHandle(), GCL_STYLE);
//        SetClassLong((HWND) GetHandle(), GCL_STYLE, style | CS_DROPSHADOW);
    }
#endif
    m_picker = picker;
    wxPopupTransientWindow::Popup(focus);
}

void wxColourPanel::Dismiss(){
    SetToolTip(_T(""));
    wxPopupTransientWindow::Dismiss();
#ifdef __WXMSW__
    if (m_osHasDropShadow){
//        DWORD style = GetClassLong((HWND) GetHandle(), GCL_STYLE);
//        SetClassLong((HWND) GetHandle(), GCL_STYLE, style & ~CS_DROPSHADOW);
    }
//    ::PostMessage((HWND) m_picker->GetHandle(), BM_SETSTATE, FALSE, 0);
#endif
}

// Events

void wxColourPanel::OnMouseMotion(wxMouseEvent& e){
    wxColour prevOver = m_overColour;
    bool prevOverMore = m_overMore;
    wxString colourName;
    GetColourFromPoint(e.GetX(), e.GetY(), m_overColour, m_overMore, colourName);

    if (prevOver != m_overColour || m_overMore != prevOverMore){
        SetToolTip(colourName);
        Refresh();
    }
}

void wxColourPanel::OnMouseDown(wxMouseEvent& e){
    bool moreColours;
    wxColour colour;
    wxString colourName;

    GetColourFromPoint(e.GetX(), e.GetY(), colour, moreColours, colourName);
    Dismiss();
    if (moreColours){
        wxColourDialog cd(::wxGetTopLevelParent(m_picker));
        if (cd.ShowModal() == wxID_OK)
            m_selectedColour = cd.GetColourData().GetColour();
    }else if (colour != wxNullColour)
        m_selectedColour = colour;

    m_picker->SetColour(m_selectedColour);
}

void wxColourPanel::OnMouseLeave(wxMouseEvent& WXUNUSED(e)){
    m_overMore = false;
    m_overColour = wxNullColour;
    Refresh();
}

void wxColourPanel::OnPaint(wxPaintEvent& WXUNUSED(e)){
    wxPaintDC dc(this);
    int x, y;
    wxSize size = GetClientSize();
    wxColour colourMenu = wxSystemSettings::GetColour(wxSYS_COLOUR_MENU);
    wxBufferedDC bdc(&dc, size);

    if (m_isFlat)
        bdc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
    else
        bdc.SetPen(wxPen(colourMenu));

    bdc.SetBrush(wxBrush(colourMenu, wxSOLID));
    bdc.Clear();
    bdc.DrawRectangle(0, 0, size.x, size.y);

    int alpha = 48;
    wxColour selection = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour lighterSelection = wxColour(
        (colourMenu.Red() * (255 - alpha) + selection.Red() * alpha) >> 8,
        (colourMenu.Green() * (255 - alpha) + selection.Green() * alpha) >> 8,
        (colourMenu.Blue() * (255 - alpha) + selection.Blue() * alpha) >> 8);

    bool found = false;
    int squareStart = SQUARE_WIDTH + SQUARE_SEPARATION;
    for (x = 0; x < COLUMNS; x++)
        for (y = 0; y < ROWS; y++){
            if (ms_colorTable[y][x].m_colour == m_selectedColour){
                found = true;
                bdc.SetBrush(wxBrush(lighterSelection, wxSOLID));
                bdc.SetPen(wxPen(selection));
                bdc.DrawRectangle(SQUARE_SEPARATION + x * squareStart - SQUARE_SEPARATION / 2,
                                  SQUARE_SEPARATION + y * squareStart - SQUARE_SEPARATION / 2,
                                  squareStart, squareStart);
            }
            if (m_overColour != wxNullColour && ms_colorTable[y][x].m_colour == m_overColour){
                bdc.SetBrush(wxBrush(selection, wxSOLID));
                bdc.SetPen(wxPen(selection));
                bdc.DrawRectangle(SQUARE_SEPARATION + x * squareStart - SQUARE_SEPARATION / 2,
                                  SQUARE_SEPARATION + y * squareStart - SQUARE_SEPARATION / 2,
                                  squareStart, squareStart);
            }
        }

    int paletteHeight = squareStart * ROWS + SQUARE_SEPARATION;
    if (m_overMore || !found){
        if (m_overMore){
            bdc.SetBrush(wxBrush(selection, wxSOLID));
            bdc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        }else{
            bdc.SetBrush(wxBrush(lighterSelection, wxSOLID));
            bdc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
        }
        bdc.SetPen(wxPen(selection));
        bdc.DrawRectangle(6, paletteHeight, size.x - 12, size.y - paletteHeight - 6);
    }else
        bdc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

    bdc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    wxString msgMoreColours = _("More Colours...");
    int w, h;
    bdc.GetTextExtent(msgMoreColours, &w, &h);
    bdc.DrawText(msgMoreColours, (size.x - w) / 2, paletteHeight + (size.y - paletteHeight - 6 - h) / 2);

    bdc.DrawBitmap(m_back, SQUARE_SEPARATION, SQUARE_SEPARATION, true);
}

// Private methods

void wxColourPanel::GetColourFromPoint(int x, int y, wxColour& colour, bool& more, wxString& name){
    int selX = (x - SQUARE_SEPARATION / 2) / (SQUARE_WIDTH + SQUARE_SEPARATION);
    int selY = (y - SQUARE_SEPARATION / 2) / (SQUARE_WIDTH + SQUARE_SEPARATION);
    wxSize size = GetClientSize();

    colour = wxNullColour;
    more = false;
    name = _T("");
    int paletteHeight = (SQUARE_WIDTH + SQUARE_SEPARATION) * ROWS + SQUARE_SEPARATION;
    if (selX >= 0 && selX < COLUMNS && y >= 0 && selY < ROWS){
        name = ms_colorTable[selY][selX].m_name;
        colour = ms_colorTable[selY][selX].m_colour;
    }else if (x >= 6 && x < size.x - 6 && y >= paletteHeight && y < size.y - 6){
        more = true;
        name = _("More colours");
    }
}

void wxColourPanel::PaintSquare(wxImage& image, wxImage& maskImage, int x, int y, wxColour col){
    wxColour dark = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    for (int xx = x; xx < x + SQUARE_WIDTH; xx++)
        for (int yy = y; yy < y + SQUARE_WIDTH; yy++){
            wxColour color = col;
            if (x == xx || xx == x + SQUARE_WIDTH - 1) color = dark;
            if (y == yy || yy == y + SQUARE_WIDTH - 1) color = dark;
            image.SetRGB(xx, yy, color.Red(), color.Green(), color.Blue());
            maskImage.SetRGB(xx, yy, 255, 255, 255);
        }
}

//----------------------------------------------------------------
// Class wxColourPickerEvent
//----------------------------------------------------------------

wxColourPickerEvent::wxColourPickerEvent(int id, wxColour& colour) :
    wxCommandEvent(wxEVT_COMMAND_COLOUR_CHANGED, id)
{
    m_colour = colour;
}

wxColour wxColourPickerEvent::GetColour(){
    return m_colour;
}

wxEvent* wxColourPickerEvent::Clone() const{
    wxColourPickerEvent *event = new wxColourPickerEvent(*this);
    event->m_colour = m_colour;
    return event;
}
