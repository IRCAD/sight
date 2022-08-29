/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <core/LazyInstantiator.hpp>

#include <data/mt/locked_ptr.hpp>
#include <data/Object.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreSharedPtr.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::viz::scene3d::detail
{

// Traits that provides the type of manager for a given Ogre resource
template<class T>
struct resource_manager_traits;

template<>
struct resource_manager_traits<Ogre::Texture>
{
    using type = Ogre::TextureManager;
};

template<>
struct resource_manager_traits<Ogre::Mesh>
{
    using type = Ogre::MeshManager;
};

template<class T>
using resource_manager_traits_t = typename resource_manager_traits<T>::type;

/**
 * @brief  Interface for GPU resource management sharing.
 *
 * This is based on a classic document/view design, where the document is the source data (image, mesh, etc...) and
 * the view is the GPU representation.
 *
 * The resource manager associates each document, uniquely identified by its ID (such as returned by
 * sight::data::Object::getID()), with an Ogre GPU resource. This allows to share this GPU resource from multiple
 * callers that use the same object.
 *
 * The resource manager also allows to avoid unnecessary updates of the resource. When calling load(), it will compare
 * the modification stamp held in the sight::data::Object against the stamp of the resource. Thus, the concrete
 * implementation of the load code, internalLoad() will only be called once per update of the object, whatever the
 * number of times load() is called. Please keep in mind that any non-const locked_ptr access to a data will increase
 * the modification stamp, so they should only be done when necessary.
 */
template<class OBJECT, class RESOURCE, class LOADER>
class IResourceManager
{
public:

    virtual ~IResourceManager();

    /// Adds a reference to a GPU resource. If it does not exist, it creates a new one, otherwise
    /// this returns the existing one.
    /// @param object CPU representation of the object
    /// @param suffixId optional suffix to the object identifier, this allows to create different copies of the object
    /// @return shared pointer on the resource
    Ogre::SharedPtr<RESOURCE> instantiate(CSPTR(OBJECT), const std::string& suffixId = "");

    /// Removes a reference to a GPU resource. If this was the last reference, it destroys it.
    void release(Ogre::SharedPtr<RESOURCE>);

    /// Returns true if the GPU resource is out of date
    bool updateNeeded(Ogre::SharedPtr<RESOURCE>) const;

    /// Loads the texture into GPU memory if the GPU resource is out of date
    /// @returns True if the GPU resource has been updated, false if it was already up-to-date
    std::pair<bool, typename LOADER::return_t> load(Ogre::SharedPtr<RESOURCE>);

    /**
     * @brief Gets the unique instance of this class.
     * @return the singleton instance.
     */
    static std::shared_ptr<IResourceManager<OBJECT, RESOURCE, LOADER> > get();

private:

    struct Resource
    {
        Ogre::SharedPtr<RESOURCE> resource;
        CWPTR(OBJECT) object;
        std::size_t use_count;
        std::uint64_t lastModified;
        typename LOADER::return_t loadingResult; // Extra attribute to store resource-specific loading data
    };

    std::map<std::string, Resource> m_registry;
};

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
IResourceManager<OBJECT, RESOURCE, LOADER>::~IResourceManager()
{
    SIGHT_WARN_IF("All resources were not freed properly", !m_registry.empty());
}

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
Ogre::SharedPtr<RESOURCE> IResourceManager<OBJECT, RESOURCE, LOADER>::instantiate(
    CSPTR(OBJECT)_object,
    const std::string& _suffixId
)
{
    SIGHT_ASSERT("Data object is null", _object);

    const std::string full_id = _object->getID() + "_Resource" + _suffixId;
    auto it                   = m_registry.find(full_id);
    if(it != m_registry.end())
    {
        SIGHT_DEBUG("Instantiate resource: " << _object->getID());
        it->second.use_count++;
        return it->second.resource;
    }

    SIGHT_DEBUG("Create resource: " << _object->getID());
    auto resource = resource_manager_traits_t<RESOURCE>::getSingleton().create(
        full_id,
        viz::scene3d::RESOURCE_GROUP,
        true
    );

    m_registry[full_id] = {resource, _object, 1, ~0ul, {}};

    return resource;
}

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
void IResourceManager<OBJECT, RESOURCE, LOADER>::release(Ogre::SharedPtr<RESOURCE> _resource)
{
    if(_resource == nullptr)
    {
        return;
    }

    auto it = m_registry.find(_resource->getName());
    if(it == m_registry.end())
    {
        SIGHT_THROW_EXCEPTION(std::out_of_range("No resource found for this object"));
    }

    if(--it->second.use_count == 0)
    {
        m_registry.erase(it);
        resource_manager_traits_t<RESOURCE>::getSingleton().remove(_resource);
    }
}

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
bool IResourceManager<OBJECT, RESOURCE, LOADER>::updateNeeded(Ogre::SharedPtr<RESOURCE> _resource) const
{
    auto it = m_registry.find(_resource->getName());
    if(it == m_registry.end())
    {
        SIGHT_THROW_EXCEPTION(std::out_of_range("No resource found for this object"));
    }

    return it->second.lastModified != it->second.object.lock()->lastModified();
}

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
std::pair<bool, typename LOADER::return_t> IResourceManager<OBJECT, RESOURCE, LOADER>::load(
    Ogre::SharedPtr<RESOURCE> _resource
)
{
    auto it = m_registry.find(_resource->getName());
    if(it == m_registry.end())
    {
        SIGHT_THROW_EXCEPTION(std::out_of_range("No resource found for this object"));
    }

    if(it->second.lastModified != it->second.object.lock()->lastModified())
    {
        const sight::data::mt::locked_ptr lock(it->second.object.lock());

        SIGHT_DEBUG("Update resource: " << _resource->getName());
        const auto result = LOADER::load(*lock, _resource.get());
        it->second.lastModified  = it->second.object.lock()->lastModified();
        it->second.loadingResult = result;

        return {true, result};
    }

    return {false, it->second.loadingResult};
}

// ----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class LOADER>
std::shared_ptr<IResourceManager<OBJECT, RESOURCE, LOADER> > IResourceManager<OBJECT, RESOURCE, LOADER>::get()
{
    return core::LazyInstantiator<IResourceManager<OBJECT, RESOURCE, LOADER> >::getInstance();
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
