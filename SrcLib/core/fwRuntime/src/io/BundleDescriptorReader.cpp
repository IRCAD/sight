/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <sstream>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <libxml/parser.h>
#include <libxml/xinclude.h>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/dl/Library.hpp"
#include "fwRuntime/io/BundleDescriptorReader.hpp"
#include "fwRuntime/io/XMLSubstitute.hpp"
#include "fwRuntime/io/Validator.hpp"

//#include <fwCore/base.hpp>

namespace fwRuntime
{

namespace io
{

std::string BundleDescriptorReader::CLASS("class");

std::string BundleDescriptorReader::EXTENSION("extension");

std::string BundleDescriptorReader::EXTENSION_POINT("extension-point");

std::string BundleDescriptorReader::ID("id");

std::string BundleDescriptorReader::IMPLEMENTS("implements");

std::string BundleDescriptorReader::LIBRARY("library");

std::string BundleDescriptorReader::NAME("name");

std::string BundleDescriptorReader::PLUGIN("plugin");

std::string BundleDescriptorReader::REQUIREMENT("requirement");

std::string BundleDescriptorReader::SCHEMA("schema");

std::string BundleDescriptorReader::VERSION("version");

std::string BundleDescriptorReader::POINT("point");



const BundleDescriptorReader::BundleContainer BundleDescriptorReader::createBundles(const boost::filesystem::path& location) throw(RuntimeException)
{
        // Normalizes the path.
        boost::filesystem::path normalizedPath(location);
        normalizedPath.normalize();

        // Asserts that the repository is a valid directory path.
        if(boost::filesystem::exists(normalizedPath) == false || boost::filesystem::is_directory(normalizedPath) == false)
        {
                throw RuntimeException("'" + normalizedPath.string() + "': not a directory.");
        }

        // Walk through the repository entries.
        BundleContainer                                                 bundles;
        boost::filesystem::directory_iterator   currentEntry(normalizedPath);
        boost::filesystem::directory_iterator   endEntry;
        for(; currentEntry != endEntry; ++currentEntry)
        {
                boost::filesystem::path entryPath = *currentEntry;

                if(boost::filesystem::is_directory(entryPath) == true)
                {
                        try
                        {
                                bundles.push_back( BundleDescriptorReader::createBundle(entryPath) );
                        }
                        catch(const RuntimeException& runtimeException)
                        {
                                //OSLM_WARN( "'"<< entryPath.string() << "': skipped. " << runtimeException.what() );
                        }
                }
        }
        return bundles;
}



::boost::shared_ptr<Bundle> BundleDescriptorReader::createBundle(const boost::filesystem::path& location) throw(RuntimeException)
{
        // Get the descriptor location.
        const boost::filesystem::path   completeLocation(
                        location.is_complete() == true ?
                                location : boost::filesystem::current_path() / location);

        boost::filesystem::path descriptorLocation(location / "plugin.xml");
        if(boost::filesystem::exists(descriptorLocation) == false)
        {
                throw RuntimeException("'plugin.xml': file not found.");
        }

        // Validation

#ifndef FWRUNTIME_VER

        const boost::filesystem::path pluginXSDLocation( boost::filesystem::current_path() / "share/runtime_0-0/plugin.xsd" );

#else

        std::ostringstream fileLocation;
        fileLocation << "share/fwRuntime_" <<  FWRUNTIME_VER << "/plugin.xsd";
        const boost::filesystem::path pluginXSDLocation( boost::filesystem::current_path() / fileLocation.str() );

#endif

        Validator       validator(pluginXSDLocation);
        if( validator.validate(descriptorLocation) == false )
        {
                throw RuntimeException("Invalid bundle descriptor file. " + validator.getErrorLog());
        }

        // Get the document.
//      xmlDocPtr document = xmlParseFile(descriptorLocation.native_file_string().c_str());
        xmlDocPtr document = ::fwRuntime::io::XMLSubstitute::getDefault()->load(descriptorLocation);
        if(document == 0)
        {
                throw RuntimeException("Unable to read the bundle descriptor file.");
        }

        try
        {
                // Get the root node.
                xmlNodePtr rootNode = xmlDocGetRootElement(document);

                if (xmlXIncludeProcessTree (rootNode) == -1)
                {
                        throw RuntimeException("Unable to manage xinclude !");
                }

                if(xmlStrcmp(rootNode->name, (const xmlChar*) PLUGIN.c_str()) != 0)
                {
                        throw RuntimeException("Unexpected XML element");
                }

                // Creates and process the plugin element.
                 ::boost::shared_ptr<Bundle> bundle = processPlugin(rootNode, completeLocation);

                // Job's done!
                xmlFreeDoc(document);
                return bundle;
        }
        catch(std::exception& exception)
        {
                xmlFreeDoc(document);
                throw exception;
        }
}

//-----------------------------------------------------------------------------

::boost::shared_ptr<Bundle> BundleDescriptorReader::createBundleFromXmlPlugin( const boost::filesystem::path & location ) throw ( RuntimeException )
{
        // Get the descriptor location.
        const boost::filesystem::path tmpCompleteLocation ( location.is_complete() == true ? location : boost::filesystem::current_path() / location);
        const boost::filesystem::path completeLocation ( tmpCompleteLocation.branch_path() );

        ::boost::filesystem::path descriptorLocation ( tmpCompleteLocation );
        if( ::boost::filesystem::exists(descriptorLocation) == false )
        {
                throw RuntimeException("'plugin.xml': file not found.");
        }

        // Validation

#ifndef FWRUNTIME_VER

        const boost::filesystem::path pluginXSDLocation( boost::filesystem::current_path() / "share/runtime_0-0/plugin.xsd" );

#else

        std::ostringstream fileLocation;
        fileLocation << "share/fwRuntime_" <<  FWRUNTIME_VER << "/plugin.xsd";
        const boost::filesystem::path pluginXSDLocation( boost::filesystem::current_path() / fileLocation.str() );

#endif

        Validator       validator(pluginXSDLocation);
        if( validator.validate(descriptorLocation) == false )
        {
                throw RuntimeException("Invalid bundle descriptor file. " + validator.getErrorLog());
        }

        // Get the document.
//      xmlDocPtr document = xmlParseFile(descriptorLocation.native_file_string().c_str());
        xmlDocPtr document = ::fwRuntime::io::XMLSubstitute::getDefault()->load(descriptorLocation);
        if(document == 0)
        {
                throw RuntimeException("Unable to read the bundle descriptor file.");
        }

        try
        {
                // Get the root node.
                xmlNodePtr rootNode = xmlDocGetRootElement(document);

                if (xmlXIncludeProcessTree (rootNode) == -1)
                {
                        throw RuntimeException("Unable to manage xinclude !");
                }

                if(xmlStrcmp(rootNode->name, (const xmlChar*) PLUGIN.c_str()) != 0)
                {
                        throw RuntimeException("Unexpected XML element");
                }

                // Creates and process the plugin element.
                 ::boost::shared_ptr<Bundle> bundle = processPlugin(rootNode, completeLocation);

                // Job's done!
                xmlFreeDoc(document);
                return bundle;
        }
        catch(std::exception& exception)
        {
                xmlFreeDoc(document);
                throw exception;
        }
}

//-----------------------------------------------------------------------------

::boost::shared_ptr<ConfigurationElement> BundleDescriptorReader::processConfigurationElement(xmlNodePtr node, const ::boost::shared_ptr<Bundle> bundle) throw(RuntimeException)
{
        //xmlKeepBlanksDefault(0);
        // Creates the configuration element.
        std::string name((const char*) node->name);
         ::boost::shared_ptr<ConfigurationElement> configurationElement(new ConfigurationElement(bundle, name));

        // Processes all attributes.
        xmlAttrPtr curAttr;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                std::string name((const char*) curAttr->name);
                std::string value((const char*) curAttr->children->content);

                configurationElement->setAttributeValue(name, value);
        }

        // Process child nodes.
        xmlNodePtr curChild = node->children;
        for(curChild = node->children; curChild != 0; curChild = curChild->next)
        {
                if(curChild->type == XML_TEXT_NODE && !xmlIsBlankNode(curChild))
                {
                        std::string value((const char*) curChild->content);
                        // Even whitespace (non XML_TEXT_NODE) are considered as valid XML_TEXT_NODE
                        if( value.find("\n")!=std::string::npos || value.find("\t")!=std::string::npos )
                        {
                                OSLM_WARN("white space and/or carriage return considered as XML_TEXT_NODE : the parsing is dangerous -> to be fixed");
                        }
                        configurationElement->setValue(
                                configurationElement->getValue() + value );
                        continue;
                }

                if(curChild->type == XML_ELEMENT_NODE)
                {
                         ::boost::shared_ptr<ConfigurationElement> element(processConfigurationElement(curChild, bundle));
                        configurationElement->addConfigurationElement(element);
                        continue;
                }
        }

        // Job's done.
        return configurationElement;
}

::boost::shared_ptr<Extension> BundleDescriptorReader::processExtension(xmlNodePtr node, const ::boost::shared_ptr<Bundle> bundle) throw(RuntimeException)
{
        // Processes all extension attributes.
        xmlAttrPtr curAttr;
        std::string point;
        std::string identifier;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
                {
                        identifier = (const char*) curAttr->children->content;
                        continue;
                }

                if(xmlStrcmp(curAttr->name, (const xmlChar*) IMPLEMENTS.c_str()) == 0)
                {
                        point = (const char*) curAttr->children->content;
                        continue;
                }
        }

        // Creates the extension instance.
         ::boost::shared_ptr<Extension> extension(new Extension(bundle, identifier, point, node));

        // Processes child nodes which are configuration elements.
        xmlNodePtr curChild;
        for(curChild = node->children; curChild != 0; curChild = curChild->next)
        {
                if(curChild->type == XML_ELEMENT_NODE)
                {
                         ::boost::shared_ptr<ConfigurationElement> element(processConfigurationElement(curChild, bundle));
                        extension->addConfigurationElement(element);
                }
        }

        // Job's done.
        return extension;
}

std::pair< ::boost::shared_ptr<ExtensionPoint> , std::vector< ::boost::shared_ptr<Extension> > > BundleDescriptorReader::processPoint(xmlNodePtr node, const ::boost::shared_ptr<Bundle> bundle) throw(RuntimeException)
{
        // Creates the extension instance.
        xmlAttrPtr curAttr;
        std::string schema;
        std::string identifier;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
                {
                        identifier = (const char*) curAttr->children->content;
                        continue;
                }

                if(xmlStrcmp(curAttr->name, (const xmlChar*) SCHEMA.c_str()) == 0)
                {
                        schema = (const char*) curAttr->children->content;
                        continue;
                }
        }
         ::boost::shared_ptr<ExtensionPoint> extensionPoint(new ExtensionPoint(bundle, identifier, schema));

        // Processes child nodes which declare identifier as extensions.
        std::vector< ::boost::shared_ptr<Extension> > extensionContainer ;
        xmlNodePtr curChild;
        for(curChild = node->children; curChild != 0; curChild = curChild->next)
        {
                if(curChild->type == XML_ELEMENT_NODE)
                {
                        if( xmlStrcmp(curChild->name, (const xmlChar*) IMPLEMENTS.c_str()) == 0 )
                        {
                                std::string extensionId =  (const char*) curChild->children->content ;
                                 ::boost::shared_ptr<Extension> extension(new Extension(bundle, identifier , extensionId, curChild));
                                extensionContainer.push_back( extension ) ;
                        }
                }
        }

        return std::pair< ::boost::shared_ptr<ExtensionPoint> , std::vector< ::boost::shared_ptr<Extension> > >( extensionPoint , extensionContainer ) ;
}

::boost::shared_ptr<ExtensionPoint> BundleDescriptorReader::processExtensionPoint(xmlNodePtr node, const ::boost::shared_ptr<Bundle> bundle) throw(RuntimeException)
{
        // Processes all extension attributes.
        xmlAttrPtr      curAttr;
        std::string     identifier;
        std::string     schema;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
                {
                        identifier = (const char*) curAttr->children->content;
                        continue;
                }

                if(xmlStrcmp(curAttr->name, (const xmlChar*) SCHEMA.c_str()) == 0)
                {
                        schema = (const char*) curAttr->children->content;
                        continue;
                }
        }

        // Creates the extension instance.
         ::boost::shared_ptr<ExtensionPoint> point(new ExtensionPoint(bundle, identifier, schema));

        // Job's done.
        return point;
}


::boost::shared_ptr<dl::Library> BundleDescriptorReader::processLibrary(xmlNodePtr node) throw(RuntimeException)
{
        // Processes all plugin attributes.
        xmlAttrPtr curAttr;
        std::string name;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) NAME.c_str()) == 0)
                {
                        name = (const char*) curAttr->children->content;
                        continue;
                }
        }

        // Creates the library
         ::boost::shared_ptr<dl::Library> library( new dl::Library(name) );
        return library;
}


::boost::shared_ptr<Bundle> BundleDescriptorReader::processPlugin(xmlNodePtr node, const boost::filesystem::path& location) throw(RuntimeException)
{
        // Processes all plugin attributes.
        xmlAttrPtr      curAttr;
        std::string     bundleIdentifier;
        std::string     version;
        std::string     pluginClass;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
                {
                        bundleIdentifier = (const char*) curAttr->children->content;
                        continue;
                }

                if(xmlStrcmp(curAttr->name, (const xmlChar*) CLASS.c_str()) == 0)
                {
                        pluginClass = (const char*) curAttr->children->content;
                        continue;
                }

                if(xmlStrcmp(curAttr->name, (const xmlChar*) VERSION.c_str()) == 0)
                {
                        version = (const char*) curAttr->children->content;
                        continue;
                }
        }

        // Creates the bundle.
         ::boost::shared_ptr<Bundle> bundle;
        if(pluginClass.empty() == true)
        {
                bundle = ::boost::shared_ptr<Bundle>( new Bundle(location, bundleIdentifier, version) );
        }
        else
        {
                bundle = ::boost::shared_ptr<Bundle>( new Bundle(location, bundleIdentifier, version, pluginClass) );
        }

        // Processes all child nodes.
        xmlNodePtr curChild;
        for(curChild = node->children; curChild != 0; curChild = curChild->next)
        {
                // Skip non element nodes.
                if(curChild->type != XML_ELEMENT_NODE)
                {
                        continue;
                }

                // Extension declaration.
                if(xmlStrcmp(curChild->name, (const xmlChar*) EXTENSION.c_str()) == 0)
                {
                         ::boost::shared_ptr<Extension> extension(processExtension(curChild, bundle));
                        bundle->addExtension(extension);
                        continue;
                }

                // Extension point declaration.
                if(xmlStrcmp(curChild->name, (const xmlChar*) EXTENSION_POINT.c_str()) == 0)
                {
                         ::boost::shared_ptr<ExtensionPoint> point(processExtensionPoint(curChild, bundle));
                        bundle->addExtensionPoint(point);
                        continue;
                }

                // Library declaration.
                if(xmlStrcmp(curChild->name, (const xmlChar*) LIBRARY.c_str()) == 0)
                {
                         ::boost::shared_ptr<dl::Library> library(processLibrary(curChild));
                        bundle->addLibrary(library);
                        continue;
                }

                // Requirement declaration.
                if(xmlStrcmp(curChild->name, (const xmlChar*) REQUIREMENT.c_str()) == 0)
                {
                        const std::string       requirement(processRequirement(curChild));
                        bundle->addRequirement(requirement);
                }

                // Point declaration.
                if(xmlStrcmp(curChild->name, (const xmlChar*) POINT.c_str()) == 0)
                {
                        std::pair< ::boost::shared_ptr<ExtensionPoint> , std::vector< ::boost::shared_ptr<Extension> > > point = processPoint(curChild,bundle);

                        for( std::vector< ::boost::shared_ptr<Extension> >::iterator iter = point.second.begin() ; iter != point.second.end() ; ++iter )
                        {
                                bundle->addExtension( *iter );
                        }

                        bundle->addExtensionPoint(point.first );
                }
        }

        // Job's done.
        return bundle;
}

const std::string BundleDescriptorReader::processRequirement(xmlNodePtr node) throw(RuntimeException)
{
        // Processes all requirement attributes.
        xmlAttrPtr curAttr;
        std::string identifier;
        for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
        {
                if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
                {
                        identifier = (const char*) curAttr->children->content;
                        continue;
                }
        }

        // Do some sanity checking.
        if(identifier.length() == 0)
        {
                throw RuntimeException("Invalid attribute.");
        }

        // Job's done
        return identifier;
}

} // namesapce io

} // namespace fwRuntime
