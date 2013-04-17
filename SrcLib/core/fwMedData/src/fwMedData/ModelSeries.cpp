/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/Image.hpp>
#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/ModelSeries.hpp"

fwDataRegisterMacro( ::fwMedData::ModelSeries );

namespace fwMedData
{

ModelSeries::ModelSeries(::fwData::Object::Key key): Series(key)
{
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries()
{}

//------------------------------------------------------------------------------

void ModelSeries::shallowCopy(const ::fwData::Object::csptr &_source)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_attrReconstructionDB = other->m_attrReconstructionDB;
}

//------------------------------------------------------------------------------

void ModelSeries::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );

    m_attrReconstructionDB = other->m_attrReconstructionDB;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

