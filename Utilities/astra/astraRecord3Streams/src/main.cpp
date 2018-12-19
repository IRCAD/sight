/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include <OpenNI.h>

#include <iostream>

namespace po = ::boost::program_options;
namespace fs = ::boost::filesystem;

using namespace openni;

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{

    std::string folder;
    int dev = 0;
    Device device;
    try
    {
        po::options_description options("astraRecorder Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("rgb_dev,d", po::value<int>(&dev)->required()->default_value(0), "rgb camera device default: O")
            ("folder,f", po::value<std::string>(&folder)->required()->default_value("./astraRecord"),
            "set the output folder");

        po::positional_options_description pos;
        pos.add("folder", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        std::cout<<"-- Astra Record 3 Streams --"<<std::endl;
        std::cout<<"-- Utility to take a snapshot of IR, Depth and Color Image."<<std::endl;
        std::cout<<"-- Please make sure that only one (1) Astra is connected."<<std::endl;
        std::cout<<"-- COMMANDS:"<<std::endl;
        std::cout<<"-- * Press 'q' or ESC to quit "<<std::endl;
        std::cout<<"-- * Press 'r' or SPACE to take a snapshot."<<std::endl;
        std::cout<<"-- INFOS:"<<std::endl;
        std::cout<<"-- ! Since IR and RGB cameras cannot be streamed in same time, fps will be very slow."<<std::endl;
        std::cout<<"-- ! Saved image folder: " + folder <<std::endl;
        std::cout<<"-- ! RGB Camera dev: " << dev <<std::endl;
        std::cout<<"----------------------------"<<std::endl;

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
            OpenNI::shutdown();
            return EXIT_FAILURE;
        }

        rc = device.open(ANY_DEVICE);

        if (rc != STATUS_OK)
        {
            std::cerr<<"Couldn't open device"<< OpenNI::getExtendedError()<<std::endl;
            device.close();
            OpenNI::shutdown();
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
                infrared.setVideoMode(videoModes[5]);
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

        ::cv::VideoCapture rgbGrabber;
        VideoStream* streams[] = {&depth, &infrared};

        depth.setMirroringEnabled(false);
        infrared.setMirroringEnabled(false);
        ::cv::Mat rgbIm, dIm, irIm;

        int imageCount = 0;

        VideoFrameRef d, ir;

        bool hasDepth = false,  hasIR = false;

        while (true)
        {
            int readyStream = -1;
            rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, 100);

            if(rc == STATUS_TIME_OUT)
            {
                // if timeout grab color directly.
                hasDepth    = true;
                hasIR       = true;
                readyStream = -1;
            }
            else if (rc != STATUS_OK)
            {
                std::cerr<<OpenNI::getExtendedError()<<std::endl;
                break;
            }

            int key;

            if(readyStream == 0)
            {
                depth.readFrame(&d);
                const auto depthPixels = reinterpret_cast<const uint16_t*>(d.getData());

                dIm.create(d.getHeight(), d.getWidth(), CV_16UC1);
                memcpy( dIm.data, depthPixels, static_cast<size_t>(d.getDataSize()) );
                ::cv::imshow("Depth", dIm * 10);
                hasDepth = true;
                depth.stop();

            }
            else if(readyStream == 1)
            {
                infrared.readFrame(&ir);
                const auto infraredPixels = reinterpret_cast<const uint8_t*>(ir.getData());

                irIm.create(ir.getHeight(), ir.getWidth(), CV_8UC3);
                memcpy( irIm.data, infraredPixels, static_cast<size_t>(ir.getDataSize()) );
                ::cv::imshow("IR", irIm);
                hasIR = true;
                infrared.stop();
            }

            // Since we cannot access infrared and rgb streams at same time, we should open & close the color grabber
            // each time. This is very slow.
            if(hasDepth && hasIR)
            {
                rgbGrabber.open(dev);
                rgbGrabber.set(CV_CAP_PROP_FRAME_WIDTH, 640);
                rgbGrabber.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

                const bool isGrabbed = rgbGrabber.grab();

                if(isGrabbed)
                {
                    rgbGrabber.retrieve(rgbIm);

                    ::cv::imshow("RGB", rgbIm);
                }
                // release the color grabber, this allows IR with OpenNI.
                rgbGrabber.release();
                hasDepth = hasIR = false;

                //(re-)start OpenNI streams.
                infrared.start();
                depth.start();
            }

            key = ::cv::waitKey(5);

            if(key == 27 || key == 'q')
            {
                std::cout<<"quit..."<<std::endl;
                break;

            }

            if(key == 'r' || key == 32)
            {
                std::cout<<"snapshot of all 3 images ";

                const std::string num = std::to_string(imageCount);
                const std::string ext(".tiff");

                const ::fs::path depthFile = saveF / ("Depth_" + num + ext);
                const ::fs::path irFile    = saveF / ("Infrared_" + num + ext);
                const ::fs::path rgbFile   = saveF / ("Color_" + num + ext);

                ::cv::imwrite(depthFile.string(), dIm);
                std::cout<<". ";
                ::cv::imwrite(irFile.string(), irIm);
                std::cout<<". ";
                ::cv::imwrite(rgbFile.string(), rgbIm);
                std::cout<<". done! "<<std::endl;

                imageCount++;
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
        device.close();
        OpenNI::shutdown();
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "Unknown error" << std::endl;
        device.close();
        OpenNI::shutdown();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
