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

#include <utest/filter.hpp>

#include <doctest/doctest.h>

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::dicom::reader_writer")
{
    TEST_CASE("basic")
    {
        if(utest::filter::ignore_slow_tests())
        {
            return;
        }

        // cspell:ignore Acuson
        test_image("us/Enhanced US Volume Storage/GE, 3D, lossy JPEG");
        test_image("us/Ultrasound Image Storage/Philips, RLE, palette color");
        test_image("us/Ultrasound Multi-frame Image Storage/Acuson, 2D+t, lossy JPEG");
        test_image("us/Ultrasound Multi-frame Image Storage/GE, 2D+t, RLE");
        test_image("us/Ultrasound Multi-frame Image Storage/Philips, 2D+t,  lossy JPEG");
    }
}

} // namespace sight::io::dicom::ut
