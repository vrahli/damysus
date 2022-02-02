#include <stdio.h>
#include <iostream>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"

#include "App.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

// OCall implementations
void ocall_print(const char* str) {
    printf("%s\n", str);
}

int initialize() {
  if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
    std::cout << "Fail to initialize enclave." << std::endl;
    return 1;
  }
  return 0;
}

int generate_rnum() {
  if (initialize() == 1) { return 1; }
  int ptr;
  sgx_status_t status = generate_random_number(global_eid, &ptr);
  std::cout << status << std::endl;
  if (status != SGX_SUCCESS) {
    std::cout << "noob" << std::endl;
  }
  printf("Random number: %d\n", ptr);

  return 0;
}

int main(int argc, char const *argv[]) {
  return generate_rnum();
}
