/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_CONNECTEDCOMPONENTPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_CONNECTEDCOMPONENTPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{


/**
 * @brief   This class proposes a dialog box to edit operator parameters.
 * @class   ConnectedComponentParameterEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note    This class is not commented, because it is not used in the open framework tutorial.
 * @todo    ConnectedComponentParameterEditor is not commented.
 */
class FWWX_CLASS_API ConnectedComponentParameterEditor : public wxDialog
{

public :

    FWWX_API ConnectedComponentParameterEditor( wxWindow * parent );

    FWWX_API ~ConnectedComponentParameterEditor();

    FWWX_API int getBackground();

    FWWX_API double getThreshold();

    FWWX_API int getNbComponent();

private :

    void createContent();

    double m_dThreshold;
    long m_iBackground;
    long m_iNbComponent;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_CONNECTEDCOMPONENTPARAMETEREDITOR_HPP_
