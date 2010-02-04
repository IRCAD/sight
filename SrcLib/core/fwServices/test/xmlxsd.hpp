/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICES_TEST_XMLXSD_HPP_
#define FWSERVICES_TEST_XMLXSD_HPP_
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/shared_ptr.hpp>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/io/Validator.hpp>
#include <fwCore/base.hpp>
#include <sstream>
#include <iostream>
#include <stdio.h> 
#include <fstream>


::boost::shared_ptr< ::fwRuntime::ConfigurationElement > configTest1()
{
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > element(new ::fwRuntime::EConfigurationElement("rootElement1"));
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > subelement = element->addConfigurationElement("subElement");
    subelement->setAttributeValue("myAttr","myValue");
    return element;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > configTest2()
{
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > element(new ::fwRuntime::EConfigurationElement("rootElement2"));
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > subelement1 = element->addConfigurationElement("subElement");
    subelement1->setAttributeValue("myAttr","myValue");
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > subelement2 = element->addConfigurationElement("subElement");
    subelement2->setValue("myValue");
    
    return element;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > configTest3()
{
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > element(new ::fwRuntime::EConfigurationElement("rootElement3"));
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > subelement1 = element->addConfigurationElement("subElement");
    subelement1->setAttributeValue("myAttr","myValue");
  ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > subelement2 = element->addConfigurationElement("subElement");
    subelement2->setValue("myValue");
    subelement2->setAttributeValue("myAttr","myValue");
    
    return element;
}

xmlDocPtr toXml(::boost::shared_ptr< ::fwRuntime::ConfigurationElement > element)
{
	xmlNodePtr _elementNodePtr = xmlNewNode( NULL,  xmlCharStrdup( element->getName().c_str() ) );
	::fwRuntime::ConfigurationElement2XML( element , _elementNodePtr ) ;
	xmlDocPtr xmlDoc	= xmlNewDoc(BAD_CAST "1.0") ;
	xmlNodePtr xmlNode	= xmlCopyNode(_elementNodePtr, 1) ;
	xmlDocSetRootElement(xmlDoc, xmlNode);
	return xmlDoc ;
}
void validateConfigurationElement(::boost::shared_ptr< ::fwRuntime::ConfigurationElement > element, std::string xsdFilename)
{
	//::boost::filesystem::path schema1("rc/test1.xsd") ;
 ::boost::shared_ptr< ::fwRuntime::io::Validator > validator( new ::fwRuntime::io::Validator(::boost::filesystem::path(xsdFilename))) ;
   xmlDocPtr xmlDoc = toXml(element) ;	
   xmlNode* node;
   	node = xmlDocGetRootElement(xmlDoc);
   	OSLM_INFO("###############################################" ) ;
   	OSLM_INFO("TEST RESULT FOR CONFIGURATION ELEMENT " << element->getName() << " checked by schema " << xsdFilename ) ;
   	if( validator->validate( node ) == true )
   	{
   		OSLM_INFO("TEST VALID") ;
   	}
   	else
   	{
   		OSLM_INFO("TEST INVALID : " << validator->getErrorLog() ) ;
   	}
   	OSLM_INFO("###############################################" ) ;
}
/// Helper for writing xml structure
int streamIOWrite(void * context,  const char * buffer, int len)
{
	std::ostream *pOs = static_cast< std::ostream * >( context );
	assert(pOs);
	return   pOs->write(buffer, len)?len:-1;

}
/// Helper for writing xml structure
int streamIOClose(void * context)
{
	std::ostream *pOs = static_cast< std::ostream * >( context );
	assert(pOs);
	pOs->flush();
	return   0;
}
/// Helper for writing xml structure
void toStream( xmlDocPtr doc, std::ostream &os )
{
	void *IOContexte = &os;

	char *encoding="UTF-8";
	int formatWithSpace=1;
	xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(encoding);
	//xmlCharEncodingHandlerPtr handler = xmlGetCharEncodingHandler(XML_CHAR_ENCODING_UTF8); doesnt work :/

	assert(handler);

	//xmlThrDefIndentTreeOutput(4);
	xmlOutputBufferPtr outBuff = xmlOutputBufferCreateIO( streamIOWrite , streamIOClose, IOContexte, handler);
	xmlSaveFormatFileTo( outBuff, doc, encoding, formatWithSpace );
}
void toFile( xmlDocPtr doc, std::string filename)
{
	std::stringstream myStream ;
	toStream( doc , myStream ) ;
	std::ofstream file;
	file.open(filename.c_str());
	file << myStream.str() ;
	file.close();
}
void toOSLM_INFO( xmlDocPtr doc , std::string prefix)
{
	std::stringstream myStream ;
	std::ofstream file;
	toStream( doc , myStream ) ;
	OSLM_INFO(prefix << std::endl << myStream.str() ) ;
}

int testXMLXSD()
{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > element1 = configTest1();
	OSLM_INFO("configuration element TEST1 " << std::endl << *element1) ;
	xmlDocPtr xmlDoc1 = toXml(element1) ;
	toFile( xmlDoc1 , "test1.xml") ;
	toOSLM_INFO( xmlDoc1 , "test1" ) ;
	validateConfigurationElement(element1,std::string("rc/test1.xsd") );
	
	//::boost::filesystem::path schema1("rc/test1.xsd") ;
// ::boost::shared_ptr< ::fwRuntime::io::Validator > validator1( new ::fwRuntime::io::Validator(::boost::filesystem::path("rc/test1.xsd"))) ;
//   	xmlNode* node1;
//   	node1 = xmlDocGetRootElement(xmlDoc1);
//   	if( validator1->validate( node1 ) == true )
//   	{
//   		OSLM_INFO("TEST1 VALID") ;
//   	}
//   	else
//   	{
//   		OSLM_INFO("TEST1 INVALID") ;
//   	}
    xmlFreeDoc(xmlDoc1);

	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > element2 = configTest2();
	OSLM_INFO("configuration element TEST1 " << std::endl << *element2) ;
	xmlDocPtr xmlDoc2 = toXml(element2) ;
	toFile( xmlDoc2 , "test2.xml") ;
	toOSLM_INFO( xmlDoc2 , "test2" ) ;
	validateConfigurationElement(element2,std::string("rc/test1.xsd") );
    xmlFreeDoc(xmlDoc2);
    
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > element3 = configTest3();
	OSLM_INFO("configuration element TEST1 " << std::endl << *element3) ;
	xmlDocPtr xmlDoc3 = toXml(element3) ;
	toFile( xmlDoc3 , "test3.xml") ;
	toOSLM_INFO( xmlDoc3 , "test3" ) ;
    xmlFreeDoc(xmlDoc3);
    
    
    xmlCleanupParser();
    xmlMemoryDump();
    return 1 ;
}

#endif /*XMLXSD_HPP_*/
