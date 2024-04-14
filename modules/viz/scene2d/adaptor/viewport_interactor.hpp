/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <viz/scene2d/adaptor.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="interactor" type="sight::module::viz::scene2d::adaptor::viewport_interactor">
   </service>
   @endcode
 *
 */
class viewport_interactor : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(viewport_interactor, sight::viz::scene2d::adaptor);

    viewport_interactor() noexcept;

    ~viewport_interactor() noexcept override;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    void zoom(bool _zoom_in);

private:

    bool m_viewport_is_translated {};
    sight::viz::scene2d::vec2d_t m_last_coord_event {};

    sight::data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::inout> m_viewport {this, "viewport"};
};

} // namespace sight::module::viz::scene2d::adaptor
