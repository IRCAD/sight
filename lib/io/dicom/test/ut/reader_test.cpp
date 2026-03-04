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

#include "helper.hpp"

#include <io/dicom/reader/file.hpp>

#include <utest/filter.hpp>

#include <utest_data/data.hpp>

#include <boost/uuid/detail/sha1.hpp>

#include <doctest/doctest.h>

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::dicom::reader")
{
    TEST_CASE("read_jms_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        // cspell: ignore Genou
        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/JMSGenou");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(512), size[0]);
        CHECK_EQ(std::size_t(512), size[1]);
        CHECK_EQ(std::size_t(404), size[2]);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ct_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        const double delta = 0.00001;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(512), size[0]);
        CHECK_EQ(std::size_t(512), size[1]);
        CHECK_EQ(std::size_t(129), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.57), delta);
        CHECK_LT(std::abs(spacing[1] - 0.57), delta);
        CHECK_LT(std::abs(spacing[2] - 1.6), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK_LT(std::abs(image_series->window_center().front() - 0.0), delta);

        // Check window width
        CHECK_LT(std::abs(image_series->window_width().front() - 0.0), delta);

        // Check image type
        CHECK_EQ(core::type::INT16, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_mr_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set =
            read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/46-MR-BARRE-MONO2-12-shoulder");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(1024), size[0]);
        CHECK_EQ(std::size_t(1024), size[1]);
        CHECK_EQ(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.2), delta);
        CHECK_LT(std::abs(spacing[1] - 0.2), delta);
        CHECK_LT(std::abs(spacing[2] - 6.6), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - (-180.058)), delta);
        CHECK_LT(std::abs(origin[1] - (-97.1478)), delta);
        CHECK_LT(std::abs(origin[2] - 112.828), delta);

        // Check window center
        CHECK_LT(std::abs(image_series->window_center().front() - 1000), delta);

        // Check window width
        CHECK_LT(std::abs(image_series->window_width().front() - 2000), delta);

        // Check image type
        CHECK_EQ(core::type::DOUBLE, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ot_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/42-OT-BARRE-MONO2-8-colon");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(512), size[0]);
        CHECK_EQ(std::size_t(512), size[1]);
        CHECK_EQ(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 1), delta);
        CHECK_LT(std::abs(spacing[1] - 1), delta);
        CHECK_LT(std::abs(spacing[2] - 1), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_multiple_rescale_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/83-CT-MultipleRescale");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        // Get internal buffer
        const auto dump_lock = image_series->dump_lock();
        auto* buffer         = image_series->buffer();
        CHECK(buffer);

        // Compute sha1 digest
        boost::uuids::detail::sha1 sha1;
        sha1.process_bytes(static_cast<char*>(buffer), image_series->size_in_bytes());
        boost::uuids::detail::sha1::digest_type digest = {0};
        sha1.get_digest(digest);

        // Check digests
        boost::uuids::detail::sha1::digest_type expected_digest = {
#if BOOST_VERSION >= 108600
            48, 42, 44, 21, 84, 159, 219, 25, 39, 159, 109, 192, 251, 147, 200, 220, 132, 178, 192, 182
#else
            808070165U, 1419762457U, 664759744U, 4220766428U, 2226307254U
#endif
        };

        for(std::size_t i = 0 ; i < sizeof(digest) / sizeof(*digest) ; ++i)
        {
            CHECK_MESSAGE(expected_digest[i] == digest[i], "Multiple rescale image hash comparison failed");
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ct_with_surview_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/84-CT-Surview");
        CHECK_EQ(std::size_t(3), series_set->size());

        for(const auto& series : *series_set)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
            CHECK(image_series);
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_mr_with_temporal_position_series_set")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/85-MR-TemporalPosition");
        CHECK_EQ(std::size_t(4), series_set->size());

        for(const auto& series : *series_set)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
            CHECK(image_series);
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ct_series_set_issue01")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "sight/Patient/Dicom/DicomDB/86-CT-Skull");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_enhanced_us_volume")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "us/Enhanced US Volume Storage/GE, 3D, lossy JPEG");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->front());
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(187), size[0]);
        CHECK_EQ(std::size_t(157), size[1]);
        CHECK_EQ(std::size_t(75), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.5), delta);
        CHECK_LT(std::abs(spacing[1] - 0.5), delta);
        CHECK_LT(std::abs(spacing[2] - 0.5), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - (-436.88594387944)), delta);
        CHECK_LT(std::abs(origin[1] - (-41.288431662308369)), delta);
        CHECK_LT(std::abs(origin[2] - (-68.743955174627899)), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_image_aixplorer")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        // cspell:ignore Aixplorer
        const auto& series_set = read(utest_data::dir() / "us/Ultrasound Image Storage/Aixplorer");
        CHECK_EQ(std::size_t(2), series_set->size());

        for(const auto& series : *series_set)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
            CHECK(image_series);

            if(image_series->get_file().filename() == "us_monochrome2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(1440), size[0]);
                CHECK_EQ(std::size_t(1080), size[1]);
                CHECK_EQ(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 0.03), delta);
                CHECK_LT(std::abs(spacing[1] - 0.03), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK(image_series->window_center().empty());

                // Check window width
                CHECK(image_series->window_width().empty());

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
            else if(image_series->get_file().filename() == "us_rgb.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(1440), size[0]);
                CHECK_EQ(std::size_t(1080), size[1]);
                CHECK_EQ(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 0.03), delta);
                CHECK_LT(std::abs(spacing[1] - 0.03), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK(image_series->window_center().empty());

                // Check window width
                CHECK(image_series->window_width().empty());

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_image_ge_lossy_jpeg")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(utest_data::dir() / "us/Ultrasound Image Storage/GE, lossy JPEG");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(636), size[0]);
        CHECK_EQ(std::size_t(434), size[1]);
        CHECK_EQ(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.05), delta);
        CHECK_LT(std::abs(spacing[1] - 0.05), delta);
        CHECK_LT(std::abs(spacing[2] - 1.0), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_image_kretztechnik")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        // cspell:ignore Kretztechnik
        const auto& series_set = read(
            utest_data::dir()
            / "us/Ultrasound Image Storage/GE, pixel spacing, Kretztechnik non-cartesian volume"
        );
        CHECK_EQ(std::size_t(2), series_set->size());

        for(const auto& series : *series_set)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
            CHECK(image_series);

            // cspell:ignore kretz
            if(image_series->get_file().filename() == "us_kretz.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(800), size[0]);
                CHECK_EQ(std::size_t(600), size[1]);
                CHECK_EQ(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 1.0), delta);
                CHECK_LT(std::abs(spacing[1] - 1.0), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK(image_series->window_center().empty());

                // Check window width
                CHECK(image_series->window_width().empty());

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
            else if(image_series->get_file().filename() == "us_kretz2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(1136), size[0]);
                CHECK_EQ(std::size_t(852), size[1]);
                CHECK_EQ(std::size_t(1), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 0.01), delta);
                CHECK_LT(std::abs(spacing[1] - 0.01), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK(image_series->window_center().empty());

                // Check window width
                CHECK(image_series->window_width().empty());

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_image_philips_16bit_palette")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(
            utest_data::dir() / "us/Ultrasound Image Storage/Philips, 16 bit palette color, aspect ratio"
        );
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(480), size[0]);
        CHECK_EQ(std::size_t(430), size[1]);
        CHECK_EQ(std::size_t(1), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.01), delta);
        CHECK_LT(std::abs(spacing[1] - 3.6), delta);
        CHECK_LT(std::abs(spacing[2] - 1.0), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT16, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_image_philips_rle_palette")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set =
            read(utest_data::dir() / "us/Ultrasound Image Storage/Philips, RLE, palette color");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(800), size[0]);
        CHECK_EQ(std::size_t(600), size[1]);
        CHECK_EQ(std::size_t(13), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.01), delta);
        CHECK_LT(std::abs(spacing[1] - 0.01), delta);
        CHECK_LT(std::abs(spacing[2] - 1.0), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_multiframe_acuson")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        // cspell:ignore Acuson
        const auto& series_set = read(
            utest_data::dir() / "us/Ultrasound Multi-frame Image Storage/Acuson, 2D+t, lossy JPEG"
        );
        CHECK_EQ(std::size_t(2), series_set->size());

        for(const auto& series : *series_set)
        {
            const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series);
            CHECK(image_series);

            if(image_series->get_file().filename() == "us_acuson.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(576), size[0]);
                CHECK_EQ(std::size_t(456), size[1]);
                CHECK_EQ(std::size_t(28), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 1.0), delta);
                CHECK_LT(std::abs(spacing[1] - 1.0), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK_LT(std::abs(image_series->window_center().front() - 128.0), delta);

                // Check window width
                CHECK_LT(std::abs(image_series->window_width().front() - 255.0), delta);

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
            else if(image_series->get_file().filename() == "us_acuson2.dcm")
            {
                const double delta = 0.01;

                // Check number of dimensions
                CHECK_EQ(std::size_t(3), image_series->num_dimensions());

                // Check size
                const auto& size = image_series->size();
                CHECK_EQ(std::size_t(576), size[0]);
                CHECK_EQ(std::size_t(456), size[1]);
                CHECK_EQ(std::size_t(55), size[2]);

                // Check spacing
                const auto& spacing = image_series->spacing();
                CHECK_LT(std::abs(spacing[0] - 1.0), delta);
                CHECK_LT(std::abs(spacing[1] - 1.0), delta);
                CHECK_LT(std::abs(spacing[2] - 1.0), delta);

                // Check origin
                const auto& origin = image_series->origin();
                CHECK_LT(std::abs(origin[0] - 0.0), delta);
                CHECK_LT(std::abs(origin[1] - 0.0), delta);
                CHECK_LT(std::abs(origin[2] - 0.0), delta);

                // Check window center
                CHECK_LT(std::abs(image_series->window_center().front() - 128.0), delta);

                // Check window width
                CHECK_LT(std::abs(image_series->window_width().front() - 255.0), delta);

                // Check image type
                CHECK_EQ(core::type::UINT8, image_series->type());
            }
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_multiframe_ge_rle")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set =
            read(utest_data::dir() / "us/Ultrasound Multi-frame Image Storage/GE, 2D+t, RLE");
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(636), size[0]);
        CHECK_EQ(std::size_t(434), size[1]);
        CHECK_EQ(std::size_t(7), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.03), delta);
        CHECK_LT(std::abs(spacing[1] - 0.03), delta);
        CHECK_LT(std::abs(spacing[2] - 1.0), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("read_ultrasound_multiframe_philips_lossy_jpeg")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        const auto& series_set = read(
            utest_data::dir() / "us/Ultrasound Multi-frame Image Storage/Philips, 2D+t,  lossy JPEG"
        );
        CHECK_EQ(std::size_t(1), series_set->size());

        const auto& image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
        CHECK(image_series);

        const double delta = 0.01;

        // Check number of dimensions
        CHECK_EQ(std::size_t(3), image_series->num_dimensions());

        // Check size
        const auto& size = image_series->size();
        CHECK_EQ(std::size_t(800), size[0]);
        CHECK_EQ(std::size_t(600), size[1]);
        CHECK_EQ(std::size_t(292), size[2]);

        // Check spacing
        const auto& spacing = image_series->spacing();
        CHECK_LT(std::abs(spacing[0] - 0.01), delta);
        CHECK_LT(std::abs(spacing[1] - 0.01), delta);
        CHECK_LT(std::abs(spacing[2] - 1.0), delta);

        // Check origin
        const auto& origin = image_series->origin();
        CHECK_LT(std::abs(origin[0] - 0.0), delta);
        CHECK_LT(std::abs(origin[1] - 0.0), delta);
        CHECK_LT(std::abs(origin[2] - 0.0), delta);

        // Check window center
        CHECK(image_series->window_center().empty());

        // Check window width
        CHECK(image_series->window_width().empty());

        // Check image type
        CHECK_EQ(core::type::UINT8, image_series->type());
    }
}

} // namespace sight::io::dicom::ut
