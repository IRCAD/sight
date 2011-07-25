/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARL_PLAYER_H
#define _ARL_PLAYER_H
#include <arlcore/Common.h>

#include <string>
#include <fstream>

namespace arlCore
{
    class Player
    {
    /**
     * @class   Player
     * @author  IRCAD (Research and Development Team)
     * @date    04/2008
     * @brief   Abstract class for Player devices
     */
    public:
        //! @brief Constructor
        ARLCORE_API Player( void );

        //! @brief Destructor
        ARLCORE_API ~Player( void );

        //! @return true if the player is ready
        ARLCORE_API bool readyPlayer( void );

        //! @return Number of recording that are read 
        ARLCORE_API unsigned int initPlayer( const std::string &fileName );

        //! @brief Return the complete path and the name of the header file 
        ARLCORE_API bool getPlayerFilename( std::string &fileName );

        //! @brief If true, do it again from the file beginning, once the end has been reached
        ARLCORE_API bool setLoop( bool=true );

        //! @return Return true if the file end has been reached
        ARLCORE_API bool getEOF( void ) const;

    protected:
        virtual bool readHeader( const std::string &fileName )=0;
        virtual unsigned int playNext( void )=0;

        /**
         * @brief A appeler En fin de fichier ou en cas d'anomalie
         * @return Nombre d'enregistrements lus
         */
        ARLCORE_API unsigned int playEnd( void );
        ARLCORE_API unsigned int setEOF( unsigned int n );
        ARLCORE_API bool goAfterHeader( void );

        std::string m_playerDirectory;
        std::string m_playerFullName;
        std::fstream m_playerHandle;

    private:
        long int m_afterHeader;
        bool m_isReady;
        bool m_eof;
        bool m_loop;
    };
} // namespace ARLCORE
#endif // _ARL_PLAYER_H
