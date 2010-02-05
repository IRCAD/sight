#include "fwXML/policy/NeverSplitPolicy.hpp"

namespace fwXML
{

bool NeverSplitPolicy::split(::boost::shared_ptr< ::fwTools::Object > obj)
{
    return false;
}

}// namespace fwXML
