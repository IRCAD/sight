/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP_
#define _FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP_

#include <iterator>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"


namespace fwRuntime
{

namespace io
{
        struct BundleDescriptorReader;
} // namespace io


struct ConfigurationElement;


/**
 * @brief       Defines the generic configuration element container class.
 * @struct      ConfigurationElementContainer
 * @date        2004-2009
 * @author      IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API ConfigurationElementContainer
{
        friend struct BundleDescriptorReader;


        /**
         * @name        Type Definitions
         *
         * @{
         */

        typedef std::vector< ::boost::shared_ptr<ConfigurationElement> >        Container;      ///< Defines the configuration element container type.
        typedef Container::iterator                                                                                     Iterator;       ///< Defines the configuration element container iterator type.

        //@}


        /**
         * @brief       Retrieves the iterator on the first managed configuration
         *                      element.
         *
         * @return      an iterator
         */
        FWRUNTIME_API Iterator begin();

        /**
         * @brief       Retrieves the iterator on the end of the configuration element container
         *
         * @return      an iterator
         */
        FWRUNTIME_API Iterator end();

        /**
         * @brief               Retrieves the first configuration corresponding to the specified name.
         *
         * @param[in]   name    a string containing a configuration element name
         *
         * @return              a pointer to a configuration element instance or null if none
         */
        FWRUNTIME_API const ::boost::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string & name ) const;


        /**
         * @brief               Retrieves the configurations corresponding to the specified name.
         *
         * @param[in]   name    a string containing a configuration element name
         *
         * @return              a configuration element container that contains configuration element instances
         */
        FWRUNTIME_API ConfigurationElementContainer findAllConfigurationElement( const std::string & name ) const;

        /**
         * @brief               Return the number of configurations
         * @return              Return the number of configurations
         */
        FWRUNTIME_API unsigned int size() const;

protected:

        /**
         * @brief               Adds a new configuration element to the extension.
         *
         * @param[in]   element a shared pointer to the configuration element to add
         */
        FWRUNTIME_API void addConfigurationElement( ::boost::shared_ptr< ConfigurationElement > element );


private:

        Container m_elements;   ///< all configuration elements

};


} // namespace fwRuntime


#endif // #define _FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP_
