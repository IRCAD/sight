/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/data/config.hpp>

#include "data/structure_traits.hpp"

#include <boost/bimap/bimap.hpp>

namespace sight::data
{

/**
 * @brief This class defines some helper for the structure traits.
 */
class SIGHT_DATA_CLASS_API structure_traits_helper
{
public:

    using class_translator_t = boost::bimaps::bimap<data::structure_traits::structure_class, std::string>;
    /// Map to translate structure class (string vs enum)
    SIGHT_DATA_API static const class_translator_t CLASSTRANSLATOR;

    using category_translator_t = boost::bimaps::bimap<data::structure_traits::category, std::string>;
    /// Map to translate structure categories (string vs enum)
    SIGHT_DATA_API static const category_translator_t CATEGORYTRANSLATOR;
};

} // namespace sight::data
