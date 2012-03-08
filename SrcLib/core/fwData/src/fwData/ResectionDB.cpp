/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/Resection.hpp"
#include "fwData/ResectionDB.hpp"

fwDataRegisterMacro( ::fwData::ResectionDB );
namespace fwData
{

//------------------------------------------------------------------------------

const Object::FieldID ResectionDB::ID_RESECTIONS = "ID_RESECTIONS";

//------------------------------------------------------------------------------

ResectionDB::ResectionDB ()
{
    SLM_TRACE_FUNC();
    this->setField( ResectionDB::ID_RESECTIONS );
}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ResectionDB::shallowCopy( ResectionDB::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::deepCopy( ResectionDB::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
}

//------------------------------------------------------------------------------

boost::uint32_t  ResectionDB::getResectionSize() const
{
    return this->getField( ResectionDB::ID_RESECTIONS )->children().size();
}

//------------------------------------------------------------------------------

void ResectionDB::addResection( ::fwData::Resection::sptr _resection )
{
    this->addFieldElement(ResectionDB::ID_RESECTIONS, _resection);
}

//------------------------------------------------------------------------------

std::pair< ResectionDB::ResectionIterator, ResectionDB::ResectionIterator > ResectionDB::getResections()
{
    ResectionIterator begin(  getField( ResectionDB::ID_RESECTIONS )->children().begin() );
    ResectionIterator   end(  getField( ResectionDB::ID_RESECTIONS )->children().end()   );
    return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< ResectionDB::ResectionConstIterator, ResectionDB::ResectionConstIterator > ResectionDB::getResections() const
{
    ResectionConstIterator begin(  getField( ResectionDB::ID_RESECTIONS )->children().begin()   );
    ResectionConstIterator   end(  getField( ResectionDB::ID_RESECTIONS )->children().end()   );
    return std::make_pair( begin, end );
}

} // end namespace fwData
