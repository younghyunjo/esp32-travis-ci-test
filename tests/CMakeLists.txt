CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

######## CPPUTEST START ########
SET (CPPUTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fixture/cpputest)
SET (CPPUTEST_LIB ${CPPUTEST_DIR}/lib/libCppUTest.a ${CPPUTEST_DIR}/lib/libCppUTestExt.a)
SET (CPPUTEST_INCDIR ${CPPUTEST_DIR}/include)

include(ExternalProject)
ExternalProject_Add(cpputest 
	PREFIX ${CPPUTEST_DIR}
	GIT_REPOSITORY https://github.com/cpputest/cpputest.git
	GIT_TAG v3.8
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CPPUTEST_DIR}
)
######## CPPUTEST END ########3

################## MACRO START ##############
MACRO(INCLUDE_DIRECTORIES_ESP_IDF_COMPONENTS)
    FILE (GLOB_RECURSE new_list $ENV{IDF_PATH}/components/*.h)
    FOREACH (file_path ${new_list})
        STRING (FIND ${file_path} "/newlib/" IS_NEWLIB)
        IF (NOT ${IS_NEWLIB} EQUAL -1)
            CONTINUE()
        ENDIF()
        STRING (FIND ${file_path} "/wpa_supplicant/" IS_WPA_SUPPLICANT)
        IF (NOT ${IS_WPA_SUPPLICANT} EQUAL -1)
            CONTINUE()
        ENDIF()
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        INCLUDE_DIRECTORIES(${dir_path})
        GET_FILENAME_COMPONENT(dir_path ${dir_path} PATH)
        #MESSAGE(${dir_path})
        INCLUDE_DIRECTORIES(${dir_path})
    ENDFOREACH()
ENDMACRO()
############### MACRO END ##################

LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
INCLUDE (CodeCoverage)
SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

SET (PROJECT_COMPONETS_DIRECTORY ${PROJECT_BINARY_DIR}/../components)
INCLUDE_DIRECTORIES(
	./fixture
	./mock
	${PROJECT_BINARY_DIR}/../build/include
	${CPPUTEST_INCDIR}
)

INCLUDE_DIRECTORIES_ESP_IDF_COMPONENTS()
ENABLE_TESTING()

##### test analog sensor start #####
ADD_EXECUTABLE(test_analog_sensor
	${PROJECT_COMPONETS_DIRECTORY}/analog_sensor/analog_sensor.c
	mock/m_adc.c
	mock/m_esp_log.c
	analog_sensor.cpp
	../main/help.c
	test_main.cpp
)

TARGET_INCLUDE_DIRECTORIES (test_analog_sensor
	PUBLIC ${PROJECT_COMPONETS_DIRECTORY}/analog_sensor/include/
)

TARGET_LINK_LIBRARIES (test_analog_sensor
	${CPPUTEST_LIB}
)

ADD_DEPENDENCIES(test_analog_sensor cpputest)
ADD_TEST(NAME analog_sensor COMMAND test_analog_sensor -c)
SETUP_TARGET_FOR_COVERAGE(coverage test_analog_sensor coverage)
##### test analog sensor end #####

