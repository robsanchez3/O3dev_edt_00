#!/usr/bin/env bash
# ---------------------------------------------------------------------------
#  generate_update_package.sh
#  Generates app_int.bin, app_ospi.bin and manifest.ini from the ELF.
#  manifest_crc32 is computed over the manifest itself (placeholder zeroed).
# ---------------------------------------------------------------------------
set -euo pipefail

# ========================== CONFIGURATION ==================================
PRODUCT="O3 interface"
HW_REVISION="edt EVK070027B"
# SW_VERSION and O3_LIB_VERSION are extracted from the ELF (see main.c / fsm_o3_operation.c)
# ===========================================================================

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUT_DIR="$ROOT_DIR/Update_package"
ELF_PATH="$ROOT_DIR/STM32CubeIDE/Debug/O3dev_edt_00.elf"
INT_BIN="$OUT_DIR/app_int.bin"
OSPI_BIN="$OUT_DIR/app_ospi.bin"
MANIFEST="$OUT_DIR/manifest.ini"
OSPI_SECTION="ExtFlashSection"

mkdir -p "$OUT_DIR"

# ---------- tool helpers ---------------------------------------------------
find_objcopy() {
  if command -v arm-none-eabi-objcopy >/dev/null 2>&1; then
    command -v arm-none-eabi-objcopy
    return 0
  fi
  find /mnt/c/ST /mnt/c/TouchGFX -type f -name arm-none-eabi-objcopy.exe 2>/dev/null | head -n 1
}

calc_crc32() {
  python3 -c "
import pathlib, zlib, sys
p = pathlib.Path(sys.argv[1])
print(f'{zlib.crc32(p.read_bytes()) & 0xffffffff:08X}')
" "$1"
}

# ---------- pre-checks -----------------------------------------------------
if [[ ! -f "$ELF_PATH" ]]; then
  echo "ERROR: ELF not found: $ELF_PATH" >&2
  exit 1
fi

OBJCOPY="$(find_objcopy)"
if [[ -z "$OBJCOPY" ]]; then
  echo "ERROR: arm-none-eabi-objcopy not found" >&2
  exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
  echo "ERROR: python3 not found in WSL PATH" >&2
  exit 1
fi

# ---------- extract binaries -----------------------------------------------
ELF_WIN="$(wslpath -w "$ELF_PATH")"
INT_WIN="$(wslpath -w "$INT_BIN")"
OSPI_WIN="$(wslpath -w "$OSPI_BIN")"

echo "Using objcopy: $OBJCOPY"

"$OBJCOPY" -O binary --remove-section="$OSPI_SECTION" "$ELF_WIN" "$INT_WIN"
"$OBJCOPY" -O binary --only-section="$OSPI_SECTION"   "$ELF_WIN" "$OSPI_WIN"

# ---------- extract versions from ELF --------------------------------------
extract_tag() {
  strings "$ELF_PATH" | grep -oP "##${1}=\\K[^#]+" || true
}

SW_VERSION=$(extract_tag "SW_VERSION")
O3_LIB_VERSION=$(extract_tag "O3_LIB_VERSION")
BUILD_DATE=$(extract_tag "BUILD_DATE" | tr -s ' ')
BUILD_TIME=$(extract_tag "BUILD_TIME")

if [[ -z "$SW_VERSION" ]]; then
  echo "ERROR: ##SW_VERSION=...## not found in ELF" >&2
  exit 1
fi
if [[ -z "$O3_LIB_VERSION" ]]; then
  echo "ERROR: ##O3_LIB_VERSION=...## not found in ELF" >&2
  exit 1
fi
if [[ -z "$BUILD_DATE" ]]; then
  echo "ERROR: ##BUILD_DATE=...## not found in ELF" >&2
  exit 1
fi
if [[ -z "$BUILD_TIME" ]]; then
  echo "ERROR: ##BUILD_TIME=...## not found in ELF" >&2
  exit 1
fi

echo "SW_VERSION:      $SW_VERSION"
echo "O3_LIB_VERSION:  $O3_LIB_VERSION"
echo "BUILD_DATE:      $BUILD_DATE"
echo "BUILD_TIME:      $BUILD_TIME"

# ---------- collect metadata -----------------------------------------------
INT_SIZE=$(stat -c%s "$INT_BIN")
OSPI_SIZE=$(stat -c%s "$OSPI_BIN")
INT_CRC=$(calc_crc32 "$INT_BIN")
OSPI_CRC=$(calc_crc32 "$OSPI_BIN")

# ---------- write manifest with placeholder --------------------------------
CRC_PLACEHOLDER="00000000"

cat > "$MANIFEST" <<EOF
[manifest]
product=${PRODUCT}
hw_revision=${HW_REVISION}
sw_version=${SW_VERSION}
o3_lib_version=${O3_LIB_VERSION}
build_date=${BUILD_DATE} ${BUILD_TIME}

[app_int]
filename=app_int.bin
size=${INT_SIZE}
crc32=${INT_CRC}

[app_ospi]
filename=app_ospi.bin
size=${OSPI_SIZE}
crc32=${OSPI_CRC}

[integrity]
manifest_crc32=${CRC_PLACEHOLDER}
EOF

# ---------- compute manifest CRC over content before [integrity] -----------
MANIFEST_CRC=$(python3 -c "
import zlib, sys
data = open(sys.argv[1], 'rb').read()
marker = b'[integrity]'
pos = data.find(marker)
if pos < 0:
    sys.exit('ERROR: [integrity] section not found in manifest')
print(f'{zlib.crc32(data[:pos]) & 0xffffffff:08X}')
" "$MANIFEST")
sed -i "s/manifest_crc32=${CRC_PLACEHOLDER}/manifest_crc32=${MANIFEST_CRC}/" "$MANIFEST"

# ---------- summary --------------------------------------------------------
echo ""
echo "Generated:"
echo "  $INT_BIN  (${INT_SIZE} bytes, CRC32=${INT_CRC})"
echo "  $OSPI_BIN (${OSPI_SIZE} bytes, CRC32=${OSPI_CRC})"
echo "  $MANIFEST (self-CRC32=${MANIFEST_CRC})"
