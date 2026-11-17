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

#include <io/igtl/detail/raw_message.hpp>

#include <doctest/doctest.h>

#include <igtlMessageHeader.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace sight::io::igtl
{

TEST_SUITE("sight::io::igtl")
{
    TEST_CASE("raw_message_serialization")
    {
        using detail::raw_message;

        const std::vector<raw_message::raw_data_t> payloads {{}, {'\0', 'a', '\0', '\x7f'}};
        const std::vector<std::uint16_t> header_versions {
            1
#if defined(OpenIGTLink_HEADER_VERSION) && OpenIGTLink_HEADER_VERSION >= 2
            , 2
#endif
        };

        for(const auto header_version : header_versions)
        {
            for(const auto& payload : payloads)
            {
                CAPTURE(header_version);
                CAPTURE(payload.size());
                // Call through MessageBase, as the network code does.
                auto raw_sent = raw_message::New("INTEGER");
                ::igtl::MessageBase::Pointer sent(raw_sent.GetPointer());
                raw_sent->append(payload);
                sent->SetDeviceName("scalar");
#ifdef OpenIGTLink_HEADER_VERSION
                sent->SetHeaderVersion(header_version);
#if OpenIGTLink_HEADER_VERSION >= 2
                if(header_version == 2)
                {
                    REQUIRE(sent->SetMetaDataElement("test", IANA_TYPE_US_ASCII, "raw payload"));
                }
#endif
#endif
                REQUIRE(sent->Pack() == 1);

                if(header_version == 1)
                {
                    // The legacy wire format is a native uint32 length followed by the raw bytes.
                    REQUIRE(sent->GetPackBodySize() == sizeof(std::uint32_t) + payload.size());
                    std::uint32_t size = 0;
                    std::memcpy(&size, sent->GetPackBodyPointer(), sizeof(size));
                    CHECK(size == payload.size());
                    if(!payload.empty())
                    {
                        const auto* body = static_cast<const char*>(sent->GetPackBodyPointer());
                        CHECK(std::memcmp(body + sizeof(size), payload.data(), payload.size()) == 0);
                    }
                }

                // Receive a separate header and body, including CRC verification.
                auto header = ::igtl::MessageHeader::New();
                header->InitPack();
                std::memcpy(
                    header->GetPackPointer(),
                    sent->GetPackPointer(),
                    static_cast<std::size_t>(header->GetPackSize())
                );
                REQUIRE((header->Unpack(1) & ::igtl::MessageHeader::UNPACK_HEADER) != 0);
                CHECK(std::string(header->GetDeviceType()) == "INTEGER");
                CHECK(std::string(header->GetDeviceName()) == "scalar");

                auto received = raw_message::New("INTEGER");
                received->SetMessageHeader(header);
                received->AllocatePack();
                REQUIRE(received->GetPackBodySize() == sent->GetPackBodySize());
                std::memcpy(
                    received->GetPackBodyPointer(),
                    sent->GetPackBodyPointer(),
                    static_cast<std::size_t>(sent->GetPackBodySize())
                );
                REQUIRE((received->Unpack(1) & ::igtl::MessageHeader::UNPACK_BODY) != 0);
                CHECK(received->get_message() == payload);
#if defined(OpenIGTLink_HEADER_VERSION) && OpenIGTLink_HEADER_VERSION >= 2
                if(header_version == 2)
                {
                    std::string metadata;
                    REQUIRE(received->GetMetaDataElement("test", metadata));
                    CHECK(metadata == "raw payload");
                }
#endif
            }
        }
    }
}

} // namespace sight::io::igtl
