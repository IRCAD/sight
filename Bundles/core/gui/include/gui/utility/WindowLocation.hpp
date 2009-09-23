/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_UTILITY_LOCATION_HPP_
#define _GUI_UTILITY_LOCATION_HPP_

#include <wx/wx.h>
#include <wx/gdicmn.h>

#include <fwCore/base.hpp>

class wxTopLevelWindow;
class wxWindow;

namespace gui
{

/**
 * @namespace ::gui::utility
 */
namespace utility
{

/**
 * @brief	Represents the size and position of a window.
 * @class	WindowLocation.
 * @author	IRCAD (Research and Development Team)
 * @date	2009.
 */
class WindowLocation : public ::fwCore::BaseObject
{

public:

	fwCoreClassDefinitionsWithFactoryMacro( (WindowLocation)(::fwCore::BaseObject::Baseclass),
				( ((wxWindow*))  ) ( ((wxTopLevelWindow*)) ) ( ((const wxString)) ),
				new WindowLocation) ;

	/**
	 * @brief	Constructor
	 *
	 * @param	window	a pointer to window
	 */
	WindowLocation( wxWindow * window );

	/**
	 * @brief	Constructor
	 *
	 * @param	window	a pointer to a top level window
	 */
	WindowLocation( wxTopLevelWindow * window );

	/**
	 * @brief	Constructor
	 *
	 * @param	definition	a string containing the window state definition
	 *
	 * @exception	Failed	the window location could not be build
	 *
	 * @see		toString
	 */
	WindowLocation( const wxString & definition );


	/**
	 * @brief	Applies the state on the given window
	 *
	 * @param	window	a pointer to a window to state will apply to
	 */
	void applyOn( wxWindow * window ) const;

	/**
	 * @brief	Applies the state on the given top level window
	 *
	 * @param	topLevelWindow	a pointer to a window to state will apply to
	 */
	void applyOn( wxTopLevelWindow * topLevelWindow ) const;

	/**
	 * @brief	Represents the window state as a string.
	 *
	 * @return	a string containing the window state representation
	 */
	const wxString toString() const;


private:

	wxPoint	m_position;	///< the position
	wxSize	m_size;		///< the size
	int		m_state;	///< the state (maximize, minized, full screen)

};



} // namespace utility

} // namespace gui



#endif /*_GUI_UTILITY_LOCATION_HPP_*/
