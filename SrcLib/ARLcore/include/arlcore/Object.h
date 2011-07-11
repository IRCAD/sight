/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_OBJECT_H
#define _ARLCORE_OBJECT_H
#include <arlcore/Common.h>

#include <sstream>
#include <string>
#include <iostream>
#include <vector>

namespace arlCore
{
    //! @brief All types of classes
    enum ARLCORE_CLASS { ARLCORE_CLASS_OBJECT, ARLCORE_CLASS_PLANESYSTEM, ARLCORE_CLASS_CAMERA, ARLCORE_CLASS_TRACKER,
                ARLCORE_CLASS_POINT, ARLCORE_CLASS_POINTLIST, ARLCORE_CLASS_TAG, ARLCORE_CLASS_TAGS,
                ARLCORE_CLASS_MATRIXR, ARLCORE_CLASS_SCENE,
                ARL_CLASS_SEQUENCE, ARL_CLASS_XML, ARL_CLASS_TOOL, ARL_CLASS_ROBOT, ARL_CLASS_IMAGE,
                ARL_CLASS_VIDEO, ARL_CLASS_SERVER, ARL_CLASS_AURORA, ARL_CLASS_PILINE,
                ARL3D_VTKACTOR,
                ARLCORE_CLASS_NBTYPES };

    static std::string ARLCORE_CLASS_NAMES[ARLCORE_CLASS_NBTYPES]={ "OBJECT", "PLANESYSTEM", "CAMERA", "TRACKER",
                "POINT", "POINTLIST", "TAG", "TAGS", "MATRIXR", "SCENE",
                "SEQUENCE", "XML", "TOOL", "ROBOT", "IMAGE", "VIDEO", "SERVER", "AURORA" };

    //! @brief Avalaible levels of verbosity
    enum ARLCORE_LOG_VERBOSE { ARLCORE_LOG_MUTE, ARLCORE_LOG_DISCREET, ARL_LOG_REGULAR, ARLCORE_LOG_GARRULOUS, ARLCORE_LOG_UNQUENCHABLE, ARLCORE_LOG_VERBOSE_NBTYPES };

    const unsigned int FIRSTUPDATEINDEX = 1;
    /**
     * @class   Object
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Classe de base
     * Contient des informations temporelles, de validité de l'objet, de Mutex
     */
    class Object
    {
    public:
        //! @brief Constructor
        ARLCORE_API Object( ARLCORE_CLASS objectClass=ARLCORE_CLASS_OBJECT, const std::string &objectName="" );

        //! @brief Constructor by copy
        ARLCORE_API Object( const Object& );

        //! @brief Affectation
        ARLCORE_API Object& operator=( const Object& );

        //! @brief Destructor
        ARLCORE_API virtual ~Object( void );

        //! @return The type of the current object
        ARLCORE_API ARLCORE_CLASS getType( void ) const;

        //! @return Description of the current object
        ARLCORE_API virtual std::string getString( void ) const;

        //! @brief Display the description of the current object
        ARLCORE_API bool print( void ) const;

        //! @brief Save the current object
        ARLCORE_API virtual bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Load the current object
        ARLCORE_API virtual bool load( const std::string &fileName );

        //! @brief Save the current object with a default name
        ARLCORE_API bool save( void ) const;

        //! @brief Serialize the current object
        ARLCORE_API std::string virtual serialize( void ) const;

        //! @brief Unserialize the current object
        ARLCORE_API bool unserialize( const std::string & );

        //! @return The name of the current object
        ARLCORE_API std::string getName( void ) const;

        //! @return Is the object OK ?
        ARLCORE_API bool isOK( void ) const;

        //! @return Number of object of the current class
        ARLCORE_API unsigned int getCounter( void ) const;

        //! @return Numero of the object in order of creation [1,..[
        ARLCORE_API unsigned int getNo( void ) const;
        
        //! @return Date and Timestamp of the last modification of the current object
        //! @todo Replace long int date and time by a appropriate structure of time
        ARLCORE_API bool getTime( long int &date, long int &time ) const;
        ARLCORE_API long int getDate( void ) const;
        ARLCORE_API long int getTime( void ) const;
        //ARLCORE_API pt::datetime getTime( void ) const;

        //! @brief Initialize chronometer
        ARLCORE_API bool startLap( void );

        //! @return lap in seconds et initialize chronometer
        ARLCORE_API double getLap( void );

        //! @return number of iterations per second on the base of time of the last iteration
        ARLCORE_API double getItPerSec( void );

        //! @return If the date is not the same as the object one
        ARLCORE_API bool isChanged( const long int &date, const long int &time ) const;

        //! @return The update index which increment every update of the object
        ARLCORE_API long int getUpdateIndex( void ) const;

        //! @return The date is the same as the object one or the object date is invariable : (0,0)
        ARLCORE_API bool isEquivalent( const long int &date, const long int &time ) const;

        //ARLCORE_API bool isEquivalent( const long int &time ) const;
        // Protected ??
        ARLCORE_API bool setTime( void ); // Now
        ARLCORE_API bool setTime( const long int &date, const long int &time );
        ARLCORE_API bool setTime( const long int &time );

        /**
         * @brief Set object's time with the greater value
         * @return true if the object time has changed
         */
        ARLCORE_API bool setMaxTime( long int date, long int time );

        /**
         * @brief Set object's time with the lesser value
         * @return true if the object time has changed
         */
        ARLCORE_API bool setMinTime( long int date, long int time );

        //! @return A default and unique filename for the current object
        ARLCORE_API std::string getFileName( void )const;

        //! @brief Setup 'updateIndex' if an extern modification occurs
        ARLCORE_API void update( void );

        //! @brief Set the level of verbosity for the current object
        ARLCORE_API void setVerboseLevel( ARLCORE_LOG_VERBOSE );

        //! @brief Get the level of verbosity of the current object
        ARLCORE_LOG_VERBOSE getVerboseLevel( void );

        /**
         * @brief Set the level of verbosity for all objects of the class
         * If it's set, it takes precedence over object's level
         */
        ARLCORE_API static void setStaticVerboseLevel( ARLCORE_LOG_VERBOSE );

        //! @brief Get the global level of verbosity of the class
        ARLCORE_API static ARLCORE_LOG_VERBOSE getStaticVerboseLevel( void );

        /**
         * @brief Reset the global level of verbosity for the class
         * The local verbosity will take again the precedence
         */
        ARLCORE_API static void resetStaticVerboseLevel( void );

        //! @brief Change the object's name
        ARLCORE_API void setName( const std::string& );

        ARLCORE_API bool isWriteLocked( void ) const;
        ARLCORE_API bool isReadLocked( void ) const;

        ARLCORE_API unsigned int lockReadMutex( void );
        ARLCORE_API unsigned int unlockReadMutex( void );

    protected:
        ARLCORE_API virtual bool unserialize( std::stringstream &s );

        //! @brief Avalaible levels of seriousness for log messages
        ARLCORE_API enum ARLCORE_LOG_SERIOUSNESS { ARLCORE_LOG_ERROR, ARLCORE_LOG_WARNING, ARLCORE_LOG_INFO_LEVEL2, ARLCORE_LOG_INFO_LEVEL1, ARLCORE_LOG_SERIOUSNESS_NBTYPES };
        std::stringstream m_log;

        //! @brief Display log information about the current object
        ARLCORE_API void log( ARLCORE_LOG_SERIOUSNESS, const std::stringstream& ) const;
        ARLCORE_API void log( ARLCORE_LOG_SERIOUSNESS, const std::string& ) const;
        ARLCORE_API void log( ARLCORE_LOG_SERIOUSNESS, const char* ) const;
        ARLCORE_API void log( ARLCORE_LOG_SERIOUSNESS );

        ARLCORE_API bool setOK( bool b=true );

        // Date et time of the last modification of the object
        //pt:datetime m_time;
        long int m_date, m_time;
        
        ARLCORE_API bool lockWriteMutex( void );
        ARLCORE_API bool unlockWriteMutex( void );

        //ARLCORE_API void setUpdateIndex( long int );

    private:
        //! @brief Private copy
        void copy( const Object& o );

        //! @brief Class type of the object
        ARLCORE_CLASS m_class;

        //! @brief Name of the object
        std::string m_name;

        //! @brief Validity of the object
        bool m_ok;

        //! @brief Number of object for each class
        static unsigned int m_counter[ARLCORE_CLASS_NBTYPES];

        unsigned int m_no;
        //datetime m_lapTime;
        long int m_lapTime;
        long int m_lapDate;

        ARLCORE_LOG_VERBOSE m_verboseLevel;
        static ARLCORE_LOG_VERBOSE m_staticVerboseLevel;

        bool m_writeMutex;
        int m_readMutex;
        long int m_updateIndex;
    };
} // namespace arlCore
#endif // _ARLCORE_OBJECT_H
