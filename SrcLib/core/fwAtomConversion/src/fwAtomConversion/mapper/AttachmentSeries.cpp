/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
