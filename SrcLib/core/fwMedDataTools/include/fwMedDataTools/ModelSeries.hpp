/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATATOOLS_MODELSERIES_HPP__
#define __FWMEDDATATOOLS_MODELSERIES_HPP__

#include "fwMedDataTools/config.hpp"

#include <fwData/Color.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <string>

namespace fwMedDataTools
{

/**
 * @brief   This class contains helper to manipulate ::fwData::ModelSeries.
 * @class   ModelSeries
 */
class ModelSeries
{
public:

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be created.
     * @param _modelSeries Model series where the mesh must be added.
     * @param _rec Reconstruction that must be added
     */
    FWMEDDATATOOLS_API static void addReconstruction( ::fwMedData::ModelSeries::sptr& _modelSeries,
                                                      ::fwData::Reconstruction::sptr& _rec);

    /**
     * @brief Create a reconstruction from a mesh.
     * @param _mesh Mesh that must be added
     * @param _organName Organ name to associate with the mesh
     * @param _structureType Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     * @return Output reconstruction.
     */
    FWMEDDATATOOLS_API static ::fwData::Reconstruction::sptr
    createReconstructionFromMesh( const ::fwData::Mesh::sptr& _mesh,
                                  const std::string& _organName,
                                  const std::string& _structureType,
                                  const ::fwData::Color::sptr& _color = ::fwData::Color::New(),
                                  ::fwData::Material::REPRESENTATION_MODE _mode = ::fwData::Material::MODE_SURFACE,
                                  bool _visible = true);

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be automatically created.
     * @param _modelSeries Model series where the mesh must be added.
     * @param _mesh Mesh that must be added
     * @param _organName Organ name to associate with the mesh
     * @param _structureType Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     */
    FWMEDDATATOOLS_API static void
    addMesh( ::fwMedData::ModelSeries::sptr& _modelSeries,
             const ::fwData::Mesh::sptr& _mesh,
             const std::string& _organName, const std::string& _structureType,
             const ::fwData::Color::sptr& _color = ::fwData::Color::New(),
             ::fwData::Material::REPRESENTATION_MODE _mode = ::fwData::Material::MODE_SURFACE,
             bool _visible = true);
};

} // end namespace fwMedDataTools

#endif // __FWMEDDATATOOLS_MODELSERIES_HPP__
