/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_NAMESPACE_HPP__
#define __FWCOM_NAMESPACE_HPP__

/**
 * @brief Namespace containing fw4spl communication tools
 */
namespace fwCom
{

/**
 * @brief fwCom's exceptions
 */
namespace exception
{


} // namespace exception


/**
 * @brief fwCom's utilities
 */
namespace util
{


} // namespace util



/**
 * @page SigSlot fwCom Signal/Slot system
 *
 * @tableofcontents
 *
 * fwCom library provides a set of tools dedicated to communication. These
 * communications are based on
 * <a href="http://en.wikipedia.org/wiki/Signals_and_slots">Signal and slots concept</a>.
 *
 * fwCom provides the following features :
 *  - function and method wrapping
 *  - direct slot calling
 *  - asynchronous slot calling
 *  - ability to work with multiple threads
 *  - auto-deconnection of slot and signals
 *  - arguments loss between slots and signals
 *
 *
 * @section Slots Slots
 *
 * Slots are wrappers for functions and class methods that can be attached to a
 * fwThread::Worker. The purpose of this class is to provide synchronous and
 * asynchronous mecanisms for method and function calling.
 *
 * Slots have a common base class : SlotBase. This allows to store them in the
 * same container. Slots are designed to permit calling them by knowing only the
 * argument types.
 *
 * Examples :
 *
 * This declares a slot wrapping the function `sum`, which is a function with the
 * signature `int (int, int)`
 *
 *     ::fwCom::Slot< int (int, int) >::sptr slotSum = ::fwCom::newSlot( &sum );
 *
 * This declares a Slot wrapping the method `start` with signature `void()` of
 * the object `a` which class type is `A`
 *
 *     ::fwCom::Slot< void () >::sptr slotStart = ::fwCom::newSlot( &A::start, &a );
 *
 * This executes the slots using the method `run`.
 *
 *     slotSum->run(40,2);
 *     slotStart->run();
 *
 * This executes the slots using the method `call`, which returns the result of
 * the function/method execution.
 *
 *     int result = slotSum->call(40,2);
 *     slotStart->call();
 *
 * The same, through a SlotBase :
 *
 *     ::fwCom::Slot< size_t (std::string) > slotLen = ::fwCom::Slot< size_t (std::string) >::New( &len );
 *     ::fwCom::SlotBase::sptr slotBaseLen = slotLen;
 *     ::fwCom::SlotBase::sptr slotBaseSum = slotSum;
 *     slotBaseSum->run(40,2);
 *     slotBaseSum->run<int, int>(40,2);
 *     slotBaseLen->run<std::string>("R2D2"); // This one needs the explicit argument type
 *     result = slotBaseSum->call<int>(40,2);
 *     result = slotBaseSum->call<int, int, int>(40,2);
 *     result = slotBaseLen->call<size_t, std::string>("R2D2");
 *
 * @subsection SlotAsync Asynchronous calls
 *
 * Slots are able to work with fwThread::Worker. If a Slot has a Worker, each
 * asynchronous execution request will be done in it's worker, otherwise
 * asynchronous requests can not be satisfied without specifying a worker.
 *
 * Setting worker example :
 *
 *     ::fwThread::Worker::sptr w = ::fwThread::Worker::New();
 *     slotSum->setWorker(w);
 *     slotStart->setWorker(w);
 *
 * @subsubsection SlotAsync Asynchronous `run`
 *
 * `asyncRun` method returns a <a
 * href="http://www.boost.org/doc/libs/1_52_0/doc/html/thread/synchronization.html#thread.synchronization.futures">`boost::shared_future<
 * void >`</a>, that makes it possible to wait for end-of-execution.
 *
 *     ::boost::future< void > future = slotStart->asyncRun();
 *     // do something else ...
 *     future.wait(); //ensures slotStart is finished before continuing
 *
 * @subsubsection SlotAsync Asynchronous `call`
 *
 * `asyncCall` method returns a <a
 * href="http://www.boost.org/doc/libs/1_52_0/doc/html/thread/synchronization.html#thread.synchronization.futures">
 * `boost::shared_future< R >`</a> where R is the return type, this allows to
 * wait for end-of-execution and to get the computed value.
 *
 *     ::boost::future< int > future = slotSum->asyncCall();
 *     // do something else ...
 *     future.wait(); //ensures slotStart is finished before continuing
 *     int result = future.get();
 *
 *
 */



} // namespace fwCom

#endif /* __FWCOM_NAMESPACE_HPP__ */

