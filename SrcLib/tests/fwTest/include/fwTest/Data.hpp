/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST__DATA_HPP__
#define __FWTEST__DATA_HPP__

#include <boost/filesystem/path.hpp>

#include "fwTest/Exception.hpp"
#include "fwTest/config.hpp"

namespace fwTest
{

class FWTEST_CLASS_API Data
{
public:
    FWTEST_API static boost::filesystem::path dir() throw(fwTest::Exception);


    static const std::string s_TEST_DATA_DIR_ENV_VAR;
};

} // namespace fwTest

#endif // __FWTEST__DATA_HPP__
