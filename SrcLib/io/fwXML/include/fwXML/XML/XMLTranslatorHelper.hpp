/*
 * XMLTranslatorHelper.hpp
 *
 *  Created on: Oct 8, 2008
 *      Author: vagnus
 */

#ifndef XMLTRANSLATORHELPER_HPP_
#define XMLTRANSLATORHELPER_HPP_

#include <string>
#include <iterator>
#include "libxml/tree.h"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/ObjectTracker.hpp"
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
    static xmlNodePtr MasterNode( ::boost::shared_ptr< ::fwTools::Object> obj );


    /**
     * @brief for debug/quality test only test if objet is of type OBJECTTYPE and source->name is equal to OBJECTTYPE
     * class name
     */
    template<class OBJECTTYPE>
    inline static bool check( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source)
    {
        bool valid = boost::dynamic_pointer_cast< OBJECTTYPE >(toUpdate);
        const std::string srcName = (const char*)source->name;
        valid &= ( srcName ==  fwCore::TypeDemangler<OBJECTTYPE>().getLeafClassname() );
        return valid;
    }


    /**
     * @brief return a xmlNode by calling the corresponding XMLTranslator Specialized to the given object
     * Use ClassFactoryRegistry , use TrivialXMLTranslator if no correspondance
     */
    static xmlNodePtr toXML( ::boost::shared_ptr< ::fwTools::Object> obj );


    /**
     * @brief return a xmlNode by calling the corresponding XMLTranslator Specialized to the given object
     * Use ClassFactoryRegistry , use TrivialXMLTranslator if no correspondance and visit all its Field
     */
    static xmlNodePtr toXMLRecursive( ::boost::shared_ptr< ::fwTools::Object> obj );


    /**
     * @brief modify the  given object return from the  xmlNode by calling the corresponding XMLTranslator Specialized
     * Use ClassFactoryRegistry , if no correspondance let object unchanged
     * Object must be castable to source->name class
     */
    static void fromXML( ::boost::shared_ptr< ::fwTools::Object> toUpdate, xmlNodePtr source );


    /**
     * @brief return a newObject form an XML node : if object already created use the previous one
     */
    static ::boost::shared_ptr< ::fwTools::Object> fromXML( xmlNodePtr source );

    /*
     * @brief to node append a new Property with given value
     * the value is converted to string using boost::lexical cast
     * @return the current node
     */
    template<class BASICTYPE >
    static xmlNodePtr addProp( xmlNodePtr node, const std::string &propName, const BASICTYPE &value)
    {
        assert(node);
        assert( !propName.empty() );

        std::string strValue = boost::lexical_cast<std::string>(value);
        xmlNewProp( node,
                    xmlStrdup( BAD_CAST propName.c_str() ),
                    xmlStrdup( BAD_CAST strValue.c_str() )
                  );
        return node;
    }


    /*
     * @brief to node get the  Property
     * the retrun value is converted from string using boost::lexical cast
     * @return the value
     * @warning
     */
    template<class BASICTYPE >
    static BASICTYPE getProp( xmlNodePtr node, const std::string &propName )
    {
        assert(node);
        assert( !propName.empty() );

        xmlChar* psAttr = xmlGetProp ( node, BAD_CAST propName.c_str () );
        assert (psAttr);

        const std::string sAttr = (const char*)psAttr;
        BASICTYPE value = boost::lexical_cast<BASICTYPE>(sAttr);

        xmlFree(psAttr);
        return value;
    }

    /*
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

    /*
     * @brief an XML helper to create a simple XML element
     * @return a xml node ptr referencing <name>1</name>
     */
    static xmlNodePtr newElement( const  std::string &name,  bool value );

    /// note iterator on a container of smart pointeur of the same type !!!
    template<class Iterator>
    static xmlNodePtr homogeneousContainerToXml(const std::string &rootName, Iterator begin, Iterator end)
    {
        xmlNodePtr root = xmlNewNode(NULL, xmlStrdup( BAD_CAST rootName.c_str() ) );

        if ( begin == end)
        {
            return root;
        }

//      // set the translator from *only* the first element
//      // append objet serialisation
//      ::boost::shared_ptr< ::fwXML::XMLTranslator > translator;
//      const std::string &commonClassName = (*begin)->className();
//      translator = ::fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator >( (*begin)->className() );
//      assert(translator);
//      // we *ONLY* use an translator data are supposed to be homogeneous
//
        for ( Iterator i=begin ; i!= end ; ++i )
        {
            xmlNodePtr node = toXMLRecursive( *i );
            xmlAddChild(root,node);
        }


        return root;
    }



    template<class InserterIterator>
    static void  containerFromXml( xmlNodePtr rootNode, InserterIterator  inserter )
    {
        // parse rootNode child then generateObject and insertit
        assert(rootNode );
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




//  template<class Iterator>
//  static xmlNodePtr homogeneousContainertoXml(const std::string &rootName, Iterator begin, Iterator end)
//  {
//      xmlNodePtr root = xmlNewNode(NULL, xmlStrdup( BAD_CAST rootName.c_str() ) );
//
//      if ( begin == end)
//      {
//          return root;
//      }
//
//      // set the translator from *only* the first element
//      // append objet serialisation
//       ::boost::shared_ptr< fwXML::XMLTranslator > translator;
//      const std::type_info &ti = fwTools::ClassName::get( (*begin)->className() );
//      translator = fwTools::ClassFactoryRegistry::create< fwXML::XMLTranslator  >(  ti  );
//      assert(translator);
//      // we *ONLY* use an translator data are supposed to be homogeneous
//
//      for ( Iterator i=begin ; i!= end ; ++i )
//      {
//          xmlNodePtr node = translator->getXMLFrom( *i );
//          xmlAddChild(root,node);
//      }
//
//      return root;
//  }

protected:
    XMLTranslatorHelper();
    virtual ~XMLTranslatorHelper();
};

typedef XMLTranslatorHelper XMLTH;

}

#endif /* XMLTRANSLATORHELPER_HPP_ */
