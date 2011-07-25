/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_PARAMETERS_H
#define _ARLCORE_PARAMETERS_H
#include <arlcore/Common.h>

#include <vector>
#include <string>
#include <map>
#include <boost/any.hpp>

namespace arlCore
{
    class Parameters
    {
    /**
     * @class   Parameters
     * @author  IRCAD (Research and Development Team)
     * @date    07/2007
     * @brief   This class should be used with real time application only.
     * The purpose of this class is to access/modify parameters (double or boolean)that are loaded 
     * when an application is launched. It allows to read parameters that
     * can modify the software behaviour. For example, in the case you use a tracking
     * software with N cameras, you could indicate in a file how many cameras
     * you want to use for your tracking. The functions in this class are devoted
     * to help you doing such things. 
     *  
     */
    public:
        //! @brief Constructor
        ARLCORE_API Parameters( const std::string &name="", const std::string &fileName="" );

        //! @brief Desstructor
        ARLCORE_API ~Parameters( void );

        //! @brief Display the list of parameters
        ARLCORE_API bool printParameters( void ) const;

        //! @brief Reload value of parameters from current file
        ARLCORE_API bool reloadParameters( void );

        //! @brief Reload value of parameters from a new file
        ARLCORE_API bool reloadParameters( const std::string &fileName );

        //! @brief Save the list of parameters (Not yet implemented)
        ARLCORE_API bool saveParameters( const std::string &fileName, bool overwrite=true ) const;

        //! @return True if the type of the no parameter is 'type'
        ARLCORE_API bool getType( unsigned int no, const std::type_info &type ) const;

        //! @return True if the type of the named parameter is 'type'
        ARLCORE_API bool getType( const std::string &name, const std::type_info &type ) const;

        //! @return Number of all parameters
        ARLCORE_API unsigned int getNbParameters( void ) const;

        //! @return Number of all bool parameters
        ARLCORE_API unsigned int getNbBoolParameters( void ) const;

        //! @return Number of all double parameters
        ARLCORE_API unsigned int getNbDoubleParameters( void ) const;

        /**
         * @brief Set all parameters with their default values
         * @return Number of parameters set
         */
        ARLCORE_API unsigned int setAllDefault( void );

        /**
         * @brief Set all bool parameters to b
         * @return Number of parameters set
         */
        ARLCORE_API unsigned int setAllBoolParameters( bool b=true );

        /**
         * @brief Set all bool parameters to false
         * @return Number of parameters set
         */
        ARLCORE_API unsigned int resetAllBoolParameters( void );

        /**
         * @brief Set all bool parameters with d
         * @return Number of parameters set
         */
        ARLCORE_API unsigned int setAllDoubleParameters( double d );

        /**
         * @return Value of the parameter no if it's boolean
         * @warning If the parameter no is not boolean, return false
         */
        ARLCORE_API bool getBool( unsigned int no ) const;
        ARLCORE_API bool getBool( const std::string &name ) const;

        /**
         * @return Value of the parameter no if it's a double
         * @warning If the parameter no is not a double, return 0.0
         */
        ARLCORE_API double getDouble( unsigned int no ) const;
        ARLCORE_API double getDouble( const std::string &name ) const;

        /**
         * @brief Set the parameter no to b, if it exists and if it's boolean
         * @return If the parameter was set
         */
        ARLCORE_API bool setBool( unsigned int no, bool b );
        ARLCORE_API bool setBool( const std::string &name, bool b );

        /**
         * @brief Set the parameter no to d, if it exists and if it's a double
         * @return If the parameter was set
         */
        ARLCORE_API bool setDouble( unsigned int no, double d );
        ARLCORE_API bool setDouble( const std::string &name, double d ); 

        //! @return Description of all parameters
        ARLCORE_API std::string getString( void ) const;

    protected:
        //! @brief Used by child classes to initialize their own parameters
        ARLCORE_API unsigned int init( const std::string[], const boost::any[], unsigned int nb );

        //! @brief Used by child classes to initialize their own parameters from a file which name is m_paramFile
        ARLCORE_API unsigned int load( void );

    private:
        //! @brief Clear all tables (i.e. before loading parameters)
        void clear( void );

        //! @brief Index from a name if it's exists
        bool getIndex( const std::string &name, unsigned int &index ) const;

        //! @brief Name of the file of parameters
        std::string m_paramFile;

        //! @brief Name of the list of parameters
        std::string m_name;

        //! @brief Description of each parameter
        std::vector<std::string> m_paramNames;

        //! @brief Values of each parameter
        std::vector<boost::any> m_paramValues;

        //! @brief Default values of each parameter
        std::vector<boost::any> m_paramDefaults;

        //! @brief Default values of each parameter
        std::map< std::string, unsigned int > m_index ;
    };
} // namespace arlCore
#endif // _ARLCORE_PARAMETERS_H
