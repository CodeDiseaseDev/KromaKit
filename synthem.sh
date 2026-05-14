#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$SCRIPT_DIR/scripts/lib/paths.sh"
exec "$SCRIPT_DIR/scripts/synthem.sh" "$@"
