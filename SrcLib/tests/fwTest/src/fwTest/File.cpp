/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwTest/File.hpp"

#include <boost/filesystem/path.hpp>
#include <fstream>

namespace fwTest
{

bool File::contentEquals(const ::boost::filesystem::path &lfile, const ::boost::filesystem::path &rfile)
{
    using namespace std;
    const streambuf::int_type eof = streambuf::traits_type::eof();

    std::ifstream lstream(lfile.c_str());
    std::ifstream rstream(rfile.c_str());

    streambuf * lbuf = lstream.rdbuf();
    streambuf * rbuf = rstream.rdbuf();

    char lchar, rchar;
    while (true)
    {
        lchar = lbuf->sbumpc();
        rchar = rbuf->sbumpc();

        if (lchar == eof && rchar == eof)
        {
            return true;
        }

        if (lchar != rchar || lchar == eof || rchar == eof )
        {
            break;
        }
    }

    return false;
}

}  //namespace fwTest

