/************************************************************************
 *
 * Copyright (C) 2014-2016 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include "fwMedDataTools/ModelSeries.hpp"

#include <fwCom/Signal.hxx>
#include <fwCore/spyLog.hpp>

namespace fwMedDataTools
{

//------------------------------------------------------------------------------

void ModelSeries::addMesh(::fwMedData::ModelSeries::sptr &_modelSeries, const ::fwData::Mesh::sptr& _mesh,
                          const std::string& _organName, const std::string& _structureType,
                          const ::fwData::Color::sptr& _colour, ::fwData::Material::RepresentationType _mode,
                          bool _visible)
{
    //reconstruction creation
    ::fwData::Reconstruction::sptr reconstructionQuadMesh =
        createReconstructionFromMesh(_mesh, _organName, _structureType, _colour,_mode, _visible);

    addReconstruction(_modelSeries, reconstructionQuadMesh);

    auto sig = _modelSeries->signal< ::fwMedData::ModelSeries::ReconstructionsAddedSignalType>(
        ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG);

    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructionsVector;
    reconstructionsVector.push_back(reconstructionQuadMesh);
    sig->asyncEmit(reconstructionsVector);
}

//------------------------------------------------------------------------------

::fwData::Reconstruction::sptr ModelSeries::createReconstructionFromMesh( const ::fwData::Mesh::sptr& _mesh,
                                                                          const std::string& _organName,
                                                                          const std::string& _structureType,
                                                                          const ::fwData::Color::sptr& _color,
                                                                          ::fwData::Material::RepresentationType _mode,
                                                                          bool _visible)
{
    ::fwData::Color::sptr diffuse = ::fwData::Color::New();
    diffuse->setRGBA(_color->red(), _color->green(), _color->blue(), _color->alpha());
    ::fwData::Material::sptr pMaterial = ::fwData::Material::New();
    pMaterial->setDiffuse(diffuse);
    pMaterial->setRepresentationMode(_mode);

    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();
    reconstruction->setMesh(_mesh);
    reconstruction->setIsVisible(_visible);
    reconstruction->setOrganName(_organName);
    reconstruction->setStructureType(_structureType);
    reconstruction->setMaterial(pMaterial);

    return reconstruction;
}

//------------------------------------------------------------------------------

void ModelSeries::addReconstruction( ::fwMedData::ModelSeries::sptr& _modelSeries, ::fwData::Reconstruction::sptr& _rec)
{
    ::fwMedData::ModelSeries::ReconstructionVectorType recDB = _modelSeries->getReconstructionDB();

    FW_RAISE_IF("Reconstruction is invalid.", _rec == nullptr);
    FW_RAISE_IF("Reconstruction already exists in ModelSeries.",
                std::find(recDB.begin(), recDB.end(), _rec) != recDB.end());

    recDB.push_back(_rec);
    _modelSeries->setReconstructionDB(recDB);
}

//------------------------------------------------------------------------------

} // end namespace fwDataTools


