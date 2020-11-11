# Title: Makefile (deadman_land)
# Author: Griffin Bonner
# Lab: EEHPC (Tinoosh Mohsenin), University of Maryland Baltimore County
# Date: 10/2/2020
# Description: makefile for application layer landing module

# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
PROJ_OBJ += deadman_land.o

CRAZYFLIE_BASE=../..
include $(CRAZYFLIE_BASE)/Makefile
