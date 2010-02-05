#include <iostream>

#include "fwXML/policy/ListManagedPolicy.hpp"

namespace fwXML{

bool ListManagedPolicy::split(::boost::shared_ptr< ::fwTools::Object > obj)
{
    std::string className = obj->getLeafClassname();
    return m_classNames.find( className )!= m_classNames.end();

}

void ListManagedPolicy::addSplitClass(const std::string &name)
{
    m_classNames.insert( name );
}

} //namespace fwXML
