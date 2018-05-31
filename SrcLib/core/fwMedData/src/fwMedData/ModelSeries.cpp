/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
