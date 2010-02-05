#include <sstream>

#include "fwXML/policy/UniquePathPolicy.hpp"

namespace fwXML
{

UniquePathPolicy::UniquePathPolicy(const std::string &filename)
: m_uniquePath ( filename ) 
{}

UniquePathPolicy::~UniquePathPolicy()
{}

void UniquePathPolicy::setUniquePath(const std::string &filename)
{
    m_uniquePath = filename;
}

::boost::filesystem::path UniquePathPolicy::getPath(::boost::shared_ptr < ::fwTools::Object > obj )
{
     return m_uniquePath;
}

}//namespace fwXML
