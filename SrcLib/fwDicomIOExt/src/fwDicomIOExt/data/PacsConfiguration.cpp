/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwDicomIOExt/data/PacsConfiguration.hpp"

fwDataRegisterMacro( ::fwDicomIOExt::data::PacsConfiguration );

namespace fwDicomIOExt
{
namespace data
{

PacsConfiguration::PacsConfiguration(::fwData::Object::Key key) :
    m_attrLocalApplicationTitle(""),
    m_attrPacsHostName(""),
    m_attrPacsApplicationTitle(""),
    m_attrPacsApplicationPort(0),
    m_attrMoveApplicationTitle(""),
    m_attrMoveApplicationPort(0),
    m_attrRetrieveMethod(MOVE_RETRIEVE_METHOD)
{
}

//------------------------------------------------------------------------------

PacsConfiguration::~PacsConfiguration()
{
}

//------------------------------------------------------------------------------

void PacsConfiguration::shallowCopy(const Object::csptr &_source )
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_attrLocalApplicationTitle = other->m_attrLocalApplicationTitle;
    m_attrPacsHostName = other->m_attrPacsHostName;
    m_attrPacsApplicationTitle = other->m_attrPacsApplicationTitle;
    m_attrPacsApplicationPort = other->m_attrPacsApplicationPort;
    m_attrMoveApplicationTitle = other->m_attrMoveApplicationTitle;
    m_attrMoveApplicationPort = other->m_attrMoveApplicationPort;
    m_attrRetrieveMethod = other->m_attrRetrieveMethod;
}

//------------------------------------------------------------------------------

void PacsConfiguration::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_attrLocalApplicationTitle = other->m_attrLocalApplicationTitle;
    m_attrPacsHostName = other->m_attrPacsHostName;
    m_attrPacsApplicationTitle = other->m_attrPacsApplicationTitle;
    m_attrPacsApplicationPort = other->m_attrPacsApplicationPort;
    m_attrMoveApplicationTitle = other->m_attrMoveApplicationTitle;
    m_attrMoveApplicationPort = other->m_attrMoveApplicationPort;
    m_attrRetrieveMethod = other->m_attrRetrieveMethod;
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace fwDicomIOExt
