/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QTime>
#include <QMutex>
#include <QWaitCondition>

#include <fwData/Mesh.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwTools/UUID.hpp>

#include "opSofa/SofaThread.hpp"


SofaThread::SofaThread(SofaBusiness* sofa, std::vector< ::fwData::Mesh::sptr > *meshs, ::fwServices::IService::sptr service)
{
    // Update attributs
    this->sofa = sofa;
    this->meshs = meshs;
    this->service = service;
    this->msg = ::fwComEd::MeshMsg::New();

    stopRun = true;
    // Create message
    msg->addEvent(::fwComEd::MeshMsg::VERTEX_MODIFIED);

    //To trigger refreshVtk from the separated thread in the run() method
    connect(this, SIGNAL(refreshVtkRequestSignal()), this, SLOT(refreshVtk()));
}

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
        Q_EMIT refreshVtkRequestSignal();

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

void SofaThread::refreshVtk()
{
    mutex.lock();

    // Send message of each mesh at vtk to refresh screen
    int size = meshs->size();
    for (int i=0; i<size; ++i) {
        ::fwServices::IEditionService::notify(service, meshs->at(i), msg);
    }

    // wake thread sofa
    condition.wakeOne();

    mutex.unlock();
}

void SofaThread::stop()
{
    stopRun = true;
    this->wait(100);
}

bool SofaThread::isRunning()
{
    return !stopRun;
}
