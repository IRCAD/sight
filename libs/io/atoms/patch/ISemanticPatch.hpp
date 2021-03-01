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

#include "io/atoms/config.hpp"
#include "io/atoms/patch/helper/Object.hpp"
#include "io/atoms/patch/IPatch.hpp"

#include <core/base.hpp>
#include <core/mt/types.hpp>

#include <string>

namespace sight::atoms
{
class Object;
}

namespace sight::io::atoms::patch
{

/**
 * @brief Base class of contextual patches.
 */
class IO_ATOMS_CLASS_API ISemanticPatch : public IPatch
{

public:
    SIGHT_DECLARE_CLASS(ISemanticPatch)
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Constructor
    IO_ATOMS_API ISemanticPatch();

    /// Copy constructor
    IO_ATOMS_API ISemanticPatch( const ISemanticPatch& cpy );

    /// Destructor
    IO_ATOMS_API virtual ~ISemanticPatch();

    /**
     * @brief Applies modifications between given objects.
     *
     * @param previous object from which modifications will be applied
     * @param current object receiving modifications
     * @param newVersions map containing already patched sub objects
     *
     * @pre previous and current objects must have the same classname and version.
     */
    IO_ATOMS_API virtual void apply(const SPTR(sight::atoms::Object)& previous,
                                    const SPTR(sight::atoms::Object)& current,
                                    io::atoms::patch::IPatch::NewVersionsType& newVersions) override;

    /**
     * @brief Returns true if patch is applicable into the context from origin to target versions.
     * @note This method is thread safe.
     */
    IO_ATOMS_API virtual bool isApplicable(
        const std::string& context,
        const std::string& originVersion,
        const std::string& targetVersion) const;

protected:

    ///Struct used to store a context and the versions where the patch can be applied
    struct Context
    {
        std::string m_context;
        std::string m_originVersion;
        std::string m_targetVersion;

        Context(const std::string& context, const std::string& originVersion, const std::string& targetVersion) :
            m_context(context),
            m_originVersion(originVersion),
            m_targetVersion(targetVersion)
        {
        }

        //------------------------------------------------------------------------------

        bool operator==(const Context& b) const
        {
            return m_context == b.m_context
                   && m_originVersion == b.m_originVersion
                   && m_targetVersion == b.m_targetVersion;
        }
    };

    /**
     * @brief Add a context where the patch can be applied
     * @note This method is thread safe.
     */
    IO_ATOMS_API virtual void addContext(const std::string& context,
                                         const std::string& originVersion,
                                         const std::string& targetVersion);

    /**
     * @name Typedefs
     * @{ */
    typedef std::vector< Context > ContextVectorType;
    /**  @} */

    /// Mutex to protect concurrent access for m_contexts
    mutable core::mt::ReadWriteMutex m_mutex;

    ///Vector used to store the contexts where the patch can be applied
    ContextVectorType m_contexts;

};

} //fwAtomsPatch
