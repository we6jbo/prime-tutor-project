#!/usr/bin/env bash
set -u
OUT="${1:-$HOME/t14-technical-scan-$(date +%Y%m%d-%H%M%S).txt}"
{
  echo "T14 Technical Setup Scan"
  echo "Generated: $(date --iso-8601=seconds)"
  echo
  echo "== OS =="; cat /etc/os-release 2>/dev/null || true
  echo; echo "== Kernel =="; uname -a
  echo; echo "== CPU =="; lscpu 2>/dev/null | sed -n '1,25p'
  echo; echo "== Memory =="; free -h
  echo; echo "== Storage =="; lsblk -o NAME,SIZE,FSTYPE,MOUNTPOINTS,MODEL
  echo; echo "== Graphics =="; lspci -nnk 2>/dev/null | grep -A3 -Ei 'VGA|3D|Display' || true
  echo; echo "== Network =="; lspci -nnk 2>/dev/null | grep -A3 -Ei 'Ethernet|Network' || true
  echo; echo "== Desktop/Session =="; printf 'XDG_CURRENT_DESKTOP=%s\nXDG_SESSION_TYPE=%s\n' "${XDG_CURRENT_DESKTOP:-}" "${XDG_SESSION_TYPE:-}"
  echo; echo "== Development tools =="
  for c in gcc g++ cmake qmake6 qmake git gh hf python; do command -v "$c" >/dev/null 2>&1 && "$c" --version 2>&1 | head -n 2 || true; done
  echo; echo "== Official package snapshots =="; pacman -Q firefox cmake gcc qt6-base qt5-base git 2>/dev/null || true
  echo; echo "== Context file =="
  CTX="${WE6JBO_CONTEXT_FILE:-$HOME/.local/state/we6jbo-context/context.json}"
  echo "$CTX"; [ -f "$CTX" ] && cat "$CTX" || echo "not found"
  echo; echo "== Recovery-relevant files (read-only listing) =="
  ls -ld "$HOME/swatcher" "$HOME/swatcher/recovery" "$HOME/swatcher/aug30-2026-backup" 2>/dev/null || true
  echo; echo "NOTE: This scan is read-only and does not collect passwords, browser credentials, tokens, or private keys."
} | tee "$OUT"
echo "Saved: $OUT"
