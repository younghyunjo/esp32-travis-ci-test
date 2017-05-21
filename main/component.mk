#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_ADD_INCLUDEDIRS := . MQTTClient-C/src MQTTClient-C/src/mbedtls MQTTPacket/src

COMPONENT_SRCDIRS := $(COMPONENT_ADD_INCLUDEDIRS)

