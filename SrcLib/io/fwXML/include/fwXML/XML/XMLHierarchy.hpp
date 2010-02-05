#ifndef XMLHIERARCHY_HPP_
#define XMLHIERARCHY_HPP_

#include <map>

#include <libxml/tree.h>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"
#include "fwXML/XML/XMLAggregator.hpp"


namespace fwXML
{

/**
 * @brief the purpose of this class is to *only* store information about topology on object hierarchy and XML representation
 * @note this class is a singleton
 * The class store information about topology
 * \li keep mapping between object and its father
 * \li keep mapping between object anf its xml representation
 * \li keep mapping betwen  object and the Aggregator which contain its xml representation
 * These informations are to be filled by other classes aka . Visitor which discover father & XMLPartionner which choose the
 * aggregation unit for each XML class
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLHierarchy // WINSUW : public Singleton< XMLHierarchy >
{
public:

    typedef std::map< ::boost::weak_ptr< fwTools::Object >, ::boost::weak_ptr< fwTools::Object > >                      ChildFatherMap;
    typedef std::map< ::boost::weak_ptr< fwTools::Object >, std::set< ::boost::weak_ptr< fwTools::Object > >  >         FatherChildrenMap;
    typedef std::map< ::boost::weak_ptr< fwTools::Object >, xmlNodePtr>                                     ObjectXMLNodeMap;
    typedef std::map< ::boost::weak_ptr< fwTools::Object >, ::boost::shared_ptr<XMLAggregator>  >                   ObjectAggregatorMap;


    // WINSUX friend class Singleton< XMLHierarchy >;
        FWXML_API static ::boost::shared_ptr< XMLHierarchy > getDefault()
    {
        if ( m_ClassInstance.get() == NULL )
        {
            m_ClassInstance = ::boost::shared_ptr< XMLHierarchy >( new XMLHierarchy );
        }
        return m_ClassInstance;
    }

    FWXML_API virtual ~XMLHierarchy();

    /**
     * @brief get a reference on map child->father
     */
    FWXML_API  ChildFatherMap & mapChildFather();

    /**
      * @brief get a reference on map father -> its children
      */
    FWXML_API  FatherChildrenMap & mapFatherChildren();

    /**
     * @brief get a reference on map object * -> xmlNodePtr
     */
    FWXML_API  ObjectXMLNodeMap & mapObjectXMLNode();

    /**
     * @brief get a reference on map xmlNodePtr -> shared pointer IXMLAggregator
     */
    FWXML_API  ObjectAggregatorMap & mapObjectAggregator();


    /**
     * @brief clear all container
     */
    FWXML_API void clearAll();



protected :

    FWXML_API XMLHierarchy();

    /**
     * @brief informe about hierarchical father of each object. To be filled by visitor
     * @param Key is the child ( type ::boost::weak_ptr fwTools::Object )
     * @param Value is  its father ( type ::boost::weak_ptr fwTools::Object ), NULL if none
     */
    ChildFatherMap m_mapChildFather;


    /**
     * @brief informe about hierarchical children of each object. To be filled by visitor
     * @param Key is the child ( type ::boost::weak_ptr fwTools::Object )
     * @param Value is  its children ( type set of ::boost::weak_ptr fwTools::Object ), empty set if none
     */
    FatherChildrenMap m_mapFatherChildren;

    /**
     * @brief map between object pointer & xml representation
     * @param Key is a fwTools::Object  weak pointer
     * @param value is a xml Node Pointer
     */
    ObjectXMLNodeMap m_mapObjectXMLNode;

    /**
     * @brief map between an Object and the aggregator which contain its xml node
     * @param Key is a data Object pointer  weak Pointer
     * @param value is a xml XMLAggregator smart Pointer
     */
    ObjectAggregatorMap m_mapObjectAggregator;

    //WINSUX
    FWXML_API static ::boost::shared_ptr< XMLHierarchy > m_ClassInstance;


};

}

#endif /*XMLHIERARCHY_HPP_*/
