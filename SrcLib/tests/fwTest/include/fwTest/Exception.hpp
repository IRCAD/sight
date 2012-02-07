/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST__EXCEPTION_HPP__
#define __FWTEST__EXCEPTION_HPP__

#include <stdexcept>
#include <string>

#include "fwTest/config.hpp"

namespace fwTest
{


class FWTEST_CLASS_API Exception : public std::runtime_error
{

public:
    FWTEST_API Exception ( const std::string &err );

};




} // namespace fwTest

#endif // __FWTEST__EXCEPTION_HPP__
