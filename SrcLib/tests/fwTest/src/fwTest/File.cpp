
#include <fstream>
#include <boost/filesystem/path.hpp>
#include "fwTest/File.hpp"

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

}; //namespace fwTest

