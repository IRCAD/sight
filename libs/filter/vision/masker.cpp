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
const cv::Mat masker::MORPHELEMENT =
    cv::getStructuringElement(
        masker::MORPHTYPE,
        cv::Size(
            2 * masker::MORPHSIZE + 1,
            2 * masker::MORPHSIZE + 1
        ),
        cv::Point(masker::MORPHSIZE, masker::MORPHSIZE)
    );

//------------------------------------------------------------------------------

masker::masker(const col_space& _c, const detection_mode& _d) :
    m_colorspace(_c),
    m_detectionmode(_d)
{
}

//------------------------------------------------------------------------------

masker::~masker()
= default;

//------------------------------------------------------------------------------

void masker::train_foreground_model(
    const cv::Mat& _rgb_img,
    const cv::Mat& _selection_mask,
    const unsigned int _num_clusters,
    const double _noise
)
{
    cv::Mat rgb_img_copy;
    _rgb_img.copyTo(rgb_img_copy, _selection_mask);

    // This step put some additive gaussian noise in the image.
    // It allows to perform a more robust learning step by providing different value close to the pixel values.
    cv::Mat gaussian_noise = cv::Mat(rgb_img_copy.size(), _rgb_img.type());
    cv::randn(gaussian_noise, 0, _noise);
    cv::addWeighted(rgb_img_copy, 1.0, gaussian_noise, 1.0, 0.0, rgb_img_copy);

    const cv::Mat s = sight::filter::vision::masker::make_training_samples(
        rgb_img_copy,
        _selection_mask,
        this->m_colorspace
    );
    this->m_foreground_model = sight::filter::vision::masker::train_model_from_samples(s, _num_clusters);
}

//------------------------------------------------------------------------------

void masker::train_background_model(
    const cv::Mat& _rgb_img,
    const cv::Mat& _selection_mask,
    const unsigned int _num_clusters
)
{
    const cv::Mat s = sight::filter::vision::masker::make_training_samples(_rgb_img, _selection_mask, m_colorspace);
    m_background_model = sight::filter::vision::masker::train_model_from_samples(s, _num_clusters);
}

//------------------------------------------------------------------------------

cv::Mat masker::make_mask(const cv::Mat& _test_img, const cv::Size& _down_size, cv::InputArray _test_img_mask) const
{
    SIGHT_ASSERT("Threshold is not set", m_has_set_threshold);

    cv::Mat t2;
    cv::Mat test_img_mask2;
    // OpenCV check if the downSize is different from the testImg size. If not, a copy is just performed.
    cv::resize(_test_img, t2, _down_size);
    cv::Mat m;

    if(!_test_img_mask.empty())
    {
        cv::resize(_test_img_mask.getMat(), test_img_mask2, _down_size);
    }

    const cv::Mat i = convert_colour_space(t2, m_colorspace);

    switch(m_detectionmode)
    {
        case fg_ll:
        {
            cv::Mat fg_response = make_response_image(i, m_foreground_model, test_img_mask2);
            cv::threshold(fg_response, m, m_threshold, 255, cv::THRESH_BINARY);
            break;
        }

        case bg_ll:
        {
            cv::Mat bg_response = make_response_image(i, m_background_model, test_img_mask2);
            cv::threshold(bg_response, m, m_threshold, 255, cv::THRESH_BINARY_INV);
            break;
        }

        case ll_ratio:
        {
            cv::Mat fg_response = make_response_image(i, m_foreground_model, test_img_mask2);
            cv::Mat bg_response = make_response_image(i, m_background_model, test_img_mask2);
            cv::threshold(fg_response - bg_response, m, m_threshold, 255, cv::THRESH_BINARY);
            break;
        }
    }

    m.convertTo(m, CV_8UC1);

    //get mask back to original size:
    cv::resize(m, m, _test_img.size());
    cv::threshold(m, m, 125, 255, cv::THRESH_BINARY);

    //eliminate mask holes with erosion/dilation
    cv::Mat filtered_mask1 = remove_mask_holes(m, 2, _test_img_mask);

    return filtered_mask1;
}

//------------------------------------------------------------------------------

void masker::set_threshold(double _t)
{
    m_threshold         = _t;
    m_has_set_threshold = true;
}

//------------------------------------------------------------------------------

bool masker::is_model_learned()
{
    switch(m_detectionmode)
    {
        case fg_ll:
            return !m_foreground_model.empty();

        case bg_ll:
            return !m_background_model.empty();

        default: // LLRatio
            return !m_foreground_model.empty() && !m_background_model.empty();
    }
}

//------------------------------------------------------------------------------

cv::Mat masker::make_response_image(
    const cv::Mat& _i,
    const cv::Ptr<cv::ml::EM> _model,
    cv::Mat& _in_img_mask
)
{
    const int cn                = _i.channels();
    const int w                 = _i.cols;
    const bool uses_filter_mask = !_in_img_mask.empty();

    cv::Mat output         = cv::Mat::zeros(_i.rows, _i.cols, CV_32FC1);
    const uchar* pixel_ptr = static_cast<uchar*>(_i.data);

    // Parallelization of pixel prediction
    cv::parallel_for_(
        cv::Range(0, _i.rows * _i.cols),
        [&](const cv::Range& _range)
        {
            cv::Mat sample = cv::Mat::zeros(cn, 1, CV_32FC1);
            for(int r = _range.start ; r < _range.end ; ++r)
            {
                const int i = r / w;
                const int j = r % w;

                if(uses_filter_mask)
                {
                    if(_in_img_mask.at<uchar>(i, j) == 0)
                    {
                        continue;
                    }
                }

                for(int channel_idx = 0 ; channel_idx < cn ; ++channel_idx)
                {
                    sample.at<float>(channel_idx) = pixel_ptr[i * w * cn + j * cn + channel_idx];
                }

                output.at<float>(i, j) = static_cast<float>(_model->predict2(sample, cv::noArray())[0]);
            }
        });

    return output;
}

//------------------------------------------------------------------------------

cv::Mat masker::convert_colour_space(const cv::Mat& _src, const col_space& _c)
{
    cv::Mat output;
    switch(_c)
    {
        case bgr:
            _src.copyTo(output);
            break;

        case h_sv:
        {
            cv::cvtColor(_src, output, cv::COLOR_BGR2HSV);
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

        case l_ab:
        {
            cv::cvtColor(_src, output, cv::COLOR_BGR2Lab);
            std::array<cv::Mat, 3> s; //destination array
            cv::split(output, s);     //split source
            std::vector<cv::Mat> array_to_merge;
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[2]);
            array_to_merge.push_back(s[1]);
            cv::merge(array_to_merge, output);
            break;
        }

        case y_cr_cb:
        {
            cv::cvtColor(_src, output, cv::COLOR_BGR2YCrCb);
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

cv::Ptr<cv::ml::EM> masker::train_model_from_samples(const cv::Mat& _samples, const unsigned int _num_clusters)
{
    cv::Ptr<cv::ml::EM> m = cv::ml::EM::create();
    m->setClustersNumber(static_cast<int>(_num_clusters));
    m->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
    m->setTermCriteria(cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));
    m->trainEM(_samples, cv::noArray(), cv::noArray(), cv::noArray());
    return m;
}

//------------------------------------------------------------------------------

cv::Mat masker::make_training_samples(const cv::Mat& _t, const cv::Mat& _mask, const col_space& _c)
{
    cv::Mat train_img = masker::convert_colour_space(_t, _c);

    const int cn = train_img.channels();

    // Get the N non zero coordinates inside the mask
    cv::Mat non_zero_coordinates;
    cv::findNonZero(_mask, non_zero_coordinates);

    // Create a N*cn matrix to store the value of each non zero pixels in a linear matrix
    cv::Mat samples(non_zero_coordinates.rows, cn, CV_64FC1);

    // Fill it by copying from the original image to the linear one
    cv::parallel_for_(
        cv::Range(0, non_zero_coordinates.rows),
        [&](const cv::Range& _range)
        {
            for(int r = _range.start ; r < _range.end ; ++r)
            {
                cv::Point position = non_zero_coordinates.at<cv::Point>(r);
                // Implicit cast from Vec3b to Vec3d avoiding static_cast<float> in next for loop
                cv::Vec3d pixel = train_img.at<cv::Vec3b>(position);
                for(int channel_idx = 0 ; channel_idx < cn ; ++channel_idx)
                {
                    samples.at<double>(r, channel_idx) = pixel[channel_idx];
                }
            }
        });

    return samples;
}

//------------------------------------------------------------------------------

cv::Mat masker::remove_mask_holes(const cv::Mat& _m, std::size_t _n, cv::InputArray _inside_mask)
{
    cv::Mat mask;
    _m.copyTo(mask);

    cv::Mat k = MORPHELEMENT.clone();
    k.setTo(1);

    // Perform some erosion/dilatation to remove small areas
    for(std::size_t i = 0 ; i < _n ; i++)
    {
        cv::erode(mask, mask, k);
    }

    for(std::size_t i = 0 ; i < _n ; i++)
    {
        cv::dilate(mask, mask, k);
    }

    // Perform a last opening to smooth the edge of the final mask
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, MORPHELEMENT);
    cv::dilate(mask, mask, MORPHELEMENT);

    // Get connected components from the mask and label them
    cv::Mat labels;
    int nb_labels = cv::connectedComponents(mask, labels, 8, CV_32S);

    // Erode the original mask
    cv::Mat inside_mask_eroded;
    cv::erode(_inside_mask, inside_mask_eroded, k);

    // Perform a diff to get areas connected to the border of the mask
    cv::Mat diff = mask - inside_mask_eroded;
    cv::Mat res  = cv::Mat::zeros(mask.rows, mask.cols, mask.type());

    // Browse all labels
    cv::parallel_for_(
        cv::Range(0, nb_labels),
        [&](const cv::Range& _range)
        {
            for(int r = _range.start ; r < _range.end ; ++r)
            {
                cv::Mat tmp = cv::Mat::zeros(mask.rows, mask.cols, mask.type());
                // Get the binary image corresponding to the current label
                cv::Mat bin_tmp = (labels == r);
                // Do a 'and' between the diff mask and the current label mask
                cv::bitwise_and(diff, bin_tmp, tmp);

                // If the 'and' is not empty, it means that it's an area connected to the border of the insideMask
                // Otherwise, it's an unconnected small area inside the mask
                if(cv::countNonZero(tmp) != 0)
                {
                    res.setTo(255, bin_tmp);
                }
            }
        });

    return res;
}

//------------------------------------------------------------------------------

} // namespace sight::filter::vision
