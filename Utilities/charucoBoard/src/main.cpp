/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/program_options.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

namespace po = boost::program_options;

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    int squareX, squareY, markerBits;
    size_t pX, pY;
    float squareLength, markerLength;
    ::cv::Size outSize;
    std::string file;
    try
    {
        po::options_description options("charucoBoard Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("squareX,x", po::value<int>(&squareX)->required()->default_value(7),
            "set the number of square on the board's width")
            ("squareY,y", po::value<int>(&squareY)->required()->default_value(5),
            "set the number of square on the board's height")
            ("pixelX,w", po::value<size_t>(&pX)->required()->default_value(1920), "set image witdh (in pixel)")
            ("pixelY,e", po::value<size_t>(&pY)->required()->default_value(1080), "set image height (in pixel)")
            ("squareLength,s", po::value<float>(&squareLength)->required()->default_value(10),
            "set the chessboard square side length")
            ("markerLength,m", po::value<float>(&markerLength)->required()->default_value(8),
            "set the markers side length")
            ("markerBits,b", po::value<int>(&markerBits)->required()->default_value(6), "set the marker size in bits")
            ("file,f", po::value<std::string>(&file)->required()->default_value("charucoBoard_5_7.jpeg"),
            "set the output file");

        po::positional_options_description pos;
        pos.add("squareX", 1).add("squareY", 1).add("pixelX", 1).add("pixelY", 1).add("squareLength", 1).add(
            "markerLength", 1).add("markerBits", 1).add("file", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        if (vm.count("help"))
        {
            std::cout << "usage: " << argv[0] <<
                " nmberOfSquarePerLength nmberOfSquarePerWidth imageWitdhInPixels imageHeightInPixels squareSize markerSize outfile.(jpg|png|tiff|...) "
                      << std::endl;
            std::cout << options << std::endl;
            return 0;
        }
        po::notify(vm);

        outSize = ::cv::Size(pX, pY);

        ::cv::Mat boardImg;

        const int nbMarkers = ( squareX * squareY ) / 2;

        //Determine which dictionary to use
        ::cv::aruco::PREDEFINED_DICTIONARY_NAME dictionaryName;
        if(markerBits == 4)
        {
            if(nbMarkers <= 50)
            {
                dictionaryName = ::cv::aruco::DICT_4X4_50;
            }
            else if(nbMarkers <= 100)
            {
                dictionaryName = ::cv::aruco::DICT_4X4_100;
            }
            else if(nbMarkers <= 250)
            {
                dictionaryName = ::cv::aruco::DICT_4X4_250;
            }
            else
            {
                dictionaryName = ::cv::aruco::DICT_4X4_1000;
            }
        }
        else if(markerBits == 5)
        {
            if(nbMarkers <= 50)
            {
                dictionaryName = ::cv::aruco::DICT_5X5_50;
            }
            else if(nbMarkers <= 100)
            {
                dictionaryName = ::cv::aruco::DICT_5X5_100;
            }
            else if(nbMarkers <= 250)
            {
                dictionaryName = ::cv::aruco::DICT_5X5_250;
            }
            else
            {
                dictionaryName = ::cv::aruco::DICT_5X5_1000;
            }
        }
        else if(markerBits == 6)
        {
            if(nbMarkers <= 50)
            {
                dictionaryName = ::cv::aruco::DICT_6X6_50;
            }
            else if(nbMarkers <= 100)
            {
                dictionaryName = ::cv::aruco::DICT_6X6_100;
            }
            else if(nbMarkers <= 250)
            {
                dictionaryName = ::cv::aruco::DICT_6X6_250;
            }
            else
            {
                dictionaryName = ::cv::aruco::DICT_6X6_1000;
            }
        }
        else if(markerBits == 7)
        {
            if(nbMarkers <= 50)
            {
                dictionaryName = ::cv::aruco::DICT_7X7_50;
            }
            else if(nbMarkers <= 100)
            {
                dictionaryName = ::cv::aruco::DICT_7X7_100;
            }
            else if(nbMarkers <= 250)
            {
                dictionaryName = ::cv::aruco::DICT_7X7_250;
            }
            else
            {
                dictionaryName = ::cv::aruco::DICT_7X7_1000;
            }
        }
        else
        {
            std::cerr<<"Cannot generate dictionary with marker size of :"<<markerBits<<std::endl;
            return EXIT_FAILURE;
        }

        std::cout<<"Generating board with dictionary of type: "<<dictionaryName<<std::endl;

        ::cv::Ptr< ::cv::aruco::Dictionary > dictionary = ::cv::aruco::generateCustomDictionary(nbMarkers, markerBits,
                                                                                                ::cv::aruco::getPredefinedDictionary(
                                                                                                    dictionaryName));

        cv::Ptr< ::cv::aruco::CharucoBoard > board = ::cv::aruco::CharucoBoard::create(squareX, squareY, squareLength,
                                                                                       markerLength, dictionary);

        board->draw(outSize, boardImg);

        ::cv::imwrite(file, boardImg);
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
