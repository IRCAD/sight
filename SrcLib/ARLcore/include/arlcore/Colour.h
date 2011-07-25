/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_COLOUR_H
#define _ARLCORE_COLOUR_H
#include <arlcore/Common.h>

#include <string>

#include <arlcore/Object.h>

namespace arlCore
{
    /**
     * @class   Colour
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   RGB colour
     */
    class Colour
    {
    public:
        //! Constant colours
        ARLCORE_API static const Colour Black;
        ARLCORE_API static const Colour Red;
        ARLCORE_API static const Colour Green;
        ARLCORE_API static const Colour Blue;
        ARLCORE_API static const Colour ElectricBlue;
        ARLCORE_API static const Colour Yellow;
        ARLCORE_API static const Colour Violet;
        ARLCORE_API static const Colour White;

        //! @brief Constructor (Default Black)
        ARLCORE_API Colour( unsigned int R=0, unsigned int G=0, unsigned int B=0 );

        //! @brief Copy constructor
        ARLCORE_API Colour( const Colour & );

        //! @brief Destructor
        ARLCORE_API ~Colour( void );

        //! @brief Affectation
        ARLCORE_API Colour& operator=(const Colour&);

        //! @brief Set a new colour
        ARLCORE_API void setColour( const Colour& );

        //! @brief Set a new colour
        ARLCORE_API void setColour( unsigned int R, unsigned int G, unsigned int B );

        //! @brief Set a new colour in function of the n value
        ARLCORE_API void setMapColour( long int n );

        //! @brief Get the current colour
        ARLCORE_API void getColour( unsigned int &R, unsigned int &G, unsigned int &B ) const;

        //! @return Description of the current colour
        ARLCORE_API std::string getString() const;

    private:
        unsigned int m_R, m_G, m_B;
    };
} // namespace arlCore
#endif // _ARLCORE_COLOUR_H
