#include "fwXML/policy/AlwaysSplitPolicy.hpp"

namespace fwXML
{

bool AlwaysSplitPolicy::split(::boost::shared_ptr< ::fwTools::Object > obj)
{
    return true;
}

} //namespace fwXML
