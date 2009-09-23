/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_ERODEPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_ERODEPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief	This class proposes a dialog box to edit operator parameters.
 * @class	ErodeParameterEditor.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @note 	This class is not commented, because it is not used in the open framework tutorial.
 * @todo 	ErodeParameterEditor is not commented.
 */
class FWWX_CLASS_API ErodeParameterEditor : public wxDialog
{

public :

	FWWX_API ErodeParameterEditor( wxWindow * parent, wxString name );

	FWWX_API ~ErodeParameterEditor();

	FWWX_API float getX() ;
	FWWX_API float getY() ;
	FWWX_API float getZ() ;

private :

	wxTextCtrl * textCtrl1 ;
	wxTextCtrl * textCtrl2 ;
	wxTextCtrl * textCtrl3 ;

	long m_x ;
	long m_y ;
	long m_z ;


};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_ERODEPARAMETEREDITOR_HPP_
