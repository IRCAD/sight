/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opSofa/sofa/OglModelF4S.hpp"
#include "opSofa/sofa/MeshF4S.hpp"

/**
 *  Method to call instead of OglModel::load() to direclty load the triangular mesh
 *  data from F4S
 *  @param pMesh : pointer to the triangular mesh
 */
void OglModelF4S::loadMesh(::fwData::Mesh::sptr pMesh)
{
    MeshF4S *mesh = new MeshF4S();
    mesh->loadMesh(pMesh);

    sofa::helper::io::Mesh *objLoader = mesh;

    setMesh(*objLoader,false);
    applyUVTransformation();
}
