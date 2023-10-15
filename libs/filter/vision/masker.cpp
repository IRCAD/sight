/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "masker.hpp"

#include <core/spy_log.hpp>

namespace sight::filter::vision
{

// Define the morphological element used
const cv::Mat masker::s_MORPHELEMENT =
    cv::getStructuringElement(
        masker::s_MORPHTYPE,
        cv::Size(
            2 * masker::s_MORPHSIZE + 1,
            2 * masker::s_MORPHSIZE + 1
        ),
        cv::Point(masker::s_MORPHSIZE, masker::s_MORPHSIZE)
    );

//------------------------------------------------------------------------------

masker::masker(const ColSpace& c, const DetectionMode& d) :
    m_COLORSPACE(c),
    m_DETECTIONMODE(d)
{
}

//------------------------------------------------------------------------------

masker::~masker()
= default;

//------------------------------------------------------------------------------

void masker::trainForegroundModel(
    const cv::Mat& rgbImg,
    const cv::Mat& selectionMask,
    const unsigned int numClusters,
    const double noise
)
{
    cv::Mat rgbImgCopy;
    rgbImg.copyTo(rgbImgCopy, selectionMask);

    // This step put some additive gaussian noise in the image.
    // It allows to perform a more robust learning step by providing different value close to the pixel values.
    cv::Mat gaussian_noise = cv::Mat(rgbImgCopy.size(), rgbImg.type());
    cv::randn(gaussian_noise, 0, noise);
    cv::addWeighted(rgbImgCopy, 1.0, gaussian_noise, 1.0, 0.0, rgbImgCopy);

    const cv::Mat s = sight::filter::vision::masker::makeTrainingSamples(rgbImgCopy, selectionMask, this->m_COLORSPACE);
    this->m_foregroundModel = sight::filter::vision::masker::trainModelFromSamples(s, numClusters);
}

//------------------------------------------------------------------------------

void masker::trainBackgroundModel(
    const cv::Mat& rgbImg,
    const cv::Mat& selectionMask,
    const unsigned int numClusters
)
{
    const cv::Mat s = sight::filter::vision::masker::makeTrainingSamples(rgbImg, selectionMask, m_COLORSPACE);
    m_backgroundModel = sight::filter::vision::masker::trainModelFromSamples(s, numClusters);
}

//------------------------------------------------------------------------------

cv::Mat masker::makeMask(const cv::Mat& testImg, const cv::Size& downSize, cv::InputArray testImgMask) const
{
    SIGHT_ASSERT("Threshold is not set", m_hasSetThreshold);

    cv::Mat t2;
    cv::Mat testImgMask2;
    // OpenCV check if the downSize is different from the testImg size. If not, a copy is just performed.
    cv::resize(testImg, t2, downSize);
    cv::Mat m;

    if(!testImgMask.empty())
    {
        cv::resize(testImgMask.getMat(), testImgMask2, downSize);
    }

    const cv::Mat I = convertColourSpace(t2, m_COLORSPACE);

    switch(m_DETECTIONMODE)
    {
        case fgLL:
        {
            cv::Mat fgResponse = makeResponseImage(I, m_foregroundModel, testImgMask2);
            cv::threshold(fgResponse, m, m_threshold, 255, cv::THRESH_BINARY);
            break;
        }

        case bgLL:
        {
            cv::Mat bgResponse = makeResponseImage(I, m_backgroundModel, testImgMask2);
            cv::threshold(bgResponse, m, m_threshold, 255, cv::THRESH_BINARY_INV);
            break;
        }

        case LLRatio:
        {
            cv::Mat fgResponse = makeResponseImage(I, m_foregroundModel, testImgMask2);
            cv::Mat bgResponse = makeResponseImage(I, m_backgroundModel, testImgMask2);
            cv::threshold(fgResponse - bgResponse, m, m_threshold, 255, cv::THRESH_BINARY);
            break;
        }
    }

    m.convertTo(m, CV_8UC1);

    //get mask back to original size:
    cv::resize(m, m, testImg.size());
    cv::threshold(m, m, 125, 255, cv::THRESH_BINARY);

    //eliminate mask holes with erosion/dilation
    cv::Mat filteredMask1 = removeMaskHoles(m, 2, testImgMask);

    return filteredMask1;
}

//------------------------------------------------------------------------------

void masker::setThreshold(double t)
{
    m_threshold       = t;
    m_hasSetThreshold = true;
}

//------------------------------------------------------------------------------

bool masker::isModelLearned()
{
    switch(m_DETECTIONMODE)
    {
        case fgLL:
            return !m_foregroundModel.empty();

        case bgLL:
            return !m_backgroundModel.empty();

        default: // LLRatio
            return !m_foregroundModel.empty() && !m_backgroundModel.empty();
    }
}

//------------------------------------------------------------------------------

cv::Mat masker::makeResponseImage(
    const cv::Mat& I,
    const cv::Ptr<cv::ml::EM> model,
    cv::Mat& inImgMask
)
{
    const int cn              = I.channels();
    const int w               = I.cols;
    const bool usesFilterMask = !inImgMask.empty();

    cv::Mat output        = cv::Mat::zeros(I.rows, I.cols, CV_32FC1);
    const uchar* pixelPtr = static_cast<uchar*>(I.data);

    // Parallelization of pixel prediction
    cv::parallel_for_(
        cv::Range(0, I.rows * I.cols),
        [&](const cv::Range& range)
        {
            cv::Mat sample = cv::Mat::zeros(cn, 1, CV_32FC1);
            for(int r = range.start ; r < range.end ; ++r)
            {
                const int i = r / w;
                const int j = r % w;

                if(usesFilterMask)
                {
                    if(inImgMask.at<uchar>(i, j) == 0)
                    {
                        continue;
                    }
                }

                for(int channel_idx = 0 ; channel_idx < cn ; ++channel_idx)
                {
                    sample.at<float>(channel_idx) = pixelPtr[i * w * cn + j * cn + channel_idx];
                }

                output.at<float>(i, j) = static_cast<float>(model->predict2(sample, cv::noArray())[0]);
            }
        });

    return output;
}

//------------------------------------------------------------------------------

cv::Mat masker::convertColourSpace(const cv::Mat& src, const ColSpace& c)
{
    cv::Mat output;
    switch(c)
    {
        case BGR:
            src.copyTo(output);
            break;

        case HSv:
        {
            cv::cvtColor(src, output, cv::COLOR_BGR2HSV);
            std::array<cv::Mat, 3> s; //destination array
            cv::split(output, s);     //split source
            std::vector<cv::Mat> array_to_merge;
            array_to_merge.push_back(s[0]);
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[1]); //we remove v channel for illumination invariance. This is done here by
                                            // duplicating the S channel
            cv::merge(array_to_merge, output);
            break;
        }

        case lAB:
        {
            cv::cvtColor(src, output, cv::COLOR_BGR2Lab);
            std::array<cv::Mat, 3> s; //destination array
            cv::split(output, s);     //split source
            std::vector<cv::Mat> array_to_merge;
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[2]);
            array_to_merge.push_back(s[1]);
            cv::merge(array_to_merge, output);
            break;
        }

        case yCrCb:
        {
            cv::cvtColor(src, output, cv::COLOR_BGR2YCrCb);
            std::array<cv::Mat, 3> s; //destination array
            cv::split(output, s);     //split source
            std::vector<cv::Mat> array_to_merge;
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[2]);
            array_to_merge.push_back(s[1]);
            cv::merge(array_to_merge, output);
            break;
        }
    }

    return output;
}

//------------------------------------------------------------------------------

cv::Ptr<cv::ml::EM> masker::trainModelFromSamples(const cv::Mat& samples, const unsigned int numClusters)
{
    cv::Ptr<cv::ml::EM> m = cv::ml::EM::create();
    m->setClustersNumber(static_cast<int>(numClusters));
    m->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
    m->setTermCriteria(cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));
    m->trainEM(samples, cv::noArray(), cv::noArray(), cv::noArray());
    return m;
}

//------------------------------------------------------------------------------

cv::Mat masker::makeTrainingSamples(const cv::Mat& t, const cv::Mat& mask, const ColSpace& c)
{
    cv::Mat trainImg = masker::convertColourSpace(t, c);

    const int cn = trainImg.channels();

    // Get the N non zero coordinates inside the mask
    cv::Mat nonZeroCoordinates;
    cv::findNonZero(mask, nonZeroCoordinates);

    // Create a N*cn matrix to store the value of each non zero pixels in a linear matrix
    cv::Mat samples(nonZeroCoordinates.rows, cn, CV_64FC1);

    // Fill it by copying from the original image to the linear one
    cv::parallel_for_(
        cv::Range(0, nonZeroCoordinates.rows),
        [&](const cv::Range& range)
        {
            for(int r = range.start ; r < range.end ; ++r)
            {
                cv::Point position = nonZeroCoordinates.at<cv::Point>(r);
                // Implicit cast from Vec3b to Vec3d avoiding static_cast<float> in next for loop
                cv::Vec3d pixel = trainImg.at<cv::Vec3b>(position);
                for(int channel_idx = 0 ; channel_idx < cn ; ++channel_idx)
                {
                    samples.at<double>(r, channel_idx) = pixel[channel_idx];
                }
            }
        });

    return samples;
}

//------------------------------------------------------------------------------

cv::Mat masker::removeMaskHoles(const cv::Mat& m, std::size_t n, cv::InputArray insideMask)
{
    cv::Mat mask;
    m.copyTo(mask);

    cv::Mat k = s_MORPHELEMENT.clone();
    k.setTo(1);

    // Perform some erosion/dilatation to remove small areas
    for(std::size_t i = 0 ; i < n ; i++)
    {
        cv::erode(mask, mask, k);
    }

    for(std::size_t i = 0 ; i < n ; i++)
    {
        cv::dilate(mask, mask, k);
    }

    // Perform a last opening to smooth the edge of the final mask
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, s_MORPHELEMENT);
    cv::dilate(mask, mask, s_MORPHELEMENT);

    // Get connected components from the mask and label them
    cv::Mat labels;
    int nbLabels = cv::connectedComponents(mask, labels, 8, CV_32S);

    // Erode the original mask
    cv::Mat insideMaskEroded;
    cv::erode(insideMask, insideMaskEroded, k);

    // Perform a diff to get areas connected to the border of the mask
    cv::Mat diff = mask - insideMaskEroded;
    cv::Mat res  = cv::Mat::zeros(mask.rows, mask.cols, mask.type());

    // Browse all labels
    cv::parallel_for_(
        cv::Range(0, nbLabels),
        [&](const cv::Range& range)
        {
            for(int r = range.start ; r < range.end ; ++r)
            {
                cv::Mat tmp = cv::Mat::zeros(mask.rows, mask.cols, mask.type());
                // Get the binary image corresponding to the current label
                cv::Mat binTmp = (labels == r);
                // Do a 'and' between the diff mask and the current label mask
                cv::bitwise_and(diff, binTmp, tmp);

                // If the 'and' is not empty, it means that it's an area connected to the border of the insideMask
                // Otherwise, it's an unconnected small area inside the mask
                if(cv::countNonZero(tmp) != 0)
                {
                    res.setTo(255, binTmp);
                }
            }
        });

    return res;
}

//------------------------------------------------------------------------------

} // namespace sight::filter::vision
