/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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
#include <core/crypto/SHA256.hpp>
#include <core/tools/System.hpp>

#include <iomanip>
#include <mutex>

namespace sight::io::session
{

// Protects password concurent access
static std::mutex s_password_mutex;

// The static global password. Stored encrypted to make direct memory reading a bit harder
static core::crypto::secure_string s_password;

// This generate the hash used to encrypt the global password
inline static core::crypto::secure_string computeGlobalPasswordKey()
{
    return SIGHT_PSEUDO_RANDOM_HASH(std::to_string(sight::core::tools::System::getPID()));
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

    inline core::crypto::secure_string getPasswordHash() const
    {
        return core::crypto::hash(this->getPassword());
    }

    //------------------------------------------------------------------------------

    inline core::crypto::secure_string getPassword() const
    {
        return core::crypto::decrypt(m_password, computePasswordKey());
    }

    //------------------------------------------------------------------------------

    inline void setPassword(const core::crypto::secure_string& password)
    {
        m_password = core::crypto::encrypt(password, computePasswordKey());
    }

    //------------------------------------------------------------------------------

    inline bool checkPassword(const core::crypto::secure_string& password) const
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
    inline core::crypto::secure_string computePasswordKey() const
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

core::crypto::secure_string PasswordKeeper::getPasswordHash() const
{
    return m_pimpl->getPasswordHash();
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::getPassword() const
{
    return m_pimpl->getPassword();
}

//------------------------------------------------------------------------------

void PasswordKeeper::setPassword(const core::crypto::secure_string& password)
{
    m_pimpl->setPassword(password);
}

//------------------------------------------------------------------------------

bool PasswordKeeper::checkPassword(const core::crypto::secure_string& password) const
{
    return m_pimpl->checkPassword(password);
}

//------------------------------------------------------------------------------

void PasswordKeeper::resetPassword()
{
    return m_pimpl->resetPassword();
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::getGlobalPasswordHash()
{
    return core::crypto::hash(PasswordKeeper::getGlobalPassword());
}

//------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::getGlobalPassword()
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, computeGlobalPasswordKey());
}

//------------------------------------------------------------------------------

void PasswordKeeper::setGlobalPassword(const core::crypto::secure_string& password)
{
    std::lock_guard guard(s_password_mutex);
    s_password = core::crypto::encrypt(password, computeGlobalPasswordKey());
}

//------------------------------------------------------------------------------

bool PasswordKeeper::checkGlobalPassword(const core::crypto::secure_string& password)
{
    std::lock_guard guard(s_password_mutex);

    return core::crypto::decrypt(s_password, computeGlobalPasswordKey()) == password;
}

//------------------------------------------------------------------------------

void PasswordKeeper::resetGlobalPassword()
{
    std::lock_guard guard(s_password_mutex);

    s_password.clear();
}

} // namespace sight::io::session
