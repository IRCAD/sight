/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMedData/ModelSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::ModelSeries );

namespace fwMedData
{

ModelSeries::ModelSeries(::fwData::Object::Key key) : Series(key)
{
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries()
{
}

//------------------------------------------------------------------------------

void ModelSeries::shallowCopy(const ::fwData::Object::csptr &_source)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_reconstructionDB = other->m_reconstructionDB;
}

//------------------------------------------------------------------------------

void ModelSeries::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_reconstructionDB.clear();
    for(const ::fwData::Reconstruction::sptr &rec : other->m_reconstructionDB)
    {
        m_reconstructionDB.push_back(::fwData::Object::copy(rec, cache));
    }
}

//------------------------------------------------------------------------------

} // namespace fwMedData

