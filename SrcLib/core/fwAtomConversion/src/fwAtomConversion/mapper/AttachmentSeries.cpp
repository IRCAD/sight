/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/mapper/AttachmentSeries.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/registry/macros.hpp"

#include <fwAtoms/String.hpp>

#include <fwMedData/AttachmentSeries.hpp>

#include <fwTools/UUID.hpp>

namespace fwAtomConversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( ::fwAtomConversion::mapper::AttachmentSeries, ::fwMedData::AttachmentSeries);

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr AttachmentSeries::convert( ::fwData::Object::sptr object,
                                                   DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    ::fwAtomConversion::DataVisitor visitor ( object, cache );
    metaclass.visit(visitor);
    ::fwAtoms::Object::sptr atom = visitor.getAtomObject();

    ::fwMedData::AttachmentSeries::sptr attachment = ::fwMedData::AttachmentSeries::dynamicCast(object);

    atom->setAttribute("attachment_availability", ::fwAtoms::String::New(std::string("EMBEDDED")));
    ::boost::filesystem::path attachFile = attachment->getAttachmentPath();
    atom->setAttribute("attachment_path", ::fwAtoms::String::New(attachFile.filename().string()) );

    return atom;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr AttachmentSeries::convert(  ::fwAtoms::Object::sptr atom,
                                                   AtomVisitor::DataCacheType& cache,
                                                   const AtomVisitor::IReadPolicy& uuidPolicy
                                                   )
{
    ::fwAtomConversion::AtomVisitor visitor ( atom, cache, uuidPolicy );
    visitor.visit();
    ::fwData::Object::sptr data                    = visitor.getDataObject();
    ::fwMedData::AttachmentSeries::sptr attachment = ::fwMedData::AttachmentSeries::dynamicCast(data);

    return attachment;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace fwAtomConversion
