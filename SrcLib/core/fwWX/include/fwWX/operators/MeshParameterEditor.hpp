/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_MESHPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_MESHPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief   This class proposes a dialog box to edit operator parameters.
 * @class   MeshParameterEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note    This class is not commented, because it is not used in the open framework tutorial.
 * @todo    MeshParameterEditor is not commented.
 */
class FWWX_CLASS_API MeshParameterEditor : public wxDialog
{

public :

    FWWX_API MeshParameterEditor( wxWindow * parent );

    FWWX_API ~MeshParameterEditor();

    FWWX_API int getValue() ;

    FWWX_API int getMinVoxelSize() ;

    FWWX_API int getMaxVoxelSize() ;

private :

    wxTextCtrl * textCtrl1 ;
    wxTextCtrl * textCtrl2 ;
    wxTextCtrl * textCtrl3 ;

    long m_value;

    long m_minVoxelSize;

    long m_maxVoxelSize;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_MESHPARAMETEREDITOR_HPP_
