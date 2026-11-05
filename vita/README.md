# Neon Brawl — PS Vita native port

This directory is the native PS Vita port work area. The original browser game remains at the repository root.

## Status

**Prototype scaffold only — not a playable port and no VPK has been produced yet.** The browser game's JavaScript, rendering, input, audio, and game loop still need to be ported to native code.

## Build direction

Target: homebrew PS Vita using VitaSDK and SDL2 (`vitaSDL2`). Build on GitHub Actions so a computer is not required locally. The first milestone is a bootable SDL2 app; later milestones will port gameplay and assets, then package a VPK.

Do not treat a successful scaffold build as proof that the game itself is ported.