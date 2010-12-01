#ifndef OPSOFA_SOFABUSINESS_HPP_
#define OPSOFA_SOFABUSINESS_HPP_

#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/PatientDB.hpp>
#include <sofa/component/typedef/Sofa_typedef.h>
#include <sofa/component/visualmodel/OglModel.h>
#include "opSofa/SofaThread.hpp"

using namespace sofa::simulation::tree;
using sofa::component::visualmodel::OglModel;

class SofaThread;

/**
 * @brief Manages the SOFA framework
 */
class SofaBusiness
{

public:
    SofaBusiness();
    virtual ~SofaBusiness();
    void loadScn(std::string, ::fwData::Acquisition::sptr, ::fwServices::IService::sptr);
    void loadMesh(::fwData::TriangularMesh::sptr, ::fwServices::IService::sptr);
	unsigned int getTimeStepAnimation();
	void setTimeStepAnimation(unsigned int timeStepAnimation);
	void animate();
	void startThread();
	void stopThread();
    void reset();

private:
    void fillOglModelVector(GNode*, std::vector<OglModel*>*);
    void fillTriangularMeshVector(::fwData::Acquisition::sptr, std::vector<fwData::TriangularMesh::sptr>*);
    void translationPointer(OglModel*, ::fwData::TriangularMesh::sptr);
    void clearTranslationPointer();

    /**
     * @brief Pointer to the GNode object containing the physical parameters
     */
	GNode *groot;

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
    std::vector<fwData::TriangularMesh::sptr> *meshs;

};

#endif // OPSOFA_SOFABUSINESS_HPP_
