#include "opSofa/sofa/MeshF4S.hpp"


using namespace sofa::defaulttype;

/**
 *  Method in charge of the load of the triangular mesh data from F4S in the
 *  mesh object from SOFA
 *  @param pMesh : pointer to the triangular mesh
 */
void MeshF4S::loadTriangularMesh(::fwData::TriangularMesh::sptr pMesh) {
    // Copy points to Sofa
    int const nbPoints = pMesh->getNumPoints();
    std::vector<std::vector<float > > &fromMesh = pMesh->points();
    vertices.resize(nbPoints);
    for (int p=0;p<nbPoints;p++)
    {
      vertices[p][0] = (SReal)fromMesh[p][0];
      vertices[p][1] = (SReal)fromMesh[p][1];
      vertices[p][2] = (SReal)fromMesh[p][2];
    }
    
    // Copy cells to Sofa
    int const nbCells = pMesh->getNumCells();
    std::vector<std::vector<int > > &fromMesh2 = pMesh->cells();
    facets.resize(nbCells);
    for (int f=0;f<nbCells;f++)
    {
        facets[f].resize(3);
        facets[f][0].resize(3);
        facets[f][1].resize(3);
        facets[f][2].resize(3);
        facets[f][0][0] = fromMesh2[f][0];
        facets[f][0][1] = fromMesh2[f][1];
        facets[f][0][2] = fromMesh2[f][2];
    }
}
