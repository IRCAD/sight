/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_BOXPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_BOXPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include <fwData/Image.hpp>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       BoxParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        BoxParameterEditor is not commented.
 */
class FWWX_CLASS_API BoxParameterEditor : public wxDialog
{

public :

        FWWX_API BoxParameterEditor( wxWindow * parent, ::fwData::Image::sptr pImage );

        FWWX_API ~BoxParameterEditor();

        FWWX_API int getXMin();

        FWWX_API int getYMin();

        FWWX_API int getZMin();

        FWWX_API int getXMax();

        FWWX_API int getYMax();

        FWWX_API int getZMax();

        FWWX_API bool isInside();

        FWWX_API int getFillValue();

private :

        void createContent();

        long m_xMin;
        long m_yMin;
        long m_zMin;
        long m_xMax;
        long m_yMax;
        long m_zMax;
        bool m_isInside;
        long m_fillValue;

        wxCheckBox * m_checkbox;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_BOXPARAMETEREDITOR_HPP_
