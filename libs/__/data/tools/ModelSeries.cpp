/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "data/tools/ModelSeries.hpp"

#include <core/com/signal.hxx>
#include <core/spy_log.hpp>

namespace sight::data::tools
{

//------------------------------------------------------------------------------

void ModelSeries::addMesh(
    const data::ModelSeries::sptr& _modelSeries,
    const data::Mesh::sptr& _mesh,
    const std::string& _organName,
    const std::string& _structureType,
    const data::Color::sptr& _colour,
    data::Material::RepresentationType _mode,
    bool _visible
)
{
    //reconstruction creation
    data::Reconstruction::sptr reconstructionQuadMesh =
        createReconstructionFromMesh(_mesh, _organName, _structureType, _colour, _mode, _visible);

    addReconstruction(_modelSeries, reconstructionQuadMesh);

    auto sig = _modelSeries->signal<data::ModelSeries::ReconstructionsAddedSignalType>(
        data::ModelSeries::RECONSTRUCTIONS_ADDED_SIG
    );

    data::ModelSeries::ReconstructionVectorType reconstructionsVector;
    reconstructionsVector.push_back(reconstructionQuadMesh);
    sig->async_emit(reconstructionsVector);
}

//------------------------------------------------------------------------------

data::Reconstruction::sptr ModelSeries::createReconstructionFromMesh(
    const data::Mesh::sptr& _mesh,
    const std::string& _organName,
    const std::string& _structureType,
    const data::Color::sptr& _color,
    data::Material::RepresentationType _mode,
    bool _visible
)
{
    data::Color::sptr diffuse = std::make_shared<data::Color>();
    diffuse->setRGBA(_color->red(), _color->green(), _color->blue(), _color->alpha());
    data::Material::sptr pMaterial = std::make_shared<data::Material>();
    pMaterial->setDiffuse(diffuse);
    pMaterial->setRepresentationMode(_mode);

    data::Reconstruction::sptr reconstruction = std::make_shared<data::Reconstruction>();
    reconstruction->setMesh(_mesh);
    reconstruction->setIsVisible(_visible);
    reconstruction->setOrganName(_organName);
    reconstruction->setStructureType(_structureType);
    reconstruction->setMaterial(pMaterial);

    return reconstruction;
}

//------------------------------------------------------------------------------

void ModelSeries::addReconstruction(
    const data::ModelSeries::sptr& _modelSeries,
    const data::Reconstruction::sptr& _rec
)
{
    data::ModelSeries::ReconstructionVectorType recDB = _modelSeries->getReconstructionDB();

    SIGHT_THROW_IF("Reconstruction is invalid.", _rec == nullptr);
    SIGHT_THROW_IF(
        "Reconstruction already exists in ModelSeries.",
        std::find(recDB.begin(), recDB.end(), _rec) != recDB.end()
    );

    recDB.push_back(_rec);
    _modelSeries->setReconstructionDB(recDB);
}

//------------------------------------------------------------------------------

} // end namespace sight::data::tools
