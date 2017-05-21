LINKER_SCRIPT=esp32.common.ld

cp ld/esp32.common.ld $IDF_PATH/components/esp32/ld
make
