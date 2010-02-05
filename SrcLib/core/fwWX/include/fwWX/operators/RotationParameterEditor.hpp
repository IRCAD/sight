/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS__ROTATIONPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS__ROTATIONPARAMETEREDITOR_HPP_

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace operators
{

/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       RotationParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        RotationParameterEditor is not commented.
 */
class FWWX_CLASS_API RotationParameterEditor : public wxDialog
{

public :

        FWWX_API RotationParameterEditor( wxWindow * parent );

        FWWX_API ~RotationParameterEditor();

        FWWX_API int getRotation();

        FWWX_API int getAxis();

private :

        wxRadioBox * m_directionRadioBox ;
        wxRadioBox *m_rotationRadioBox;
        wxRadioButton *m_p90RadioButton;
        wxRadioButton *m_m90RadioButton;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS__ROTATIONPARAMETEREDITOR_HPP_
