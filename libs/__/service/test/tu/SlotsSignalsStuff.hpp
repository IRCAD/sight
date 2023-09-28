/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/signals.hpp>

#include <service/base.hpp>

namespace sight::service::ut
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

    SIGHT_DECLARE_CLASS(Buffer, data::Object);

    Buffer()
    {
    }

    ~Buffer() noexcept override = default;

    //------------------------------------------------------------------------------

    void shallow_copy(const Object::csptr& /*unused*/) override
    {
    }
};

//------------------------------------------------------------------------------

class basic_srv : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(basic_srv, service::base);

    static constexpr std::string_view s_BUFFER_INOUT = "buffer";

protected:

    data::ptr<Buffer, data::Access::inout> m_buffer {this, s_BUFFER_INOUT, true};
};

class SBasicTest : public basic_srv,
                   public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SBasicTest, basic_srv);

    bool m_updateFinished {false};
    bool m_swapFinished {false};

    SBasicTest();

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    void starting() override;
    void stopping() override;
    void swapping(std::string_view key) override;
    void updating() override;
};

//------------------------------------------------------------------------------

class SReaderTest : public basic_srv
{
public:

    SIGHT_DECLARE_SERVICE(SReaderTest, basic_srv);

    SReaderTest()
    = default;

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

    void updating() override;
};

//------------------------------------------------------------------------------

class SReader2Test : public basic_srv
{
public:

    typedef core::com::signal<void ()> ChangedSignalType;

    SIGHT_DECLARE_SERVICE(SReader2Test, basic_srv);

    SReader2Test();

    static const core::com::signals::key_t CHANGED_SIG;

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

    void updating() override;

    ChangedSignalType::sptr m_sigChanged;
};

//------------------------------------------------------------------------------

class SShowTest : public basic_srv,
                  public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SShowTest, basic_srv);

    SShowTest();

    int m_receiveCount {0};
    int m_changeCount {0};

    void change();

    static const core::com::slots::key_t CHANGE_SLOT;
    typedef core::com::slot<void ()> ChangeSlotType;

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

    void updating() override;

    connections_t getAutoConnections() const override;

    ChangeSlotType::sptr m_slotChange;

    core::mt::mutex m_mutex;
};

//------------------------------------------------------------------------------

class SShow2Test : public basic_srv,
                   public ServiceRetarder
{
public:

    SIGHT_DECLARE_SERVICE(SShow2Test, basic_srv);

    static const core::com::slots::key_t UPDATE_BUFFER_SLOT;

    SShow2Test();

    int m_receiveCount {0};

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

    void updating() override;

    /// Slot to receive update
    void updateBuffer();
};

//------------------------------------------------------------------------------

} // namespace sight::service::ut
