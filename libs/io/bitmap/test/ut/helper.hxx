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

#include <core/tools/system.hpp>
#include <core/tools/uuid.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>

#include <io/bitmap/writer.hpp>
#include <io/opencv/image.hpp>
#include <io/opencv/type.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using sight::core::tools::uuid;

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static data::image::sptr get_synthetic_image(
    std::optional<std::uint32_t> _seed     = std::nullopt,
    core::type _type                       = core::type::UINT8,
    enum data::image::pixel_format _format = data::image::rgb
)
{
    using key_t = std::tuple<std::optional<std::uint32_t>, core::type, enum data::image::pixel_format>;
    static std::map<key_t, data::image::sptr> s_generated;

    const key_t key = std::make_tuple(_seed, _type, _format);

    if(const auto& it = s_generated.find(key); it == s_generated.end())
    {
        auto image           = std::make_shared<data::image>();
        const auto dump_lock = image->dump_lock();

        utest_data::generator::image::generate_image(
            image,
            {256, 256, 0},
            {0, 0, 0},
            {0, 0, 0},
            _type,
            _format,
            _seed
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

inline static cv::Mat image_to_mat(const data::image::sptr& _image, bool _clone = true)
{
    // Convert origin to cv::Mat
    const auto dump_lock = _image->dump_lock();
    const auto& sizes    = _image->size();
    auto mat             = cv::Mat(
        std::vector<int> {int(sizes[1]), int(sizes[0])},
        io::opencv::type::to_cv(_image->type(), _image->num_components()),
        _image->buffer()
    );

    if(_clone)
    {
        mat = mat.clone();
    }

    switch(_image->pixel_format())
    {
        case data::image::pixel_format::rgb:
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
            break;

        case data::image::pixel_format::rgba:
            cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
            break;

        default:
            // No conversion needed
            break;
    }

    return mat;
}

//------------------------------------------------------------------------------

inline static data::image::sptr mat_to_image(cv::Mat& _mat, bool _clone = true)
{
    auto image           = std::make_shared<data::image>();
    const auto dump_lock = image->dump_lock();

    cv::Mat mat_copy;
    cv::Mat& mat_ref = _clone ? (mat_copy = _mat.clone()) : _mat;

    const auto cv_type = io::opencv::type::from_cv(mat_ref.type());

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

    io::opencv::image::copy_from_cv(*image, mat_ref);

    sight::data::helper::medical_image::check_image_slice_index(image);

    return image;
}

//------------------------------------------------------------------------------

inline static data::image::sptr read_image(const std::filesystem::path& _path)
{
    CPPUNIT_ASSERT(std::filesystem::exists(_path) && std::filesystem::is_regular_file(_path));

    auto mat = cv::imread(_path.string(), cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    return mat_to_image(mat, false);
}

//------------------------------------------------------------------------------

// Borrowed from openCV sample
inline static double compute_psnr(const data::image::sptr& _expected, const data::image::sptr& _actual)
{
    // Convert origin to cv::Mat
    const cv::Mat& expected_mat = image_to_mat(_expected);
    const cv::Mat& actual_mat   = image_to_mat(_actual);

    CPPUNIT_ASSERT(!expected_mat.empty() && !actual_mat.empty());
    CPPUNIT_ASSERT(expected_mat.rows == actual_mat.rows);
    CPPUNIT_ASSERT(expected_mat.cols == actual_mat.cols);
    CPPUNIT_ASSERT(expected_mat.type() == actual_mat.type());

    cv::Mat compute_mat(expected_mat.rows, expected_mat.cols, CV_64F);
    cv::subtract(expected_mat, actual_mat, compute_mat);
    cv::multiply(compute_mat, compute_mat, compute_mat);

    const double max_value = std::pow(2, _expected->type().size() * 8) - 1;

    return 10.0 * log10(max_value * max_value / cv::mean(compute_mat).val[0]);
}

//------------------------------------------------------------------------------

inline static std::string mode_to_string(const writer::mode& _mode)
{
    switch(_mode)
    {
        case writer::mode::best:
            return "BEST";

        case writer::mode::fast:
            return "FAST";

        default:
            return "DEFAULT";
    }
}

//------------------------------------------------------------------------------

inline static std::string pixel_format_to_string(const enum data::image::pixel_format& _format)
{
    switch(_format)
    {
        case data::image::pixel_format::rgb:
            return "RGB";

        case data::image::pixel_format::rgba:
            return "RGBA";

        case data::image::pixel_format::bgr:
            return "BGR";

        case data::image::pixel_format::bgra:
            return "BGRA";

        case data::image::pixel_format::gray_scale:
            return "GRAY_SCALE";

        case data::image::pixel_format::rg:
            return "RG";

        default:
            return "UNDEFINED";
    }
}

//------------------------------------------------------------------------------

inline static std::pair<std::string, std::string> backend_to_string(const backend& _backend)
{
    auto backend_string =
        _backend == backend::libjpeg
        ? std::make_pair(std::string("LIBJPEG"), std::string(".jpg"))
        : _backend == backend::libtiff
        ? std::make_pair(std::string("LIBTIFF"), std::string(".tiff"))
        : _backend == backend::libpng
        ? std::make_pair(std::string("LIBPNG"), std::string(".png"))
        : _backend == backend::openjpeg
        ? std::make_pair(std::string("OPENJPEG"), std::string(".jp2"))
        : _backend == backend::openjpeg_j2_k
        ? std::make_pair(std::string("OPENJPEG"), std::string(".j2k"))
        : _backend == backend::nvjpeg
        ? std::make_pair(std::string("NVJPEG"), std::string(".jpg"))
        : _backend == backend::nvjpeg2k
        ? std::make_pair(std::string("NVJPEG2K"), std::string(".jp2"))
        : _backend == backend::nvjpeg2k_j2k
        ? std::make_pair(std::string("NVJPEG2K"), std::string(".j2k"))
        : std::make_pair(std::string("DEFAULT"), std::string(".tiff"));

    return backend_string;
}

//------------------------------------------------------------------------------

inline static std::string file_suffix(const backend& _backend, const writer::mode& _mode)
{
    const auto [backend_string, ext_string] = backend_to_string(_backend);
    const std::string mode_string = mode_to_string(_mode);

    return "_" + backend_string + "_" + mode_string + ext_string;
}

} // namespace sight::io
