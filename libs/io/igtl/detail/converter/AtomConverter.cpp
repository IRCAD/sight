/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/igtl/detail/converter/AtomConverter.hpp"

#include "io/igtl/detail/archiver/MemoryReadArchive.hpp"
#include "io/igtl/detail/archiver/MemoryWriteArchive.hpp"
#include "io/igtl/detail/DataConverter.hpp"
#include "io/igtl/detail/RawMessage.hpp"

#include <atoms/conversion/convert.hpp>
#include <atoms/Object.hpp>

#include <io/atoms/Reader.hpp>
#include <io/atoms/Writer.hpp>

#include <filesystem>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string AtomConverter::s_IGTL_TYPE          = "ATOMS";
const std::string AtomConverter::s_FWDATA_OBJECT_TYPE = data::Object::classname();

converterRegisterMacro(io::igtl::detail::converter::AtomConverter);

AtomConverter::AtomConverter()
{
}

//-----------------------------------------------------------------------------

AtomConverter::~AtomConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer AtomConverter::fromFwDataObject(data::Object::csptr src) const
{
    typedef io::igtl::detail::archiver::MemoryWriteArchive MemoryWriteArchiveType;

    io::atoms::Writer writer(sight::atoms::conversion::convert(data::Object::constCast(src)));

    RawMessage::Pointer msg                   = RawMessage::New(AtomConverter::s_IGTL_TYPE);
    MemoryWriteArchiveType::sptr memoryWriter = std::make_shared<MemoryWriteArchiveType>(msg->getMessage());
    writer.write(memoryWriter);
    memoryWriter->writeArchive();
    return ::igtl::MessageBase::Pointer(msg.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr AtomConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    typedef io::igtl::detail::archiver::MemoryReadArchive MemoryReadArchiveType;
    io::atoms::Reader reader;

    RawMessage::Pointer msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(src.GetPointer()));

    MemoryReadArchiveType::sptr memoryReader = std::make_shared<MemoryReadArchiveType>(
        &msg->getMessage()[0],
        msg->getMessage().size()
    );

    sight::atoms::Base::sptr atomObj = reader.read(memoryReader);
    data::Object::sptr obj           = sight::atoms::conversion::convert(sight::atoms::Object::dynamicCast(atomObj));

    return obj;
}

//-----------------------------------------------------------------------------

IConverter::sptr AtomConverter::New()
{
    return std::make_shared<AtomConverter>();
}

//-----------------------------------------------------------------------------

std::string const& AtomConverter::getIgtlType() const
{
    return AtomConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& AtomConverter::getFwDataObjectType() const
{
    return AtomConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter

} // namespace sight::io::igtl::detail
