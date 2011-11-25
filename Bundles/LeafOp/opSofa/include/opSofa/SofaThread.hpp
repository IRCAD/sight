#ifndef OPSOFA_SOFATHREAD_H
#define OPSOFA_SOFATHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "opSofa/SofaBusiness.hpp"
#include <fwComEd/MeshMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/PatientDB.hpp>

class SofaBusiness;

/**
 * @brief Manages the separated thread dedicated to the SOFA animation processing
 */
class SofaThread : public QThread
{
    Q_OBJECT

public:
    SofaThread(SofaBusiness*, std::vector<fwData::Mesh::sptr>*, ::fwServices::IService::sptr service);
    void stop();
    bool isRunning();

signals:
    /**
     * @brief Signal emitted to request the display to be refreshed
     */
    void refreshVtkRequestSignal();

public slots:
    void refreshVtk();

private:
    void run();

    /**
     * @brief Flag to request the thread to stop
     */
    bool stopRun;

    /**
     * @brief Pointer to the SofaBusiness object
     */
    SofaBusiness *sofa;

    /**
     * @brief Pointer to the message structure
     */
    fwComEd::MeshMsg::sptr msg;

    /**
     * @brief Pointer to the service Fw4spl
     */
    fwServices::IService::sptr service;

    /**
     * @brief list of mesh
     */
    std::vector<fwData::Mesh::sptr> *meshs;

    /**
     * @brief mutex to synchronize threads
     */
    QMutex mutex;

    /**
     * @brief provides a condition variable for synchronizing threads
     */
    QWaitCondition condition;

};

#endif // OPSOFA_SOFATHREAD_H
