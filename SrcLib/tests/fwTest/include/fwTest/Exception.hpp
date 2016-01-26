/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST_EXCEPTION_HPP__
#define __FWTEST_EXCEPTION_HPP__

#include "fwTest/config.hpp"

#include <stdexcept>
#include <string>

namespace fwTest
{


class FWTEST_CLASS_API Exception : public std::runtime_error
{

public:
    FWTEST_API Exception ( const std::string &err );

};




} // namespace fwTest

#endif // __FWTEST_EXCEPTION_HPP__
