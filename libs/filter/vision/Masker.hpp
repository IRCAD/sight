/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#pragma once

#include "filter/vision/config.hpp"

#include <opencv2/ml.hpp>
#include <opencv2/opencv.hpp>

namespace sight::filter::vision
{

/**
 * @brief Define supported color spaces
 */
enum ColSpace
{
    BGR,
    HSv,
    lAB,
    yCrCb
};

/**
 * @brief Define supported image masking mode
 */
enum DetectionMode
{
    fgLL,   /// Using only a foreground likelihood color model
    bgLL,   /// Using only a background likelihood color model
    LLRatio /// Using a ratio between foreground and background color model
};

/**
 * @brief Class performing OpenCV Expectation Maximization segmentation after a learning step of two color models. One
 * for the foreground objects that we need to segment and a second one for the background.
 *
 * @see Documentation from OpenCV https://docs.opencv.org/master/dc/dd6/ml_intro.html#ml_intro_em
 */
class FILTER_VISION_CLASS_API Masker
{
public:

    /// Constructor
    FILTER_VISION_API Masker(const ColSpace& c, const DetectionMode& d);

    /// Destructor
    FILTER_VISION_API ~Masker();

    /// Train the foreground color model defined with a number of clusters inside a mask on a given image
    FILTER_VISION_API void trainForegroundModel(
        const cv::Mat& rgbImg,
        const cv::Mat& selectionMask,
        const unsigned int numClusters,
        const double noise = 0.0
    );

    /// Train the background color model defined with a number of clusters inside a mask on a given image
    FILTER_VISION_API void trainBackgroundModel(
        const cv::Mat& rgbImg,
        const cv::Mat& selectionMask,
        const unsigned int numClusters
    );

    /// Perform an image masking based on the learned model on a downscaled image inside a given mask
    FILTER_VISION_API cv::Mat makeMask(
        const cv::Mat& testImg,
        const cv::Size& downSize,
        cv::InputArray filterMask
    ) const;

    /// Set threshold value to get final binary image
    FILTER_VISION_API void setThreshold(double t);

    /// Return if a model is learned
    FILTER_VISION_API bool isModelLearned(void);

private:

    /// Make a response mask from a model on a given image inside a mask
    static cv::Mat makeResponseImage(
        const cv::Mat& inImg,
        const cv::Ptr<cv::ml::EM> model,
        cv::Mat& filterMask
    );

    /// Convert the colorspace of an image
    static cv::Mat convertColourSpace(const cv::Mat& src, const ColSpace& c);

    /// Train the model from samples
    static cv::Ptr<cv::ml::EM> trainModelFromSamples(const cv::Mat& samples, const unsigned int numClusters);

    /// Get samples of an image inside a mask to train the model
    static cv::Mat makeTrainingSamples(const cv::Mat& trainImg, const cv::Mat& mask, const ColSpace& c);

    /// OpenCV operations to remove holes in the mask
    static cv::Mat removeMaskHoles(const cv::Mat& m, std::size_t n, cv::InputArray insideMask);

    /// Foreground and background models
    cv::Ptr<cv::ml::EM> m_foregroundModel;
    cv::Ptr<cv::ml::EM> m_backgroundModel;

    /// Colorspace and detection mode of the image masking model
    const ColSpace m_COLORSPACE;
    const DetectionMode m_DETECTIONMODE;

    /// Threshold of image masking model to get binary mask
    double m_threshold;

    /// Store if the threshold is set
    bool m_hasSetThreshold;

    /// Morphological element type
    static const int s_MORPHTYPE = cv::MORPH_ELLIPSE;

    /// Morphological element size
    static const int s_MORPHSIZE = 1;

    /// Morphological element
    static const cv::Mat s_MORPHELEMENT;
};

} // namespace sight::filter
