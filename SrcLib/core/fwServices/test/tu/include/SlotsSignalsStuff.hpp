/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_SLOTSSIGNALSSTUFF_HPP_
#define _FWSERVICES_TEST_TU_SLOTSSIGNALSSTUFF_HPP_

#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>

#include <fwServices/IService.hpp>

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

class ServiceRetarder
{

public:

    ServiceRetarder() :
        m_startRetarder(500),
        m_stopRetarder(500),
        m_updateRetarder(500),
        m_receiveRetarder(500),
        m_swapRetarder(500)
    {}

    ::boost::posix_time::milliseconds m_startRetarder;
    ::boost::posix_time::milliseconds m_stopRetarder;
    ::boost::posix_time::milliseconds m_updateRetarder;
    ::boost::posix_time::milliseconds m_receiveRetarder;
    ::boost::posix_time::milliseconds m_swapRetarder;
};

//------------------------------------------------------------------------------

class Buffer : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
            (Buffer)(::fwData::Object),
            (()),
            ::fwData::factory::New< Buffer >);

    Buffer( ::fwData::Object::Key key ){}
    ~Buffer(){}

    void deepCopy( const Object::csptr& _source ){}
};

//------------------------------------------------------------------------------

class IBasicTest : public ::fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (IBasicTest)(::fwServices::IService) ) ;
};

class SBasicTest : public IBasicTest, public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SBasicTest)(IBasicTest) ) ;

    bool m_updateFinished;
    bool m_swapFinished;

    SBasicTest();

protected:

    virtual void configuring() throw ( ::fwTools::Failed ){}
    virtual void starting() throw ( ::fwTools::Failed );
    virtual void stopping() throw ( ::fwTools::Failed );
    virtual void swapping() throw ( ::fwTools::Failed );
    virtual void updating() throw ( ::fwTools::Failed );
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ){}
};

//------------------------------------------------------------------------------

class SReaderTest : public IBasicTest
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SReaderTest)(IBasicTest) ) ;

    SReaderTest(){};

protected:

    virtual void configuring() throw ( ::fwTools::Failed ){}
    virtual void starting() throw ( ::fwTools::Failed ){}
    virtual void stopping() throw ( ::fwTools::Failed ){}
    virtual void swapping() throw ( ::fwTools::Failed ){};
    virtual void updating() throw ( ::fwTools::Failed );
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ){}
};

//------------------------------------------------------------------------------

class SReader2Test : public IBasicTest
{
public:

    typedef ::fwCom::Signal< void () > ChangedSignalType;

    fwCoreServiceClassDefinitionsMacro ( (SReader2Test)(IBasicTest) ) ;

    SReader2Test();

    static const ::fwCom::Signals::SignalKeyType s_CHANGED_SIG;

protected:

    virtual void configuring() throw ( ::fwTools::Failed ){}
    virtual void starting() throw ( ::fwTools::Failed ){}
    virtual void stopping() throw ( ::fwTools::Failed ){}
    virtual void swapping() throw ( ::fwTools::Failed ){};
    virtual void updating() throw ( ::fwTools::Failed );
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ){}

    ChangedSignalType::sptr m_sigChanged;
};

//------------------------------------------------------------------------------

class SShowTest : public IBasicTest, public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SShowTest)(IBasicTest) ) ;


    SShowTest();

    int m_receiveCount;
    int m_changeCount;

    void change();

    static const ::fwCom::Slots::SlotKeyType s_CHANGE_SLOT;
    typedef ::fwCom::Slot<void()> ChangeSlotType;

protected:

    virtual void configuring() throw ( ::fwTools::Failed ){}
    virtual void starting() throw ( ::fwTools::Failed ){}
    virtual void stopping() throw ( ::fwTools::Failed ){}
    virtual void swapping() throw ( ::fwTools::Failed ){}
    virtual void updating() throw ( ::fwTools::Failed ){}
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    ChangeSlotType::sptr m_slotChange;

    ::fwCore::mt::Mutex m_mutex;
};

//------------------------------------------------------------------------------

class SShow2Test : public IBasicTest, public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SShow2Test)(IBasicTest) ) ;


    SShow2Test();

    int m_receiveCount;

protected:

    virtual void configuring() throw ( ::fwTools::Failed ){}
    virtual void starting() throw ( ::fwTools::Failed ){}
    virtual void stopping() throw ( ::fwTools::Failed ){}
    virtual void swapping() throw ( ::fwTools::Failed ){}
    virtual void updating() throw ( ::fwTools::Failed );
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
};

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices

#endif // _FWSERVICES_TEST_TU_SLOTSSIGNALSSTUFF_HPP_
