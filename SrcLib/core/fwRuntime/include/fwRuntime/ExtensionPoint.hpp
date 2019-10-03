/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwRuntime/BundleElement.hpp"
#include "fwRuntime/config.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/Runtime.hpp"

#include <fwCore/base.hpp>

#include <boost/filesystem/path.hpp>

#include <algorithm>
#include <iostream>
#include <string>

namespace fwRuntime
{
struct Bundle;

namespace io
{
struct BundleDescriptorReader;
struct Validator;
}
}

namespace fwRuntime
{

/**
 * @brief   Defines the extension point class.
 */
struct FWRUNTIME_CLASS_API ExtensionPoint : public BundleElement
{
    friend struct ::fwRuntime::io::BundleDescriptorReader;

    typedef Extension::Container ConfigurationElementContainer;

    /**
     * @brief   Retrieves all configuration elements contributed by extensions
     *          connected to the extension point instance.
     *
     * @return  a container with all found configuration elements
     */
    const ConfigurationElementContainer getAllConfigurationElements() const
    {
        typedef std::back_insert_iterator< ConfigurationElementContainer >  Inserter;

        ConfigurationElementContainer container;
        Inserter inserter(container);

        getAllConfigurationElements<Inserter>( inserter );

        return container;
    }

    /**
     * @brief       Retrieves all configuration elements contributed by extensions
     *              connected to the extension point instance.
     *
     * @param[out]  output  an output iterator that will be used to store shared
     *              pointer to the found configuration elements
     */
    template<typename OutputIterator>
    void getAllConfigurationElements( OutputIterator& output ) const
    {
        // Retrieves all connected extensions.
        typedef std::vector< std::shared_ptr< Extension > > ExtensionContainer;
        typedef std::back_insert_iterator< ExtensionContainer > Inserter;

        ExtensionContainer extensions;
        Inserter inserter(extensions);

        getAllExtensions( inserter );

        // Walk through the collected extensions to extract configuration elements.
        for( ExtensionContainer::const_iterator i = extensions.begin(); i != extensions.end(); ++i )
        {
            std::shared_ptr< Extension >   extension( *i );
            if ( extension->isEnable() )
            {
                std::copy( extension->begin(), extension->end(), output);
            }
            OSLM_DEBUG_IF("getAllConfigurationElements for point=" <<  extension->getPoint() <<
                          " extension" << extension->getIdentifier() << "extension disabled", !extension->isEnable());
        }
    }

    /**
     * @brief       Retrieves all extensions contributed to the point instance.
     *
     * @param[out]  output  an output iterator that will be used to store shared
     *              pointers to found extensions
     */
    template<typename OutputIterator>
    void getAllExtensions( OutputIterator& output ) const
    {
        Runtime* rntm( Runtime::getDefault() );

        for( Runtime::ExtensionIterator i = rntm->extensionsBegin(); i != rntm->extensionsEnd(); ++i )
        {
            std::shared_ptr< Extension >   extension( *i );
            if( extension->getPoint() == m_id && extension->isEnable() == true
                && extension->validate() == Extension::Valid
                )
            {
                *output = extension;
                ++output;
            }
        }
    }

    /**
     * @brief   Retrieves the extension point identifier.
     *
     * @return  a string containing the extension point identifier
     */
    FWRUNTIME_API const std::string& getIdentifier() const;

    /**
     * @brief   Retrieves the extension validator.
     *
     * @return  a shared pointer to the extension validator, or null when none
     */
    FWRUNTIME_API std::shared_ptr< io::Validator > getExtensionValidator() const;

    protected:

        /**
         * @brief       constructor
         *
         * @param[in]   bundle  a shared pointer to the bundle where the extension
         *                      point is declared
         * @param[in]   id      a string containing the extension point identifier
         * @param[in]   schema  a path to a file containing an XML schema used to
         *                      validate extensions contributed to the point.
         */
        ExtensionPoint(
            const std::shared_ptr<Bundle>       bundle,
            const std::string&              id,
            const boost::filesystem::path&  schema);

    private:

        const std::string m_id;                                     ///< a string containing the extension point
                                                                    // identifier
        const ::boost::filesystem::path m_schema;                   ///< a path to the XML schema used to validate
                                                                    // contributed extensions
        mutable std::shared_ptr< io::Validator >    m_validator;///< a shared pointer to the extension validator

        /**
         * @brief   Assignment operator.
         *
         * @remark  Assignment is forbidden for this class.
         */
        void operator=(const ExtensionPoint&) noexcept;

};

} // namespace fwRuntime
