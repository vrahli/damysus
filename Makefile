#
# Copyright (C) 2011-2016 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

######## My Settings ########

SALTICIDAE=$(realpath ./salticidae)
Salticidae_Include_Paths = -I$(SALTICIDAE)/include
Salticidae_Lib_Paths = -L$(SALTICIDAE)/lib

#LOCAL=${HOME}/.local
# for -I
#export CPATH           := ${CPATH}:$(LOCAL)/include/pbc
# for -L
#export LIBRARY_PATH    := ${LIBRARY_PATH}:$(LOCAL)/lib
# for
#export LD_LIBRARY_PATH := ${LD_LIBRARY_PATH}:$(LOCAL)/lib
# for rpath
#export LD_RUN_PATH     := ${LD_RUN_PATH}:$(LOCAL)/lib

CFLAGS = `pkg-config --cflags libcrypto openssl libuv` # gio-2.0 openssl
LDLIBS = `pkg-config --libs   libcrypto openssl libuv` #-lgmp # gio-2.0 openssl


######## SGX SDK Settings ########

SGX_SDK ?= /opt/intel/sgxsdk
#SGX_MODE ?= HW
SGX_MODE ?= SIM
SGX_ARCH ?= x64
SGX_DEBUG ?= 0
SGX_PRERELEASE ?= 1

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
		SGX_COMMON_CFLAGS += -O0 -g
else
		SGX_COMMON_CFLAGS += -O2
endif

######## SGX-SSL ########

SGXSSL_UNTRUSTED_LIB_PATH := /opt/intel/sgxssl/lib64/
SGXSSL_TRUSTED_LIB_PATH := /opt/intel/sgxssl/lib64/
SGXSSL_INCLUDE_PATH := /opt/intel/sgxssl/include/

######## App Settings ########

ifneq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts_sim
else
	Urts_Library_Name := sgx_urts
endif

# Non-SGX files
Nsgx_App_Cpp_Files := $(wildcard App/*.cpp)
Nsgx_App_Cpp_Files := $(filter-out App/test.cpp App/foo.cpp App/Start.cpp App/App.cpp App/Keys.cpp App/Client.cpp App/Server.cpp, $(Nsgx_App_Cpp_Files))
# Includes SGX files
App_Cpp_Files :=  $(Nsgx_App_Cpp_Files) App/sgx_utils/sgx_utils.cpp
App_Include_Paths := -IApp -I$(SGX_SDK)/include $(Salticidae_Include_Paths) # -I$(SGXSSL_INCLUDE_PATH)

App_C_Flags := $(SGX_COMMON_CFLAGS) -fPIC -Wno-attributes $(App_Include_Paths)

# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
		App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
		App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
		App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cpp_Flags := $(App_C_Flags) -std=c++14 $(CFLAGS)
App_Link_Flags := $(SGX_COMMON_CFLAGS) -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -L$(SGXSSL_UNTRUSTED_LIB_PATH) -lsgx_usgxssl $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae
# -lpthread

#$(info $$(CXX) is [${CXX}])
#$(info $$(SGXSSL_UNTRUSTED_LIB_PATH) is [${SGXSSL_UNTRUSTED_LIB_PATH}])
#$(info $$(SGXSSL_TRUSTED_LIB_PATH) is [${SGXSSL_TRUSTED_LIB_PATH}])
#$(info $$(SGXSSL_INCLUDE_PATH) is [${SGXSSL_INCLUDE_PATH}])
#$(info $$(App_Link_Flags) is [${App_Link_Flags}])
#$(info $$(App_Cpp_Flags) is [${App_Cpp_Flags}])

ifneq ($(SGX_MODE), HW)
	App_Link_Flags += -lsgx_uae_service_sim
else
	App_Link_Flags += -lsgx_uae_service
endif

Nsgx_App_Cpp_Objects := $(Nsgx_App_Cpp_Files:.cpp=.o)
App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := sgxserver

######## Enclave Settings ########

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif
Crypto_Library_Name := sgx_tcrypto

Enclave_Cpp_Files := Enclave/EnclaveShare.cpp Enclave/Enclave.cpp Enclave/EnclaveAccum.cpp Enclave/EnclaveComb.cpp Enclave/EnclaveFree.cpp Enclave/EnclaveOP.cpp Enclave/EnclaveCh.cpp Enclave/EnclaveChComb.cpp
#Enclave_C_Files := Enclave/ecdsatest.c
Enclave_Include_Paths := -IEnclave -I$(SGX_SDK)/include -I$(SGX_SDK)/include/libcxx -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/stlport -I$(SGXSSL_INCLUDE_PATH)

Enclave_C_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Enclave_Include_Paths) #-include "tsgxsslio.h"
Enclave_Cpp_Flags := $(Enclave_C_Flags) -std=c++11 -nostdinc++

#Security_Link_Flags := -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -pie

Enclave_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles \
	-L$(SGXSSL_TRUSTED_LIB_PATH) -Wl,--whole-archive -lsgx_tsgxssl -Wl,--no-whole-archive -lsgx_tsgxssl_crypto \
	-L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_pthread -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0
# -lsgx_tsetjmp
# AFTER sgx_tsgxssl_crypto? -lpthread
# BEFORE tcxx? -lsgx_uae_service_sim -lsgx_urts_sim -lsgx_pthread

Enclave_Cpp_Objects := $(Enclave_Cpp_Files:.cpp=.o) #$(Enclave_C_Files:.c=.o)
#$(info $$(Enclave_Cpp_Objects) is [${Enclave_Cpp_Objects}])

Enclave_Name := enclave.so
Signed_Enclave_Name := enclave.signed.so
Enclave_Config_File := Enclave/Enclave.config.xml

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif


.PHONY: all run

ifeq ($(Build_Mode), HW_RELEASE)
all: $(App_Name) sgxclient sgxkeys $(Enclave_Name)
	@echo "The project has been built in release hardware mode."
	@echo "Please sign the $(Enclave_Name) first with your signing key before you run the $(App_Name) to launch and access the enclave."
	@echo "To sign the enclave use the command:"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <your key> -enclave $(Enclave_Name) -out <$(Signed_Enclave_Name)> -config $(Enclave_Config_File)"
	@echo "You can also sign the enclave using an external signing tool. See User's Guide for more details."
	@echo "To build the project in simulation mode set SGX_MODE=SIM. To build the project in prerelease mode set SGX_PRERELEASE=1 and SGX_MODE=HW."
else
all: $(App_Name) sgxclient sgxkeys $(Signed_Enclave_Name)
endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/$(App_Name)
	@echo "RUN  =>  $(App_Name) [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif

######## Non-SGX Objects ########

server: App/Server.o $(Nsgx_App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae $(Salticidae_Include_Paths)
	@echo "LINK <= $@"

client: App/Client.o App/Stats.o App/Signs.o App/Sign.o App/Nodes.o App/NodeInfo.o App/KeysFun.o App/Transaction.o # $(Nsgx_App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae $(Salticidae_Include_Paths)
	@echo "LINK <= $@"

keys: App/Keys.o App/KeysFun.o # $(Nsgx_App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae $(Salticidae_Include_Paths)
	@echo "LINK <= $@"


######## App Objects ########

App/Enclave_u.c: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include --search-path $(SGXSSL_INCLUDE_PATH)
	@echo "GEN  =>  $@"

App/Enclave_u.o: App/Enclave_u.c
	@$(CC) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp
	@$(CXX) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/Server.cpp App/Enclave_u.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(App_Link_Flags) $(App_Include_Paths)
	@echo "LINK =>  $@"

sgxclient: App/Client.cpp App/Stats.o App/Signs.o App/Sign.o App/Nodes.o App/NodeInfo.o App/KeysFun.o App/Transaction.o #App/Enclave_u.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(App_Link_Flags) $(App_Include_Paths)
	@echo "LINK <= $@"

sgxkeys: App/Keys.cpp App/KeysFun.o
	@$(CXX) $^ -o $@ $(App_Link_Flags) $(App_Include_Paths)
	@echo "LINK <= $@"


######## Enclave Objects ########

Enclave/Enclave_t.c: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd Enclave && $(SGX_EDGER8R) --trusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include --search-path $(SGXSSL_INCLUDE_PATH)
	@echo "GEN  =>  $@"

Enclave/Enclave_t.o: Enclave/Enclave_t.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave/%.o: Enclave/%.cpp Enclave/Enclave_t.c
	@$(CXX) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

Enclave/%.o: Enclave/%.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC  <=  $<"

$(Enclave_Name): Enclave/Enclave_t.o $(Enclave_Cpp_Objects)
	@echo $(Enclave_Link_Flags)
	@$(CXX) $^ -o $@ $(Enclave_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_Name): $(Enclave_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave/Enclave_private.pem -enclave $(Enclave_Name) -out $@ -config $(Enclave_Config_File)
	@echo "SIGN =>  $@"

.PHONY: clean

clean:
	@rm -f $(App_Name) sgxclient sgxkeys $(Enclave_Name) $(Signed_Enclave_Name) $(App_Cpp_Objects) App/Keys.o App/Client.o App/Server.o App/Enclave_u.* $(Enclave_Cpp_Objects) Enclave/Enclave_t.*
