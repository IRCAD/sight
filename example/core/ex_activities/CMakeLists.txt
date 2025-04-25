sight_add_target(ex_activities TYPE APP)

add_dependencies(
    ex_activities
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_activity
    data
    module_service
    module_filter_image
    module_ui
    module_ui_icons
    module_io_vtk
    module_io_activity
    activity_viz_negato
    config_ui_activity
    module_viz_sample
    module_viz_scene3d
    module_data
)

# Allow noctura theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config parameters PARAM_VALUES ExActivities_Base_AppCfg ExActivities_AppCfgParam)

module_param(
    module_ui
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
        "sight::module::ui::icons/ircad_white.svg"
        "Password required"
        "  Please enter your password: "
        "sight::module::ui::icons/ircad_white.svg"
)
