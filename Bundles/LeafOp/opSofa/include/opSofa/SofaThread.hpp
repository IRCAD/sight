/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFATHREAD_HPP__
#define __OPSOFA_SOFATHREAD_HPP__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <fwComEd/MeshMsg.hpp>

#include <fwServices/IEditionService.hpp>

#include "opSofa/SofaBusiness.hpp"


class SofaBusiness;

namespace fwData
{
    class Mesh;
}

/**
 * @brief Manages the separated thread dedicated to the SOFA animation processing
 */
class SofaThread : public QThread
{
    Q_OBJECT

public:
    SofaThread(SofaBusiness*, std::vector< SPTR(::fwData::Mesh) >*, ::fwServices::IService::sptr service);
    void stop();
    bool isRunning();

Q_SIGNALS:
    /**
     * @brief Signal emitted to request the display to be refreshed
     */
    void refreshVtkRequestSignal();

public Q_SLOTS:
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
    std::vector< SPTR(::fwData::Mesh) > *meshs;

    /**
     * @brief mutex to synchronize threads
     */
    QMutex mutex;

    /**
     * @brief provides a condition variable for synchronizing threads
     */
    QWaitCondition condition;

};

#endif // __OPSOFA_SOFATHREAD_HPP__

