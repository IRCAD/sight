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

#include <iostream>
#include <random>

namespace po = ::boost::program_options;
namespace fs = ::boost::filesystem;

//------------------------------------------------------------------------------

::cv::Mat rotMat(const double, const double, const double);
::cv::Mat generateIntrinsicCalibration(const ::cv::Size&);
std::vector< ::cv::Mat > generateExtrinsicCalibration();
std::vector< ::cv::Mat > generatePositionAndOrientationOfChessboard(const size_t, const ::cv::Mat&, const ::cv::Mat&,
                                                                    const ::cv::Mat&);
void generatePhoto(const ::cv::Mat&, const ::cv::Mat&, const std::vector< ::cv::Mat >&, const ::cv::Size&, const double,
                   const std::string&, const fs::path&, const size_t nCam);

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    size_t nImg;
    int pX, pY;
    std::string file, outFolder;
    double sizeImgInmm;
    try
    {
        po::options_description options("calDataGenerator Usage");
        options.add_options()
            ("help,h", "produce help message")
            ("pixelX,x", po::value<int>(&pX)->required()->default_value(1920), "set image width (in pixel)")
            ("pixelY,y", po::value<int>(&pY)->required()->default_value(1080), "set image height (in pixel)")
            ("nImg,n", po::value<size_t>(&nImg)->required()->default_value(20), "set number of image to produce")
            ("sizeInmm,s", po::value<double>(&sizeImgInmm)->required()->default_value(320),
            "set total width of board in mm")
            ("file,f", po::value<std::string>(&file)->required()->default_value("charucoBoard_13_9.bmp"),
            "set the input file")
            ("outFolder,o", po::value<std::string>(&outFolder)->required()->default_value("./test"),
            "set the output folder");

        po::positional_options_description pos;
        pos.add("pixelX", 1).add("pixelY", 1).add("nImg", 1).add("sizeInmm", 1).add("file", 1).add("outFolder", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);

        if (vm.count("help") || argc == 1)
        {
            std::cout << "usage: " << argv[0] <<
                " outputImageWidth outputImageHeight numberOfImageToProduce sizeImgInmm infile.(jpg|png|tiff|...) outputFolder "
                      << std::endl;
            std::cout << options << std::endl;
            return EXIT_SUCCESS;
        }
        po::notify(vm);

        // Check if output directory exists.
        if(!fs::exists(outFolder) || !fs::is_directory(outFolder))
        {
            std::cerr<<"Output directory: '"<< outFolder <<"' didn't exists, it will be created."<<std::endl;
            fs::create_directory(outFolder);
        }

        // Create cam1 and cam2 folders.
        const fs::path cam1folder = outFolder + "/cam1";
        const fs::path cam2folder = outFolder + "/cam2";

        if(!fs::exists(cam1folder) || !fs::is_directory(cam1folder))
        {
            std::cerr<<"cam1 directory: '"<< cam1folder.string() <<"' didn't exists, it will be created."<<std::endl;
            fs::create_directory(cam1folder);
        }

        if(!fs::exists(cam2folder) || !fs::is_directory(cam2folder))
        {
            std::cerr<<"cam2 directory: '"<< cam2folder.string() <<"' didn't exists, it will be created."<<std::endl;
            fs::create_directory(cam2folder);
        }

        ::cv::Mat K1, K2, c0 = ::cv::Mat::zeros(3, 1, CV_64F), c1, T0 = ::cv::Mat::eye(4, 4, CV_64F);
        ::cv::Size imSize(pX, pY);
        std::vector< ::cv::Mat > centAndT;
        std::vector< ::cv::Mat > Tc;

        K1 = generateIntrinsicCalibration(imSize);
        K2 = generateIntrinsicCalibration(imSize);

        centAndT = generateExtrinsicCalibration();

        // Save generated calibration in a file.
        std::ofstream out;
        out.open(outFolder + "/Calibration.txt");
        out<<"K1 :"<<std::endl<<K1<<std::endl;
        out<<"K2 :"<<std::endl<<K2<<std::endl;
        out<<"T :"<<std::endl<<centAndT[1]<<std::endl;
        out.close();

        // Print calibration.
        std::cout<<"K1 :"<<std::endl<<K1<<std::endl;
        std::cout<<"K2 :"<<std::endl<<K2<<std::endl;
        std::cout<<"T :"<<std::endl<<centAndT[1]<<std::endl;

        c1 = -(centAndT[1].rowRange(0, 3).colRange(0, 3)).t() * centAndT[1].rowRange(0, 3).col(3);
        Tc = generatePositionAndOrientationOfChessboard(nImg, c0, c1, centAndT[0]);
        std::cout<<std::endl<<std::endl;
        generatePhoto(K1, T0, Tc, imSize, sizeImgInmm, file, cam1folder, 1);
        std::cout<<std::endl;
        generatePhoto(K2, centAndT[1], Tc, imSize, sizeImgInmm, file, cam2folder, 2);

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

//--------------------------------------------------------------------

::cv::Mat rotMat(const double theta, const double phi, const double psi)
{
    ::cv::Mat Rx, Ry, Rz;
    Rx = ::cv::Mat::zeros(3, 3, CV_64F);
    Ry = ::cv::Mat::zeros(3, 3, CV_64F);
    Rz = ::cv::Mat::zeros(3, 3, CV_64F);

    Rz.at<double>(0, 0) = cos(psi);
    Rz.at<double>(0, 1) = -sin(psi);
    Rz.at<double>(1, 0) = sin(psi);
    Rz.at<double>(1, 1) = cos(psi);
    Rz.at<double>(2, 2) = 1;

    Ry.at<double>(0, 0) = cos(phi);
    Ry.at<double>(0, 2) = sin(phi);
    Ry.at<double>(2, 0) = -sin(phi);
    Ry.at<double>(2, 2) = cos(phi);
    Ry.at<double>(1, 1) = 1;

    Rx.at<double>(1, 1) = cos(theta);
    Rx.at<double>(1, 2) = -sin(theta);
    Rx.at<double>(2, 1) = sin(theta);
    Rx.at<double>(2, 2) = cos(theta);
    Rx.at<double>(0, 0) = 1;

    return(Rz*Ry*Rx);
}

//--------------------------------------------------------------------

::cv::Mat generateIntrinsicCalibration(const ::cv::Size& imSize)
{
    // Generate an artificial intrinsic calibration given in mm, with a random focal centered around 20mm.
    std::random_device generator;
    std::normal_distribution<double> distribution(0, 1);

    const double d       = 0.03*1280.0/imSize.width;
    const double f       = distribution(generator) + 18;
    const double alpha_u = f/d;
    const double alpha_v = alpha_u + distribution(generator)*4;

    ::cv::Mat K = ::cv::Mat::zeros(3, 3, CV_64F);

    K.at<double>(0, 0) = alpha_u;
    K.at<double>(1, 1) = alpha_v;
    K.at<double>(0, 2) = static_cast<int>(imSize.width)/2;
    K.at<double>(1, 2) = static_cast<int>(imSize.height)/2;
    K.at<double>(2, 2) = 1;

    return K;
}

//--------------------------------------------------------------------

std::vector< ::cv::Mat > generateExtrinsicCalibration()
{
    // Generate an artificial extrinsic matrix given in mm, with a rotation around the x axis
    // of angle randomly chosen in the range : [pi/8;pi/4].
    std::random_device generator;
    std::normal_distribution<double> normDistribution(0, 1);
    std::uniform_real_distribution<double> unifDistribution(0, 1);

    ::cv::Mat R, Rt, T         = ::cv::Mat::zeros(4, 4, CV_64F);
    ::cv::Mat centerOfRotation = ::cv::Mat::zeros(3, 1, CV_64F), c;
    const double theta = unifDistribution(generator) * CV_PI/8 + CV_PI/8;
    std::vector< ::cv::Mat > returnObj;

    R = rotMat(theta, 0, 0);
    ::cv::transpose(R, Rt);
    centerOfRotation.at<double>(2, 0) = 470 + normDistribution(generator)*2;

    c = -R*centerOfRotation + centerOfRotation;
    c = -Rt*c;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            T.at<double>(i, j) = Rt.at<double>(i, j);
        }
        T.at<double>(i, 3) = c.at<double>(i, 0);
    }

    T.at<double>(3, 3) = 1;

    returnObj.push_back(centerOfRotation);
    returnObj.push_back(T);
    return returnObj;

}

//------------------------------------------------------------------------------

std::vector< ::cv::Mat > generatePositionAndOrientationOfChessboard(const size_t nImg, const ::cv::Mat& c1,
                                                                    const ::cv::Mat& c2,
                                                                    const ::cv::Mat& centerOfRotation)
{
    std::random_device generator;
    std::normal_distribution<double> normDistribution(0, 1);
    std::vector< ::cv::Mat > returnObj;

    ::cv::Mat directionVector, centers = ::cv::Mat::zeros(3, 1, CV_64F), x0, y0, R0 = ::cv::Mat::zeros(3, 3, CV_64F), R,
    Rt, temp;

    directionVector = centerOfRotation -(c2+c1)/2;
    directionVector = directionVector / ::cv::norm(directionVector);

    x0                  = (c2 - c1)/ ::cv::norm(c2-c1);
    x0                  = x0 - directionVector * directionVector.dot(x0);
    x0                  = x0/ ::cv::norm(x0);
    y0                  = directionVector.cross(x0);
    R0.at<double>(0, 0) = x0.at<double>(0, 0);
    R0.at<double>(1, 0) = x0.at<double>(1, 0);
    R0.at<double>(2, 0) = x0.at<double>(2, 0);
    R0.at<double>(0, 1) = y0.at<double>(0, 0);
    R0.at<double>(1, 1) = y0.at<double>(1, 0);
    R0.at<double>(2, 1) = y0.at<double>(2, 0);
    R0.at<double>(0, 2) = directionVector.at<double>(0, 0);
    R0.at<double>(1, 2) = directionVector.at<double>(1, 0);
    R0.at<double>(2, 2) = directionVector.at<double>(2, 0);

    for(size_t k = 0; k < nImg; k++)
    {
        ::cv::Mat T = ::cv::Mat::zeros(4, 4, CV_64F);
        const double angles[3] = {normDistribution(generator) * CV_PI/10,
                                  normDistribution(generator) * CV_PI/10,
                                  normDistribution(generator) * CV_PI/10};
        R = R0*rotMat(angles[0], angles[1], angles[2]);
        ::cv::invert(R, Rt);
        centers.at<double>(0, 0) =
            centerOfRotation.at<double>(2, 0)/10*normDistribution(generator) + centerOfRotation.at<double>(0, 0);
        centers.at<double>(1, 0) =
            centerOfRotation.at<double>(2, 0)/10*normDistribution(generator) + centerOfRotation.at<double>(1, 0);
        centers.at<double>(2, 0) = centerOfRotation.at<double>(2, 0)/10*normDistribution(generator);
        temp                     = -Rt* (centerOfRotation + centers);

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                T.at<double>(i, j) = Rt.at<double>(i, j);
            }
            T.at<double>(i, 3) = temp.at<double>(i, 0);
        }
        T.at<double>(3, 3) = 1;
        returnObj.push_back(T);

    }
    return returnObj;
}

//-----------------------------------------------------------------------------

void generatePhoto(const cv::Mat& K, const cv::Mat& T0, const std::vector<cv::Mat>& T1, const cv::Size& imgSize,
                   const double sizeImgInmm, const std::string& boardFile, const fs::path& outFolder,
                   const size_t nCam)
{
    const ::cv::Mat inImgt = ::cv::imread(boardFile);
    ::cv::Mat inImg;
    ::cv::cvtColor(inImgt, inImg, CV_RGB2GRAY);
    ::cv::GaussianBlur(inImg, inImg, ::cv::Size(5, 5), 1);

    ::cv::Mat R0(T0.colRange(0, 3).rowRange(0, 3)), c0(T0.col(3).rowRange(0, 3)), cameraCenter;
    cameraCenter = -R0.t()*c0;

    ::cv::Mat x0((T0.row(0).colRange(0, 3)).t()),
    y0((T0.row(1).colRange(0, 3)).t()),
    z0((T0.row(2).colRange(0, 3)).t());
    for(size_t k = 0; k < T1.size(); k++)
    {

        ::cv::Mat photo = ::cv::Mat::ones(imgSize.height, imgSize.width, CV_8U)*170;
        ::cv::Mat pl    = ::cv::Mat::zeros(4, 1, CV_64F);

        ::cv::Mat ray, A, sol, solH, B, dampos, imgOffset;
        double tempxg, tempxd;

        A                          = ::cv::Mat::zeros(4, 4, CV_64F);
        B                          = ::cv::Mat::zeros(4, 1, CV_64F);
        solH                       = ::cv::Mat::ones(4, 1, CV_64F);
        imgOffset                  = ::cv::Mat::zeros(4, 1, CV_64F);
        imgOffset.at<double>(0, 0) = inImg.size[1]/2;
        imgOffset.at<double>(1, 0) = inImg.size[0]/2;

        pl.at<double>(2, 0) = 1;
        pl                  = T1[k].t()*pl;
        for(int i = 0; i < imgSize.height; i++)
        {
            for(int j = 0; j < imgSize.width; j++)
            {
                // Ray vector in mm.
                ray = z0
                      + 1/K.at<double>(0, 0)*x0*(j-K.at<double>(0, 2))
                      + 1/K.at<double>(1, 1)*y0*(i-K.at<double>(1, 2));

                // System of equation.
                A.at<double>(0, 0) = pl.at<double>(0, 0);
                A.at<double>(0, 1) = pl.at<double>(1, 0);
                A.at<double>(0, 2) = pl.at<double>(2, 0);
                A.at<double>(1, 0) = 1;
                A.at<double>(1, 3) = ray.at<double>(0, 0);
                A.at<double>(2, 1) = 1;
                A.at<double>(2, 3) = ray.at<double>(1, 0);
                A.at<double>(3, 2) = 1;
                A.at<double>(3, 3) = ray.at<double>(2, 0);

                B.at<double>(0, 0) = -pl.at<double>(3, 0);
                B.at<double>(1, 0) = cameraCenter.at<double>(0, 0);
                B.at<double>(2, 0) = cameraCenter.at<double>(1, 0);
                B.at<double>(3, 0) = cameraCenter.at<double>(2, 0);

                if(std::abs(::cv::determinant(A)) > 0.00001)
                {
                    sol                   = A.inv()*B;
                    solH.at<double>(0, 0) = sol.at<double>(0, 0);
                    solH.at<double>(1, 0) = sol.at<double>(1, 0);
                    solH.at<double>(2, 0) = sol.at<double>(2, 0);

                    dampos = T1[k]* solH* std::max(inImg.size[0], inImg.size[1])/sizeImgInmm + imgOffset;
                    const double dampos1 = dampos.at<double>(0, 0);
                    const double dampos2 = dampos.at<double>(1, 0);
                    if((dampos1 > 0) && (dampos1 < (inImg.size[1]-1)) && (dampos2 > 0) && (dampos2 < (inImg.size[0]-1)))
                    {
                        const double deltav = dampos1 - static_cast<int>(dampos1);
                        const double deltau = dampos2 - static_cast<int>(dampos2);
                        const int v         = static_cast<int>(dampos1);
                        const int u         = static_cast<int>(dampos2);
                        tempxg = (1-deltau)*
                                 inImg.at< unsigned char >(u, v) + deltau*inImg.at< unsigned char >(u+1, v);
                        tempxd = (1-deltau)*
                                 inImg.at< unsigned char >(u, v+1) + deltau*inImg.at< unsigned char >(u+1, v+1);
                        photo.at< unsigned char >(i,
                                                  j) = static_cast<unsigned char>((1 - deltav)*tempxg + deltav*tempxd);
                    }
                }
            }
        }

        // Generate filename and write image.
        const fs::path filename = outFolder.string() + "/Pic_" + std::to_string(k+1) + ".bmp";

        ::cv::imwrite(filename.string(), photo);

        const double percentage = static_cast<double>(((nCam - 1) * T1.size() + k + 1)) /
                                  static_cast<double>((T1.size() * 2));

        std::cout<< static_cast<int>(percentage * 100)<<"%\t| Generating '"<< filename <<"' done. "<<std::endl;
    }
}
