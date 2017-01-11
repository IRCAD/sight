/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPacsIO/data/PacsConfiguration.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwPacsIO::data::PacsConfiguration );

namespace fwPacsIO
{
namespace data
{

PacsConfiguration::PacsConfiguration(::fwData::Object::Key key) :
    m_localApplicationTitle(""),
    m_pacsHostName(""),
    m_pacsApplicationTitle(""),
    m_pacsApplicationPort(0),
    m_moveApplicationTitle(""),
    m_moveApplicationPort(0),
    m_retrieveMethod(MOVE_RETRIEVE_METHOD)
{
}

//------------------------------------------------------------------------------

PacsConfiguration::~PacsConfiguration()
{
}

//------------------------------------------------------------------------------

void PacsConfiguration::shallowCopy(const Object::csptr& _source )
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_localApplicationTitle = other->m_localApplicationTitle;
    m_pacsHostName          = other->m_pacsHostName;
    m_pacsApplicationTitle  = other->m_pacsApplicationTitle;
    m_pacsApplicationPort   = other->m_pacsApplicationPort;
    m_moveApplicationTitle  = other->m_moveApplicationTitle;
    m_moveApplicationPort   = other->m_moveApplicationPort;
    m_retrieveMethod        = other->m_retrieveMethod;
}

//------------------------------------------------------------------------------

void PacsConfiguration::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    PacsConfiguration::csptr other = PacsConfiguration::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_localApplicationTitle = other->m_localApplicationTitle;
    m_pacsHostName          = other->m_pacsHostName;
    m_pacsApplicationTitle  = other->m_pacsApplicationTitle;
    m_pacsApplicationPort   = other->m_pacsApplicationPort;
    m_moveApplicationTitle  = other->m_moveApplicationTitle;
    m_moveApplicationPort   = other->m_moveApplicationPort;
    m_retrieveMethod        = other->m_retrieveMethod;
}

//------------------------------------------------------------------------------

} //namespace data
} //namespace fwPacsIO
