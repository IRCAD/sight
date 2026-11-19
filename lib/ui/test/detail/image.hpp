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

#pragma once

#include <ui/test/image.hpp>

#include <QImage>
#include <QString>

#include <cstdint>
#include <filesystem>
#include <string>

namespace sight::ui::test::detail
{

/// A path as Qt reads it: UTF-16, whatever the code page of the machine.
[[nodiscard]] inline QString qt_path(const std::filesystem::path& _path)
{
    return QString::fromStdU16String(_path.u16string());
}

//------------------------------------------------------------------------------

SIGHT_UI_TEST_API void validate_threshold(const image_threshold& _threshold);

struct image_metrics
{
    bool same_size {false};
    std::uint64_t bad_pixels {0};
    std::uint64_t total_pixels {0};
    int max_channel_difference {0};
    image_threshold tolerance;
    QSize actual_size;
    QSize expected_size;

    [[nodiscard]] SIGHT_UI_TEST_API bool matches() const;
    [[nodiscard]] SIGHT_UI_TEST_API std::string describe() const;
};

[[nodiscard]] SIGHT_UI_TEST_API image_metrics compare_images(
    const QImage& _actual,
    const QImage& _expected,
    const image_threshold& _threshold
);

/// Black for identical channels, absolute RGB differences otherwise; a size mismatch is entirely red.
[[nodiscard]] SIGHT_UI_TEST_API QImage image_difference(const QImage& _actual, const QImage& _expected);

SIGHT_UI_TEST_API void save_image(const QImage& _image, const std::filesystem::path& _path);

/// Requires consecutive byte-identical RGB captures, not merely two captures within the reference tolerance.
class frame_stability
{
public:

    [[nodiscard]] SIGHT_UI_TEST_API bool observe(const QImage& _image);
    //------------------------------------------------------------------------------

    void reset()
    {
        m_previous = {};
    }

private:

    QImage m_previous;
};

} // namespace sight::ui::test::detail
