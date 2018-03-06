/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/program_options.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

namespace po = boost::program_options;

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    int id, size, borderBits;
    std::string file;
    try
    {
        po::options_description options("arucoMarker Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("makerid,m", po::value<int>(&id)->required()->default_value(100), "set the marker ID (0:1023)")
            ("file,f", po::value<std::string>(&file)->required()->default_value("marker_100.jpeg"),
            "set the output file")
            ("size,s", po::value<int>(&size)->required()->default_value(300), "set the size in pixels for the marker")
            ("border,b", po::value<int>(&borderBits)->default_value(1), "width of the marker border");

        po::positional_options_description pos;
        pos.add("makerid", 1).add("file", 1).add("size", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        if (vm.count("help"))
        {
            std::cout << "usage: " << argv[0] << " makerID outfile.(jpg|png|tiff|...) sizeInPixels" << std::endl;
            std::cout << options << std::endl;
            return 0;
        }
        po::notify(vm);

        ::cv::Mat marker;

        ::cv::Ptr< ::cv::aruco::Dictionary > dictionary;
        //Use the aruco original dictionary
        dictionary = ::cv::aruco::Dictionary::get(::cv::aruco::DICT_ARUCO_ORIGINAL);

        ::cv::aruco::drawMarker(dictionary, id, size, marker, borderBits);

        ::cv::imwrite(file, marker);
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Unknown error" << std::endl;
        return 0;
    }
    return 1;
}
