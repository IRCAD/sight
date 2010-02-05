/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_ANISOTROPICFILTERPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_ANISOTROPICFILTERPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{


/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       AnisotropicFilterParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        AnisotropicFilterParameterEditor is not commented.
 */
class FWWX_CLASS_API AnisotropicFilterParameterEditor : public wxDialog
{

public :

        FWWX_API AnisotropicFilterParameterEditor( wxWindow * parent );

        FWWX_API ~AnisotropicFilterParameterEditor();

        FWWX_API double getTimeStep();

        FWWX_API int getNbIterations();

        FWWX_API double getConductance();

private :

        void createContent();

        double m_dTimeStep;
        long m_iNbIterations;
        double m_dConductance;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_ANISOTROPICFILTERPARAMETEREDITOR_HPP_
