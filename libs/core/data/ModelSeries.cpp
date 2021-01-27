/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/ModelSeries.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Exception.hpp>
#include <data/Image.hpp>
#include <data/Reconstruction.hpp>
#include <data/registry/macros.hpp>

fwDataRegisterMacro( ::sight::data::ModelSeries )

namespace sight::data
{

const core::com::Signals::SignalKeyType ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG   = "reconstructionsAdded";
const core::com::Signals::SignalKeyType ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG = "reconstructionsRemoved";

ModelSeries::ModelSeries(data::Object::Key _key) :
    Series(_key)
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

void ModelSeries::shallowCopy(const data::Object::csptr& _source)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::shallowCopy(_source);

    m_reconstructionDB = other->m_reconstructionDB;
    m_dicomReference   = other->m_dicomReference;
}

//------------------------------------------------------------------------------

void ModelSeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    ModelSeries::csptr other = ModelSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::cachedDeepCopy(_source, cache);

    m_reconstructionDB.clear();
    for(const data::Reconstruction::sptr& rec : other->m_reconstructionDB)
    {
        m_reconstructionDB.push_back(data::Object::copy(rec, cache));
    }
    m_dicomReference = data::Object::copy(other->m_dicomReference);
}

//------------------------------------------------------------------------------

} // namespace sight::data
