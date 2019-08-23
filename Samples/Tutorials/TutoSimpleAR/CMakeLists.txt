fwLoadProperties()

set(BUNDLE_PATH "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

file(DOWNLOAD https://owncloud.ircad.fr/index.php/s/ZGo6FTVOsQlWycG/download "${BUNDLE_PATH}/calibration.xml" SHOW_PROGRESS
     EXPECTED_HASH SHA256=2f983ec650e9df2329354c68d9aff3818741dda921c1857692886cf1bbd947b2)

file(DOWNLOAD https://owncloud.ircad.fr/index.php/s/oyo3mDMObZcRpQM/download "${BUNDLE_PATH}/aruco_tag.m4v" SHOW_PROGRESS
      EXPECTED_HASH SHA256=92ffe4f0f89380985980a5c1c2fb7536543dee237cddd21922ead2c4e83ecbe2)

generic_install()
