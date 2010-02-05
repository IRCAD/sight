/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS__SUBSAMPLINGPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS__SUBSAMPLINGPARAMETEREDITOR_HPP_

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace operators
{

/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       SubSamplingParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        SubSamplingParameterEditor is not commented.
 */
class FWWX_CLASS_API SubSamplingParameterEditor : public wxDialog
{

public :

        FWWX_API SubSamplingParameterEditor( wxWindow * parent );

        FWWX_API ~SubSamplingParameterEditor();

        FWWX_API int getFrequence();

        FWWX_API int getDirection();

private :

        wxRadioBox * m_directionRadioBox ;
        long m_frequence ; //frequence, (en fait l'inverse de la fr�quence)
        int m_direction ;       //1=X, 2=Y, 3=Z.
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS__SUBSAMPLINGPARAMETEREDITOR_HPP_
