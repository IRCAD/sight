/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series_set.hpp>

#include <service/controller.hpp>

namespace sight::module::ui::qt::series
{

/**
 * @brief This service is used to select a series from a series set and then extract it to the appropriate typed data.
 *
 * This service is a work in progress and will be updated in Sight 22.0 to pop-up a dialog to let the user select the
 * series.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section Signals Signals
 * - \b imageSelected(): Emitted when an image is selected.
 * - \b modelSelected(): Emitted when a model is selected.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::qt::series::select_dialog" >
           <in key="seriesSet" uid="..."/>
           <out key="modelSeries" uid="..."/>
           <out key="imageSeries" uid="..."/>
           <out key="image" uid="..."/>
        </service>
       @endcode
 * @subsection Input Input
 * - \b seriesSet [sight::data::series_set]: database series from which the series extracted.
 *
 * @subsection Output Output
 * - \b modelSeries [sight::data::model_series]: output model series extracted from the database series.
 * - \b imageSeries [sight::data::image_series]: output image series extracted from the database series.
 * - \b image [sight::data::image]: output image extracted from the database series.
 */
class MODULE_UI_QT_CLASS_API select_dialog : public service::controller
{
public:

    /// Type of signal sent when a screen is selected.
    using selected_signal_t = core::com::signal<void ()>;

    static const core::com::signals::key_t IMAGE_SELECTED_SIG;
    static const core::com::signals::key_t MODEL_SELECTED_SIG;

    SIGHT_DECLARE_SERVICE(select_dialog, service::controller);

    /// Constructor
    MODULE_UI_QT_API select_dialog();

    /// Destructor
    MODULE_UI_QT_API ~select_dialog() override = default;

protected:

    /// Does nothing
    MODULE_UI_QT_API void starting() override;

    /// Configure the service
    MODULE_UI_QT_API void configuring() override;

    /// Does nothing
    MODULE_UI_QT_API void stopping() override;

    /// Extract the object(s)
    MODULE_UI_QT_API void updating() override;

private:

    sight::data::ptr<sight::data::series_set, sight::data::Access::in> m_series_set {this, "seriesSet"};
    sight::data::ptr<sight::data::model_series, sight::data::Access::out> m_modelSeries {this, "modelSeries"};
    sight::data::ptr<sight::data::image_series, sight::data::Access::out> m_imageSeries {this, "imageSeries"};
    sight::data::ptr<sight::data::image, sight::data::Access::out> m_image {this, "image"};
};

} // namespace sight::module::ui::qt::series
