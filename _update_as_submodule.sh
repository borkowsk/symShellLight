#!/bin/bash
# @date 2026-04-29 (last modification)
# ====================================
# Ten skrypt aktualizuje SymShellLight zainstalowany jako submodul innego repo.

cd ..
git submodule update --remote --merge --recursive
git add .
git commit -m "Aktualizacja submodułu do najnowszej wersji"
git push
