/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "detail/image.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

namespace sight::ui::test
{

//------------------------------------------------------------------------------

image_threshold threshold(int _channel, double _max_bad_ratio, std::string_view _why)
{
    // The reason goes to the step log, so that a relaxed check never goes unnoticed.
    if(_why.empty())
    {
        throw std::invalid_argument("an image threshold override requires a reason");
    }

    image_threshold result {.channel = _channel, .max_bad_ratio = _max_bad_ratio, .reason = std::string(_why)};
    detail::validate_threshold(result);
    return result;
}

namespace detail
{

//------------------------------------------------------------------------------

void validate_threshold(const image_threshold& _threshold)
{
    if(_threshold.channel < 0 || _threshold.channel > 255
       || !std::isfinite(_threshold.max_bad_ratio)
       || _threshold.max_bad_ratio < 0. || _threshold.max_bad_ratio > 1.)
    {
        throw std::invalid_argument("image thresholds require a channel in [0,255] and a finite ratio in [0,1]");
    }

    // Only the defaults go without a reason.
    const image_threshold defaults;
    if(_threshold.reason.empty()
       && (_threshold.channel != defaults.channel || _threshold.max_bad_ratio != defaults.max_bad_ratio))
    {
        throw std::invalid_argument("an image threshold override requires a reason");
    }
}

//------------------------------------------------------------------------------

bool image_metrics::matches() const
{
    return same_size && total_pixels > 0
           && static_cast<double>(bad_pixels) / static_cast<double>(total_pixels) <= tolerance.max_bad_ratio;
}

//------------------------------------------------------------------------------

std::string image_metrics::describe() const
{
    return std::format(
        "{} (actual {}x{}, expected {}x{}); bad pixels {}/{}; ratio {:.6f} (maximum {:.6f}); "
        "channel difference {} (threshold {})",
        same_size ? "same dimensions" : "different dimensions",
        actual_size.width(),
        actual_size.height(),
        expected_size.width(),
        expected_size.height(),
        bad_pixels,
        total_pixels,
        total_pixels == 0 ? 0. : static_cast<double>(bad_pixels) / static_cast<double>(total_pixels),
        tolerance.max_bad_ratio,
        max_channel_difference,
        tolerance.channel
    );
}

//------------------------------------------------------------------------------

static void require_image(const QImage& _image)
{
    if(_image.isNull())
    {
        throw std::invalid_argument("an empty image cannot be captured or compared");
    }
}

//------------------------------------------------------------------------------

static int channel_difference(QRgb _first, QRgb _second)
{
    return std::max(
            {
                std::abs(qRed(_first) - qRed(_second)),
                std::abs(qGreen(_first) - qGreen(_second)),
                std::abs(qBlue(_first) - qBlue(_second))
            });
}

//------------------------------------------------------------------------------

image_metrics compare_images(const QImage& _actual, const QImage& _expected, const image_threshold& _threshold)
{
    validate_threshold(_threshold);
    require_image(_actual);
    require_image(_expected);
    image_metrics metrics {
        .same_size     = _actual.size() == _expected.size(),
        .total_pixels  = static_cast<std::uint64_t>(_actual.width()) * static_cast<std::uint64_t>(_actual.height()),
        .tolerance     = _threshold,
        .actual_size   = _actual.size(),
        .expected_size = _expected.size(),
    };
    // No resize: different dimensions are a failure, whatever the thresholds.
    if(!metrics.same_size)
    {
        return metrics;
    }

    // One canonical 8-bit RGB representation; alpha and storage format do not affect the metric.
    const QImage actual   = _actual.convertToFormat(QImage::Format_RGB32);
    const QImage expected = _expected.convertToFormat(QImage::Format_RGB32);
    for(int y = 0 ; y < actual.height() ; ++y)
    {
        const auto* row       = reinterpret_cast<const QRgb*>(actual.constScanLine(y));
        const auto* reference = reinterpret_cast<const QRgb*>(expected.constScanLine(y));
        for(int x = 0 ; x < actual.width() ; ++x)
        {
            const int difference = channel_difference(row[x], reference[x]);
            metrics.max_channel_difference = std::max(metrics.max_channel_difference, difference);
            metrics.bad_pixels            += difference > _threshold.channel ? 1 : 0;
        }
    }

    return metrics;
}

//------------------------------------------------------------------------------

QImage image_difference(const QImage& _actual, const QImage& _expected)
{
    require_image(_actual);
    require_image(_expected);
    // Pixels cannot be matched between two sizes: all red tells so at a glance.
    QImage diff(_actual.size(), QImage::Format_RGB32);
    if(_actual.size() != _expected.size())
    {
        diff.fill(Qt::red);
        return diff;
    }

    const QImage actual   = _actual.convertToFormat(QImage::Format_RGB32);
    const QImage expected = _expected.convertToFormat(QImage::Format_RGB32);
    for(int y = 0 ; y < actual.height() ; ++y)
    {
        const auto* row       = reinterpret_cast<const QRgb*>(actual.constScanLine(y));
        const auto* reference = reinterpret_cast<const QRgb*>(expected.constScanLine(y));
        auto* output          = reinterpret_cast<QRgb*>(diff.scanLine(y));
        for(int x = 0 ; x < actual.width() ; ++x)
        {
            output[x] = qRgb(
                std::abs(qRed(row[x]) - qRed(reference[x])),
                std::abs(qGreen(row[x]) - qGreen(reference[x])),
                std::abs(qBlue(row[x]) - qBlue(reference[x]))
            );
        }
    }

    return diff;
}

//------------------------------------------------------------------------------

void save_image(const QImage& _image, const std::filesystem::path& _path)
{
    require_image(_image);
    std::filesystem::create_directories(_path.parent_path());
    if(!_image.save(qt_path(_path), "PNG"))
    {
        throw std::runtime_error("cannot save image: " + qt_path(_path).toStdString());
    }
}

//------------------------------------------------------------------------------

bool frame_stability::observe(const QImage& _image)
{
    require_image(_image);
    // Exact: a frame within the tolerance of the previous one may still be moving.
    const QImage current = _image.convertToFormat(QImage::Format_RGB32);
    const bool stable    = !m_previous.isNull() && current == m_previous;
    m_previous = current;
    return stable;
}

} // namespace detail

} // namespace sight::ui::test
