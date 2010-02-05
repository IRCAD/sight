/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_GRADIENTPROPAGATIONPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_GRADIENTPROPAGATIONPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       GradientPropagationParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        GradientPropagationParameterEditor is not commented.
 */
class FWWX_CLASS_API GradientPropagationParameterEditor : public wxDialog
{

public :

        FWWX_API GradientPropagationParameterEditor( wxWindow * parent );

        FWWX_API ~GradientPropagationParameterEditor();

        FWWX_API int getSeedX();
        FWWX_API int getSeedY();
        FWWX_API int getSeedZ();

        FWWX_API int getIntensityMin();
        FWWX_API int getIntensityMax();

        FWWX_API int getColor();

        FWWX_API double getGradientMin();
        FWWX_API double getGradientMax();

private :

        void createContent();

        long m_seedX;
        long m_seedY;
        long m_seedZ;
        long m_intensityMin;
        long m_intensityMax;
        long m_color;
        double m_gradientMin;
        double m_gradientMax;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_GRADIENTPROPAGATIONPARAMETEREDITOR_HPP_
