#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
ELF_PATH="$ROOT_DIR/STM32CubeIDE/Debug/O3dev_edt_00.elf"
INT_BIN_PATH="$ROOT_DIR/app_int.bin"
INT_CRC_PATH="$ROOT_DIR/app_int.crc"
OSPI_BIN_PATH="$ROOT_DIR/app_ospi.bin"
OSPI_CRC_PATH="$ROOT_DIR/app_ospi.crc"
OSPI_SECTION="ExtFlashSection"

if [[ ! -f "$ELF_PATH" ]]; then
  echo "ERROR: ELF not found: $ELF_PATH" >&2
  exit 1
fi

find_objcopy() {
  if command -v arm-none-eabi-objcopy >/dev/null 2>&1; then
    command -v arm-none-eabi-objcopy
    return 0
  fi

  find /mnt/c/ST /mnt/c/TouchGFX -type f -name arm-none-eabi-objcopy.exe 2>/dev/null | head -n 1
}

calc_crc32() {
  local path="$1"
  python3 - <<PY
import pathlib, zlib
p = pathlib.Path(r'''$path''')
print(f"{zlib.crc32(p.read_bytes()) & 0xffffffff:08X}")
PY
}

OBJCOPY="$(find_objcopy)"
if [[ -z "$OBJCOPY" ]]; then
  echo "ERROR: arm-none-eabi-objcopy not found in PATH, /mnt/c/ST, or /mnt/c/TouchGFX" >&2
  exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
  echo "ERROR: python3 not found in WSL PATH" >&2
  exit 1
fi

ELF_PATH_WIN="$(wslpath -w "$ELF_PATH")"
INT_BIN_PATH_WIN="$(wslpath -w "$INT_BIN_PATH")"
OSPI_BIN_PATH_WIN="$(wslpath -w "$OSPI_BIN_PATH")"

echo "Using objcopy:"
echo "  $OBJCOPY"

"$OBJCOPY" -O binary --remove-section="$OSPI_SECTION" "$ELF_PATH_WIN" "$INT_BIN_PATH_WIN"
"$OBJCOPY" -O binary --only-section="$OSPI_SECTION" "$ELF_PATH_WIN" "$OSPI_BIN_PATH_WIN"

INT_CRC_VALUE="$(calc_crc32 "$INT_BIN_PATH")"
OSPI_CRC_VALUE="$(calc_crc32 "$OSPI_BIN_PATH")"

printf '%s\n' "$INT_CRC_VALUE" > "$INT_CRC_PATH"
printf '%s\n' "$OSPI_CRC_VALUE" > "$OSPI_CRC_PATH"

echo "Generated:"
echo "  $INT_BIN_PATH"
echo "  $INT_CRC_PATH"
echo "  $OSPI_BIN_PATH"
echo "  $OSPI_CRC_PATH"
echo "CRC32 internal: $INT_CRC_VALUE"
echo "CRC32 OSPI:     $OSPI_CRC_VALUE"
