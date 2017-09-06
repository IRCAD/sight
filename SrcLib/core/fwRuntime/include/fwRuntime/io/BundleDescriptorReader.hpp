/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_IO_BUNDLEDESCRIPTORREADER_HPP__
#define __FWRUNTIME_IO_BUNDLEDESCRIPTORREADER_HPP__

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <libxml/parser.h>
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/dl/Library.hpp"

namespace fwRuntime
{


struct Bundle;
struct ConfigurationElement;
struct Extension;
struct ExtensionPoint;

namespace io
{


/**
 * @brief   Defines the bundle descriptor reader class.
 */
struct BundleDescriptorReader
{
    typedef std::vector< std::shared_ptr<Bundle> >  BundleContainer;
    /// Pair of created extension point associated with extensions
    typedef std::pair< SPTR(ExtensionPoint), std::vector< SPTR(Extension) > > PointExtensionsPairType;

    /**
     * @brief       Look for a descriptor at the specified location,
     *              reads it and creates a bundle with it.
     *
     * @param[in]   location    a path to a directory that should contain a bundle description
     *
     * @return      a shared pointer to the created bundle
     */
    FWRUNTIME_API static std::shared_ptr<Bundle> createBundle(const boost::filesystem::path& location);;

    /**
     * @brief       Look for a descriptor at the specified location,
     *              reads it and creates a bundle with it.
     *
     * @param[in]   location    a path to a plugin.xml that should contain a bundle description
     *
     * @return      a shared pointer to the created bundle
     */
    FWRUNTIME_API static std::shared_ptr<Bundle> createBundleFromXmlPlugin(const ::boost::filesystem::path & location);

    /**
     * @brief       Creates all bundles that are found at the given location.
     *
     * @param[in]   location    a path to a directory containing bundles
     *
     * @return      a container with all created bundles
     */
    FWRUNTIME_API static const BundleContainer createBundles(const boost::filesystem::path& location);

    /**
     * @brief   Processes a configuration element XML node.
     *
     * @param   node    a pointer to the XML node that represents a configuration element
     * @param   bundle  a pointer to the bundle the extension will be attached to
     *
     * @return  a pointer to the created configuration element
     */
    FWRUNTIME_API static std::shared_ptr<ConfigurationElement> processConfigurationElement(xmlNodePtr node,
                                                                                           const std::shared_ptr<Bundle> bundle)
   ;

    private:

        static std::string CLASS;       ///< Defines the class XML element name.
        static std::string EXTENSION;   ///< Defines the extension XML element name.
        static std::string EXTENSION_POINT; ///< Defines the extension-point XML element name.
        static std::string ID;          ///< Defines the id XML element name.
        static std::string IMPLEMENTS;  ///< Defines the implements XML element name.
        static std::string NAME;        ///< Defines the name XML element name.
        static std::string LIBRARY;     ///< Defines the library XML element name.
        static std::string PLUGIN;      ///< Defines the plugin XML element name.
        static std::string REQUIREMENT; ///< Defines the requirement XML element name.
        static std::string SCHEMA;      ///< Defines the schema XML element name.
        static std::string VERSION;     ///< Defines the version XML element name.
        static std::string POINT;       ///< Defines the version XML element name.

        /**
         * @brief   Processes an extension XML node.
         *
         * @param   node    a pointer to the XML node that represents an extension
         * @param   bundle  a pointer to the bundle the extension will be attached to
         *
         * @return  a pointer to the created extension
         */
        static std::shared_ptr<Extension> processExtension(xmlNodePtr node,
                                                           const std::shared_ptr<Bundle> bundle);


        /**
         * Processes a node that is a point
         *
         * @param   node    a pointer to the xml document node that represents a requirement
         * @param   bundle  a pointer to the bundle the extension will be attached to
         * @return  a pair with the created extension point and extensions
         */
        static PointExtensionsPairType processPoint(xmlNodePtr node,
                                                    const std::shared_ptr<Bundle> bundle);

        /**
         * @brief   Processes an extension point XML node.
         *
         * @param   node    a pointer to the XML node that represents an extension point
         * @param   bundle  a pointer to the bundle declaring the extension point
         *
         * @return  a pointer to the created extension point
         */
        static std::shared_ptr<ExtensionPoint> processExtensionPoint(xmlNodePtr node,
                                                                     const std::shared_ptr<Bundle> bundle);

        /**
         * @brief   Processes a library XML node.
         *
         * @param   node    a pointer to the XML node that represents a library declaration
         *
         * @return  a pointer to the created library
         */
        static std::shared_ptr<dl::Library> processLibrary(xmlNodePtr node);

        /**
         * Processes a node that is a plugin element.
         *
         * @param   node        a pointer to the xml document node that represents a bundle
         * @param   location    a path to a directory containing the bundle
         *
         * @return  a pointer to the created bundle
         */
        static std::shared_ptr<Bundle> processPlugin(xmlNodePtr node,
                                                     const boost::filesystem::path& location);

        /**
         * Processes a node that is a requirement
         *
         * @param   node        a pointer to the xml document node that represents a requirement
         *
         * @return  a string containing the requirement's value
         */
        static const std::string processRequirement(xmlNodePtr node);
};


} // namespace io

} // namespace fwRuntime


#endif // __FWRUNTIME_IO_BUNDLEDESCRIPTORREADER_HPP__
