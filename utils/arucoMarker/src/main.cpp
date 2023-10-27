/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <boost/program_options.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

namespace po = boost::program_options;

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    int id          = 0;
    int size        = 0;
    int border_bits = 0;
    std::string file;
    try
    {
        po::options_description options("arucoMarker Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("makerid,m", po::value<int>(&id)->required()->default_value(100), "set the marker ID (0:1023)")
        (
            "file,f",
            po::value<std::string>(&file)->required()->default_value("marker_100.jpeg"),
            "set the output file"
        )
            ("size,s", po::value<int>(&size)->required()->default_value(300), "set the size in pixels for the marker")
            ("border,b", po::value<int>(&border_bits)->default_value(1), "width of the marker border");

        po::positional_options_description pos;
        pos.add("makerid", 1).add("file", 1).add("size", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        if(vm.count("help") != 0U)
        {
            std::cout << "usage: " << argv[0] << " makerID outfile.(jpg|png|tiff|...) sizeInPixels" << std::endl;
            std::cout << options << std::endl;
            return 0;
        }

        po::notify(vm);

        cv::Mat marker;

#if CV_MAJOR_VERSION > 4 || (CV_MAJOR_VERSION == 4 && CV_VERSION_MINOR > 5)
        //Use the aruco original dictionary
        const auto& dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);

        cv::aruco::generateImageMarker(dictionary, id, size, marker, border_bits);
#else
        //Use the aruco original dictionary
        const auto& dictionary = cv::aruco::Dictionary::get(cv::aruco::DICT_ARUCO_ORIGINAL);

        cv::aruco::drawMarker(dictionary, id, size, marker, border_bits);
#endif

        cv::imwrite(file, marker);
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
