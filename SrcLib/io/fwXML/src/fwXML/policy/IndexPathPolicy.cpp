#include <sstream>

#include "fwXML/policy/IndexPathPolicy.hpp"

namespace fwXML
{

::boost::filesystem::path IndexPathPolicy::getPath(::boost::shared_ptr < ::fwTools::Object > obj)
{

    if ( m_index[obj] != 0 )
    {
        // index exist m_index[obj] correctly set
    }
    else
    {
        //if not, added to m_index, and increment m_lastIndexAvailable[obj->className()]
        m_index[obj] = m_lastIndexAvailable[ obj->getLeafClassname() ]++ ;
    }

    std::stringstream ss ;
    ss << obj->getLeafClassname() << "_" << m_index[obj] << ".xml"  ;

    return ::boost::filesystem::path(  ss.str() ) ;
}

}//namespace fwXML
