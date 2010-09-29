/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_LOCATION_HPP_
#define _FWWX_LOCATION_HPP_

#include <wx/wx.h>
#include <wx/gdicmn.h>

#include <fwCore/base.hpp>

#include "fwWX/config.hpp"

class wxTopLevelWindow;
class wxWindow;

namespace fwWX
{

/**
 * @brief   Represents the size and position of a window.
 * @class   WindowLocation.
 * @author  IRCAD (Research and Development Team)
 * @date    2009.
 */
class FWWX_CLASS_API WindowLocation : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WindowLocation)(::fwCore::BaseObject),
                ( ((wxWindow*))  ) ( ((wxTopLevelWindow*)) ) ( ((const wxString)) ),
                new WindowLocation) ;

    /**
     * @brief   Constructor
     *
     * @param   window  a pointer to window
     */
    FWWX_API WindowLocation( wxWindow * window );

    /**
     * @brief   Constructor
     *
     * @param   window  a pointer to a top level window
     */
    FWWX_API WindowLocation( wxTopLevelWindow * window );

    /**
     * @brief   Constructor
     *
     * @param   definition  a string containing the window state definition
     *
     * @exception   Failed  the window location could not be build
     *
     * @see     toString
     */
    FWWX_API WindowLocation( const wxString & definition );


    /**
     * @brief   Applies the state on the given window
     *
     * @param   window  a pointer to a window to state will apply to
     */
    FWWX_API void applyOn( wxWindow * window ) const;

    /**
     * @brief   Applies the state on the given top level window
     *
     * @param   topLevelWindow  a pointer to a window to state will apply to
     */
    FWWX_API void applyOn( wxTopLevelWindow * topLevelWindow ) const;

    /**
     * @brief   Represents the window state as a string.
     *
     * @return  a string containing the window state representation
     */
    FWWX_API const wxString toString() const;


private:

    wxPoint m_position; ///< the position
    wxSize  m_size;     ///< the size
    int     m_state;    ///< the state (maximize, minized, full screen)

};


} // namespace fwWX



#endif /*_FWWX_LOCATION_HPP_*/
