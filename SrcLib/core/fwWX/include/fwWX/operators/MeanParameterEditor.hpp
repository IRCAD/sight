/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_MEANPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_MEANPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace operators
{


/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       MeanParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        MeanParameterEditor is not commented.
 */
class FWWX_CLASS_API MeanParameterEditor : public wxDialog
{

public :

        FWWX_API MeanParameterEditor( wxWindow * parent, wxString title=_("Mean image") );

        FWWX_API ~MeanParameterEditor();

        FWWX_API float getX() ;
        FWWX_API float getY() ;
        FWWX_API float getZ() ;

private :

        wxTextCtrl * textCtrl1 ;
        wxTextCtrl * textCtrl2 ;
        wxTextCtrl * textCtrl3 ;

        long m_x ;
        long m_y ;
        long m_z ;


};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_MEANPARAMETEREDITOR_HPP_
