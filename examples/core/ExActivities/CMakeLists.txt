sight_add_target(ExActivities TYPE APP)

add_dependencies(
    ExActivities
    sightrun
    module_appXml
    module_ui_base
    module_ui_qt
    module_activity
    data
    module_service
    module_ui_base
    module_ui_flaticons
    module_io_vtk
    module_io_activity
    activity_viz_negato
    config_ui_activity
    module_viz_sample
    module_viz_scene3d
    module_data
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config parameters PARAM_VALUES ExActivities_Base_AppCfg ExActivities_AppCfgParam)

module_param(
    module_ui_base
    PARAM_LIST preferences_enabled
               preferences_password_policy
               preferences_encryption_policy
               preferences_exit_on_password_error
               preferences_new_password_dialog_title
               preferences_new_password_dialog_message
               preferences_new_password_dialog_icon
               preferences_password_dialog_title
               preferences_password_dialog_message
               preferences_password_dialog_icon
    PARAM_VALUES
        true
        once
        forced
        true
        "New password required"
        "  Please enter a new password: "
        "sight::module::ui::flaticons/ircad.svg"
        "Password required"
        "  Please enter your password: "
        "sight::module::ui::flaticons/ircad.svg"
)

sight_generate_profile(ExActivities)
