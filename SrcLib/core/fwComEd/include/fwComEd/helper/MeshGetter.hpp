/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_MESHGETTER_HPP__
#define __FWCOMED_HELPER_MESHGETTER_HPP__

#include <fwTools/Type.hpp>
#include <fwMemory/BufferObject.hpp>

#include <fwData/Mesh.hpp>

#include "fwComEd/helper/ArrayGetter.hpp"
#include "fwComEd/config.hpp"

namespace fwComEd
{

namespace helper
{

/**
 * @brief   Helper to manage Mesh. Lock the mesh buffer before to modify it.
 */
class FWCOMED_CLASS_API MeshGetter
{
public:
    FWCOMED_API MeshGetter( ::fwData::Mesh::csptr mesh );

    FWCOMED_API virtual ~MeshGetter();

    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstPointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstPointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstPointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellNormalsMultiArrayType     getCellNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstPointTexCoordsMultiArrayType  getPointTexCoords() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWCOMED_API ::fwData::Mesh::ConstCellTexCoordsMultiArrayType   getCellTexCoords() const;

    /// Returns the internal ::fwData::Mesh
    FWCOMED_API ::fwData::Mesh::csptr getMesh() const;

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    FWCOMED_API bool isClosed();

protected:

    ::fwData::Mesh::csptr m_mesh;

    ::fwComEd::helper::ArrayGetter::uptr m_helperPoints;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellTypes;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellData;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellDataOffsets;
    ::fwComEd::helper::ArrayGetter::uptr m_helperPointColors;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellColors;
    ::fwComEd::helper::ArrayGetter::uptr m_helperPointNormals;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellNormals;
    ::fwComEd::helper::ArrayGetter::uptr m_helperPointTexCoords;
    ::fwComEd::helper::ArrayGetter::uptr m_helperCellTexCoords;

};


} // namespace helper

} // namespace fwData

#endif // __FWCOMED_HELPER_MESHGETTER_HPP__
