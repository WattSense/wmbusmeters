# groups all meters sources
set(METERS_SRC
    meter_amiplus.cc
    meter_apator08.cc
    meter_apator162.cc
    meter_cma12w.cc
    meter_ebzwmbe.cc
    meter_ehzp.cc
    meter_esyswm.cc
    meter_eurisii.cc
    meter_fhkvdataiii.cc
    meter_hydrodigit.cc
    meter_hydrus.cc
    meter_iperl.cc
    meter_izar.cc
    meter_lansenth.cc
    meter_mkradio3.cc
    meter_multical21.cc
    meter_multical302.cc
    meter_omnipower.cc
    meter_q400.cc
    meter_qcaloric.cc
    meter_rfmamb.cc
    meter_rfmtx1.cc
    meter_supercom587.cc
    meter_vario451.cc
)

# groups core lib sources
set(CORE_SRC
	aes.cc
	aescmac.cc
	cmdline.cc
	config.cc
	dvparser.cc
	meters.cc
	printer.cc
	serial.cc
	shell.cc
	util.cc
	units.cc
	wmbus.cc
	wmbus_amb8465.cc
	wmbus_im871a.cc
	wmbus_cul.cc
	wmbus_d1tc.cc
	wmbus_rtlwmbus.cc
	wmbus_simulator.cc
	wmbus_rawtty.cc
	wmbus_wmb13u.cc
    wmbus_utils.cc
)
set(PUB_HEADERS
    dvparser.h
    manufacturers.h
    meters.h
    meters_common_implementation.h
    serial.h
    units.h
    util.h
    wmbus_utils.h
    wmbus.h)
# core library
add_library(core_lib SHARED ${CORE_SRC} ${METERS_SRC})
set_target_properties(core_lib PROPERTIES
                      OUTPUT_NAME wmbusmeters)
set_target_properties(core_lib PROPERTIES PUBLIC_HEADER "${PUB_HEADERS}")
target_link_libraries(core_lib PRIVATE pthread)

# generated files
set(SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/scripts")
add_custom_command(OUTPUT version.h
                   COMMAND ${SCRIPTS_DIR}/gen_version.sh ${CMAKE_BINARY_DIR}/src/version.h)
add_custom_command(OUTPUT short_manual.h
                   COMMAND ${SCRIPTS_DIR}/gen_manual.sh ${CMAKE_BINARY_DIR}/src/short_manual.h
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# main binary
add_executable(wmbusmeters main.cc version.h short_manual.h)
target_include_directories(wmbusmeters PRIVATE ${CMAKE_BINARY_DIR}/src)
target_link_libraries(wmbusmeters core_lib ${DEBUG_LIBS})

# test binary
add_executable(testinternals testinternals.cc)
target_link_libraries(testinternals core_lib)

install(TARGETS wmbusmeters core_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wmbusmeters)
