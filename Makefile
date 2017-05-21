#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := app-template

include $(IDF_PATH)/make/project.mk

tags:
	rm tags
	ctags -R --c++-kinds=+p *
	ctags -R --c++-kinds=+p $(IDF_PATH)
