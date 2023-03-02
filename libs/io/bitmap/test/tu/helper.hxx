/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/ImageSeries.hpp>

#include <io/opencv/Image.hpp>
#include <io/opencv/Type.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using sight::core::tools::UUID;

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static data::Image::sptr getSyntheticImage(
    std::optional<std::uint32_t> seed = std::nullopt,
    core::Type type                   = core::Type::UINT8,
    data::Image::PixelFormat format   = data::Image::RGB
)
{
    using key_t = std::tuple<std::optional<std::uint32_t>, core::Type, data::Image::PixelFormat>;
    static std::map<key_t, data::Image::sptr> s_generated;

    const key_t key = std::make_tuple(seed, type, format);

    if(const auto& it = s_generated.find(key); it == s_generated.end())
    {
        auto image           = data::Image::New();
        const auto dump_lock = image->dump_lock();

        utestData::generator::Image::generateImage(
            image,
            {256, 256, 0},
            {0, 0, 0},
            {0, 0, 0},
            type,
            format,
            seed
        );

        // de-randomize a bit the image to make it easier to compare and to workaround a strange bug with pure synthetic
        // data in nvJPEG.
        /// @see https://github.com/NVIDIA/CUDALibrarySamples/issues/100

        auto image_it = image->begin<std::uint8_t>();
        auto end      = image->end<std::uint8_t>();

        while(image_it != end)
        {
            const auto value = *image_it++;

            for(std::size_t i = 0, i_max = 8 ; i < i_max && image_it != end ; ++i, ++image_it)
            {
                *image_it = value;
            }
        }

        s_generated.insert_or_assign(key, image);

        return image;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static cv::Mat imageToMat(const data::Image::sptr& image, bool clone = true)
{
    // Convert origin to cv::Mat
    const auto dump_lock = image->dump_lock();
    const auto& sizes    = image->getSize();
    auto mat             = cv::Mat(
        std::vector<int> {int(sizes[1]), int(sizes[0])},
        io::opencv::Type::toCv(image->getType(), image->numComponents()),
        image->getBuffer()
    );

    if(clone)
    {
        mat = mat.clone();
    }

    switch(image->getPixelFormat())
    {
        case data::Image::PixelFormat::RGB:
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
            break;

        case data::Image::PixelFormat::RGBA:
            cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
            break;

        default:
            // No conversion needed
            break;
    }

    return mat;
}

//------------------------------------------------------------------------------

inline static data::Image::sptr matToImage(cv::Mat& mat, bool clone = true)
{
    auto image           = data::Image::New();
    const auto dump_lock = image->dump_lock();

    cv::Mat mat_copy;
    cv::Mat& mat_ref = clone ? (mat_copy = mat.clone()) : mat;

    const auto cv_type = io::opencv::Type::fromCv(mat_ref.type());

    switch(cv_type.second)
    {
        case 3:
            cv::cvtColor(mat_ref, mat_ref, cv::COLOR_BGR2RGB);
            break;

        case 4:
            cv::cvtColor(mat_ref, mat_ref, cv::COLOR_BGRA2RGBA);
            break;

        default:
            // No conversion needed
            break;
    }

    io::opencv::Image::copyFromCv(*image, mat_ref);

    sight::data::helper::MedicalImage::checkImageSliceIndex(image);

    return image;
}

//------------------------------------------------------------------------------

inline static data::Image::sptr readImage(const std::filesystem::path& path)
{
    CPPUNIT_ASSERT(std::filesystem::exists(path) && std::filesystem::is_regular_file(path));

    auto mat = cv::imread(path.string(), cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    return matToImage(mat, false);
}

//------------------------------------------------------------------------------

// Borrowed from openCV sample
inline static double computePSNR(const data::Image::sptr& expected, const data::Image::sptr& actual)
{
    // Convert origin to cv::Mat
    const cv::Mat& expected_mat = imageToMat(expected);
    const cv::Mat& actual_mat   = imageToMat(actual);

    CPPUNIT_ASSERT(!expected_mat.empty() && !actual_mat.empty());
    CPPUNIT_ASSERT(expected_mat.rows == actual_mat.rows);
    CPPUNIT_ASSERT(expected_mat.cols == actual_mat.cols);
    CPPUNIT_ASSERT(expected_mat.type() == actual_mat.type());

    cv::Mat compute_mat(expected_mat.rows, expected_mat.cols, CV_64F);
    cv::subtract(expected_mat, actual_mat, compute_mat);
    cv::multiply(compute_mat, compute_mat, compute_mat);

    const double max_value = std::pow(2, expected->getType().size() * 8) - 1;

    return 10.0 * log10(max_value * max_value / cv::mean(compute_mat).val[0]);
}

} // namespace sight::io
