/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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
 * @subsection SlotAsyncCalls Asynchronous calls
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
 * @subsubsection SlotAsyncRun Asynchronous `run`
 *
 * `asyncRun` method returns a <a
 * href="http://www.boost.org/doc/libs/1_52_0/doc/html/thread/synchronization.html#thread.synchronization.futures">`boost::shared_future<
 * void >`</a>, that makes it possible to wait for end-of-execution.
 *
 *     ::boost::future< void > future = slotStart->asyncRun();
 *     // do something else ...
 *     future.wait(); //ensures slotStart is finished before continuing
 *
 * @subsubsection SlotAsyncCall Asynchronous `call`
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
 * @subsubsection SlotAsyncWeakCall WeakCalls
 *
 * Slots asynchronous execution has been made 'weak'. That does mean that when an
 * async call/run is pending in a worker queue:
 * - if the slot is detroyed before the execution of this call, the call will be canceled.
 * - if the slot's worker is changed before the execution of this call, the
 *   call will be canceled.
 *
 *
 * @section Signals Signals
 *
 * Signal allows to perform grouped calls on slots.  In this purpose, Signal
 * provides a mechanism to connect slots to itself.
 *
 * Examples:
 *
 * The following instruction declares a Signal able to execute slots of type
 * `void()` :
 *
 *     ::fwCom::Signal< void() >::sptr sig = ::fwCom::Signal< void() >::New();
 *
 * This connects a Slot having the same type as the previously declared Signal,
 * and connects the Slot to this Signal :
 *
 *     sig->connect(slotStart);
 *
 *
 * Finally, the following instruction will trigger the execution of all slots
 * connected to this Signal:
 *
 *     sig->emit();
 *
 * Thus, it is possible to connect multiple slots having the same type to the
 * same Signal and trigger simultaneously their execution.
 *
 * Following the same idea, signals can take several arguments and be triggered
 * by passing the right arguments to `emit`.
 *
 * The following will declare a Signal of type `void(int, int)` and connects it
 * to slots of type `void (int)` and `int (int, int)`.
 *
 *     using namespace fwCom;
 *     Signal< void(int, int) >::sptr sig2 = Signal< void(int, int) >::New();
 *     Slot< int(int, int) >::sptr    slot1 = Slot< int(int, int) >::New(...);
 *     Slot< void(int) >::sptr        slot2 = Slot< void(int) >::New(...);
 *
 *     sig2->connect(slot1);
 *     sig2->connect(slot2);
 *
 *     sig2->emit(21, 42);
 *
 * In the latter example, 2 points need to highlighted :
 *  - the return type of the Signal is `void`. Signal cannot return values, so
 *  their return type is always declared as `void`. Thus, it is not possible to
 *  retrieve `slot1` Slot return value if it is executed using a Signal.
 *  Therefore, both slots are run succesfully.
 *  - the arguments types of `slot2` slot doesn't match exactly sig2 signature.
 *  `slot2` is nevertheless successfully connected and executed. `slot2` receive
 *  the value 21 as argument, 42 is ignored).
 *
 *
 * @subsection SignalAsyncEmit Asynchronous emit
 *
 * As slots can work asynchronously, triggering a Signal with
 * `asyncEmit` results in the execution of connected slots in their worker :
 *
 *     sig2->asyncEmit(21, 42);
 *
 * The instruction above has for consequence to run each connected slot in it's own
 * worker. @note Each connected slot *must have* a worker set in order to use
 * `asyncEmit`.
 *
 * @subsection SignalDisconnect Disconnection
 *
 * Finally, the `disconnect` method will cause the given Slot to be disconnected
 * from the Signal. Thus, the Slot won't be executed anymore each time the
 * Signal is triggered.
 *
 *     sig2->disconnect(slot1);
 *     sig2->emit(21, 42); //do not trigger slot1 anymore
 *
 * The instructions above will cause `slot2` Slot execution : `slot1` having
 * been disconnected, it won't be executed.
 *
 *
 * @section Connection Connection handling
 *
 * The connection of a Slot to a Signal returns a Connection handler :
 *
 *     ::fwCom::Connection connection = signal->connect(slot);
 *
 * Connection provides a mechanism which allows to temporarily disable a Slot
 * in a Signal. The slot stays connected to the Signal, but it will not be
 * triggered while the Connection is blocked :
 *
 *     ::fwCom::Connection::Blocker lock(connection);
 *     signal->emit();
 *     // 'slot' will not be executed while 'lock' is alive or until lock is
 *     // reset
 *
 * Connection handlers can also be used to disconnect a Slot from a Signal :
 *
 *     connection.disconnect();
 *     //slot is not connected anymore
 *
 * @section Autodisconnect Auto-disconnection
 *
 * Slot and signals can handle automatic disconnection :
 *  - on Slot destruction : the Slot will disconnect itself from every signals
 *  it is connected on destruction
 *  - on Signal destruction : the Signal will disconnect all connected slots
 *  before destruction
 *
 * All related connection handlers will be invalidated when an automatic
 * disconnection occurs.
 *
 */
} // namespace fwCom

#endif /* __FWCOM_NAMESPACE_HPP__ */
