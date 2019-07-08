CC = g++
CFLAGS = -std=c++11 -fpermissive
# c++17 -g -w
#  -D_DEBUG -Wshadow -Wno-sign-compare 

# DEBUG MODE: run : make BUILD=debug or $ make debug
ifeq ($(BUILD),debug)   
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g -Wall -Wextra 
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O3 
# -s
# -02: 
# -03: optimize space by running inline
endif

AVX2:=$(shell cat /proc/cpuinfo | grep -c avx2)
AVX:=$(shell cat /proc/cpuinfo | grep -c avx)
NEON:=$(shell cat /proc/cpuinfo | grep -c neon)

ifneq ($(AVX2),0)
CFLAGS += -mavx2 -mfma 
else
# AVX2 machines will also match on AVX
ifneq ($(AVX),0)
CFLAGS += -mavx
endif
endif

ifneq ($(NEON),0)
CFLAGS+=-mfpu=neon -march=armv8-a -mtune=cortex-a53toSYNTH_1TO1_CLIPCHOICE
endif

#=======================================================================================================
TARGET_NAME = run_lpcnet_server
CURRENT_DIR = $(shell pwd)
EXTERNAL_LIB_DIR = ${CURRENT_DIR}/external_libraries/

INC = 	-I${EXTERNAL_LIB_DIR}/thrift/cmake-build/thrift-package/include/
INC += 	-I${EXTERNAL_LIB_DIR}/lame-3.100/cmake-build/libraries/include/
INC += 	-I${CURRENT_DIR}/include/
INC += 	-I${EXTERNAL_LIB_DIR}/fdk-aac/cmake-build/fdk-acc-package/include/
INC += 	-I${EXTERNAL_LIB_DIR}/openssl-1.0.2o/cmake-build/include/
INC += 	-I${EXTERNAL_LIB_DIR}/libevent-2.1.8-stable/cmake-build/include/

SRC = 	${CURRENT_DIR}/src/*.cpp 
SRC += 	${CURRENT_DIR}/src/*.c

LFLAGS = 	-L${EXTERNAL_LIB_DIR}/thrift/cmake-build/thrift-package/lib/
LFLAGS += 	-L${EXTERNAL_LIB_DIR}/lame-3.100/cmake-build/libraries/lib/
LFLAGS += 	-L${EXTERNAL_LIB_DIR}/fdk-aac/cmake-build/fdk-acc-package/lib/
LFLAGS += 	-L${EXTERNAL_LIB_DIR}/openssl-1.0.2o/cmake-build/lib/
LFLAGS += 	-L${EXTERNAL_LIB_DIR}/libevent-2.1.8-stable/cmake-build/lib/

LDFLAGS = -lthrift -lthriftnb -lmp3lame -lfdk-aac -levent -lstdc++ -lm -lssl

#=======================================================================================================
all: ${CURRENT_DIR}/bin/${TARGET_NAME}

${CURRENT_DIR}/bin/${TARGET_NAME}: 
	$(CC) $(CFLAGS) ${LFLAGS} ${SRC} $(INC) $(LDFLAGS) -o ${CURRENT_DIR}/bin/${TARGET_NAME}

clean:
	${RM} ${CURRENT_DIR}/bin/${TARGET_NAME}

