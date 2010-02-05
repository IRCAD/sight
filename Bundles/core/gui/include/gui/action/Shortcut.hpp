/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_SHORTCUT_HPP_
#define _GUI_ACTION_SHORTCUT_HPP_

#include <string>

#include <fwCore/base.hpp>

#include "gui/export.hpp"



namespace gui
{

namespace action
{

/**
 * @brief   Implements a shortcut definition class.
 *
 * @todo    Explain the syntax of a shortcut definition.
 * @todo    References the shortcut definition syntax explanations in member method documentation.
 *
 * @author  IRCAD (Research and Development Team).
 */
class Shortcut : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Shortcut)(::fwCore::BaseObject::Baseclass),
            ( ((const int))((const int ))  ) ( ((const std::string )) ),
            new Shortcut) ;

    /**
     * @name    Constructors
     */
    //@{

    /**
     * @brief   Constructor
     *
     * @param   modifiers   an integer specifying modifiers
     * @param   keyCode     an integer specifying the key code
     */
    GUI_API Shortcut( const int modifiers, const int keyCode );

    /**
     * @brief   Constructor
     *
     * @param   definition  a string containing the shortcur definition that will be parsed to retrieve the modifier part and the key code part
     */
    GUI_API Shortcut( const std::string & definition );

    //@}


    /**
     * @name    Accessors
     */
    //@{

    /**
     * @brief   Retrieves the modifiers.
     *
     * @return  an integer containing the modifiers
     */
    GUI_API const int getModifiers() const;

    /**
     * @brief   Retrieves the key code.
     *
     * @return  an integer containing the key code
     */
    GUI_API const int getKeyCode() const;

    /**
     * @brief   Reassignes the shortcut definition.
     *
     * @param[in]   definition  a string containing the definition
     */
    GUI_API void set( const std::string & definition );

    /**
     * @brief   Assignes a new definition to the shortcut.
     *
     * @param[in]   definition  a definition string
     *
     * @return  a reference to the modified shortcut
     */
    GUI_API Shortcut & operator= ( const std::string & definition );

    /**
     * @brief   Retrieves the shortcut as a definition string
     *
     * @return  a string containing the shortcut definition
     */
    GUI_API const std::string toString() const;

    /**
     * @brief   Retrieves the shortcut as a definition string
     *
     * @return  a string containing the shortcut definition
     */
    GUI_API operator std::string () const;

    //@}


    /**
     * @name    Utilities
     */
    //@{

    /**
     * @brief   Retrieves the modifiers from the given shortcut definition string.
     *
     * @param[in]   shortcut    a string containing a shortcut definition
     *
     * @return  an integer containing the modifiers part
     */
    GUI_API static const int getModifiersFromString( const std::string & shortcut );

    /**
     * @brief   Retrieves the key code from the given shortcut definition string.
     *
     * @param   shortcut[in]    a string containing the shortcut definition
     *
     * @return  an integer containing the key code
     *
     * @todo    Add support all none ascii keys!
     */
    GUI_API static const int getKeyCodeFromString( const std::string & shortcut );

    /**
     * @brief   Retrieves the string representation for the given modifiers and key code.
     *
     * @param[in]   modifiers   an integer containing a combination of modifiers
     * @param[in]   keyCode     an integer containing a key code
     *
     * @return  a string containing the modifier combination and key code representation
     */
    GUI_API static const std::string getString( const int modifiers, const int keyCode );

    /**
     * @brief   Retrieves the string representation of the given modifiers.
     *
     * @param[in]   modifiers   an integer containing a combination of modifiers
     *
     * @return  a string containing the modifier combination representation
     */
    GUI_API static const std::string getStringFromModifiers( const int modifiers );

    /**
     * @brief   Retrieves the string representation of the given key code.
     *
     * @param[in]   keyCode an integer containing a key code
     *
     * @return  a string containing the key code representation
     */
    GUI_API static const std::string getStringFromKeyCode( const int keyCode );

    //@}

private:

    /**
     * @brief   an integer representing the modifiers in the shortcut definition
     *
     * @remark  This any bitwise combination of wxACCEL_ALT, wxACCEL_SHIFT, wxACCEL_CTRL and wxACCEL_NORMAL.
     */
    int         m_modifiers;

    /**
     * @brief   an integer representing the key code part of the shortcut
     */
    int         m_keyCode;
};

} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_SHORTCUT_HPP_*/
