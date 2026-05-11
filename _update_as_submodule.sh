#!/bin/bash
# @date 2026-05-11 (last modification)
# ====================================
# Ten skrypt aktualizuje SymShellLight zainstalowany jako submodul innego repo.
set -e
cd ..
git submodule update --remote --merge --recursive
git add .
git commit -m "Aktualizacja submodułu do najnowszej wersji"
git push
