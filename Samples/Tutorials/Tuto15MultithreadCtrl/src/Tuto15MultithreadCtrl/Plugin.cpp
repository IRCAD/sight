/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto15MultithreadCtrl/Plugin.hpp"

#include "Tuto15MultithreadCtrl/SReadArray.hpp"

#include <fwCom/Signal.hxx>

#include <fwCore/spyLog.hpp>

#include <fwData/Array.hpp>

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwThread/Worker.hpp>

#include <cmath>
#include <functional>

namespace Tuto15MultithreadCtrl
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto15MultithreadCtrl::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));

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

//------------------------------------------------------------------------------

int Plugin::run() throw()
{
    ::fwData::Array::sptr array = ::fwData::Array::New();

    ::fwServices::registry::ServiceFactory::sptr serviceFactory = ::fwServices::registry::ServiceFactory::getDefault();

    // Create the 3 services to read, show and increment the values.
    ::fwServices::IService::sptr srvRead =
        serviceFactory->create("::fwServices::IController", "::Tuto15MultithreadCtrl::SReadArray");
    ::fwServices::IService::sptr srvShow =
        serviceFactory->create("::fwServices::IController", "::Tuto15MultithreadCtrl::SShowArray");
    ::fwServices::IService::sptr srvIncrement =
        serviceFactory->create("::fwServices::IController", "::Tuto15MultithreadCtrl::SIncrementArray");

    // Register the services
    ::fwServices::OSR::registerService(array, srvRead);
    ::fwServices::OSR::registerService(array, srvShow);
    ::fwServices::OSR::registerService(array, srvIncrement);

    SLM_ASSERT("Failed to create service", srvRead);
    SLM_ASSERT("Failed to create service", srvShow);
    SLM_ASSERT("Failed to create service", srvIncrement);

    // Create 3 workers for each service
    ::fwThread::Worker::sptr worker1 = ::fwThread::Worker::New();
    ::fwThread::Worker::sptr worker2 = ::fwThread::Worker::New();
    ::fwThread::Worker::sptr worker3 = ::fwThread::Worker::New();

    // Associate the workers to the services
    srvRead->setWorker(worker1);
    srvShow->setWorker(worker2);
    srvIncrement->setWorker(worker3);

    // Connect the array "modified" signal to the services show and increment.
    auto sig = array->signal< ::fwData::Object::ModifiedSignalType>( ::fwData::Object::s_MODIFIED_SIG );

    ::fwCom::Connection showConnection      = sig->connect(srvShow->slot( ::fwServices::IService::s_UPDATE_SLOT) );
    ::fwCom::Connection incrementConnection = sig->connect(srvIncrement->slot( "startTimer" ) );

    // Start the services
    srvRead->start().wait();
    srvShow->start().wait();
    srvIncrement->start().wait();

    // Update the reader
    srvRead->update().wait();

    unsigned long long count = 1<<30;
    double d                 = 4<<2;

    OSLM_INFO("Computing " << count << " square roots.");

    for (unsigned long long i = 0; i < count; ++i)
    {
        d = std::sqrt(d);
    }

    OSLM_INFO("Done computing " << count << " square roots : " << d);

    // Disconnect the signals and slots
    showConnection.disconnect();
    incrementConnection.disconnect();

    // Stop the services
    srvRead->stop().wait();
    srvShow->stop().wait();
    srvIncrement->stop().wait();

    // Unregister the services
    ::fwServices::OSR::unregisterService(srvRead);
    ::fwServices::OSR::unregisterService(srvShow);
    ::fwServices::OSR::unregisterService(srvIncrement);

    srvRead.reset();
    srvShow.reset();
    srvIncrement.reset();

    // Stop the workers
    worker1->stop();
    worker2->stop();
    worker3->stop();

    return 0;
}

} // namespace Tuto15MultithreadCtrl
