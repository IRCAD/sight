/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <filesystem>

#include <opencv2/opencv.hpp>

#include <OpenNI.h>

#include <iostream>

namespace po = ::boost::program_options;
namespace fs = std::filesystem;

using namespace openni;

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{

    std::string folder;
    int dev = 0;
    try
    {
        po::options_description options("astraRecorder Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("rgb_dev,d", po::value<int>(&dev)->required()->default_value(0), "rgb camera device default: O")
            ("folder,f", po::value<std::string>(&folder)->required()->default_value("./astraRecord"),
            "set the output folder");

        po::positional_options_description pos;
        pos.add("folder", 1).add("rgb_dev", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        std::cout<<"-- Astra Viewer --"<<std::endl;
        std::cout<<"-- Utility to view Depth and infrared or color images."<<std::endl;
        std::cout<<"-- Please make sure that only one (1) Astra is connected."<<std::endl;
        std::cout<<"-- COMMANDS:"<<std::endl;
        std::cout<<"-- * Press 'q' or ESC to quit."<<std::endl;
        std::cout<<"-- * Press 'r' or SPACE to take a snapshot."<<std::endl;
        std::cout<<"-- * Press 'i' to switch between infrared and color (default) stream."<<std::endl;
        std::cout<<"-- * Press 'm' to merge infrared/color into depth."<<std::endl;
        std::cout<<"-- * Press 'p' to pause/play."<<std::endl;
        std::cout<<"-- INFOS:"<<std::endl;
        std::cout<<"-- ! Saved image folder: " + folder <<std::endl;
        std::cout<<"-- ! RGB Camera dev: " << dev <<std::endl;
        std::cout<<"------------------"<<std::endl;

        if (vm.count("help"))
        {
            std::cout << "usage: " << argv[0] << "" << std::endl;
            std::cout << options << std::endl;
            return EXIT_FAILURE;
        }

        po::notify(vm);

        const ::fs::path saveF = folder;

        // Check if output directory exists.
        if(!::fs::exists(saveF) || !::fs::is_directory(saveF))
        {
            std::cerr<<"Output directory: '"<< saveF <<"' didn't exists, it will be created."<<std::endl;
            ::fs::create_directory(saveF);
        }

        Status rc = OpenNI::initialize();

        if (rc != STATUS_OK)
        {
            std::cerr<<"Initialize failed" << OpenNI::getExtendedError() <<std::endl;
            return 1;
        }

        Device device;
        rc = device.open(ANY_DEVICE);
        device.setImageRegistrationMode(ImageRegistrationMode::IMAGE_REGISTRATION_DEPTH_TO_COLOR);

        if (rc != STATUS_OK)
        {
            std::cerr<<"Couldn't open device"<< OpenNI::getExtendedError()<<std::endl;
            return EXIT_FAILURE;
        }

        VideoStream depth, infrared;

        if (device.getSensorInfo(SENSOR_DEPTH) != nullptr)
        {
            rc = depth.create(device, SENSOR_DEPTH);
            if (rc == STATUS_OK)
            {
                rc = depth.start();
                VideoMode modeDepth;
                // Force 640 x 480.
                modeDepth.setResolution( 640, 480 );
                modeDepth.setFps( 30 );
                modeDepth.setPixelFormat( PIXEL_FORMAT_DEPTH_1_MM );
                depth.setVideoMode(modeDepth);

                if (rc != STATUS_OK)
                {
                    std::cerr<< "Couldn't start the color stream" << OpenNI::getExtendedError() << std::endl;
                }
            }
            else
            {
                std::cerr<< "Couldn't create depth stream" << OpenNI::getExtendedError() <<std::endl;
            }
        }

        if (device.getSensorInfo(SENSOR_IR) != nullptr)
        {
            rc = infrared.create(device, SENSOR_IR);
            if (rc == STATUS_OK)
            {

                auto const& videoModes = device.getSensorInfo(SENSOR_IR)->getSupportedVideoModes();
                infrared.setVideoMode(videoModes[5]); //640*480, 30 fps, RGB888.
                rc = infrared.start();
                if (rc != STATUS_OK)
                {
                    std::cerr<< "Couldn't start the infrared stream"<< OpenNI::getExtendedError() << std::endl;
                }
            }
            else
            {
                std::cerr<<"Couldn't create infrared stream"<< OpenNI::getExtendedError()<< std::endl;
            }
        }

        VideoStream* streams[] = {&depth, &infrared};

        depth.setMirroringEnabled(false);
        infrared.setMirroringEnabled(false);
        ::cv::Mat dIm, ircolorIm, depthToDisplay;

        int imageCount = 0;

        ::cv::VideoCapture rgbGrabber;

        bool color = true;
        bool pause = false;
        bool merge = false;

        while (true)
        {
            VideoFrameRef d, ir;
            int key;
            if(!pause)
            {
                int readyStream = -1;
                rc = OpenNI::waitForAnyStream(streams, 2, &readyStream);
                if (rc != STATUS_OK)
                {
                    std::cerr<<OpenNI::getExtendedError()<<std::endl;
                    break;
                }

                if(readyStream == 0)
                {
                    depth.readFrame(&d);

                    const auto depthPixels = reinterpret_cast<const uint16_t*>(d.getData());

                    dIm.create(d.getHeight(), d.getWidth(), CV_16UC1);
                    memcpy( dIm.data, depthPixels, static_cast<size_t>(d.getDataSize()) );

                    // Convert depth for display.
                    dIm.convertTo( depthToDisplay, CV_8UC3, 255.0 / (8000) );

                    // Blend images.
                    if(merge)
                    {
                        ::cv::Mat mergeIm, gray, dc3;
                        ::cv::cvtColor(ircolorIm, gray, CV_RGB2GRAY);
                        ::cv::cvtColor(depthToDisplay, dc3, CV_GRAY2BGR);
                        ::cv::addWeighted(dc3, 0.5, ircolorIm, 0.5, 0, mergeIm);
                        ::cv::imshow("Depth", mergeIm);
                    }
                    else
                    {
                        ::cv::imshow("Depth", depthToDisplay);
                    }

                }
                else if(readyStream == 1)
                {
                    infrared.readFrame(&ir);
                    const auto infraredPixels = reinterpret_cast<const uint8_t*>(ir.getData());

                    ircolorIm.create(ir.getHeight(), ir.getWidth(), CV_8UC3);
                    memcpy( ircolorIm.data, infraredPixels, static_cast<size_t>(ir.getDataSize()) );
                    ::cv::imshow("IR / Color", ircolorIm);
                }

                if(color)
                {
                    infrared.stop();
                    // if color was closed, re-open it
                    if(!rgbGrabber.isOpened())
                    {
                        rgbGrabber.open(dev);
                        // Force 640* 480.
                        rgbGrabber.set(CV_CAP_PROP_FRAME_WIDTH, 640);
                        rgbGrabber.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
                    }

                    const bool isGrabbed = rgbGrabber.grab();
                    if(isGrabbed)
                    {
                        rgbGrabber.retrieve(ircolorIm);
                        ::cv::imshow("IR / Color", ircolorIm);
                    }
                }
                else
                {
                    rgbGrabber.release();
                    infrared.start();
                }

            }

            // Check keyboard inputs.
            key = ::cv::waitKey(10);

            // Exit.
            if(key == 27 || key == 'q')
            {
                std::cout<<"quit..."<<std::endl;
                break;

            }
            // Snapshot.
            else if(key == 's' || key == 32)
            {
                std::cout<<"snapshot of all the 2 displayed streams ";

                const std::string num = std::to_string(imageCount);
                const std::string ext(".tiff");

                const ::fs::path depthFile = saveF / ("Depth_" + num + ext);

                ::fs::path irOrColorFile;
                if(!color)
                {
                    irOrColorFile = saveF / ("Infrared_" + num + ext);
                }
                else
                {
                    irOrColorFile = saveF / ("Color_" + num + ext);
                }

                ::cv::imwrite(depthFile.string(), dIm);
                std::cout<<". ";
                ::cv::imwrite(irOrColorFile.string(), ircolorIm);
                std::cout<<". done! "<<std::endl;

                imageCount++;

            }
            // Switch infrared/color.
            else if(key == 'i')
            {
                color = !color;
            }
            // Pause/Play.
            else if(key == 'p')
            {
                pause = !pause;
            }
            // Blend depth and infrared/color.
            else if(key == 'm')
            {
                merge = !merge;
            }
        }

        std::cout<<"shutdown..."<<std::endl;

        depth.stop();
        infrared.stop();
        depth.destroy();
        infrared.destroy();
        device.close();
        OpenNI::shutdown();

    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "Unknown error" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
