/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_WINDOWINGPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_WINDOWINGPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace operators
{


/**
 * @brief       This class proposes a dialog box to edit operator parameters.
 * @class       WindowingParameterEditor.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        WindowingParameterEditor is not commented.
 */
class FWWX_CLASS_API WindowingParameterEditor : public wxDialog
{

public :

        FWWX_API WindowingParameterEditor( wxWindow * parent );

        FWWX_API ~WindowingParameterEditor();

        FWWX_API int getWindowMinimun();

        FWWX_API int getWindowMaximum();

        FWWX_API int getOutputMinimum();

        FWWX_API int getOutputMaximum();

private :

        void createContent();

        long m_iWindowMinimum;
        long m_iWindowMaximum;
        long m_iOutputMinimum;
        long m_iOutputMaximum;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_WINDOWINGPARAMETEREDITOR_HPP_
