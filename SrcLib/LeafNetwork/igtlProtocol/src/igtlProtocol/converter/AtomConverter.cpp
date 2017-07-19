/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/AtomConverter.hpp"
#include "igtlProtocol/RawMessage.hpp"
#include "igtlProtocol/archiver/MemoryReadArchive.hpp"
#include "igtlProtocol/archiver/MemoryWriteArchive.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwAtoms/Object.hpp>
#include <fwAtomsBoostIO/Writer.hpp>
#include <fwAtomsBoostIO/Reader.hpp>

#include <fwAtomConversion/convert.hpp>

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

    SPTR(MemoryWriteArchiveType) memoryWriter;
    ::fwAtomsBoostIO::Writer writer(::fwAtomConversion::convert(::fwData::Object::constCast(src)));
    RawMessage::Pointer msg;

    msg          = RawMessage::New(AtomConverter::s_IGTL_TYPE);
    memoryWriter = SPTR (MemoryWriteArchiveType) (new MemoryWriteArchiveType(msg->getMessage()));
    writer.write(memoryWriter);
    memoryWriter->writeArchive();
    return ::igtl::MessageBase::Pointer(msg.GetPointer());
}

//-----------------------------------------------------------------------------

void AtomConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                    ::fwData::Object::sptr& dest) const
{
    typedef ::igtlProtocol::archiver::MemoryReadArchive MemoryReadArchiveType;

    SPTR(MemoryReadArchiveType) memoryReader;
    ::fwAtomsBoostIO::Reader reader;
    ::fwAtoms::Base::sptr atomObj;
    RawMessage::Pointer msg;

    msg          = RawMessage::Pointer(dynamic_cast< RawMessage* >(src.GetPointer()));
    memoryReader = SPTR (MemoryReadArchiveType) (new MemoryReadArchiveType(&msg->getMessage()[0],
                                                                           msg->getMessage().size()));
    atomObj = reader.read(memoryReader);
    dest->shallowCopy(fwAtomConversion::convert(fwAtoms::Object::dynamicCast(atomObj)));
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
