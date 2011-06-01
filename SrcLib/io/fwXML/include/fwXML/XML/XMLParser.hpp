/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLPARSER_HPP_
#define XMLPARSER_HPP_

#include <string>

#include <boost/filesystem/path.hpp>

#include <libxml/tree.h>

#include <fwTools/Failed.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{

struct FWXML_CLASS_API XMLParser
{


/**
 * @brief
 * Validates a xml document which has a reference to its schema
 * @param : xmlDocPtr : a xml document
 * @throw : Failed
 * @author IRCAD (Research and Development Team).
 */
FWXML_API static void validateDoc (xmlDocPtr xmlDoc) throw (::fwTools::Failed);



/**
 * @brief
 * search via ->next->next the first xml node which is a node Element !
 * @param : a node to parse via ->next->next
 * @return : the first element which is an ELEMENT_NODE, NULL if not found
 * @author IRCAD (Research and Development Team).
 */
FWXML_API static xmlNodePtr nextXMLElement (xmlNodePtr pNode);



/**
 * @brief
 * from a file : load xml data , manage Xinclude & Schema ( TODO )
 * @param : absolut path to rootFilename
 * @throw : Failed
 * @author IRCAD (Research and Development Team).
 */
FWXML_API static xmlDocPtr getXmlDocFromFile(boost::filesystem::path rootFile) throw (::fwTools::Failed);




/**
 * @brief Extracts the (text) value of a node.
 *
 * @param  xmlNodePtr : the node of which the text has to be extracted
 * @return  std::string : the value of the node
 * @throw  ::fwTools::Failed
 * @author IRCAD (Research and Development Team).
 *
 **/
FWXML_API static std::string getTextValue (xmlNodePtr pNode) throw (::fwTools::Failed);



/**
 * @brief Extracts the (text) value of an attribute of a node
 * @param  xmlNodePtr : the node of which the text has to be extracted
 * @param  const std::string& : the attribute's name
 * @return std::string : the value of the attribute
 *
 * @throw : ::fwTools::Failed if attribute doesn't exist
 * @author IRCAD (Research and Development Team).
 *
 **/
FWXML_API static std::string getAttribute (xmlNodePtr pNode, const std::string& _sAttrName) throw (::fwTools::Failed);


/**
 * brief search the *first* child of a node which have the name which is equal to be given value
 * @param start the xml node ptr where to start search in its children
 * @param value the name to find
 * @return the *first* node which have the wanted name, NULL if not found
 * @author IRCAD (Research and Development Team).
 **/
FWXML_API static xmlNodePtr findChildNamed(xmlNodePtr start, std::string value);



/**
 * @brief return a string from a xml node by creating a viewable mini docXML, for debugging purpose
 * @author IRCAD (Research and Development Team).
 */
FWXML_API static std::string toString(xmlNodePtr node);


};

}



#endif /*XMLPARSER_HPP_*/
