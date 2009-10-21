/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/io/ProfileReader.hpp"

#include <string>
#include <sstream>
#include <boost/filesystem/operations.hpp>
#include <libxml/parser.h>

#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/profile/Activater.hpp"
#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/profile/Starter.hpp"

namespace fwRuntime
{

namespace io
{



std::string	ProfileReader::ID			("id");
std::string	ProfileReader::NAME			("name");
std::string	ProfileReader::VALUE		("value");
std::string	ProfileReader::VERSION		("version");
std::string	ProfileReader::ACTIVATE		("activate");
std::string	ProfileReader::START		("start");
std::string	ProfileReader::PARAM		("param");
std::string	ProfileReader::DIS_EXT_PT	("disable-extension-point");
std::string	ProfileReader::DIS_EXT		("disable-extension");



::boost::shared_ptr< ::fwRuntime::profile::Profile > ProfileReader::createProfile( const boost::filesystem::path & path )
{
	// Normalizes the path.
	boost::filesystem::path	normalizedPath(path);
	normalizedPath.normalize();


	// Asserts that the repository is a valid directory path.
	if(boost::filesystem::exists(normalizedPath) == false || boost::filesystem::is_directory(normalizedPath) == true)
	{
		throw RuntimeException("'" + normalizedPath.string() + "': not a a file.");
	}


	// Validation

#ifndef FWRUNTIME_VER

	const boost::filesystem::path profileXSDLocation( boost::filesystem::current_path() / "share/runtime_0-0/profile.xsd" );

#else

	std::ostringstream fileLocation;
	fileLocation << "share/fwRuntime_" <<  FWRUNTIME_VER << "/profile.xsd";
	const boost::filesystem::path profileXSDLocation( boost::filesystem::current_path() / fileLocation.str() );

#endif

	Validator	validator(profileXSDLocation);

	if( validator.validate(normalizedPath) == false )
	{
		throw RuntimeException(validator.getErrorLog());
	}


	// Get the document.
	xmlDocPtr document = xmlParseFile(normalizedPath.native_file_string().c_str());
	if(document == 0)
	{
		throw RuntimeException("Unable to read the profile file.");
	}

	try
	{
		// Get the root node.
		xmlNodePtr rootNode = xmlDocGetRootElement(document);

        char* pName    = (char *) xmlGetProp(rootNode, (const xmlChar*) NAME.c_str());
        char* pVersion = (char *) xmlGetProp(rootNode, (const xmlChar*) VERSION.c_str());

        SLM_ASSERT("Application profile MUST have a name attribute"   , pName);
        SLM_ASSERT("Application profile MUST have a version attribute", pVersion);

		std::string sName( pName );
		std::string sVersion( pVersion );


        free(pName);
        free(pVersion);
		
		// Creates and process the profile element.
		 ::boost::shared_ptr< ::fwRuntime::profile::Profile > profile = processProfile(rootNode);

		profile->setName(sName);
		profile->setVersion(sVersion);

		// Job's done!
		xmlFreeDoc(document);
		return profile;
	}
	catch(std::exception& exception)
	{
		xmlFreeDoc(document);
		throw exception;
	}
}



::boost::shared_ptr< ::fwRuntime::profile::Profile > ProfileReader::processProfile(xmlNodePtr node)
{
	using namespace ::fwRuntime::profile;


	// Process child nodes.
	 ::boost::shared_ptr< Profile >	profile( new Profile() );
	xmlNodePtr				curChild = node->children;
	for(curChild = node->children; curChild != 0; curChild = curChild->next)
	{
		if(xmlStrcmp(curChild->name, (const xmlChar*) ACTIVATE.c_str()) == 0)
		{
			profile->add( processActivater(curChild) );
			continue;
		}

		if(xmlStrcmp(curChild->name, (const xmlChar*) START.c_str()) == 0)
		{
			profile->add( processStarter(curChild) );
			continue;
		}
	}
	return profile;
}



::boost::shared_ptr< ::fwRuntime::profile::Activater > ProfileReader::processActivater(xmlNodePtr node)
{
	// Processes all attributes.
	xmlAttrPtr curAttr;
	std::string identifier;
	std::string	version;
	for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
	{
		if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
		{
			identifier = (const char*) curAttr->children->content;
			continue;
		}

		if(xmlStrcmp(curAttr->name, (const xmlChar*) VERSION.c_str()) == 0)
		{
			version = (const char*) curAttr->children->content;
			continue;
		}
	}


	// Creates the activater object.
	using ::fwRuntime::profile::Activater;
	 ::boost::shared_ptr< Activater >	activater( new Activater(identifier, version) );


	// Processes child node that are the parameters
	xmlNodePtr	curChild = node->children;
	for(curChild = node->children; curChild != 0; curChild = curChild->next)
	{
		if(xmlStrcmp(curChild->name, (const xmlChar*) PARAM.c_str()) == 0)
		{
			processActivaterParam( curChild, activater );
			continue;
		}

		if(xmlStrcmp(curChild->name, (const xmlChar*) DIS_EXT_PT.c_str()) == 0)
		{
			processActivaterDisableExtensionPoint( curChild, activater );
			continue;
		}

		if(xmlStrcmp(curChild->name, (const xmlChar*) DIS_EXT.c_str()) == 0)
		{
			processActivaterDisableExtension( curChild, activater );
			continue;
		}
	}


	// Job's done.
	return activater;
}



void ProfileReader::processActivaterParam(xmlNodePtr node, ::boost::shared_ptr< ::fwRuntime::profile::Activater > activater)
{
	// Processes all attributes.
	xmlAttrPtr curAttr;
	std::string identifier;
	std::string	value;
	for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
	{
		if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
		{
			identifier = (const char*) curAttr->children->content;
			continue;
		}

		if(xmlStrcmp(curAttr->name, (const xmlChar*) VALUE.c_str()) == 0)
		{
			value = (const char*) curAttr->children->content;
			continue;
		}
	}

	// Stores the parameter into the activater.
	activater->addParameter( identifier, value );
}



void ProfileReader::processActivaterDisableExtensionPoint(xmlNodePtr node, ::boost::shared_ptr< ::fwRuntime::profile::Activater > activater)
{
	// Processes all attributes.
	xmlAttrPtr curAttr;
	std::string identifier;
	std::string	value;
	for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
	{
		if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
		{
			identifier = (const char*) curAttr->children->content;
			continue;
		}
	}

	// Stores the parameter into the activater.
	activater->addDisableExtensionPoint( identifier );
}



void ProfileReader::processActivaterDisableExtension(xmlNodePtr node, ::boost::shared_ptr< ::fwRuntime::profile::Activater > activater)
{
	// Processes all attributes.
	xmlAttrPtr curAttr;
	std::string identifier;
	std::string	value;
	for(curAttr = node->properties; curAttr != 0; curAttr = curAttr->next)
	{
		if(xmlStrcmp(curAttr->name, (const xmlChar*) ID.c_str()) == 0)
		{
			identifier = (const char*) curAttr->children->content;
			continue;
		}
	}

	// Stores the parameter into the activater.
	activater->addDisableExtension( identifier );
}



::boost::shared_ptr< ::fwRuntime::profile::Starter > ProfileReader::processStarter(xmlNodePtr node)
{
	// Processes all attributes.
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


	// Creates the activater object.
	using ::fwRuntime::profile::Starter;
	 ::boost::shared_ptr< Starter >	starter( new Starter(identifier) );
	return starter;
}



} // namespace io

} // namespace fwRuntime
