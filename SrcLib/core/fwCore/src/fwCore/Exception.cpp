
#include "fwCore/Exception.hpp"



namespace fwCore
{

Exception::Exception ( const std::string &err ) : std::runtime_error ( err )
{
}



} //namespace fwCore

