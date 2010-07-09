/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/accel.h>

#include "fwGuiWx/Shortcut.hpp"

namespace fwGuiWx
{

namespace
{
    /**
     * @brief   Defines the character separator type for the shortcut definition string tokenizer.
     */
    typedef ::boost::char_separator< char >         char_separator;

    /**
     * @brief   Defines the tokenizer that will be used to split a shortcut definition string.
     */
    typedef ::boost::tokenizer< char_separator >    tokenizer;


    /**
     * @name    Modifier & Key Lexial Values
     */
    //@{

    const std::string ALT       ( "Alt"     );  ///< alt modifier
    const std::string CTRL      ( "Ctrl"    );  ///< control modifier
    const std::string SHIFT     ( "Shift"   );  ///< shift modifier

    const std::string UP        ( "Up"      );  ///< up arrow
    const std::string RIGHT     ( "Right"   );  ///< right arrow
    const std::string DOWN      ( "Down"    );  ///< down arrow
    const std::string LEFT      ( "Left"    );  ///< left arrow

    const std::string INSERT    ( "Insert"  );  ///< insert key
    const std::string DEL       ( "Delete"  );  ///< delete key (VC cl7.1 does not support a variable named DELETE, so using DEL instead).
    const std::string HOME      ( "Home"    );  ///< home key
    const std::string END       ( "End"     );  ///< end key
    const std::string RETURN    ( "Return"  );  ///< return key
    const std::string PAGEUP    ( "Pageup"  );  ///< pageup key
    const std::string PAGEDOWN  ( "Pagedown");  ///< page down key

    const std::string F1        ( "F1"      );  ///< F1  key
    const std::string F2        ( "F2"      );  ///< F2  key
    const std::string F3        ( "F3"      );  ///< F3  key
    const std::string F4        ( "F4"      );  ///< F4  key
    const std::string F5        ( "F5"      );  ///< F5  key
    const std::string F6        ( "F6"      );  ///< F6  key
    const std::string F7        ( "F7"      );  ///< F7  key
    const std::string F8        ( "F8"      );  ///< F8  key
    const std::string F9        ( "F9"      );  ///< F9  key
    const std::string F10       ( "F10"     );  ///< F10 key
    const std::string F11       ( "F11"     );  ///< F11 key
    const std::string F12       ( "F12"     );  ///< F12 key

    //@}


    /**
     * @brief   Updates the case of the given string so it conforms to internal
     *          specifications.
     *
     * The case updates consist in having the first character in upper case
     * and all others in lower case.
     *
     * @param   source  a string to transform
     *
     * @return  the string with updated case
     */
    const std::string adaptCase( const std::string source )
    {
        typedef std::pair< std::string::iterator, std::string::iterator > ItPair;

        std::string result( source );

        if( result.length() >= 1 )
        {
            std::string::iterator   begin   ( result.begin()    );
            std::string::iterator   end     ( begin + 1         );
            ItPair                  range   ( begin, end        );

            ::boost::algorithm::to_upper( range );
        }

        if( result.length() >= 2 )
        {

            std::string::iterator   begin   ( result.begin() + 1    );
            std::string::iterator   end     ( result.end()          );
            ItPair                  range   ( begin, end            );

            ::boost::algorithm::to_lower( range );
        }

        return result;
    }

}

//-----------------------------------------------------------------------------

Shortcut::Shortcut( const int modifiers, const int keyCode )
:   m_modifiers ( modifiers     ),
    m_keyCode   ( keyCode       )
{}

//-----------------------------------------------------------------------------

Shortcut::Shortcut( const std::string & definition )
:   m_modifiers ( getModifiersFromString(definition)    ),
    m_keyCode   ( getKeyCodeFromString(definition)      )
{}

//-----------------------------------------------------------------------------

const int Shortcut::getModifiers() const
{
    return m_modifiers;
}

//-----------------------------------------------------------------------------

const int Shortcut::getKeyCode() const
{
    return m_keyCode;
}

//-----------------------------------------------------------------------------

const int Shortcut::getModifiersFromString( const std::string & shortcut )
{
    int modifiers = 0;

    // Walks trough the tokens of the shortcut definition.
    tokenizer   tokens( shortcut, char_separator("+ ") );
    for( tokenizer::iterator i = tokens.begin(); i != tokens.end(); ++i )
    {
        // Retrieves the current token.
        std::string token( adaptCase(*i) );

        // Determines the current token's matching modifier.
        if( token == ALT )
        {
            modifiers |= wxACCEL_ALT;
        }
        else if( token == CTRL )
        {
            modifiers |= wxACCEL_CTRL;
        }
        else if( token == SHIFT )
        {
            modifiers |= wxACCEL_SHIFT;
        }
    }

    return modifiers;
}

//-----------------------------------------------------------------------------

const int Shortcut::getKeyCodeFromString( const std::string & shortcut )
{
    int keyCode = 0;

    // Walks trough the tokens of the shortcut definition.
    tokenizer   tokens( shortcut, char_separator("+ ") );
    for( tokenizer::iterator i = tokens.begin(); i != tokens.end() && keyCode == 0; ++i )
    {
        // Retrieves the current token.
        std::string token( adaptCase(*i) );

        // If the current token is not a modifier, than get its key code.
        if( token != ALT && token != CTRL && token != SHIFT )
        {
            // @todo missing non-ascii key code to be added
            if      ( token == INSERT   )   keyCode = WXK_INSERT;
            else if ( token == DEL      )   keyCode = WXK_DELETE;
            else if ( token == HOME     )   keyCode = WXK_HOME;
            else if ( token == END      )   keyCode = WXK_END;
            else if ( token == RETURN   )   keyCode = WXK_RETURN;
#if wxCHECK_VERSION(2, 9, 0)
            else if ( token == PAGEUP   )   keyCode = WXK_PAGEUP;
            else if ( token == PAGEDOWN )   keyCode = WXK_PAGEDOWN;
#else
            else if ( token == PAGEUP   )   keyCode = WXK_PRIOR;
            else if ( token == PAGEDOWN )   keyCode = WXK_NEXT;
#endif
            else if ( token == UP       )   keyCode = WXK_UP;
            else if ( token == RIGHT    )   keyCode = WXK_RIGHT;
            else if ( token == DOWN     )   keyCode = WXK_DOWN;
            else if ( token == LEFT     )   keyCode = WXK_LEFT;
            else if ( token == F1       )   keyCode = WXK_F1;
            else if ( token == F2       )   keyCode = WXK_F2;
            else if ( token == F3       )   keyCode = WXK_F3;
            else if ( token == F4       )   keyCode = WXK_F4;
            else if ( token == F5       )   keyCode = WXK_F5;
            else if ( token == F6       )   keyCode = WXK_F6;
            else if ( token == F7       )   keyCode = WXK_F7;
            else if ( token == F8       )   keyCode = WXK_F8;
            else if ( token == F9       )   keyCode = WXK_F9;
            else if ( token == F10      )   keyCode = WXK_F10;
            else if ( token == F11      )   keyCode = WXK_F11;
            else if ( token == F12      )   keyCode = WXK_F12;
            else                            keyCode = (int) token[0];
        }
    }

    return keyCode;
}

//-----------------------------------------------------------------------------

const std::string Shortcut::getString( const int modifiers, const int keyCode )
{
    const std::string   modifiersString ( getStringFromModifiers( modifiers )   );
    const std::string   keyCodeString   ( getStringFromKeyCode( keyCode )       );

    if( modifiersString.empty() == false && keyCodeString.empty() == false )
    {
        return modifiersString + "+" + keyCodeString;
    }
    else
    {
        return modifiersString + keyCodeString;
    }
}

//-----------------------------------------------------------------------------

const std::string Shortcut::getStringFromModifiers( const int modifiers )
{
    std::ostringstream  buffer;
    bool                putDelimiter( false );

    if( modifiers & wxACCEL_CTRL )
    {
        buffer << CTRL;
        putDelimiter = true;
    }
    if( modifiers & wxACCEL_ALT )
    {
        if( putDelimiter )
        {
            buffer << std::string("+");
        }
        buffer << ALT;
        putDelimiter = true;
    }
    if( modifiers & wxACCEL_SHIFT )
    {
        if( putDelimiter )
        {
            buffer << std::string("+");
        }
        buffer << SHIFT;
    }

    return buffer.str();
}

//-----------------------------------------------------------------------------

const std::string Shortcut::getStringFromKeyCode( const int keyCode )
{
    std::string result;

    switch( keyCode )
    {
    // @todo missing non-ascii key code to be added
    case WXK_INSERT:    result = INSERT;    break;
    case WXK_DELETE:    result = DEL;       break;
    case WXK_HOME:      result = HOME;      break;
    case WXK_END:       result = END;       break;
    case WXK_RETURN:    result = RETURN;    break;
#if wxCHECK_VERSION(2, 9, 0)
    case WXK_PAGEUP:        result = PAGEUP;    break;
    case WXK_PAGEDOWN:      result = PAGEDOWN;  break;
#else
    case WXK_PRIOR:     result = PAGEUP;    break;
    case WXK_NEXT:      result = PAGEDOWN;  break;
#endif
    case WXK_UP:        result = UP;        break;
    case WXK_RIGHT:     result = RIGHT;     break;
    case WXK_DOWN:      result = DOWN;      break;
    case WXK_LEFT:      result = LEFT;      break;
    case WXK_F1:        result = F1;        break;
    case WXK_F2:        result = F2;        break;
    case WXK_F3:        result = F3;        break;
    case WXK_F4:        result = F4;        break;
    case WXK_F5:        result = F5;        break;
    case WXK_F6:        result = F6;        break;
    case WXK_F7:        result = F7;        break;
    case WXK_F8:        result = F8;        break;
    case WXK_F9:        result = F9;        break;
    case WXK_F10:       result = F10;       break;
    case WXK_F11:       result = F11;       break;
    case WXK_F12:       result = F12;       break;
    default:            result = keyCode;   break;
    }
    return result;
}

//-----------------------------------------------------------------------------

void Shortcut::set( const std::string & definition )
{
    m_modifiers = getModifiersFromString( definition );
    m_keyCode   = getKeyCodeFromString( definition );
}

//-----------------------------------------------------------------------------

Shortcut & Shortcut::operator= ( const std::string & definition )
{
    set( definition );
    return *this;
}

//-----------------------------------------------------------------------------

const std::string Shortcut::toString() const
{
    const std::string   modifiers   ( getStringFromModifiers( m_modifiers ) );
    const std::string   keyCode     ( getStringFromKeyCode(m_keyCode )      );

    if( modifiers.empty() == false && keyCode.empty() == false )
    {
        return modifiers + "+" + keyCode;
    }
    else
    {
        return modifiers + keyCode;
    }
}

//-----------------------------------------------------------------------------

Shortcut::operator std::string () const
{
    return toString();
}

} // namespace fwGuiWx
