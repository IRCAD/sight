/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_HELPER_MESHGETTER_HPP__
#define __FWDATATOOLS_HELPER_MESHGETTER_HPP__

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/ArrayGetter.hpp"

#include <fwData/Mesh.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/Type.hpp>

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helper to manage Mesh. Lock the mesh buffer before to modify it.
 */
class FWDATATOOLS_CLASS_API MeshGetter
{
public:
    FWDATATOOLS_API MeshGetter( ::fwData::Mesh::csptr mesh );

    FWDATATOOLS_API virtual ~MeshGetter();

    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstPointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstPointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstPointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellNormalsMultiArrayType     getCellNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstPointTexCoordsMultiArrayType  getPointTexCoords() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::ConstCellTexCoordsMultiArrayType   getCellTexCoords() const;

    /// Returns the internal ::fwData::Mesh
    FWDATATOOLS_API ::fwData::Mesh::csptr getMesh() const;

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    FWDATATOOLS_API bool isClosed();

protected:

    ::fwData::Mesh::csptr m_mesh;

    ::fwDataTools::helper::ArrayGetter::uptr m_helperPoints;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellTypes;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellData;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellDataOffsets;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperPointColors;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellColors;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperPointNormals;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellNormals;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperPointTexCoords;
    ::fwDataTools::helper::ArrayGetter::uptr m_helperCellTexCoords;

};


} // namespace helper

} // namespace fwData

#endif // __FWDATATOOLS_HELPER_MESHGETTER_HPP__
