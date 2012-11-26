/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/spyLog.hpp>

#include <fwData/Array.hpp>
#include <fwData/Object.hpp>

#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Worker.hpp>

#include "Tuto15MultithreadCtrl/Plugin.hpp"
#include "Tuto15MultithreadCtrl/SReadArray.hpp"

namespace Tuto15MultithreadCtrl
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto15MultithreadCtrl::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(::boost::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    SLM_TRACE_FUNC();
}

int Plugin::run() throw()
{

    ::fwData::Array::NewSptr array;

    ::fwServices::IService::sptr srvRead =
        ::fwServices::registry::ServiceFactory::getDefault()->create("::fwServices::IService", "::Tuto15MultithreadCtrl::SReadArray");
    ::fwServices::IService::sptr srvShow =
        ::fwServices::registry::ServiceFactory::getDefault()->create("::fwServices::IService", "::Tuto15MultithreadCtrl::SShowArray");
    ::fwServices::IService::sptr srvIncrement =
        ::fwServices::registry::ServiceFactory::getDefault()->create("::fwServices::IService", "::Tuto15MultithreadCtrl::SIncrementArray");

    ::fwServices::OSR::registerService(array, srvRead);
    ::fwServices::OSR::registerService(array, srvShow);
    ::fwServices::OSR::registerService(array, srvIncrement);

    SLM_ASSERT("Failed to create service", srvRead);
    SLM_ASSERT("Failed to create service", srvShow);
    SLM_ASSERT("Failed to create service", srvIncrement);

    ::fwThread::Worker::NewSptr worker1;
    ::fwThread::Worker::NewSptr worker2;
    ::fwThread::Worker::NewSptr worker3;

    ::fwServices::registry::ActiveWorkers::sptr workers = ::fwServices::registry::ActiveWorkers::getDefault();
    workers->addWorker("worker1", worker1);
    workers->addWorker("worker2", worker2);
    workers->addWorker("worker3", worker3);

    srvRead->setWorker(worker1);
    srvShow->setWorker(worker2);
    srvIncrement->setWorker(worker3);

    ::fwData::Object::ObjectModifiedSignalType::sptr sig
        = array->signal< ::fwData::Object::ObjectModifiedSignalType>( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    ::fwCom::Connection readConnection = sig->connect(srvRead->slot( ::fwServices::IService::s_RECEIVE_SLOT) );
    ::fwCom::Connection showConnection = sig->connect(srvShow->slot( ::fwServices::IService::s_RECEIVE_SLOT) );
    ::fwCom::Connection incrementConnection = sig->connect(srvIncrement->slot( ::fwServices::IService::s_RECEIVE_SLOT) );

    srvRead->start();
    srvShow->start();
    srvIncrement->start();

    srvRead->update();

    ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));
    srvIncrement->update();
    ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));
    srvIncrement->update();
    ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));
    srvIncrement->update();

    srvRead->stop();
    srvShow->stop();
    srvIncrement->stop();

    readConnection.disconnect();
    showConnection.disconnect();
    incrementConnection.disconnect();

    ::fwServices::OSR::unregisterService(srvRead);
    ::fwServices::OSR::unregisterService(srvShow);
    ::fwServices::OSR::unregisterService(srvIncrement);

    array.reset();

    worker1->stop();
    worker2->stop();
    worker3->stop();

    return 0;
}


} // namespace Tuto15MultithreadCtrl
