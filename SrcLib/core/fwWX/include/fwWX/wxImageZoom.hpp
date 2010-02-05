/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_WXIMAGEZOOM_HPP_
#define FWWX_WXIMAGEZOOM_HPP_

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/dragimag.h>
#include <boost/filesystem/path.hpp>
#include "fwWX/config.hpp"

namespace fwWX
{

/**
 * @brief       This class returns a wxPanel witch displays an image, and allows to zoom.
 * @class       wxImageZoom.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @todo wxImageZoom is not commented.
 */
class FWWX_CLASS_API wxImageZoom : public ::wxScrolledWindow


{
public:
        /**
        * @brief        Constructor
        *
        * @ param image Image to load in the wxImageZoom.
        */

        wxImage m_img ;
        wxBitmap m_bmp ;

        int m_compteur ;
        wxDragImage * m_dragImage ;
        wxPoint m_dragStartPos ;
        wxPoint m_Pos ;
        int m_dragMode ;
        int m_state ;


        FWWX_API wxImageZoom(wxWindow* parent, ::boost::filesystem::path _path_png ) ;
    FWWX_API void Scale() ;
    FWWX_API void OnPaint(wxPaintEvent& event) ;
    FWWX_API void OnMouseUp(wxMouseEvent& event) ;
    FWWX_API void OnMouseDown(wxMouseEvent& event) ;
    FWWX_API void OnMouseWheel(wxMouseEvent& event) ;
    FWWX_API void OnMouseEvent(wxMouseEvent& event) ;
};

} // end namespace fwWX


#endif /* FWWX_WXIMAGEZOOM_HPP_ */
