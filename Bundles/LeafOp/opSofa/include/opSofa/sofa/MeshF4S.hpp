#ifndef OPSOFA_SOFA_MESHF4S_H
#define OPSOFA_SOFA_MESHF4S_H

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

#endif // OPSOFA_SOFA_MESHF4S_H
