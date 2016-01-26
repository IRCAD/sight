/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFA_MESHF4S_HPP__
#define __OPSOFA_SOFA_MESHF4S_HPP__

#include <sofa/helper/io/Mesh.h>
#include <fwData/Mesh.hpp>

/**
 * @brief Class inherited from Mesh and permitting the SOFA framework
 * to load directly the data without reading the .trian file
 */
class MeshF4S : public sofa::helper::io::Mesh
{

public:
    void loadMesh(::fwData::Mesh::sptr);

};

#endif // __OPSOFA_SOFA_MESHF4S_HPP__
