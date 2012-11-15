//  SLM_LOG
#include <memory>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/algorithm/string/join.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>



#include <boost/signals2/signal.hpp>


template <typename F> struct convert_function_types;

template <typename R, typename C, typename ...Args >
struct convert_function_types< R (C::*) ( Args... )  >
{
    typedef R type( Args... ) ;
};

template <typename F>
struct convert_function_types< boost::function< F > >
{
    typedef F type;
};

template <typename F>
struct convert_function_types
{
    typedef typename boost::remove_pointer<F>::type type;
};

//-----------------------------------------------------------------------------


template < typename F, typename enable = void >
struct function_types ;

template < typename F>
struct function_types <F, typename boost::enable_if<typename boost::is_function< F >::type >::type >
{
    typedef  F FunctionType;
    enum { arity = boost::function_types::function_arity< FunctionType >::value } ;
    typedef typename boost::function_types::result_type<FunctionType>::type result_type;
    template < int ARG_NUMBER, bool check = false >
    struct arg
    {
        BOOST_STATIC_ASSERT( (!check) || ARG_NUMBER < boost::function_types::function_arity< FunctionType >::value );
        typedef typename boost::mpl::at_c<boost::function_types::parameter_types< FunctionType >,ARG_NUMBER>::type type;
    };
};



//-----------------------------------------------------------------------------


struct Slot
{

    struct bad_call { };
    struct bad_run { };

    virtual ~Slot(){};

    template< typename A1, typename A2, typename A3 > void run(A1 a1, A2 a2, A3 a3);
    template< typename A1, typename A2 > void run(A1 a1, A2 a2);
    template< typename A1 > void run(A1 a1);
    void run();

    template< typename R, typename A1, typename A2, typename A3 > R call(A1 a1, A2 a2, A3 a3);
    template< typename R, typename A1, typename A2 > R call(A1 a1, A2 a2);
    template< typename R, typename A1 > R call(A1 a1);
    template< typename R > R call();


protected:
    template < typename F >
    std::string getTypeName()
    {
        std::string signature = std::string("type(") + typeid(F).name() + ")";
        return signature;
    }

    std::string m_signature;

};


template< typename ... A>
struct slot_args : Slot
{
    virtual void run(A ... a) = 0;
};


template< typename R, typename ... A >
struct slot_result : slot_args<A...>
{
    virtual R call(A...) = 0;
};




template< typename F > struct SlotFunction;


template<typename R, typename ...A >
struct SlotFunction< boost::function< R ( A... ) > > : SlotFunction< R ( A... ) >
{
    typedef R FunctorType ( A... ) ;

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    { }


    virtual void run(A...a) {        m_func(a...); };
    virtual R   call(A...a) { return m_func(a...); };
protected:
    boost::function< FunctorType > m_func;
};



template<typename R, typename ...A >
struct SlotFunction< R ( A... ) > : slot_result< R, A... >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A... ) >();
    }

};







template< typename A1, typename A2, typename A3 > void Slot::run(A1 a1, A2 a2, A3 a3)
{
    typedef slot_args< A1, A2, A3 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1,a2,a3);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void(A1, A2, A3) >()  << std::endl;
        this->run(a1,a2);
    }
}
template< typename A1, typename A2 > void Slot::run(A1 a1, A2 a2)
{
    typedef slot_args< A1, A2 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1,a2);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void(A1, A2) >()  << std::endl;
        this->run(a1);
    }
}
template< typename A1 > void Slot::run(A1 a1)
{
    typedef slot_args< A1 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void(A1) >()  << std::endl;
        this->run();
    }
}
void Slot::run()
{
    typedef slot_args< > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run();
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void() >()  << std::endl;
        throw Slot::bad_run();
    }
}




template< typename R, typename A1, typename A2, typename A3 > R Slot::call(A1 a1, A2 a2, A3 a3)
{
    typedef SlotFunction< R (A1, A2, A3) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1,a2,a3);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void(A1, A2, A3) >()  << std::endl;
        return this->call<R>(a1,a2);
    }
}
template< typename R, typename A1, typename A2 > R Slot::call(A1 a1, A2 a2)
{
    typedef SlotFunction< R(A1, A2) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1,a2);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void(A1, A2) >()  << std::endl;
        return this->call<R>(a1);
    }
}
template< typename R, typename A1 > R Slot::call(A1 a1)
{
    typedef SlotFunction< R(A1) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void(A1) >()  << std::endl;
        return this->call<R>();
    }
}
template< typename R > R Slot::call()
{
    typedef slot_result< R > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void() >()  << std::endl;
        return fun->call();
    }
    throw Slot::bad_call();
}




//-----------------------------------------------------------------------------


class Slots
{
public:
    typedef std::string SlotKeyType;
    typedef boost::shared_ptr< Slot > SlotPtr;
    typedef std::map< SlotKeyType, SlotPtr > SlotMap;

    Slots(){}
    Slots( const SlotKeyType &key, SlotPtr slot )
    {
        (*this)(key, slot);
    }


    Slots& operator()( const SlotKeyType &key, SlotPtr slot )
    {
        m_slots.insert( SlotMap::value_type(key, slot) );
        return *this;
    }

    SlotPtr operator()( const SlotKeyType &key )
    {
        SlotMap::iterator it = m_slots.find(key);

        if(it != m_slots.end())
        {
            return it->second;
        }

        return SlotPtr();
    }

protected:
    SlotMap m_slots;
};




//-----------------------------------------------------------------------------



template<typename T>
struct is_shared_ptr
{
    enum { value = false };
};

template<typename T>
struct is_shared_ptr< boost::shared_ptr< T > >
{
    enum { value = true };
};


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// thread management


void WorkerThread( boost::asio::io_service & io_service )
{
std::cout << "Thread " << boost::this_thread::get_id() <<" Start\n";
io_service.run();
std::cout << "Thread " << boost::this_thread::get_id() <<" Finish\n";
}


class Worker
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef ::boost::shared_ptr< WorkType > WorkPtrType;

    Worker();
    ~Worker();

    template< typename Handler>
    void post(Handler handler)
    {
        m_ioService.post(handler);
    }

protected:
    Worker( const Worker& );
    Worker& operator=( const Worker& );


    IOServiceType m_ioService;
    WorkPtrType m_work;
    boost::thread m_thread;
};

Worker::Worker() :
    m_ioService(),
    m_work( ::boost::make_shared< WorkType >(boost::ref(m_ioService)) ),
    m_thread( ::boost::bind(&WorkerThread, boost::ref(m_ioService)) )
{
}

Worker::~Worker()
{
    m_work.reset();
    m_ioService.stop();
    m_thread.join();
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


template <typename R>
struct TaskHandler
{
    TaskHandler(boost::packaged_task<R>& task) : m_Task(boost::move(task)) {}
    TaskHandler(const TaskHandler& that) : m_Task(boost::move(that.m_Task)) {}

    void operator ()() { this->m_Task(); }

private:
    mutable boost::packaged_task<R> m_Task;
};


template <typename R>
inline boost::function< void() > moveTaskIntoFunction(boost::packaged_task<R>& task)
{
    return TaskHandler<R>(task);
}





class Method
{
public :


    Method(Slots::SlotPtr slot) : m_slot(slot) {}

    Slots::SlotPtr operator=(Slots::SlotPtr slot) { m_slot = slot ; return slot;}
    Method& operator=(const Method &other) { m_slot = other.m_slot ; return *this;}
    // template < typename SLOTPTR >
    // Method( const SLOTPTR &slot, typename boost::enable_if_c< is_shared_ptr< SLOTPTR >::value >::type*x=0 )  : m_slot(slot) {}

    template<typename F>
    Method( F f, typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity == 0 >::type*x=0  ) { setSlot(f); }
    template<typename F>
    Method( F f, typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity == 1 >::type*x=0  ) { setSlot(f,_1); }
    template<typename F>
    Method( F f, typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity == 2 >::type*x=0  ) { setSlot(f,_1,_2); }
    template<typename F>
    Method( F f, typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity == 3 >::type*x=0  ) { setSlot(f,_1,_2,_3); }

    template<typename F, typename O >
    Method( F f, O *o, typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity == 0 >::type*x=0 ) { setSlot(f,o); }
    template<typename F, typename O >
    Method( F f, O *o, typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity == 1 >::type*x=0 ) { setSlot(f,o,_1); }
    template<typename F, typename O >
    Method( F f, O *o, typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity == 2 >::type*x=0 ) { setSlot(f,o,_1,_2); }
    template<typename F, typename O >
    Method( F f, O *o, typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity == 3 >::type*x=0 ) { setSlot(f,o,_1,_2,_3); }

    template <typename ...A>
    void operator()( A... args  )
    {
        m_slot->run( args... );
    }
    template <typename ...A>
    void run( A... args  )
    {
        m_slot->run( args... );
    }
    template <typename R, typename ...A>
    R call( A... args  )
    {
        return m_slot->call<R>( args... );
    }


    template< typename ...A >
    ::boost::shared_future< void > asyncRun( Worker &worker, A... args )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A... >( args... ) );
        ::boost::unique_future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }


    template< typename R, typename ...A >
    ::boost::shared_future< R > asyncCall( Worker &worker, A... args )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A... >( args... ) );
        ::boost::unique_future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    Slots::SlotPtr slot(){return m_slot;};


    template <typename ...A>
    boost::function< void() > bindRun( A... args  )
    {
        return boost::bind( ( void (Method::*)(A...) ) &Method::run< A... >, this, args... );
    }

    template <typename R, typename ...A>
    boost::function< R() > bindCall( A... args  )
    {
        return boost::bind( ( R (Method::*)(A...) ) &Method::call<R, A...>, this, args... );
    }


protected:
    template<typename F, typename ...Bindings> void setSlot(F f, Bindings ...bindings)
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func = boost::bind(f, bindings...) ;
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }

    Slots::SlotPtr m_slot;
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct Event{};
struct KeyEvent:Event{};

struct has_slots
{
    Slots::SlotPtr slot( Slots::SlotKeyType key ){ return m_slots(key);};
protected:
    Slots m_slots;
};

//-----------------------------------------------------------------------------

class A : public has_slots
{

public:
    A()
    {
        has_slots::m_slots
            ( "start"      , Method(&A::start      , this).slot() )
            ( "update"     , Method(&A::update     , this).slot() )
            ( "onEvent"    , Method(&A::onEvent    , this).slot() )
            ( "onKeyPress" , Method(&A::onKeyPress , this).slot() )
            ;

    }
    void start(){ std::cout<<"A::start"<<std::endl; }
    void update(){ std::cout<<"A::update"<<std::endl; }
    void onEvent( Event *e )
    {
        std::cout<<"A::onEvent "<< e <<std::endl;
    }
    void onKeyPress( KeyEvent *e )
    {
        std::cout<<"A::onKeyPress " << e <<std::endl;
    }
    void print( std::string msg ){ std::cout << "A::print " <<msg<<std::endl; }

};

//-----------------------------------------------------------------------------

class B : public has_slots
{
public:
    B()
    {
        has_slots::m_slots
            ( "invoke", Method(&B::invoke, this).slot() )
            ;

    }
    void invoke() { std::cout<<"B::invoke"<<std::endl; }

    void print( std::string str ) { std::cout<< str <<std::endl; }
};

//-----------------------------------------------------------------------------


int sum(int a, int b)
{
    return a + b;
}

void fe(Event*)
{
    std::cout << "fe " <<std::endl;
}

void fke(KeyEvent*)
{
    std::cout << "fke " <<std::endl;
}


int main(int argc, char* argv[])
{

    using namespace std;

    A * a = new A();
    B b;

    Event e;
    KeyEvent ke;

    cout << "============ v2 ============" << endl;
    // v2
    {
        Method call ( &A::start, a );
        call();
        call.call<void>();
    }

    {
        Method call ( &B::invoke, &b );
        call();
        call.call<void>();
        call.call<void>("toto", 3);
        call.call<void>(true, Event());
    }


    {
        Method call ( &A::onEvent, a );
        call(&e);
        call.run<Event*>(&ke);
    }

    {
        Method call ( &sum );
        call(5,7);
        call(5,7,8);
        call.run(5,7,8);
        cout << "sum: " << call.call<int>(5,7) << endl;
    }

    {
        Method call ( &A::onKeyPress, a );
        call.run(&ke);
    }

    cout << "============ v3 ============" << endl;

    // v3
    {
        Method call ( a->slot("start") );
        call.run();
        call.call<void>();

        call = b.slot("invoke");

        Method call2 = call;
        call2.run();

        call2 = call;
        call2.run();

        call = a->slot("update");
        Method call3(call);
        call3.run();
    }

    {
        Method call ( b.slot("invoke") );
        call();
        call.call<void>();
    }

    {
        Method call ( a->slot("onEvent") );
        call.run(&e);
        call.run<Event*>(&ke);
    }

    {
        Method call ( a->slot("onKeyPress") );
        call.run(&ke);

        a->slot("onKeyPress")->run(&ke);
    }


    {

        Slots slots;
        slots
            ( "sum_slot", Method( &sum ).slot() )
            ;

        Method call ( slots("sum_slot") ) ;

        cout << "sum_slot: " << call.call<int>(44,66) << endl;
    }


    cout << "============ slot ============" << endl;

    // direct slot
    {
        a->slot("start")->run(&ke);
        a->slot("onEvent")->run(&e);
        a->slot("onKeyPress")->run(&ke);
        b.slot("invoke")->run();
    }


    cout << "============ signals ============" << endl;
    // signal
    {
          boost::signals2::signal<void (Event*)> sig;

          Method call1 ( &A::start, a );
          Method call2 ( &B::invoke, &b );
          Method call3 ( &A::onEvent, a );

          sig.connect(boost::ref(call1));
          sig.connect(boost::ref(call2));
          sig.connect(boost::ref(call3));

          sig( &e );
    }


    cout << "============ worker ============" << endl;
    cout << "============ worker & async ============" << endl;

    {
        Method invoke ( &B::invoke, &b );
        Method print ( &B::print, &b );
        // call.call<void>();
        // call.call<void>("toto", 3);
        // call.call<void>(true, Event());

        Worker w;

        w.post(invoke);
        w.post( boost::bind( &B::invoke, &b ) );
        w.post( boost::bind( &B::print, &b, "iosrv") );

        // print.bindRun(std::string("bind test"))();

        // w.post(print.bindRun(std::string("bind test")));

        w.post( boost::bind( &B::print, &b, "print : 1") );
        w.post( boost::bind( &B::print, &b, "print : 2") );
        w.post( boost::bind( &B::print, &b, "print : 3") );


        Method sumFunc ( &sum );

        Worker w1;
        Worker w2;

        boost::shared_future< int > future = sumFunc.asyncCall<int>(w1, 2,8);
        cout << "sum.is_ready(): " << future.is_ready() << endl;

        invoke.asyncRun(w1);
        print.asyncRun(w1, std::string("async run w1 0"));
        print.asyncRun(w1, std::string("async run w1 1"));
        print.asyncRun(w1, std::string("async run w1 2"));
        print.asyncRun(w1, std::string("async run w1 3"));
        print.asyncRun(w1, std::string("async run w1 4"));
        print.asyncRun(w1, std::string("async run w1 5"));


        print.asyncRun(w2, std::string("async run w2 0"));
        print.asyncRun(w2, std::string("async run w2 1"));
        print.asyncRun(w2, std::string("async run w2 2"));
        print.asyncRun(w2, std::string("async run w2 3"));
        print.asyncRun(w2, std::string("async run w2 4"));
        print.asyncRun(w2, std::string("async run w2 5"));



        future.wait();
        cout << "sum.is_ready(): " << future.is_ready() << endl;
        cout << "sum.has_value(): " << future.has_value() << endl;
        cout << "sum.has_exception(): " << future.has_exception() << endl;
        cout << "sum: " << future.get() << endl;



        print.asyncRun(w1, std::string("async run w1 with wait")).wait();
        print.asyncRun(w2, std::string("async run w2 with wait")).wait();

    }



    delete a;
    return 0;
}

/* Output :
============ v2 ============
A::start
A::start
B::invoke
B::invoke
B::invoke
B::invoke
A::onEvent 0x7fff5fbff500
A::onEvent 0x7fff5fbff4f8
sum: 12
A::onKeyPress 0x7fff5fbff4f8
============ v3 ============
A::start
A::start
B::invoke
B::invoke
A::onEvent 0x7fff5fbff500
A::onEvent 0x7fff5fbff4f8
A::onKeyPress 0x7fff5fbff4f8
A::onKeyPress 0x7fff5fbff4f8
sum_slot: 110
============ slot ============
A::start
A::onEvent 0x7fff5fbff500
A::onKeyPress 0x7fff5fbff4f8
B::invoke
============ signals ============
A::start
B::invoke
A::onEvent 0x7fff5fbff500

*/


