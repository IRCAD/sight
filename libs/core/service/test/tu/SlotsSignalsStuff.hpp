/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/com/Signals.hpp>

#include <service/IService.hpp>

namespace sight::service
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

class Buffer : public data::Object
{
public:

    SIGHT_DECLARE_CLASS(Buffer, data::Object, data::factory::New<Buffer>);

    Buffer(data::Object::Key)
    {
    }

    ~Buffer()
    {
    }

    //------------------------------------------------------------------------------

    void cachedDeepCopy(const Object::csptr&, DeepCopyCacheType&) override
    {
    }
};

//------------------------------------------------------------------------------

class IBasicTest : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IBasicTest, service::IService);

    static const KeyType s_BUFFER_INOUT;
};

class SBasicTest : public IBasicTest,
                   public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SBasicTest, IBasicTest);

    bool m_updateFinished;
    bool m_swapFinished;

    SBasicTest();

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    void starting() override;
    void stopping() override;
    void swapping(const KeyType& key) override;
    void updating() override;
};

//------------------------------------------------------------------------------

class SReaderTest : public IBasicTest
{
public:

    SIGHT_DECLARE_SERVICE(SReaderTest, IBasicTest);

    SReaderTest()
    {
    }

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void swapping() override
    {
    }

    void updating() override;
};

//------------------------------------------------------------------------------

class SReader2Test : public IBasicTest
{
public:

    typedef core::com::Signal<void ()> ChangedSignalType;

    SIGHT_DECLARE_SERVICE(SReader2Test, IBasicTest);

    SReader2Test();

    static const core::com::Signals::SignalKeyType s_CHANGED_SIG;

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void swapping() override
    {
    }

    void updating() override;

    ChangedSignalType::sptr m_sigChanged;
};

//------------------------------------------------------------------------------

class SShowTest : public IBasicTest,
                  public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SShowTest, IBasicTest);

    SShowTest();

    int m_receiveCount;
    int m_changeCount;

    void change();

    static const core::com::Slots::SlotKeyType s_CHANGE_SLOT;
    typedef core::com::Slot<void ()> ChangeSlotType;

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void swapping() override
    {
    }

    void updating() override;

    KeyConnectionsMap getAutoConnections() const override;

    ChangeSlotType::sptr m_slotChange;

    core::mt::Mutex m_mutex;
};

//------------------------------------------------------------------------------

class SShow2Test : public IBasicTest,
                   public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SShow2Test, IBasicTest);

    static const core::com::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT;

    SShow2Test();

    int m_receiveCount;

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void swapping() override
    {
    }

    void updating() override;

    /// Slot to receive update
    void updateBuffer();
};

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::service
