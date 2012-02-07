#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opSofa/SofaCoreSrv.hpp"
#include <fwData/Acquisition.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Float.hpp>
#include <fwData/Vector.hpp>
#include <fwDataIO/reader/MeshReader.hpp>
#include <QMessageBox>


namespace opSofa
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::opSofa::SofaCoreSrv, ::fwData::Acquisition ) ;

/**
 * @brief Constructor
 */
SofaCoreSrv::SofaCoreSrv() throw()
{
    sofa = NULL;
}

/**
 * @brief Destructor
 */
SofaCoreSrv::~SofaCoreSrv() throw()
{
    delete sofa;
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaCoreSrv::configuring() throw ( ::fwTools::Failed )
{
    if(m_configuration->findConfigurationElement("addTools"))
    {
        if (m_configuration->findConfigurationElement("addTools")->getValue() == "yes") {
            this->addMesh("./Bundles/opSofa_0-1/mors2.trian", "mors2");
            this->addMesh("./Bundles/opSofa_0-1/cam.trian", "cam");
        }
    }
}

/**
 * @brief Used to launch the service.
 */
void SofaCoreSrv::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

/**
 * @brief Used to stop the service.
 */
void SofaCoreSrv::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaCoreSrv::updating( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
    if (msg->hasEvent("NEW_SOFA_SCENE")) {

        // Delete object sofa
        delete sofa;

        // Get Path Scn
        ::fwData::String::csptr pathScn = ::fwData::String::dynamicConstCast(msg->getDataInfo("NEW_SOFA_SCENE"));

        // Get acquisition
        ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();

        // Create object sofa
        sofa = new SofaBusiness();
        sofa->loadScn(pathScn->value(), acq, this->getSptr());

        // Apply at sofa the number of image by second
        sofa->setTimeStepAnimation(1000/50);

        if (sofa) {
            // if animation is running
            if (sofa->isAnimate()) {
                // Stop animation
                sofa->stopThread();
            } else {
                // Start animation
                sofa->startThread();
            }
        } else {
            QMessageBox::warning(0, "Warning", "To launch animation you must first load scene file !");
        }

    }

    else if (msg->hasEvent("START_STOP_SOFA")) {
        if (sofa) {
            // if animation is running
            if (sofa->isAnimate()) {
                // Stop animation
                sofa->stopThread();
            } else {
                // Start animation
                sofa->startThread();
            }
        } else {
            QMessageBox::warning(0, "Warning", "To launch animation you must first load scene file !");
        }
    }

    else if (msg->hasEvent("EDITOR_MESH_SOFA")) {
        if (sofa) {
            ::fwData::Vector::csptr data = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("EDITOR_MESH_SOFA"));
            ::fwData::String::csptr idMesh = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr strength = ::fwData::Integer::dynamicConstCast(data->at(1));
            sofa->shakeMesh(idMesh->value(), strength->value());
        }
    }

    else if (msg->hasEvent("MOVE_MESH_SOFA")) {
        if (sofa) {
            ::fwData::Vector::csptr data = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("MOVE_MESH_SOFA"));
            ::fwData::String::csptr idMesh = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr x = ::fwData::Integer::dynamicConstCast(data->at(1));
            ::fwData::Integer::csptr y = ::fwData::Integer::dynamicConstCast(data->at(2));
            ::fwData::Integer::csptr z = ::fwData::Integer::dynamicConstCast(data->at(3));
            ::fwData::Float::csptr rx = ::fwData::Float::dynamicConstCast(data->at(4));
            ::fwData::Float::csptr ry = ::fwData::Float::dynamicConstCast(data->at(5));
            ::fwData::Float::csptr rz = ::fwData::Float::dynamicConstCast(data->at(6));
            sofa->moveMesh(idMesh->value(), x->value(), y->value(), z->value(), rx->value(), ry->value(), rz->value());
        }
    }

    else if (msg->hasEvent("KINECT_NEW_POSITION_HAND")) {
        if (sofa) {
            // Get the position x y z of the main and his id
            ::fwData::Vector::csptr data = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("KINECT_NEW_POSITION_HAND"));
            ::fwData::Integer::csptr x = ::fwData::Integer::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr y = ::fwData::Integer::dynamicConstCast(data->at(1));
            ::fwData::Integer::csptr z = ::fwData::Integer::dynamicConstCast(data->at(2));
            ::fwData::Integer::csptr id = ::fwData::Integer::dynamicConstCast(data->at(3));

            static int idTool1 = 0;
            static int idTool2 = 0;
            static int stage = 0;

            // Allow to switch tool
            if (id->value() == idTool1) {
                // move tool 1
                sofa->moveMesh("souris_mors2", x->value()/2 + 190, y->value()/2 + 152, z->value()/2 - 200, 0, 0, 0);
            } else if (id->value() == idTool2) {
                // move tool 2
                sofa->moveMesh("souris_cam", x->value()/2 + 190, y->value()/2 + 152, z->value()/2 - 200, 0, 0, 0);
            } else {
                if (stage%2) {
                    idTool1 = id->value();
                } else {
                    idTool2 = id->value();
                }
                stage++;
            }
        }
    }
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaCoreSrv::updating() throw ( ::fwTools::Failed )
{
}

/**
 * @brief info of the class
 */
void SofaCoreSrv::info ( std::ostream &_sstream )
{
}


/**
 * @brief Add a triangularMesh to the acquisition data
 *
 * @param meshPath : path to the mesh
 * @param meshName : name of the mesh
 */
void SofaCoreSrv::addMesh(std::string meshPath, std::string meshName)
{
    // Create mesh
    ::fwData::Mesh::NewSptr mesh;
    mesh->setName(meshName);
    ::fwDataIO::reader::MeshReader reader1;
    reader1.setObject(mesh);
    reader1.setFile(meshPath);
    reader1.read();

    // Create reconstruction
    ::fwData::Reconstruction::NewSptr reconstruction;
    reconstruction->setCRefStructureType("OrganType");
    reconstruction->setIsVisible(true);
    reconstruction->setGenerated3D(true);
    reconstruction->setMaskGenerated(true);
    reconstruction->setIsAutomatic(true);
    reconstruction->setAvgVolume(-1);
    reconstruction->setMesh(mesh);
    reconstruction->setOrganName(meshName);

    // add reconstruction to acquisition
    ::fwData::Acquisition::sptr acquisition = this->getObject< ::fwData::Acquisition >();
    acquisition->addReconstruction(reconstruction);
}

}
