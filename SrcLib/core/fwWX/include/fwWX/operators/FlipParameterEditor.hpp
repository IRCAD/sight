/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_FLIPPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_FLIPPARAMETEREDITOR_HPP_

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief   This class proposes a dialog box to edit operator parameters.
 * @class   FlipParameterEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note    This class is not commented, because it is not used in the open framework tutorial.
 * @todo    FlipParameterEditor is not commented.
 */
class FWWX_CLASS_API FlipParameterEditor : public wxDialog
{

public :

    FWWX_API FlipParameterEditor( wxWindow * parent );

    FWWX_API ~FlipParameterEditor();

    FWWX_API int getAxis();

private :

    wxRadioBox * m_directionRadioBox ;
};

} //operators
} //fwWX



#endif // _FWWX_OPERATORS_FLIPPARAMETEREDITOR_HPP_
