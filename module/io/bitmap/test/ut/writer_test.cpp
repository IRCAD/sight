/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/os/temp_path.hpp>

#include <data/image.hpp>

#include <io/__/service/writer.hpp>
#include <io/bitmap/writer.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

// cspell:ignore nvjpeg

namespace sight::module::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void runwriter(
    const boost::property_tree::ptree& _config,
    const sight::data::image::csptr _image,
    bool _should_fail = false
)
{
    service::base::sptr swriter = service::add("sight::module::io::bitmap::writer");
    CHECK_MESSAGE(swriter, std::string("Failed to create service 'sight::module::io::bitmap::writer'"));
    swriter->set_input(_image, "data.write");

    CHECK_NOTHROW(swriter->set_config(_config));
    CHECK_NOTHROW(swriter->configure());
    CHECK_NOTHROW(swriter->start().get());
    CHECK_NOTHROW(swriter->update().get());
    CHECK_NOTHROW(swriter->stop().get());
    service::remove(swriter);

    // Check the result...
    CHECK_EQ(
        _should_fail,
        std::dynamic_pointer_cast<sight::io::service::writer>(swriter)->has_failed()
    );
}

//------------------------------------------------------------------------------

inline static data::image::csptr get_synthetic_image()
{
    static const data::image::csptr s_GENERATED =
        []
        {
            auto image           = std::make_shared<data::image>();
            const auto dump_lock = image->dump_lock();
            image->resize({800, 600, 0}, core::type::UINT8, data::image::rgb);

            auto it        = image->begin<data::iterator::rgb>();
            const auto end = image->end<data::iterator::rgb>();

            std::uint8_t r = 0;
            std::uint8_t g = 0;
            std::uint8_t b = 0;
            std::for_each(
                it,
                end,
                [&](auto& _x)
            {
                _x.r = r;
                _x.g = g;
                _x.b = b;

                r += 1;

                if(r > 254)
                {
                    r  = 0;
                    g += 1;
                }

                if(g > 254)
                {
                    g  = 0;
                    b += 1;
                }

                if(b > 254)
                {
                    r = 0;
                    g = 0;
                    b = 0;
                }
            });

            return image;
        }();

    return s_GENERATED;
}

//------------------------------------------------------------------------------

inline static void test_enable(
    const std::filesystem::path& _temp_folder,
    const data::image::csptr& _expected_image,
    const std::vector<sight::io::bitmap::writer::mode>& _modes,
    bool _gpu_required
)
{
    const std::array<std::string, 4> extensions = {".jp2", ".jpg", ".png", ".tiff"};

    for(const auto& ext : extensions)
    {
        const auto filename   = "config_" + ext;
        const auto& file_path = _temp_folder / filename;

        std::map<sight::io::bitmap::writer::mode, std::size_t> sizes;

        for(const auto& mode : _modes)
        {
            // Add file
            service::config_t config;
            config.add("path.<xmlattr>.file", file_path.string());
            config.add("gpu_required", _gpu_required);

            // Run the service
            runwriter(config, _expected_image);

            // Only test if the file exists. Conformance tests are already done in the writer
            const std::string file_error_msg = "File '" + file_path.string() + "' doesn't exist.";
            CHECK_MESSAGE(std::filesystem::is_regular_file(file_path), file_error_msg);

            sizes.insert_or_assign(mode, std::filesystem::file_size(file_path));
        }

        // Sizes should be bigger than 0..
        CHECK(sizes[sight::io::bitmap::writer::mode::best] > std::size_t(0));
        CHECK(sizes[sight::io::bitmap::writer::mode::fast] > std::size_t(0));
        CHECK(sizes[sight::io::bitmap::writer::mode::fast] <= sizes[sight::io::bitmap::writer::mode::best]);
    }
}

TEST_SUITE("sight::module::io::bitmap::writer")
{
    TEST_CASE("basic")
    {
        core::os::temp_dir tmp_dir;
        const auto& file_path = tmp_dir / "basic.tiff";

        service::config_t config;
        config.add("path.<xmlattr>.file", file_path.string());

        const auto& expected_image = get_synthetic_image();
        runwriter(config, expected_image);

        // Only test if the file exists. Conformance tests are already done in the writer
        const std::string file_error_msg = "File '" + file_path.string() + "' doesn't exist.";
        CHECK_MESSAGE(std::filesystem::is_regular_file(file_path), file_error_msg);
        CHECK(std::filesystem::file_size(file_path) > 0);
    }

    TEST_CASE("config")
    {
        core::os::temp_dir tmp_dir;

        // Build mode list
        const std::vector modes {
            sight::io::bitmap::writer::mode::best,
            sight::io::bitmap::writer::mode::fast
        };

        const auto& expected_image = get_synthetic_image();
        {
            test_enable(tmp_dir, expected_image, modes, false);
        }

        if(sight::io::bitmap::nvjpeg())
        {
            test_enable(tmp_dir, expected_image, modes, true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap::ut
