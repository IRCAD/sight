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
    const data::model_series::sptr& _model_series,
    const data::mesh::sptr& _mesh,
    const std::string& _organ_name,
    const std::string& _structure_type,
    const data::color::sptr& _colour,
    data::material::representation_t _mode,
    bool _visible
)
{
    //reconstruction creation
    data::reconstruction::sptr reconstruction_quad_mesh =
        createReconstructionFromMesh(_mesh, _organ_name, _structure_type, _colour, _mode, _visible);

    addReconstruction(_model_series, reconstruction_quad_mesh);

    auto sig = _model_series->signal<data::model_series::reconstructions_added_signal_t>(
        data::model_series::RECONSTRUCTIONS_ADDED_SIG
    );

    data::model_series::reconstruction_vector_t reconstructions_vector;
    reconstructions_vector.push_back(reconstruction_quad_mesh);
    sig->async_emit(reconstructions_vector);
}

//------------------------------------------------------------------------------

data::reconstruction::sptr model_series::createReconstructionFromMesh(
    const data::mesh::sptr& _mesh,
    const std::string& _organ_name,
    const std::string& _structure_type,
    const data::color::sptr& _color,
    data::material::representation_t _mode,
    bool _visible
)
{
    data::color::sptr diffuse = std::make_shared<data::color>();
    diffuse->setRGBA(_color->red(), _color->green(), _color->blue(), _color->alpha());
    data::material::sptr p_material = std::make_shared<data::material>();
    p_material->setDiffuse(diffuse);
    p_material->setRepresentationMode(_mode);

    data::reconstruction::sptr reconstruction = std::make_shared<data::reconstruction>();
    reconstruction->setMesh(_mesh);
    reconstruction->setIsVisible(_visible);
    reconstruction->setOrganName(_organ_name);
    reconstruction->set_structure_type(_structure_type);
    reconstruction->setMaterial(p_material);

    return reconstruction;
}

//------------------------------------------------------------------------------

void model_series::addReconstruction(
    const data::model_series::sptr& _model_series,
    const data::reconstruction::sptr& _rec
)
{
    data::model_series::reconstruction_vector_t rec_db = _model_series->getReconstructionDB();

    SIGHT_THROW_IF("Reconstruction is invalid.", _rec == nullptr);
    SIGHT_THROW_IF(
        "Reconstruction already exists in model_series.",
        std::find(rec_db.begin(), rec_db.end(), _rec) != rec_db.end()
    );

    rec_db.push_back(_rec);
    _model_series->setReconstructionDB(rec_db);
}

//------------------------------------------------------------------------------

} // end namespace sight::data::tools
