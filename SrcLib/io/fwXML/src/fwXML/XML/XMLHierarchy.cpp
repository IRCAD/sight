#include "fwXML/XML/XMLHierarchy.hpp"


//IMPLEMENT_SINGLETON( fwXML::XMLHierarchy );
// WINSUX
::boost::shared_ptr< fwXML::XMLHierarchy > fwXML::XMLHierarchy::m_ClassInstance = ::boost::shared_ptr< fwXML::XMLHierarchy >();


namespace fwXML
{

XMLHierarchy::XMLHierarchy()
{
}

XMLHierarchy::~XMLHierarchy()
{
}



XMLHierarchy::ChildFatherMap &
XMLHierarchy::mapChildFather()
{
    return m_mapChildFather;
}

XMLHierarchy::FatherChildrenMap &
XMLHierarchy::mapFatherChildren()
{
    return m_mapFatherChildren;
}



XMLHierarchy::ObjectXMLNodeMap &
XMLHierarchy::mapObjectXMLNode()
{
    return m_mapObjectXMLNode;
}



XMLHierarchy::ObjectAggregatorMap &
XMLHierarchy::mapObjectAggregator()
{
    return m_mapObjectAggregator;
}


void XMLHierarchy::clearAll()
{
    m_mapChildFather.clear();
    m_mapFatherChildren.clear();
    m_mapObjectXMLNode.clear();
    m_mapObjectAggregator.clear();
    }


}
