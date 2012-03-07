/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST__FILE_HPP__
#define __FWTEST__FILE_HPP__

#include <boost/filesystem/path.hpp>

#include "fwTest/Exception.hpp"
#include "fwTest/config.hpp"

namespace fwTest
{

class FWTEST_CLASS_API File
{
public:
    FWTEST_API static bool contentEquals(const ::boost::filesystem::path &lfile, const ::boost::filesystem::path &rfile);

};

} // namespace fwTest

#endif // __FWTEST__FILE_HPP__
