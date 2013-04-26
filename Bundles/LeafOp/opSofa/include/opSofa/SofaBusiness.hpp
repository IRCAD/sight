/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFABUSINESS_HPP__
#define __OPSOFA_SOFABUSINESS_HPP__

#include <sofa/component/typedef/Sofa_typedef.h>
#include <sofa/component/visualmodel/OglModel.h>
#include <sofa/simulation/tree/TreeSimulation.h>
#include <sofa/simulation/tree/GNode.h>

#include <fwServices/IEditionService.hpp>

#include "opSofa/SofaThread.hpp"


using namespace sofa::simulation::tree;
using sofa::component::visualmodel::OglModel;
using namespace sofa::defaulttype;

class SofaThread;

namespace fwData
{
    class Mesh;
}

namespace fwMedData
{
    class ModelSeries;
}

namespace fwServices
{
    class IService;
}

/**
 * @brief Manages the SOFA framework
 */
class SofaBusiness
{

public:

    /**
     * @brief Constructor
     */
    SofaBusiness();
    /**
     * @brief Destructor
     */
    virtual ~SofaBusiness();


    /**
     * @brief Builds the SOFA physical model and data and instanciates the SofaThread class
     *
     * @param fileScn : path of the file .scn
     * @param acquisition : object acquisition of FW4SPL
     * @param service : pointer to the SofaService object
     */
    void loadScn(std::string, SPTR(::fwMedData::ModelSeries), SPTR(::fwServices::IService));

    /**
     * @brief Builds the SOFA physical model and data and instantiates the SofaThread class
     *
     * @param pMesh : pointer to the triangular mesh
     * @param service : pointer to the SofaService object
     */
    void loadMesh( SPTR(::fwData::Mesh), ::fwServices::IService::sptr);

    /**
     * @brief Gets time step animation
     *
     * @return time between two calculation in millisecond
     */
    unsigned int getTimeStepAnimation();

    /**
     * @brief Initializes the time step animation
     *
     * @param timeStepAnimation : time between two calculation in millisecond
     */
    void setTimeStepAnimation(unsigned int timeStepAnimation);

    /**
     * @brief Processes the deformation of the triangular mesh for the next step
     */
    void animate();

    /**
     * @brief Starts the thread dedicated to the deformation processing in SOFA
     */
    void startThread();

    /**
     * @brief Stops the thread dedicated to the deformation processing in SOFA
     */
    void stopThread();

    /**
     * @brief Get stage of the animation
     *
     * @return true if animation is running
     */
    bool isAnimate();

    /**
     * @brief Resets the SOFA scene
     *
     * Reload the initial conditions of the mesh and of the physical model in SOFA
     */
    void reset();

    /**
    * @brief Shake organ
    *
    * @param idMesh : id organ
    * @param value : value of force
    */
    void shakeMesh(std::string, int);
    void moveMesh(std::string, int, int, int, float, float, float);

private:

    /**
     * @brief Bring OglModel of Sofa
     *
     * @param node : scene root of Sofa
     * @param model : OglModel vector at fill
     */
    void fillOglModelVector(GNode*, std::vector<OglModel*>*);

    /**
     * @brief Bring SpringForceField of Sofa
     *
     * @param node : scene root of Sofa
     * @param model : SpringForceField map at fill
     */
    void fillSpringForceField(GNode*, std::map<std::string, StiffSpringForceField3*>*);

    /**
     * @brief Bring Mesh of Fw4spl
     *
     * @param acquisition : object acquisition of Fw4spl
     * @param meshs : Mesh vector at fill
     */
    void fillMeshVector( SPTR(::fwMedData::ModelSeries), std::vector< SPTR(fwData::Mesh) >*);

    /**
     * @brief Translates pointer between sofa and fw4spl to set a shared memory access
     *
     * @param visual : object visual of sofa
     * @param pMesh : object mesh of fw4spl
     */
    void translationPointer(OglModel*, SPTR(::fwData::Mesh));

    /**
     * @brief Cancel translates pointer between sofa and fw4spl
     *
     */
    void clearTranslationPointer();

    /**
     * @brief Pointer to the GNode object containing the physical parameters
     */
    GNode::SPtr groot;

    /**
     * @brief Data containing the step of deformation
     */
    unsigned int timeStepAnimation;

    /**
     * @brief Pointer to the SofaThread object
     */
    SofaThread *thread;

    /**
     * @brief Vector to mesh object
     */
    std::vector< SPTR(::fwData::Mesh) > *meshs;

    /**
     * @brief map to StiffSpringForceField3 object
     */
    std::map<std::string, StiffSpringForceField3*> *springs;

};

#endif // __OPSOFA_SOFABUSINESS_HPP__
