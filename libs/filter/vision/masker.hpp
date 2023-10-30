/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
enum col_space
{
    bgr,
    h_sv,
    l_ab,
    y_cr_cb
};

/**
 * @brief Define supported image masking mode
 */
enum detection_mode
{
    fg_ll,   /// Using only a foreground likelihood color model
    bg_ll,   /// Using only a background likelihood color model
    ll_ratio /// Using a ratio between foreground and background color model
};

/**
 * @brief Class performing OpenCV Expectation Maximization segmentation after a learning step of two color models. One
 * for the foreground objects that we need to segment and a second one for the background.
 *
 * @see Documentation from OpenCV https://docs.opencv.org/master/dc/dd6/ml_intro.html#ml_intro_em
 */
class FILTER_VISION_CLASS_API masker
{
public:

    /// Constructor
    FILTER_VISION_API masker(const col_space& _c, const detection_mode& _d);

    /// Destructor
    FILTER_VISION_API ~masker();

    /// Train the foreground color model defined with a number of clusters inside a mask on a given image
    FILTER_VISION_API void train_foreground_model(
        const cv::Mat& _rgb_img,
        const cv::Mat& _selection_mask,
        unsigned int _num_clusters,
        double _noise = 0.0
    );

    /// Train the background color model defined with a number of clusters inside a mask on a given image
    FILTER_VISION_API void train_background_model(
        const cv::Mat& _rgb_img,
        const cv::Mat& _selection_mask,
        unsigned int _num_clusters
    );

    /// Perform an image masking based on the learned model on a downscaled image inside a given mask
    [[nodiscard]] FILTER_VISION_API cv::Mat make_mask(
        const cv::Mat& _test_img,
        const cv::Size& _down_size,
        cv::InputArray _filter_mask
    ) const;

    /// Set threshold value to get final binary image
    FILTER_VISION_API void set_threshold(double _t);

    /// Return if a model is learned
    FILTER_VISION_API bool is_model_learned();

private:

    /// Make a response mask from a model on a given image inside a mask
    static cv::Mat make_response_image(
        const cv::Mat& _in_img,
        cv::Ptr<cv::ml::EM> _model,
        cv::Mat& _in_img_mask
    );

    /// Convert the colorspace of an image
    static cv::Mat convert_colour_space(const cv::Mat& _src, const col_space& _c);

    /// Train the model from samples
    static cv::Ptr<cv::ml::EM> train_model_from_samples(const cv::Mat& _samples, unsigned int _num_clusters);

    /// Get samples of an image inside a mask to train the model
    static cv::Mat make_training_samples(const cv::Mat& _train_img, const cv::Mat& _mask, const col_space& _c);

    /// OpenCV operations to remove holes in the mask
    static cv::Mat remove_mask_holes(const cv::Mat& _m, std::size_t _n, cv::InputArray _inside_mask);

    /// Foreground and background models
    cv::Ptr<cv::ml::EM> m_foreground_model;
    cv::Ptr<cv::ml::EM> m_background_model;

    /// Colorspace and detection mode of the image masking model
    const col_space m_colorspace;
    const detection_mode m_detectionmode;

    /// Threshold of image masking model to get binary mask
    double m_threshold {0.0};

    /// Store if the threshold is set
    bool m_has_set_threshold {false};

    /// Morphological element type
    static constexpr int MORPHTYPE = cv::MORPH_ELLIPSE;

    /// Morphological element size
    static constexpr int MORPHSIZE = 1;

    /// Morphological element
    static const cv::Mat MORPHELEMENT;
};

} // namespace sight::filter::vision
