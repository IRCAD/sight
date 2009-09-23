/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_OPERATORS_LABELINGPARAMETEREDITOR_HPP_
#define _FWWX_OPERATORS_LABELINGPARAMETEREDITOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
namespace operators
{

/**
 * @brief	This class proposes a dialog box to edit operator parameters.
 * @class	LabelingParameterEditor.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @note 	This class is not commented, because it is not used in the open framework tutorial.
 * @todo 	LabelingParameterEditor is not commented.
 */
class FWWX_CLASS_API LabelingParameterEditor : public wxDialog
{

public :

	FWWX_API LabelingParameterEditor( wxWindow * parent );

	FWWX_API ~LabelingParameterEditor();

	FWWX_API int getBackground();

	FWWX_API double getThreshold();

private :

	void createContent();

	double m_dThreshold;
	long m_iBackground;
};

} // operators
} // fwWX



#endif // _FWWX_OPERATORS_LABELINGPARAMETEREDITOR_HPP_
