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

#include "data/tools/model_series.hpp"

#include <core/com/signal.hxx>
#include <core/spy_log.hpp>

namespace sight::data::tools
{

//------------------------------------------------------------------------------

void model_series::addMesh(
    const data::model_series::sptr& _modelSeries,
    const data::mesh::sptr& _mesh,
    const std::string& _organName,
    const std::string& _structureType,
    const data::color::sptr& _colour,
    data::material::RepresentationType _mode,
    bool _visible
)
{
    //reconstruction creation
    data::reconstruction::sptr reconstructionQuadMesh =
        createReconstructionFromMesh(_mesh, _organName, _structureType, _colour, _mode, _visible);

    addReconstruction(_modelSeries, reconstructionQuadMesh);

    auto sig = _modelSeries->signal<data::model_series::ReconstructionsAddedSignalType>(
        data::model_series::RECONSTRUCTIONS_ADDED_SIG
    );

    data::model_series::ReconstructionVectorType reconstructionsVector;
    reconstructionsVector.push_back(reconstructionQuadMesh);
    sig->async_emit(reconstructionsVector);
}

//------------------------------------------------------------------------------

data::reconstruction::sptr model_series::createReconstructionFromMesh(
    const data::mesh::sptr& _mesh,
    const std::string& _organName,
    const std::string& _structureType,
    const data::color::sptr& _color,
    data::material::RepresentationType _mode,
    bool _visible
)
{
    data::color::sptr diffuse = std::make_shared<data::color>();
    diffuse->setRGBA(_color->red(), _color->green(), _color->blue(), _color->alpha());
    data::material::sptr pMaterial = std::make_shared<data::material>();
    pMaterial->setDiffuse(diffuse);
    pMaterial->setRepresentationMode(_mode);

    data::reconstruction::sptr reconstruction = std::make_shared<data::reconstruction>();
    reconstruction->setMesh(_mesh);
    reconstruction->setIsVisible(_visible);
    reconstruction->setOrganName(_organName);
    reconstruction->setStructureType(_structureType);
    reconstruction->setMaterial(pMaterial);

    return reconstruction;
}

//------------------------------------------------------------------------------

void model_series::addReconstruction(
    const data::model_series::sptr& _modelSeries,
    const data::reconstruction::sptr& _rec
)
{
    data::model_series::ReconstructionVectorType recDB = _modelSeries->getReconstructionDB();

    SIGHT_THROW_IF("Reconstruction is invalid.", _rec == nullptr);
    SIGHT_THROW_IF(
        "Reconstruction already exists in model_series.",
        std::find(recDB.begin(), recDB.end(), _rec) != recDB.end()
    );

    recDB.push_back(_rec);
    _modelSeries->setReconstructionDB(recDB);
}

//------------------------------------------------------------------------------

} // end namespace sight::data::tools
