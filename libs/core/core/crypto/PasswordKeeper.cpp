/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "PasswordKeeper.hpp"

#include <core/crypto/AES256.hpp>
#include <core/log/SpyLogger.hpp>
#include <core/tools/System.hpp>

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
    return PasswordKeeper::get_pseudo_password_hash(std::to_string(tools::System::getPID()).c_str());
}

//------------------------------------------------------------------------------

secure_string PasswordKeeper::get_pseudo_password_hash(const secure_string& salt) noexcept
{
    return SIGHT_PSEUDO_RANDOM_HASH(salt.c_str());
}

class PasswordKeeper::PasswordKeeperImpl final
{
public:

    SIGHT_DECLARE_CLASS(PasswordKeeperImpl);

    /// Delete default copy constructors and assignment operators
    PasswordKeeperImpl(const PasswordKeeperImpl&)            = delete;
    PasswordKeeperImpl(PasswordKeeperImpl&&)                 = delete;
    PasswordKeeperImpl& operator=(const PasswordKeeperImpl&) = delete;
    PasswordKeeperImpl& operator=(PasswordKeeperImpl&&)      = delete;

    /// Default constructor
    inline PasswordKeeperImpl() noexcept = default;

    /// Default destructor
    inline ~PasswordKeeperImpl() noexcept = default;

    //------------------------------------------------------------------------------

    inline core::crypto::secure_string get_password_hash() const
    {
        return core::crypto::hash(this->get_password());
    }

    //------------------------------------------------------------------------------

    inline core::crypto::secure_string get_password() const
    {
        return core::crypto::decrypt(m_password, compute_password_key());
    }

    //------------------------------------------------------------------------------

    inline void set_password(const core::crypto::secure_string& password)
    {
        m_password = core::crypto::encrypt(password, compute_password_key());
    }

    //------------------------------------------------------------------------------

    inline bool check_password(const core::crypto::secure_string& password) const
    {
        return core::crypto::decrypt(m_password, compute_password_key()) == password;
    }

    //------------------------------------------------------------------------------

    inline void reset_password()
    {
        m_password.clear();
    }

private:

    /// Generate a pseudo random password key to store the password obfuscated
    inline core::crypto::secure_string compute_password_key() const
    {
        return SIGHT_PSEUDO_RANDOM_HASH(std::to_string(reinterpret_cast<std::intptr_t>(this)));
    }

    /// Holds the local password. It is stored as an AES encrypted string
    core::crypto::secure_string m_password;
};

PasswordKeeper::PasswordKeeper() noexcept :
    m_pimpl(std::make_unique<PasswordKeeperImpl>())
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
PasswordKeeper::~PasswordKeeper() noexcept = default;

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::get_password_hash() const
{
    return m_pimpl->get_password_hash();
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::get_password() const
{
    return m_pimpl->get_password();
}

//------------------------------------------------------------------------------

void PasswordKeeper::set_password(const core::crypto::secure_string& password)
{
    m_pimpl->set_password(password);
}

//------------------------------------------------------------------------------

bool PasswordKeeper::check_password(const core::crypto::secure_string& password) const
{
    return m_pimpl->check_password(password);
}

//------------------------------------------------------------------------------

void PasswordKeeper::reset_password()
{
    return m_pimpl->reset_password();
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::get_global_password_hash()
{
    return core::crypto::hash(PasswordKeeper::get_global_password());
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::get_global_password()
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, get_global_password_key());
}

//------------------------------------------------------------------------------

void PasswordKeeper::set_global_password(
    const core::crypto::secure_string& password,
    [[maybe_unused]] bool restart_logger
)
{
    std::lock_guard guard(s_password_mutex);

#if defined(SIGHT_ENABLE_ENCRYPTED_LOG)
    // Check if the password is new
    if(restart_logger && core::crypto::decrypt(s_password, get_global_password_key()) != password)
    {
        // If we use encrypted log
        if(auto& logger = core::log::SpyLogger::get(); logger.is_log_encrypted())
        {
            // Check if the password is not the default one.
            // If this is the case, we do nothing as the logger is already started
            if constexpr(has_default_password())
            {
                if(password != get_default_password())
                {
                    logger.change_log_password(password);
                }
            }
            else
            {
                logger.change_log_password(password);
            }
        }
    }
#endif

    // Store the new password
    s_password = core::crypto::encrypt(password, get_global_password_key());
}

//------------------------------------------------------------------------------

bool PasswordKeeper::check_global_password(const core::crypto::secure_string& password)
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, get_global_password_key()) == password;
}

//------------------------------------------------------------------------------

void PasswordKeeper::reset_global_password()
{
    std::lock_guard guard(s_password_mutex);

    s_password.clear();
}

} // namespace sight::core::crypto
