/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwWX/WindowLocation.hpp"

#include <cassert>
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/toplevel.h>
#include <wx/window.h>

#include <fwCore/base.hpp>

namespace fwWX
{

namespace
{
    const wxString  UNKNOWN_LITERAL     ( _("unknown")      );  ///< the unknown state literal
    const wxString  ICONIZED_LITERAL    ( _("iconized")     );  ///< the iconized state literal
    const wxString  MAXIMIZED_LITERAL   ( _("maximized")    );  ///< the maximized state literal
    const wxString  FULL_SCREEN_LITERAL ( _("full screen")  );  ///< the full screen state literal

    /**
     * @brief   Defines all possible window states.
     */
    enum
    {
        UNKNOWN,    ///< the unknown state
        ICONIZED,   ///< the minimized state
        MAXIMIZED,  ///< the maximied state
        FULL_SCREEN ///< the full screen state
    };

    /**
     * @brief   Retrieves the state for the given top level window.
     *
     * @param   topLevelWindow  a pointer to a topLevelWindow
     *
     * @return  an integer containiner the window state
     */
    const int getState( wxTopLevelWindow * topLevelWindow )
    {
        int state( UNKNOWN );

        if( topLevelWindow->IsIconized() )
        {
            state = ICONIZED;
        }
        else if( topLevelWindow->IsMaximized() )
        {
            state = MAXIMIZED;
        }
        else if( topLevelWindow->IsFullScreen() )
        {
            state = FULL_SCREEN;
        }

        return state;
    }
}

//------------------------------------------------------------------------------

WindowLocation::WindowLocation( wxWindow * window )
:   m_position  ( window->GetPosition() ),
    m_size      ( window->GetSize()     ),
    m_state     ( UNKNOWN               )
{}

//------------------------------------------------------------------------------

WindowLocation::WindowLocation( wxTopLevelWindow * window )
:   m_position  ( window->GetPosition() ),
    m_size      ( window->GetSize()     ),
    m_state     ( getState(window)      )
{}

//------------------------------------------------------------------------------

WindowLocation::WindowLocation( const wxString & definition )
:   m_position  ( 0, 0      ),
    m_size      ( 0, 0      ),
    m_state     ( UNKNOWN   )
{
    bool    valid( false );

    // Matches the given window location.
    wxRegEx matcher( _("([0-9]+)x([0-9]+)([+\\-][0-9]+)([+\\-][0-9]+)") );
    bool    matches( matcher.Matches( definition )                      );

    if( matches == true )
    {
        // Retrieves the size.
        const wxString  widthSizeLiteral    ( matcher.GetMatch(definition, 1) );
        const wxString  heightSizeLiteral   ( matcher.GetMatch(definition, 2) );
        long            width;
        long            height;

        widthSizeLiteral.ToLong( &width );
        heightSizeLiteral.ToLong( &height );
        m_size.Set( width, height );

        // Retrieves the position.
        const wxString  xPositionLiteral( matcher.GetMatch(definition, 3)   );
        const wxString  yPositionLiteral( matcher.GetMatch(definition, 4)   );
        long            x;
        long            y;

        xPositionLiteral.ToLong( &x );
        yPositionLiteral.ToLong( &y );
        m_position.x = x;
        m_position.y = y;

        // Job's done.
        valid = true;
    }
    else
    {
        m_position = wxDefaultPosition;
        m_size = wxDefaultSize;
    }

    // Retrieves the state.
    if( valid == false )
    {
        const wxString  lowerDef( definition.Lower() );

        if( lowerDef == ICONIZED_LITERAL )
        {
            m_state = ICONIZED;
            valid = true;
        }
        else if( lowerDef == MAXIMIZED_LITERAL )
        {
            m_state = MAXIMIZED;
            valid = true;
        }
        else if( lowerDef == FULL_SCREEN_LITERAL )
        {
            m_state = FULL_SCREEN;
            valid = true;
        }
        else
        {
            m_state = UNKNOWN;
            valid = false;
        }
    }

    // If not valid, throws something.
    if( valid == false )
    {
        SLM_WARN("Invalid window location string.");
        // default state
        m_state = MAXIMIZED;

        // TODO FIXME
        //throw Failed( "Invalid window location string." );
    }
}

//------------------------------------------------------------------------------

void WindowLocation::applyOn( wxWindow * window ) const
{
    window->Move( m_position );
    window->SetSize( m_size );
}

//------------------------------------------------------------------------------

void WindowLocation::applyOn( wxTopLevelWindow * topLevelWindow ) const
{
    // Updates the window state.
    switch( m_state )
    {
    case ICONIZED:
        topLevelWindow->Iconize();
        break;

    case MAXIMIZED:
        topLevelWindow->Maximize();
        break;

    case FULL_SCREEN:
        topLevelWindow->ShowFullScreen( true );
        break;

    default:
        // Performs the default window location update.
        applyOn( (wxWindow*) topLevelWindow );
    }
}

//------------------------------------------------------------------------------

const wxString WindowLocation::toString() const
{
    wxString    definition;

    if( m_state == UNKNOWN )
    {
        definition = wxString::Format( _("%ix%i%+i%+i"), m_size.GetWidth(), m_size.GetHeight(), m_position.x, m_position.y );
    }
    else
    {
        switch( m_state )
        {
        case ICONIZED:
            definition = ICONIZED_LITERAL;
            break;

        case MAXIMIZED:
            definition = MAXIMIZED_LITERAL;
            break;

        case FULL_SCREEN:
            definition = FULL_SCREEN_LITERAL;
            break;

        case UNKNOWN:
        default:
            SLM_FATAL("Forbidden state");
            break;
        }
    }
    return definition;
}


} // namespace fwWX
