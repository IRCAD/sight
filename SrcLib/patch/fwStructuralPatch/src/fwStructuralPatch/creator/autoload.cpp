/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/creator/fwData/Landmarks1.hpp"
#include "fwStructuralPatch/creator/fwData/PointList1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ActivitySeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Equipment1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ImageSeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/ModelSeries1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Patient1.hpp"
#include "fwStructuralPatch/creator/fwMedData/Study1.hpp"

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{
namespace creator
{

/// Registers structural creators dedicated to data creation.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Equipment1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Patient1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::Study1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ModelSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ImageSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwMedData::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwData::Landmarks1::New());
        creators->registerCreator(::fwStructuralPatch::creator::fwData::PointList1::New());
    }

    static runner r;
};

runner runner::r;

} // namespace creator
} // namespace fwStructuralPatch
