/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_THRESHOLDPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_THRESHOLDPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace operators
{


/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       ThresholdParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        ThresholdParameterEditor is not commented.
 */
class FWWX_CLASS_API ThresholdParameterEditor : public wxDialog
{

public :

        FWWX_API ThresholdParameterEditor( wxWindow * parent );

        FWWX_API ~ThresholdParameterEditor();

        FWWX_API float getOutsideValue() ;
        FWWX_API float getInsideValue() ;
        FWWX_API float getLowerThreshold() ;
        FWWX_API float getUpperThreshold() ;

private :

        //DECLARE_EVENT_TABLE()

        wxTextCtrl * textCtrl1 ;
        wxTextCtrl * textCtrl2 ;
        wxTextCtrl * textCtrl3 ;
        wxTextCtrl * textCtrl4 ;

        long outsideValue ;
        long insideValue ;
        long lowerThreshold ;
        long upperThreshold ;

        //void onClickOk( wxCommandEvent & event ) ;

};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_THRESHOLDPARAMETEREDITOR_HPP_
