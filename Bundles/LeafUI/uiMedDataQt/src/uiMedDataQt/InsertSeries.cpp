/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/InsertSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::uiMedDataQt::InsertSeries );

namespace uiMedDataQt
{

InsertSeries::InsertSeries(::fwData::Object::Key key) :
    Series(key)
{
}

//------------------------------------------------------------------------------

InsertSeries::~InsertSeries()
{
}

//------------------------------------------------------------------------------

void InsertSeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    InsertSeries::csptr other = InsertSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);
}

//------------------------------------------------------------------------------

void InsertSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    InsertSeries::csptr other = InsertSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);
}

//------------------------------------------------------------------------------

} // namespace uiMedDataQt

