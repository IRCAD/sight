/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_FILE_H
#define _ARLCORE_FILE_H
#include <arlcore/Common.h>

#include <string>
#include <vector>
#include <fstream>

#include <vnl/vnl_vector.h>

namespace arlCore
{
    class File
    {
    /**
     * @class   File
     * @author  IRCAD (Research and Development Team)
     * @date    04/2008, 12/2008
     * @brief   Normalized file
     */
    public:
        //! @brief Constructor : Open the file
        ARLCORE_API File( const std::string &fileName, const std::string &fileType );

        //! @brief Destructor : Close the file
        ARLCORE_API ~File( void );

        //! @brief Close the file
        ARLCORE_API void close( void );

        //! @return Filetype or "" if error
        ARLCORE_API static std::string getFiletype( const std::string &fileName );

        /**
         * @brief Load header : Verify the filetype & return the version
         * @return Version. Error, if 0.0. (cf getError)
         */
        ARLCORE_API double readHeader( void );

        //! @return Is the file correctly opened ?
        ARLCORE_API bool isOK( void );

        //! @return End of File ?
        ARLCORE_API bool getEOF( void );

        //! @brief Rewind at the beginning of the file
        ARLCORE_API bool rewind( void );

        //! @brief Record current position
        ARLCORE_API bool recordCurrentPosition( void );

        //! @brief Rewind at the recorded position
        ARLCORE_API bool rewindRecordedPosition( void );

        //! @return Version number
        ARLCORE_API double getVersion( void );

        //! @return Message of error
        ARLCORE_API std::string getError( void );

        //! @return Full filename (with path)
        ARLCORE_API std::string getFullFilename( void );

        //! @return Just filename (without path)
        ARLCORE_API std::string getFilename( void );

        //! @return Folder
        ARLCORE_API std::string getFolder( void );

        //! @return Extension of file (after dot)
        ARLCORE_API std::string getExtension( void );

        //! @brief Get next token, if it exists
        ARLCORE_API bool getToken( std::string& token );

        //! @brief Get T value
        template <typename T>
        inline bool get( T&v ){m_file>>v;return !getEOF();}

        //! @brief Get std::vector<T>
        template <typename T>
        inline bool get( std::vector<T> &v )
        {
            unsigned int n;
            if(!get(n)) return false;
            else return get(v, n);
        }

        template <typename T>
        inline bool get( std::vector<T> &v, unsigned int n )
        {
            unsigned int i;
            v.resize(n);
            for( i=0 ; i<n ; ++i ) 
                m_file>>v[i];
            return !getEOF();
        }

        //! @brief Get vnl_vector<T>
        template <typename T>
        inline bool get( vnl_vector<T> &v )
        {
            unsigned int n;
            if(!get(n)) return false;
            else return get(v, n);
        }

        template <typename T>
        inline bool get( vnl_vector<T> &v, unsigned int n )
        {
            unsigned int i;
            v.set_size(n);
            for( i=0 ; i<n ; ++i ) 
                m_file>>v[i];
            return !getEOF();
        }

        //! @brief Get string value
        ARLCORE_API std::string getString( void );

        //! @brief Get string value
        ARLCORE_API bool getString( std::string& );

        //! @brief Get path relativly to current path
        ARLCORE_API std::string getPath( void );

        //! @return fstream
        ARLCORE_API std::fstream& getStream( void );

    protected:
        //! @brief Save & print message of error
        ARLCORE_API void setError( const std::string& );

    private:
        bool m_ok;
        std::fstream m_file;
        double m_version;
        std::string m_fileType, m_error;
        std::string m_fullFilename, m_folder, m_filename, m_extension;
        long int m_position;
    };
} // namespace arlCore
#endif // _ARLCORE_FILE_H
