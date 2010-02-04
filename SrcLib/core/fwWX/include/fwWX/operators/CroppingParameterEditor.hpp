/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_CROPPINGPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_CROPPINGPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief	This class proposes a dialog box to edit operator parameters.
 * @class	CroppingParameterEditor.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @note 	This class is not commented, because it is not used in the open framework tutorial.
 * @todo 	CroppingParameterEditor is not commented.
 */
class FWWX_CLASS_API CroppingParameterEditor : public wxDialog
{

public :

	FWWX_API CroppingParameterEditor( wxWindow * parent, int max );

	FWWX_API ~CroppingParameterEditor();

	FWWX_API int getMin() ;
	FWWX_API int getMax() ;
	FWWX_API int getAxis() ;

private :

	wxTextCtrl * textCtrl1 ;
	wxTextCtrl * textCtrl2 ;

	long m_min ;
	long m_max ;

	wxRadioBox * m_directionRadioBox ;
	long m_axis ;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_CROPPINGPARAMETEREDITOR_HPP_
