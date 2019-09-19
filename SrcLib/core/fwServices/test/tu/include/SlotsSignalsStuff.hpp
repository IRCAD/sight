/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
    fwCoreClassMacro(Buffer, ::fwData::Object, ::fwData::factory::New< Buffer >);

    Buffer( ::fwData::Object::Key )
    {
    }
    ~Buffer()
    {
    }

    //------------------------------------------------------------------------------

    void cachedDeepCopy( const Object::csptr&, DeepCopyCacheType& ) override
    {
    }
};

//------------------------------------------------------------------------------

class IBasicTest : public ::fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro( (IBasicTest)(::fwServices::IService) )

    static const KeyType s_BUFFER_INOUT;
};

class SBasicTest : public IBasicTest,
                   public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SBasicTest)(IBasicTest) )

    bool m_updateFinished;
    bool m_swapFinished;

    SBasicTest();

protected:

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    virtual void starting() override;
    virtual void stopping() override;
    virtual void swapping(const KeyType& key) override;
    virtual void updating() override;
};

//------------------------------------------------------------------------------

class SReaderTest : public IBasicTest
{
public:

    fwCoreServiceClassDefinitionsMacro( (SReaderTest)(IBasicTest) )

    SReaderTest()
    {
    }

protected:

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping() override
    {
    }
    virtual void updating() override;
};

//------------------------------------------------------------------------------

class SReader2Test : public IBasicTest
{
public:

    typedef ::fwCom::Signal< void () > ChangedSignalType;

    fwCoreServiceClassDefinitionsMacro( (SReader2Test)(IBasicTest) )

    SReader2Test();

    static const ::fwCom::Signals::SignalKeyType s_CHANGED_SIG;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping() override
    {
    }
    virtual void updating() override;

    ChangedSignalType::sptr m_sigChanged;
};

//------------------------------------------------------------------------------

class SShowTest : public IBasicTest,
                  public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SShowTest)(IBasicTest) )

    SShowTest();

    int m_receiveCount;
    int m_changeCount;

    void change();

    static const ::fwCom::Slots::SlotKeyType s_CHANGE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeSlotType;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping() override
    {
    }
    virtual void updating() override;

    virtual KeyConnectionsMap getAutoConnections() const override;

    ChangeSlotType::sptr m_slotChange;

    ::fwCore::mt::Mutex m_mutex;
};

//------------------------------------------------------------------------------

class SShow2Test : public IBasicTest,
                   public ServiceRetarder
{
public:

    fwCoreServiceClassDefinitionsMacro( (SShow2Test)(IBasicTest) )

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT;

    SShow2Test();

    int m_receiveCount;

protected:

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void swapping() override
    {
    }
    virtual void updating() override;

    /// Slot to receive update
    void updateBuffer();
};

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
