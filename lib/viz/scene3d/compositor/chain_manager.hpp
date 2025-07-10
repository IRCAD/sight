/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include "service/has_services.hpp"

#include <data/object.hpp>

#include <map>
#include <vector>

namespace Ogre
{

class Viewport;

} // namespace Ogre

namespace sight::viz::scene3d
{

class layer;

namespace compositor
{

/**
 * @brief   Manages the compositors in a layer view.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API chain_manager : boost::noncopyable,
                                                  public service::has_services
{
public:

    using uptr = std::unique_ptr<chain_manager>;

    using compositor_id_t    = std::string;
    using compositor_t       = std::pair<compositor_id_t, bool>;
    using compositor_chain_t = std::vector<compositor_t>;

    SIGHT_VIZ_SCENE3D_API chain_manager(const SPTR(viz::scene3d::layer)& _layer);
    SIGHT_VIZ_SCENE3D_API ~chain_manager() override;

    /// Inserts the new compositor in the compositor chain vector
    SIGHT_VIZ_SCENE3D_API void add_available_compositor(compositor_id_t _compositor_name);
    /// Clears the compositor chain
    SIGHT_VIZ_SCENE3D_API void clear_compositor_chain();
    /// Enables or disables the target compositor
    SIGHT_VIZ_SCENE3D_API void update_compositor_state(compositor_id_t _compositor_name, bool _is_enabled);

    SIGHT_VIZ_SCENE3D_API void set_compositor_chain(const std::vector<compositor_id_t>& _compositors);

    SIGHT_VIZ_SCENE3D_API compositor_chain_t get_compositor_chain();

    /// Name of the last compositor put in the compositor chain.
    /// This compositor is used to have a blend in order to get a correct final render
    SIGHT_VIZ_SCENE3D_API static const compositor_id_t FINAL_CHAIN_COMPOSITOR;

private:

    /// Adds the final compositor to the compositor chain
    void add_final_compositor();

    void update_compositor_adaptors(compositor_id_t _compositor_name, bool _is_enabled);

    /// List of available compositors, the names are associated to a boolean value which indicates whether
    /// the compositor is enabled or not
    /// WARNING: It may not match the real list of compositors in the Ogre manager, as other classes may add
    /// compositors as well. So this list is only there to know which compositors are handled by this class.
    compositor_chain_t m_compositor_chain;

    /// The parent layer's.
    WPTR(viz::scene3d::layer) m_layer;

    /// Map allowing to keep the objects of the created adaptors alive
    std::map<std::string, data::object::sptr> m_adaptors_objects_owner;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline chain_manager::compositor_chain_t chain_manager::get_compositor_chain()
{
    return m_compositor_chain;
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace sight::viz::scene3d
