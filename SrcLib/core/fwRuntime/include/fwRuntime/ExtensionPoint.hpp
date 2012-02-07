/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXTENSIONPOINT_HPP
#define _FWRUNTIME_EXTENSIONPOINT_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwCore/base.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/BundleElement.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/Runtime.hpp"

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
 * @struct  ExtensionPoint
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct ExtensionPoint : public BundleElement
{
    friend struct ::fwRuntime::io::BundleDescriptorReader;

    typedef Extension::Container    ConfigurationElementContainer;

    /**
     * @brief   Retrieves all configuration elements contributed by extensions
     *          connected to the extension point instance.
     *
     * @return  a container with all found configuration elements
     */
    const ConfigurationElementContainer getAllConfigurationElements() const
    {
        typedef std::back_insert_iterator< ConfigurationElementContainer >  Inserter;

        ConfigurationElementContainer   container;
        Inserter                        inserter(container);

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
    void getAllConfigurationElements( OutputIterator & output ) const
    {
        // Retrieves all connected extensions.
        typedef std::vector< ::boost::shared_ptr< Extension > > ExtensionContainer;
        typedef std::back_insert_iterator< ExtensionContainer > Inserter;

        ExtensionContainer  extensions;
        Inserter            inserter(extensions);

        getAllExtensions( inserter );

        // Walk through the collected extensions to extract configuration elements.
        for( ExtensionContainer::const_iterator i = extensions.begin(); i != extensions.end(); ++i )
        {
            ::boost::shared_ptr< Extension >   extension( *i );
            if ( extension->isEnable() )
            {
                std::copy( extension->begin(), extension->end(), output);
            }
            OSLM_DEBUG_IF("getAllConfigurationElements for point=" <<  extension->getPoint() <<
                    " extension" << extension->getIdentifier() << "extension disabled", !extension->isEnable())
        }
    }

    /**
     * @brief       Retrieves all extensions contributed to the point instance.
     *
     * @param[out]  output  an output iterator that will be used to store shared
     *              pointers to found extensions
     */
    template<typename OutputIterator>
    void getAllExtensions( OutputIterator & output ) const
    {
        Runtime * rntm( Runtime::getDefault() );

        for( Runtime::ExtensionIterator i = rntm->extensionsBegin(); i != rntm->extensionsEnd(); ++i )
        {
            ::boost::shared_ptr< Extension >   extension( *i );
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
    FWRUNTIME_API const std::string & getIdentifier() const;

    /**
     * @brief   Retrieves the extension validator.
     *
     * @return  a shared pointer to the extension validator, or null when none
     */
    FWRUNTIME_API ::boost::shared_ptr< io::Validator > getExtensionValidator() const;


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
            const ::boost::shared_ptr<Bundle>       bundle,
            const std::string&              id,
            const boost::filesystem::path&  schema);


private:

    const std::string                               m_id;           ///< a string containing the extension point identifier
    const ::boost::filesystem::path                 m_schema;       ///< a path to the XML schema used to validate contributed extensions
    mutable ::boost::shared_ptr< io::Validator >    m_validator;    ///< a shared pointer to the extension validator

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const ExtensionPoint&) throw();

};

} // namespace fwRuntime



#endif // #define _FWRUNTIME_EXTENSIONPOINT_HPP
