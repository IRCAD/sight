/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwMedData/ModelSeries.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Exception.hpp>
#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::ModelSeries );

namespace fwMedData
{

const ::fwCom::Signals::SignalKeyType ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG   = "reconstructionsAdded";
const ::fwCom::Signals::SignalKeyType ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG = "reconstructionsRemoved";

ModelSeries::ModelSeries(::fwData::Object::Key key) :
    Series(key)
{
    m_sigReconstructionsAdded   = ReconstructionsAddedSignalType::New();
    m_sigReconstructionsRemoved = ReconstructionsRemovedSignalType::New();

    m_signals( s_RECONSTRUCTIONS_ADDED_SIG,  m_sigReconstructionsAdded)
        ( s_RECONSTRUCTIONS_REMOVED_SIG,  m_sigReconstructionsRemoved);
}

//------------------------------------------------------------------------------

ModelSeries::~ModelSeries()
{
}

//------------------------------------------------------------------------------

void ModelSeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_reconstructionDB = other->m_reconstructionDB;
    m_dicomReference   = other->m_dicomReference;
}

//------------------------------------------------------------------------------

void ModelSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_reconstructionDB.clear();
    for(const ::fwData::Reconstruction::sptr& rec : other->m_reconstructionDB)
    {
        m_reconstructionDB.push_back(::fwData::Object::copy(rec, cache));
    }
    m_dicomReference = ::fwData::Object::copy(other->m_dicomReference);
}

//------------------------------------------------------------------------------

} // namespace fwMedData
