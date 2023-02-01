/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
inline static secure_string getGlobalPasswordKey()
{
    // NOLINTNEXTLINE(readability-redundant-string-cstr)
    return PasswordKeeper::get_pseudo_password_hash(std::to_string(tools::System::getPID()).c_str());
}

//------------------------------------------------------------------------------

secure_string PasswordKeeper::get_pseudo_password_hash(const secure_string& salt) noexcept
{
    return SIGHT_PSEUDO_RANDOM_HASH(salt);
}

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor): the class is final
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

    [[nodiscard]] inline core::crypto::secure_string getPasswordHash() const
    {
        return core::crypto::hash(this->getPassword());
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline core::crypto::secure_string getPassword() const
    {
        return core::crypto::decrypt(m_password, computePasswordKey());
    }

    //------------------------------------------------------------------------------

    inline void setPassword(const core::crypto::secure_string& password)
    {
        m_password = core::crypto::encrypt(password, computePasswordKey());
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool checkPassword(const core::crypto::secure_string& password) const
    {
        return core::crypto::decrypt(m_password, computePasswordKey()) == password;
    }

    //------------------------------------------------------------------------------

    inline void resetPassword()
    {
        m_password.clear();
    }

private:

    /// Generate a pseudo random password key to store the password obfuscated
    [[nodiscard]] inline core::crypto::secure_string computePasswordKey() const
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
    return m_pimpl->getPasswordHash();
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::get_password() const
{
    return m_pimpl->getPassword();
}

//------------------------------------------------------------------------------

void PasswordKeeper::set_password(const core::crypto::secure_string& password)
{
    m_pimpl->setPassword(password);
}

//------------------------------------------------------------------------------

bool PasswordKeeper::check_password(const core::crypto::secure_string& password) const
{
    return m_pimpl->checkPassword(password);
}

//------------------------------------------------------------------------------

void PasswordKeeper::reset_password()
{
    return m_pimpl->resetPassword();
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

    return core::crypto::decrypt(s_password, getGlobalPasswordKey());
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
    if(restart_logger && core::crypto::decrypt(s_password, getGlobalPasswordKey()) != password)
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
    s_password = core::crypto::encrypt(password, getGlobalPasswordKey());
}

//------------------------------------------------------------------------------

bool PasswordKeeper::check_global_password(const core::crypto::secure_string& password)
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, getGlobalPasswordKey()) == password;
}

//------------------------------------------------------------------------------

void PasswordKeeper::reset_global_password()
{
    std::lock_guard guard(s_password_mutex);

    s_password.clear();
}

} // namespace sight::core::crypto
