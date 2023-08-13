#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>

int gnaro_suite_init(void) { return 0; }
int gnaro_suite_clean(void) { return 0; }
void gnaro_test(void) { CU_ASSERT(1 == 1); }

// The main() function for setting up and running the tests.
// Returns a CUE_SUCCESS on successful running, another
// CUnit error code on failure.
int main(void) {
  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  // Add a suite to the registry
  pSuite = CU_add_suite("gnaro_suite", gnaro_suite_init, gnaro_suite_clean);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Add the tests to the suite
  if ((NULL == CU_add_test(pSuite, "test of gnaro", gnaro_test))) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Run all tests using the CUnit Basic interface
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
