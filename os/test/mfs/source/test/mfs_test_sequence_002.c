/*
    ChibiOS - Copyright (C) 2006..2017 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#include "mfs_test_root.h"

/**
 * @file    mfs_test_sequence_002.c
 * @brief   Test Sequence 002 code.
 *
 * @page mfs_test_sequence_002 [2] API Invalid Cases tests
 *
 * File: @ref mfs_test_sequence_002.c
 *
 * <h2>Description</h2>
 * This test sequence tests the error coded returned by the various
 * APIs when called when the system is not initialized.
 *
 * <h2>Test Cases</h2>
 * - @subpage mfs_test_002_001
 * - @subpage mfs_test_002_002
 * .
 */

/****************************************************************************
 * Shared code.
 ****************************************************************************/

#include "mfs.h"

/****************************************************************************
 * Test cases.
 ****************************************************************************/

/**
 * @page mfs_test_002_001 [2.1] Initialization error from APIs
 *
 * <h2>Description</h2>
 * The API functions are invoked without prior initialization.
 *
 * <h2>Test Steps</h2>
 * - [2.1.1] The function mfsErase() is called, MFS_ERR_INV_STATE is
 *   expected.
 * - [2.1.2] The function mfsWriteRecord() is called, MFS_ERR_INV_STATE
 *   is expected.
 * - [2.1.3] The function mfsEraseRecord() is called, MFS_ERR_INV_STATE
 *   is expected.
 * - [2.1.4] The function mfsReadRecord() is called, MFS_ERR_INV_STATE
 *   is expected.
 * - [2.1.5] The function mfsPerformGarbageCollection() is called,
 *   MFS_ERR_INV_STATE is expected.
 * .
 */

static void mfs_test_002_001_execute(void) {

  /* [2.1.1] The function mfsErase() is called, MFS_ERR_INV_STATE is
     expected.*/
  test_set_step(1);
  {
    mfs_error_t err = mfsErase(&mfs1);
    test_assert(err == MFS_ERR_INV_STATE, "mfsErase() returned wrong status");
  }

  /* [2.1.2] The function mfsWriteRecord() is called, MFS_ERR_INV_STATE
     is expected.*/
  test_set_step(2);
  {
    mfs_error_t err = mfsWriteRecord(&mfs1, 1, 16, mfs_buffer);
    test_assert(err == MFS_ERR_INV_STATE, "mfsWriteRecord() returned wrong status");
  }

  /* [2.1.3] The function mfsEraseRecord() is called, MFS_ERR_INV_STATE
     is expected.*/
  test_set_step(3);
  {
    mfs_error_t err = mfsEraseRecord(&mfs1, 1);
    test_assert(err == MFS_ERR_INV_STATE, "mfsEraseRecord() returned wrong status");
  }

  /* [2.1.4] The function mfsReadRecord() is called, MFS_ERR_INV_STATE
     is expected.*/
  test_set_step(4);
  {
    size_t size = sizeof mfs_buffer;
    mfs_error_t err = mfsReadRecord(&mfs1, 1, &size, mfs_buffer);
    test_assert(err == MFS_ERR_INV_STATE, "mfsReadRecord() returned wrong status");
  }

  /* [2.1.5] The function mfsPerformGarbageCollection() is called,
     MFS_ERR_INV_STATE is expected.*/
  test_set_step(5);
  {
    mfs_error_t err = mfsPerformGarbageCollection(&mfs1);
    test_assert(err == MFS_ERR_INV_STATE, "mfsPerformGarbageCollection() returned wrong status");
  }
}

static const testcase_t mfs_test_002_001 = {
  "Initialization error from APIs",
  NULL,
  NULL,
  mfs_test_002_001_execute
};

/**
 * @page mfs_test_002_002 [2.2] Erasing non existing record
 *
 * <h2>Description</h2>
 * An erase operation is attempted on an non-existing record.
 *
 * <h2>Test Steps</h2>
 * - [2.2.1] Record one is erased, the error MFS_ERR_NOT_FOUND is
 *   expected.
 * .
 */

static void mfs_test_002_002_setup(void) {
  mfsStart(&mfs1, &mfscfg1);
  mfsErase(&mfs1);
}

static void mfs_test_002_002_teardown(void) {
  mfsStop(&mfs1);
}

static void mfs_test_002_002_execute(void) {

  /* [2.2.1] Record one is erased, the error MFS_ERR_NOT_FOUND is
     expected.*/
  test_set_step(1);
  {
    mfs_error_t err;

    err = mfsEraseRecord(&mfs1, 1);
    test_assert(err != MFS_NO_ERROR, "record was present");
    test_assert(err == MFS_ERR_NOT_FOUND, "invalid error code");
  }
}

static const testcase_t mfs_test_002_002 = {
  "Erasing non existing record",
  mfs_test_002_002_setup,
  mfs_test_002_002_teardown,
  mfs_test_002_002_execute
};

/****************************************************************************
 * Exported data.
 ****************************************************************************/

/**
 * @brief   Array of test cases.
 */
const testcase_t * const mfs_test_sequence_002_array[] = {
  &mfs_test_002_001,
  &mfs_test_002_002,
  NULL
};

/**
 * @brief   API Invalid Cases tests.
 */
const testsequence_t mfs_test_sequence_002 = {
  "API Invalid Cases tests",
  mfs_test_sequence_002_array
};
