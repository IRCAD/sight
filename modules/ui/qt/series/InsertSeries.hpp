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

#include "modules/ui/qt/config.hpp"

#include <data/factory/new.hpp>
#include <data/Series.hpp>

namespace sight::module::ui::qt::series
{

/**
 * @brief Used as a placeholder in module::ui::qt::editor::SSelector UI to insert create and insert new series in a
 * series
 * DB.
 */
class MODULE_UI_QT_CLASS_API InsertSeries : public data::Series
{
public:

    SIGHT_DECLARE_CLASS(InsertSeries, data::Object, data::factory::New<InsertSeries>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MODULE_UI_QT_API InsertSeries(data::Object::Key key);

    /// Destructor
    MODULE_UI_QT_API virtual ~InsertSeries();

    /// Defines shallow copy
    MODULE_UI_QT_API void shallowCopy(const data::Object::csptr& _source) override;

    /// Defines deep copy
    MODULE_UI_QT_API void cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache) override;
};

} //end namespace sight::module::ui::qt::series
