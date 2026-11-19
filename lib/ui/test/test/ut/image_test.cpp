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

#include <ui/test/detail/image.hpp>

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <QColor>
#include <QString>

#include <limits>
#include <stdexcept>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::image")
{
    TEST_CASE("enforces_exact_channel_and_ratio_boundaries")
    {
        QImage expected(100, 10, QImage::Format_RGB32);
        expected.fill(qRgb(100, 100, 100));
        QImage actual = expected.copy();
        actual.setPixel(0, 0, qRgb(116, 100, 100));
        auto metrics = compare_images(actual, expected, {});
        CHECK(metrics.bad_pixels == 0);
        CHECK(metrics.matches());

        actual.setPixel(0, 0, qRgb(117, 100, 100));
        metrics = compare_images(actual, expected, {});
        CHECK(metrics.bad_pixels == 1);
        CHECK(metrics.total_pixels == 1000);
        CHECK(metrics.max_channel_difference == 17);
        CHECK(metrics.matches()); // Exactly 0.1%, not strictly below.

        actual.setPixel(1, 0, qRgb(100, 83, 100));
        metrics = compare_images(actual, expected, {});
        CHECK(metrics.bad_pixels == 2);
        CHECK_FALSE(metrics.matches());
        CHECK(compare_images(actual, expected, threshold(16, 0.002, "two differing pixels")).matches());
        CHECK_FALSE(compare_images(actual, expected, threshold(16, 0., "exact match")).matches());
    }

    TEST_CASE("compares_rgb_not_alpha_or_storage_format")
    {
        QImage actual(7, 3, QImage::Format_ARGB32);
        actual.fill(qRgba(40, 50, 60, 10));
        QImage expected(7, 3, QImage::Format_RGB888);
        expected.fill(QColor(40, 50, 60));
        CHECK(compare_images(actual, expected, threshold(0, 0., "RGB only")).matches());
        actual.setPixel(0, 0, qRgba(40, 50, 77, 255));
        CHECK(compare_images(actual, expected, {}).bad_pixels == 1);
    }

    TEST_CASE("rejects_wrong_render_sizes_and_invalid_inputs")
    {
        QImage actual(10, 10, QImage::Format_RGB32);
        actual.fill(Qt::black);
        QImage expected(20, 20, QImage::Format_RGB32);
        expected.fill(Qt::black);
        const auto metrics = compare_images(actual, expected, {});
        CHECK_FALSE(metrics.same_size);
        CHECK_FALSE(
            compare_images(
                actual,
                expected,
                threshold(255, 1., "even unlimited tolerance cannot resize")
            ).matches()
        );
        CHECK(image_difference(actual, expected).pixelColor(0, 0) == QColor(Qt::red));
        expected = QImage(10, 10, QImage::Format_RGB32);
        expected.fill(Qt::white);
        CHECK_FALSE(compare_images(actual, expected, {}).matches());
        CHECK_THROWS_AS(static_cast<void>(compare_images({}, expected, {})), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(threshold(-1, 0., "invalid")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(threshold(256, 0., "invalid")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(threshold(0, -0.1, "invalid")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(threshold(0, 1.1, "invalid")), std::invalid_argument);
        CHECK_THROWS_AS(
            static_cast<void>(threshold(0, std::numeric_limits<double>::quiet_NaN(), "invalid")),
            std::invalid_argument
        );
        CHECK_THROWS_AS(
            static_cast<void>(threshold(0, std::numeric_limits<double>::infinity(), "invalid")),
            std::invalid_argument
        );
        CHECK_THROWS_AS(static_cast<void>(threshold(0, 0., "")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(compare_images(actual, expected, {0, 0., {}})), std::invalid_argument);
    }

    TEST_CASE("writes_exact_difference_and_unicode_artifacts")
    {
        QImage expected(2, 1, QImage::Format_RGB32);
        expected.fill(qRgb(10, 20, 30));
        QImage actual = expected.copy();
        actual.setPixel(1, 0, qRgb(5, 28, 255));
        const auto diff = image_difference(actual, expected);
        CHECK(diff.pixel(0, 0) == qRgb(0, 0, 0));
        CHECK(diff.pixel(1, 0) == qRgb(5, 8, 225));

        core::os::temp_dir temporary;
        const auto path = temporary / std::filesystem::path(u"output_\u65e5") / "diff.png";
        save_image(diff, path);
        CHECK(QImage(QString::fromStdU16String(path.u16string())) == diff);
        // A filesystem error must propagate, not leave a success-shaped empty artifact.
        CHECK_THROWS_AS(save_image(diff, path / "not_a_directory.png"), std::filesystem::filesystem_error);
    }

    TEST_CASE("requires_two_consecutive_identical_rgb_frames")
    {
        frame_stability stability;
        QImage first(4, 4, QImage::Format_RGB32);
        first.fill(Qt::black);
        QImage second = first.copy();
        second.setPixel(0, 0, qRgb(1, 0, 0));
        CHECK_FALSE(stability.observe(first));
        CHECK_FALSE(stability.observe(second));
        CHECK_FALSE(stability.observe(first));
        CHECK(stability.observe(first));
        stability.reset();
        CHECK_FALSE(stability.observe(first));
        CHECK_THROWS_AS(static_cast<void>(stability.observe({})), std::invalid_argument);
    }
}

} // namespace sight::ui::test::detail
