# Prime Number Tutor

Qt/C++ GUI for Manjaro that teaches prime numbers.

Features:
- Test numbers and explain prime/composite results.
- Ctrl++ / Ctrl+= zoom in, Ctrl+- zoom out, Ctrl+0 reset.
- Config tab with font family, base font size, compact layout, and reset-to-default behavior.
- Persistent layout file: `$HOME/.primenumber_layout.ini`.
- Reads `WE6JBO_CONTEXT_FILE` when set, otherwise `~/.local/state/we6jbo-context/context.json`.
- Honors `time.visible`; it shows time only when the context allows it.
- Portable `tg_context_snapshot.json` with TG654147.
- Attempts one TG project registration when `tg-register-project` exists, but does not depend on it.
- Includes `scan-t14-setup.sh`, a read-only technical scan that intentionally excludes passwords/tokens/private keys.

## Manjaro dependencies
Prefer official repositories:

```bash
sudo pacman -S --needed base-devel cmake qt6-base
```

If Qt 6 is unavailable but Qt 5 is already installed, the CMake file can also build against Qt 5.

## Build and run

```bash
cd prime_tutor_project
./build-and-run.sh
```

## Technical scan

```bash
./scan-t14-setup.sh
```

## Verification

```bash
test -f tg_context_snapshot.json && grep -n 'TG654147' tg_context_snapshot.json
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
test -x build/prime-number-tutor && echo PASS
```

## Project purpose\n\nhttps://j03.page/2026/09/01/building-software-from-scratch/

## License\n\nLicensed under the Apache License 2.0 (`Apache-2.0`). See `LICENSE`.

## Reuse and discoverability\n\nSee `AI_REUSE.md` for machine-readable and human-readable guidance about lawful reuse by developers, research tools, coding assistants, and future AI systems.
