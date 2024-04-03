/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/com/signal.hpp>

#include <io/__/service/reader.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::core::jobs
{

class base;

}

namespace sight::data
{

class series_set;

}

namespace sight::module::io::dicom
{

/**
 * @brief GDCM Reader (ImageSeries/ModelSeries)
 *
 * @section Signals Signals
 * - \b job_created( SPTR(core::jobs::base) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * The reader can be configured as a regular reader.
 * You can select the SOP Classes that can be read using this reader.
 * It is also possible to define a filter that must be applied prior to the reading process.
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::series_set_reader" >
           <inout key="data" uid="..." />
           <config>
               <filterType>::filter::dicom::custom::no_filter<filterType/>
               <enableBufferRotation>yes</enableBufferRotation>
               <showLogDialog>yes</showLogDialog>
               <dicomdirSupport>user_selection</dicomdirSupport>
               <op_classSelection>
                   <sop_class name="CT image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
               </op_classSelection>
           </config>
       </service>
 * @endcode
 *
 * On the other hand, you can specify a service configuration using the filterConfig element:
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::series_set_reader" >
           <inout key="data" uid="..." />
           <config>
               <filterConfig>MyFilterSelectorConfig<filterConfig/>
               <enableBufferRotation>yes</enableBufferRotation>
               <showLogDialog>yes</showLogDialog>
               <dicomdirSupport>user_selection</dicomdirSupport>
               <op_classSelection>
                   <sop_class name="CT image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
               </op_classSelection>
           </config>
       </service>
   @endcode
 *
 * @code{.xml}
    <extension implements="sight::service::extension::config">
        <id>MyFilterSelectorConfig</id>
        <service>module::ui::dicom::filter_selector_dialog</service>
        <desc>"Open" action's filter selector config</desc>
        <config>
            <selection mode="include" />
            <addSelection filter="sight::filter::dicom::custom::default_dicom_filter" />
            <addSelection filter="sight::filter::dicom::custom::no_filter" />
        </config>
    </extension>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::series_set]: object to read
 * @subsection Configuration Configuration:
 * - \b filterType: Define a filter that must be applied prior to the reading process.
 * - \b enableBufferRotation (optional, default set to yes): Enable buffer rotation
 * - \b showLogDialog (optional, default set to yes): Show log dialog
 * - \b dicomdirSupport (optional, default set to user_selection ): DicomDir support mode.
 * dicomdirSupport available mode:
 *    - always (always use the DicomDir if present)
 *    - never (never use the DicomDir)
 *    - user_selection (let the user decide whether using the DicomDir or not)
 * - \b op_classSelection:  List of SOP Class selection managed by the reader
 */
class series_set_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(series_set_reader, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    /**
     * @brief   constructor
     *
     */
    series_set_reader() noexcept;

    /**
     * @brief   destructor
     */
    ~series_set_reader() noexcept override = default;

protected:

    using supported_sop_class_container_t = std::vector<std::string>;

    enum dicom_dir_support
    {
        always = 0,
        never,
        user_selection
    };

    /// Configuring method. This method is used to configure the service.
    void configuring() override;

    /// Override
    void starting() override;

    /// Override
    void stopping() override;

    /// Override
    void updating() override;

    /// Override
    void info(std::ostream& _sstream) override;

    /// Override
    std::string get_selector_dialog_title() override;

    /// Configure using GUI.
    void open_location_dialog() override;

    /// Return managed file type, here FOLDER
    sight::io::service::path_type_t get_path_type() const override;

private:

    /**
     * @brief Create a series_set and fill it using the data of the DICOM files
     * @param[in] _dicom_dir DICOM folder
     */
    SPTR(data::series_set) create_series_set(const std::filesystem::path& _dicom_dir);

    /// Selector config used to select a filter to apply
    std::string m_filter_config;

    /// Selected filter key
    std::string m_filter_type;

    /// Supported SOP Class selection
    supported_sop_class_container_t m_supported_sop_class_selection;

    /// Signal emitted when a job is created
    SPTR(job_created_signal_t) m_sig_job_created;

    /// Show log dialog
    bool m_show_log_dialog;

    /// Enable buffer rotation
    bool m_enable_buffer_rotation;

    /// Specify how to use dicomdir files
    dicom_dir_support m_dicom_dir_support;
};

} // namespace sight::module::io::dicom
