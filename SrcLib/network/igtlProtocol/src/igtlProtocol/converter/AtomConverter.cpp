/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "igtlProtocol/converter/AtomConverter.hpp"

#include "igtlProtocol/archiver/MemoryReadArchive.hpp"
#include "igtlProtocol/archiver/MemoryWriteArchive.hpp"
#include "igtlProtocol/DataConverter.hpp"
#include "igtlProtocol/RawMessage.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtoms/Object.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <boost/filesystem/operations.hpp>

namespace igtlProtocol
{
namespace converter
{
const std::string AtomConverter::s_IGTL_TYPE          = "ATOMS";
const std::string AtomConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Object::classname();

converterRegisterMacro(::igtlProtocol::converter::AtomConverter);

AtomConverter::AtomConverter()
{
}

//-----------------------------------------------------------------------------

AtomConverter::~AtomConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer AtomConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    typedef ::igtlProtocol::archiver::MemoryWriteArchive MemoryWriteArchiveType;

    ::fwAtomsBoostIO::Writer writer(::fwAtomConversion::convert(::fwData::Object::constCast(src)));

    RawMessage::Pointer msg                   = RawMessage::New(AtomConverter::s_IGTL_TYPE);
    MemoryWriteArchiveType::sptr memoryWriter = std::make_shared<MemoryWriteArchiveType>(msg->getMessage());
    writer.write(memoryWriter);
    memoryWriter->writeArchive();
    return ::igtl::MessageBase::Pointer(msg.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr AtomConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    typedef ::igtlProtocol::archiver::MemoryReadArchive MemoryReadArchiveType;
    ::fwAtomsBoostIO::Reader reader;

    RawMessage::Pointer msg = RawMessage::Pointer(dynamic_cast< RawMessage* >(src.GetPointer()));

    MemoryReadArchiveType::sptr memoryReader = std::make_shared<MemoryReadArchiveType>(&msg->getMessage()[0],
                                                                                       msg->getMessage().size());

    ::fwAtoms::Base::sptr atomObj = reader.read(memoryReader);
    ::fwData::Object::sptr obj    = ::fwAtomConversion::convert(::fwAtoms::Object::dynamicCast(atomObj));

    return obj;
}

//-----------------------------------------------------------------------------

IConverter::sptr AtomConverter::New()
{
    return std::make_shared< AtomConverter >();
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
} // namespace igtlProtocol
