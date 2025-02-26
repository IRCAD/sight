/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "password_keeper.hpp"

#include <core/crypto/aes256.hpp>
#include <core/log/spy_logger.hpp>
#include <core/tools/system.hpp>

#include <iomanip>
#include <mutex>

namespace sight::core::crypto
{

// Protects password concurent access
static std::mutex s_password_mutex;

// The static global password. Stored encrypted to make direct memory reading a bit harder
static secure_string s_password;

// This generate the hash used to encrypt the global password
inline static secure_string get_global_password_key()
{
    // NOLINTNEXTLINE(readability-redundant-string-cstr)
    return password_keeper::get_pseudo_password_hash(std::to_string(tools::system::get_pid()).c_str());
}

//------------------------------------------------------------------------------

secure_string password_keeper::get_pseudo_password_hash(const secure_string& _salt) noexcept
{
    return SIGHT_PSEUDO_RANDOM_HASH(_salt);
}

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor): the class is final
class password_keeper::password_keeper_impl final
{
public:

    SIGHT_DECLARE_CLASS(password_keeper_impl);

    /// Delete default copy constructors and assignment operators
    password_keeper_impl(const password_keeper_impl&)            = delete;
    password_keeper_impl(password_keeper_impl&&)                 = delete;
    password_keeper_impl& operator=(const password_keeper_impl&) = delete;
    password_keeper_impl& operator=(password_keeper_impl&&)      = delete;

    /// Default constructor
    password_keeper_impl() noexcept = default;

    /// Default destructor
    ~password_keeper_impl() noexcept = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] core::crypto::secure_string get_password_hash() const
    {
        return core::crypto::hash(this->get_password());
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] core::crypto::secure_string get_password() const
    {
        if(m_password.empty())
        {
            return {};
        }

        return core::crypto::decrypt(m_password, compute_password_key());
    }

    //------------------------------------------------------------------------------

    void set_password(const core::crypto::secure_string& _password)
    {
        m_password = core::crypto::encrypt(_password, compute_password_key());
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool check_password(const core::crypto::secure_string& _password) const
    {
        if(m_password.empty())
        {
            return _password.empty();
        }

        return core::crypto::decrypt(m_password, compute_password_key()) == _password;
    }

    //------------------------------------------------------------------------------

    void reset_password()
    {
        m_password.clear();
    }

private:

    /// Generate a pseudo random password key to store the password obfuscated
    [[nodiscard]] core::crypto::secure_string compute_password_key() const
    {
        return SIGHT_PSEUDO_RANDOM_HASH(std::to_string(reinterpret_cast<std::intptr_t>(this)));
    }

    /// Holds the local password. It is stored as an AES encrypted string
    core::crypto::secure_string m_password;
};

password_keeper::password_keeper() noexcept :
    m_pimpl(std::make_unique<password_keeper_impl>())
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
password_keeper::~password_keeper() noexcept = default;

//------------------------------------------------------------------------------

core::crypto::secure_string password_keeper::get_password_hash() const
{
    return m_pimpl->get_password_hash();
}

//------------------------------------------------------------------------------

core::crypto::secure_string password_keeper::get_password() const
{
    return m_pimpl->get_password();
}

//------------------------------------------------------------------------------

void password_keeper::set_password(const core::crypto::secure_string& _password)
{
    m_pimpl->set_password(_password);
}

//------------------------------------------------------------------------------

bool password_keeper::check_password(const core::crypto::secure_string& _password) const
{
    return m_pimpl->check_password(_password);
}

//------------------------------------------------------------------------------

void password_keeper::reset_password()
{
    m_pimpl->reset_password();
}

//------------------------------------------------------------------------------

core::crypto::secure_string password_keeper::get_global_password_hash()
{
    return core::crypto::hash(password_keeper::get_global_password());
}

//------------------------------------------------------------------------------

core::crypto::secure_string password_keeper::get_global_password()
{
    std::lock_guard guard(s_password_mutex);

    if(s_password.empty())
    {
        return {};
    }

    return core::crypto::decrypt(s_password, get_global_password_key());
}

//------------------------------------------------------------------------------

void password_keeper::set_global_password(
    const core::crypto::secure_string& _password,
    [[maybe_unused]] bool _restart_logger
)
{
    std::lock_guard guard(s_password_mutex);

    // Check if the password is new
    if(_restart_logger)
    {
        // If we use encrypted log
        if(core::log::g_logger.is_log_encrypted())
        {
            // Check if the password has changed and is not the default one.
            if(const auto& old_password = core::crypto::decrypt(s_password, get_global_password_key());
               old_password != _password)
            {
                core::log::g_logger.change_password(_password, old_password);
            }
        }
    }

    // Store the new password
    s_password = core::crypto::encrypt(_password, get_global_password_key());
}

//------------------------------------------------------------------------------

bool password_keeper::check_global_password(const core::crypto::secure_string& _password)
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, get_global_password_key()) == _password;
}

//------------------------------------------------------------------------------

void password_keeper::reset_global_password()
{
    std::lock_guard guard(s_password_mutex);

    s_password.clear();
}

} // namespace sight::core::crypto
