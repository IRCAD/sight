/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP__
#define __FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP__

#include "fwRuntime/config.hpp"

#include <iterator>
#include <memory>
#include <vector>

namespace fwRuntime
{

namespace io
{
struct BundleDescriptorReader;
} // namespace io

struct ConfigurationElement;

/**
 * @brief   Defines the generic configuration element container class.
 */
struct FWRUNTIME_CLASS_API ConfigurationElementContainer
{
    friend struct BundleDescriptorReader;

    /**
     * @name    Type Definitions
     *
     * @{
     */

    /// Defines the configuration element container type.
    typedef std::vector< std::shared_ptr<ConfigurationElement> > Container;
    ///< Defines the configuration element container iterator type.
    typedef Container::iterator Iterator;

    //@}

    /**
     * @brief   Retrieves the iterator on the first managed configuration
     *          element.
     *
     * @return  an iterator
     */
    FWRUNTIME_API Iterator begin();

    /**
     * @brief   Retrieves the iterator on the end of the configuration element container
     *
     * @return  an iterator
     */
    FWRUNTIME_API Iterator end();

    /**
     * @brief   Returns the configuration element container
     *
     * @return  an configuration element container
     */
    FWRUNTIME_API const Container& getElements() const;

    /**
     * @brief       Retrieves the first configuration corresponding to the specified name.
     *
     * @param[in]   name    a string containing a configuration element name
     *
     * @return      a pointer to a configuration element instance or null if none
     */
    FWRUNTIME_API const std::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string& name )
    const;

    /**
     * @brief       Test if has an configuration corresponding to the specified name.
     *
     * @param[in]   name    a string containing a configuration element name
     *
     * @return      true if the method find at least one configuration
     */
    FWRUNTIME_API bool hasConfigurationElement( const std::string& name ) const;

    /**
     * @brief       Retrieves the configurations corresponding to the specified name.
     *
     * @param[in]   name    a string containing a configuration element name
     *
     * @return      a configuration element container that contains configuration element instances
     */
    FWRUNTIME_API ConfigurationElementContainer findAllConfigurationElement( const std::string& name ) const;

    /**
     * @brief       Return the number of configurations
     * @return      Return the number of configurations
     */
    FWRUNTIME_API size_t size() const;

    protected:

        /**
         * @brief       Adds a new configuration element to the extension.
         *
         * @param[in]   element a shared pointer to the configuration element to add
         */
        FWRUNTIME_API void addConfigurationElement( std::shared_ptr< ConfigurationElement > element );

    private:

        Container m_elements; ///< all configuration elements

};

} // namespace fwRuntime

#endif // __FWRUNTIME_CONFIGURATIONELEMENTCONTAINER_HPP__
