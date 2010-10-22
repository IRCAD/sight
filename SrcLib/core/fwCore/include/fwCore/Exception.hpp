
#ifndef _FWCORE_EXCEPTION_HPP_
#define _FWCORE_EXCEPTION_HPP_


#include <stdexcept>
#include <string>

#include <boost/exception/exception.hpp>

#include "fwCore/config.hpp"

namespace fwCore
{


class FWCORE_CLASS_API Exception : public std::runtime_error
{

public:
    FWCORE_API Exception ( const std::string err );

};

} //namespace fwCore

#endif //_FWCORE_EXCEPTION_FWEXCEPTION_HPP_
