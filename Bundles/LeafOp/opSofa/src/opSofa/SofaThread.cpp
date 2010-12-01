#include <fwServices/IEditionService.hpp>
#include "opSofa/SofaThread.hpp"
#include <fwTools/UUID.hpp>
#include <QTime>

/**
 * @brief Constructor
 *
 * @param sofa : pointer to the SofaBusiness object
 * @param meshs : vector to the list of mesh
 * @param service : pointer to the SofaService object
 */
SofaThread::SofaThread(SofaBusiness* sofa, std::vector<fwData::TriangularMesh::sptr> *meshs, ::fwServices::IService::sptr service)
{
    // Update attributs
	this->sofa = sofa;
    this->meshs = meshs;
	this->service = service;

    // Create message
    msg->addEvent(::fwComEd::TriangularMeshMsg::VERTEX_MODIFIED);

    //To trigger refreshVtk from the separated thread in the run() method
	connect(this, SIGNAL(refreshVtkRequestSignal()), this, SLOT(refreshVtk()));
}

/**
 * @brief Containing the separated thread loop for the SOFA deformation processing.
 *
 * Each loop emits refreshVtkRequestSignal() which triggers the refreshVtk() method.
 * Launched by calling the public start() method.
 */
void SofaThread::run()
{
    stopRun = false;
    QTime time;
    
    unsigned int step = sofa->getTimeStepAnimation();
    while(!stopRun) {
        time.start();

        // Locks the mutex 
        mutex.lock();

        // Calculate deformation by sofa
        sofa->animate();

        // Emit signal for refresh screen
        emit refreshVtkRequestSignal();

        // Put the thread to sleep 
        condition.wait(&mutex);

        // Unlocks the mutex
        mutex.unlock();

        // Put the thread to sleep
        unsigned int sec = (unsigned int)time.elapsed();
        if (sec < step) {
            this->msleep((unsigned long)(step-sec));
        } else {
            this->msleep(1);
        }
    }	
}

/**
 * @brief Sending the NEW_MESH to the vtkSimpleMesh RendererService to refresh the display
 */
void SofaThread::refreshVtk()
{
    // Send message of each mesh at vtk to refresh screen
    int size = meshs->size();
    for (int i=0; i<size; ++i) {
        ::fwServices::IEditionService::notify(service, meshs->at(i), msg);
    }

    // wake thread sofa
    condition.wakeOne();
}

/**
 * @brief Stop the thread
 */
void SofaThread::stop()
{
    stopRun = true;
}
