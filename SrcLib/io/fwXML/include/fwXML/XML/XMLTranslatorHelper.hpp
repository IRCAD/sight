/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLTRANSLATORHELPER_HPP_
#define XMLTRANSLATORHELPER_HPP_

#include <string>
#include <iterator>
#include "libxml/tree.h"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/ObjectTracker.hpp"

#include <fwCore/Demangler.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>


namespace fwXML
{
class XMLTranslatorHelper
{

public:

    /**
     * @brief return the master node of an Object : className + unique ID as attribue
     * For example <Image id="626b2206-c55b-4a90-b215-d138dc32640e"/> RFC4122 version 4 random based generation
     */
    FWXML_API static xmlNodePtr MasterNode( ::boost::shared_ptr< ::fwTools::Object> obj );

    /**
     * @brief for debug/quality test only test if objet is of type OBJECTTYPE and source->name is equal to OBJECTTYPE
     * class name
     */
    template<class OBJECTTYPE>
    inline static bool check( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
    {
        bool valid = ::boost::dynamic_pointer_cast< OBJECTTYPE >(toUpdate);
        const std::string srcName = (const char*)source->name;
        valid &= ( srcName ==  fwCore::TypeDemangler<OBJECTTYPE>().getLeafClassname() );
        return valid;
    }

    /**
     * @brief return a xmlNode by calling the corresponding XMLTranslator Specialized to the given object
     * Use ClassFactoryRegistry , use TrivialXMLTranslator if no correspondance
     */
    FWXML_API static xmlNodePtr toXML( ::boost::shared_ptr< ::fwTools::Object> obj );

    /**
     * @brief return a xmlNode by calling the corresponding XMLTranslator Specialized to the given object
     * Use ClassFactoryRegistry , use TrivialXMLTranslator if no correspondance and visit all its Field
     */
    FWXML_API static xmlNodePtr toXMLRecursive( ::boost::shared_ptr< ::fwTools::Object> obj );

    /**
     * @brief modify the  given object return from the  xmlNode by calling the corresponding XMLTranslator Specialized
     * Use ClassFactoryRegistry , if no correspondance let object unchanged
     * Object must be castable to source->name class
     */
    FWXML_API static void fromXML( ::boost::shared_ptr< ::fwTools::Object> toUpdate, xmlNodePtr source );

    /**
     * @brief return a newObject form an XML node : if object already created use the previous one
     */
    FWXML_API static ::boost::shared_ptr< ::fwTools::Object> fromXML( xmlNodePtr source );

    /**
     * @brief to node append a new Property with given value
     * the value is converted to string using ::boost::lexical cast
     * @return the current node
     */
    template<class BASICTYPE >
    static xmlNodePtr addProp( xmlNodePtr node, const std::string &propName, const BASICTYPE &value)
    {
        SLM_ASSERT("node not instanced", node);
        assert( !propName.empty() );

        std::string strValue = ::boost::lexical_cast<std::string>(value);
        xmlNewProp( node,
                    xmlStrdup( BAD_CAST propName.c_str() ),
                    xmlStrdup( BAD_CAST strValue.c_str() )
                  );
        return node;
    }

    /**
     * @brief to node get the  Property
     * the retrun value is converted from string using ::boost::lexical cast
     * @return the value
     * @warning
     */
    template<class BASICTYPE >
    static BASICTYPE getProp( xmlNodePtr node, const std::string &propName )
    {
        SLM_ASSERT("node not instanced", node);
        assert( !propName.empty() );

        xmlChar* psAttr = xmlGetProp ( node, BAD_CAST propName.c_str () );
        assert (psAttr);

        const std::string sAttr = (const char*)psAttr;
        BASICTYPE value = ::boost::lexical_cast<BASICTYPE>(sAttr);

        xmlFree(psAttr);
        return value;
    }

    /**
     * @brief an XML helper to create a simple XML element
     * @return a xml node ptr referencing <name>*value*</name>
     */
    template<class VALUE>
    static xmlNodePtr newElement(  const  std::string &name,  VALUE value )
    {
        xmlNodePtr result = xmlNewNode( NULL, xmlStrdup( BAD_CAST name.c_str() )  );
        std::string val = ::boost::lexical_cast< std::string >( value );
        xmlNodeAddContent( result,  xmlStrdup( BAD_CAST val.c_str() ) );
        return result;
    }

    /**
     * @brief an XML helper to create a simple XML element
     * @return a xml node ptr referencing <name>1</name>
     */
    FWXML_API static xmlNodePtr newElement( const  std::string &name,  bool value );

    /**
     * @brief an XML helper to get text information to an element node
     * @return a value lexical casted in VALUE
     */
    template<class VALUE>
    static VALUE getElement(  xmlNodePtr node )
    {
        std::string str = XMLParser::getTextValue (node);
        return ::boost::lexical_cast< VALUE >( str );
    }

    /**
     * @brief an XML helper to get text information to an element node
     * @return a value lexical casted in bool
     */
    FWXML_API static bool getElement(  xmlNodePtr node );

    /// note iterator on a container of smart pointeur of the same type !!!
    template<class Iterator>
    static xmlNodePtr homogeneousContainerToXml(const std::string &rootName, Iterator begin, Iterator end)
    {
        xmlNodePtr root = xmlNewNode(NULL, xmlStrdup( BAD_CAST rootName.c_str() ) );

        if ( begin == end)
        {
            return root;
        }
        for ( Iterator i=begin ; i!= end ; ++i )
        {
            xmlNodePtr node = toXMLRecursive( *i );
            xmlAddChild(root,node);
        }
        return root;
    }

    /// note iterator on a container of values of the same type !!!
    template<class Iterator>
    static xmlNodePtr PODContainerToXml(const std::string &rootName, Iterator begin, Iterator end)
    {

        xmlNodePtr root = xmlNewNode(NULL, xmlStrdup( BAD_CAST rootName.c_str() ) );

        if (typeid( typename Iterator::value_type) == typeid(std::string))
        {
            addProp(root, "item_type", "std::string");
        }
        else
        {
            addProp(root, "item_type", ::fwCore::TypeDemangler< typename Iterator::value_type >().getLeafClassname());
        }

        if ( begin == end)
        {
            return root;
        }
        for ( Iterator i=begin ; i!= end ; ++i )
        {
            xmlNodePtr node = newElement( "item", *i );
            xmlAddChild(root,node);
        }
        return root;
    }

    template<class InserterIterator>
    static void  containerFromXml( xmlNodePtr rootNode, InserterIterator  inserter )
    {
        // parse rootNode child then generateObject and insertit
        SLM_ASSERT("rootNode not instanced", rootNode);
        if ( rootNode->children != NULL )
        {
            xmlNodePtr currentNode = XMLParser::nextXMLElement(rootNode->children);
            while (currentNode )
            {
                OSLM_DEBUG(" containerFromXml" << (const char*)currentNode->name << currentNode );
                // dot not create duplicate object
                ::fwTools::Object::sptr obj = fromXML(currentNode);

                // append to container
                typedef typename InserterIterator::container_type::value_type SharedPtrConcreteObject;
                SharedPtrConcreteObject cobj;
                cobj = ::boost::dynamic_pointer_cast< typename SharedPtrConcreteObject::element_type >( obj );
                assert ( cobj );
                *inserter = cobj;

                // go to next element
                currentNode = XMLParser::nextXMLElement(currentNode->next);
            }
        }
    }

    template<class InserterIterator>
    static void  PODcontainerFromXml( xmlNodePtr rootNode, InserterIterator  inserter )
    {
        // parse rootNode child then generateObject and insertit
        SLM_ASSERT("rootNode not instanced", rootNode);
        if ( rootNode->children != NULL )
        {
            xmlNodePtr currentNode = XMLParser::nextXMLElement(rootNode->children);
            while (currentNode )
            {
                OSLM_DEBUG(" PODcontainerFromXml" << (const char*)currentNode->name << currentNode );

                // append to container
                typedef typename InserterIterator::container_type::value_type PODType;
                PODType value = getElement< PODType >(currentNode);

                *inserter = value;

                // go to next element
                currentNode = XMLParser::nextXMLElement(currentNode->next);
            }
        }
    }

protected:
    XMLTranslatorHelper();
    virtual ~XMLTranslatorHelper();
};

typedef XMLTranslatorHelper XMLTH;

}

#endif /* XMLTRANSLATORHELPER_HPP_ */
