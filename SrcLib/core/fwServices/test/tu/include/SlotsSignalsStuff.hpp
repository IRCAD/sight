/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_SLOTSSIGNALSSTUFF_HPP__
#define __FWSERVICES_UT_SLOTSSIGNALSSTUFF_HPP__

#include <fwServices/IService.hpp>

#include <fwCom/Signals.hpp>

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
    {
    }

    std::chrono::milliseconds m_startRetarder;
    std::chrono::milliseconds m_stopRetarder;
    std::chrono::milliseconds m_updateRetarder;
    std::chrono::milliseconds m_receiveRetarder;
    std::chrono::milliseconds m_swapRetarder;
};

//------------------------------------------------------------------------------

class Buffer : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
        (Buffer)(::fwData::Object),
        (()),
        ::fwData::factory::New< Buffer >);

    Buffer( ::fwData::Object::Key key )
    {
    }
    ~Buffer()
    {
    }

    //------------------------------------------------------------------------------

    void cachedDeepCopy( const Object::csptr& _source, DeepCopyCacheType& cache )
    {
    }
};

//------------------------------------------------------------------------------

class IBasicTest : public ::fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro( (IBasicTest)(::fwServices::IService) );
};

class SBasicTest : public IBasicTest,
                   public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SBasicTest)(IBasicTest) );

    bool m_updateFinished;
    bool m_swapFinished;

    SBasicTest();

protected:

    //------------------------------------------------------------------------------

    virtual void configuring()
    {
    }
    virtual void starting();
    virtual void stopping();
    virtual void swapping();
    virtual void updating();
};

//------------------------------------------------------------------------------

class SReaderTest : public IBasicTest
{
public:

    fwCoreServiceClassDefinitionsMacro( (SReaderTest)(IBasicTest) );

    SReaderTest()
    {
    }

protected:

    //------------------------------------------------------------------------------

    virtual void configuring()
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting()
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping()
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping()
    {
    }
    virtual void updating();
};

//------------------------------------------------------------------------------

class SReader2Test : public IBasicTest
{
public:

    typedef ::fwCom::Signal< void () > ChangedSignalType;

    fwCoreServiceClassDefinitionsMacro( (SReader2Test)(IBasicTest) );

    SReader2Test();

    static const ::fwCom::Signals::SignalKeyType s_CHANGED_SIG;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring()
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting()
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping()
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping()
    {
    }
    virtual void updating();

    ChangedSignalType::sptr m_sigChanged;
};

//------------------------------------------------------------------------------

class SShowTest : public IBasicTest,
                  public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SShowTest)(IBasicTest) );

    SShowTest();

    int m_receiveCount;
    int m_changeCount;

    void change();

    static const ::fwCom::Slots::SlotKeyType s_CHANGE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeSlotType;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring()
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting()
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping()
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping()
    {
    }
    virtual void updating();

    ChangeSlotType::sptr m_slotChange;

    ::fwCore::mt::Mutex m_mutex;
};

//------------------------------------------------------------------------------

class SShow2Test : public IBasicTest,
                   public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SShow2Test)(IBasicTest) );

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT;

    SShow2Test();

    int m_receiveCount;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring()
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting()
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping()
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping()
    {
    }
    virtual void updating();

    /// Slot to receive update
    void updateBuffer();
};

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_SLOTSSIGNALSSTUFF_HPP__
