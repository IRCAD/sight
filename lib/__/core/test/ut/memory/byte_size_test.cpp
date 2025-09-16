/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/memory/byte_size.hpp>
#include <core/memory/exception/bad_cast.hpp>

#include <doctest/doctest.h>

#include <limits>

TEST_SUITE("sight::core::memory::byte_size")
{
//------------------------------------------------------------------------------

    TEST_CASE("size")
    {
        {
            sight::core::memory::byte_size bsize;
            std::string ref_iec("0 Bytes");
            std::string ref_si("0 Bytes");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }

        {
            sight::core::memory::byte_size::size_t size = 1;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::KB);
            std::string ref_iec("1000 Bytes");
            std::string ref_si("1 KB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            sight::core::memory::byte_size::size_t size = 1;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::KI_B);
            std::string ref_iec("1 KiB");
            std::string ref_si("1.024 KB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            sight::core::memory::byte_size::size_t size = 1;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::GB);
            std::string ref_iec("953.674 MiB");
            std::string ref_si("1 GB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            sight::core::memory::byte_size::size_t size = 1;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::GI_B);
            std::string ref_iec("1 GiB");
            std::string ref_si("1.07374 GB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }

        {
            double size = 1.5;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::GI_B);
            std::string ref_iec("1.5 GiB");
            std::string ref_si("1.61061 GB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }    {
            double size = 1.5;
            sight::core::memory::byte_size bsize(size, sight::core::memory::byte_size::TB);
            std::string ref_iec("1.36424 TiB");
            std::string ref_si("1.5 TB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }

        {
            std::string size("1.42 MiB");
            sight::core::memory::byte_size bsize(size);
            std::string ref_iec("1.42 MiB");
            std::string ref_si("1.48898 MB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            std::string size("1.32 GB");
            sight::core::memory::byte_size bsize(size);
            std::string ref_iec("1.22935 GiB");
            std::string ref_si("1.32 GB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            std::string size("1.123GB");
            sight::core::memory::byte_size bsize(size);
            std::string ref_iec("1.04588 GiB");
            std::string ref_si("1.123 GB");
            std::string result_iec(bsize);
            std::string result_si(bsize.get_human_readable_size(sight::core::memory::byte_size::si));
            CHECK_EQ(ref_iec, result_iec);
            CHECK_EQ(ref_si, result_si);
        }
        {
            std::string size("  1.42   MiB ");
            sight::core::memory::byte_size bsize(size);
            sight::core::memory::byte_size::size_t ref_size    = 1488977;
            sight::core::memory::byte_size::size_t result_size = bsize.size();
            CHECK_EQ(ref_size, result_size);
        }
        {
            std::string size("1.42 TB  ");
            sight::core::memory::byte_size bsize(size);
            sight::core::memory::byte_size::size_t ref_size    = 1420000000000LL;
            sight::core::memory::byte_size::size_t result_size = bsize.size();
            CHECK_EQ(ref_size, result_size);
        }
        {
            std::string size("  1.42 PiB");
            sight::core::memory::byte_size bsize(size);
            sight::core::memory::byte_size::size_t ref_size    = 1598777867716526LL;
            sight::core::memory::byte_size::size_t result_size = bsize.size();
            CHECK_EQ(ref_size, result_size);
        }

        {
            std::string size("1.66MiB");
            sight::core::memory::byte_size bsize(size);
            sight::core::memory::byte_size::size_t ref_size    = 1740636;
            sight::core::memory::byte_size::size_t result_size = bsize.size();
            CHECK_EQ(ref_size, result_size);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("invalid_size")
    {
        CHECK_THROWS_AS(sight::core::memory::byte_size a(-1.), sight::core::memory::exception::bad_cast);
        CHECK_THROWS_AS(sight::core::memory::byte_size a(std::string("-2B")), sight::core::memory::exception::bad_cast);
        sight::core::memory::byte_size a;
        CHECK_THROWS_AS(a = -3., sight::core::memory::exception::bad_cast);
        CHECK_THROWS_AS(a = std::string("-4B"), sight::core::memory::exception::bad_cast);
        CHECK_THROWS_AS(a.set_size(-5.), sight::core::memory::exception::bad_cast);
        CHECK_THROWS_AS(a.set_size(std::string("-6B")), sight::core::memory::exception::bad_cast);
        CHECK_THROWS_AS(a = std::string("7A"), sight::core::memory::exception::bad_cast);
    }
} // TEST_SUITE
