#include <sstream>

#include <fwTools/UUID.hpp>




#include "fwXML/policy/DefaultPathPolicy.hpp"

namespace fwXML
{

::boost::filesystem::path DefaultPathPolicy::getPath(::boost::shared_ptr < ::fwTools::Object > obj )
{
    std::stringstream ss ;
    ss << obj->getLeafClassname() << "_" << ::fwTools::UUID::get(obj,::fwTools::UUID::EXTENDED) << ".xml";

    return  boost::filesystem::path ( ss.str() ) ;
}

}//namespace fwXML
