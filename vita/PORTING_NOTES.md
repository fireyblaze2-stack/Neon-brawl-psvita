# Neon Brawl PS Vita port notes

## Source of truth
- The original game is the root `index.html` (HTML Canvas, 800x400 logical resolution).
- Keep that file unchanged as the reference implementation.
- `src/main.c` is only an SDL2 boot/input/rendering scaffold. It is not the original game.

## Confirmed original-game systems
- States: title, play, shop, talk/story, customization, pause, versus, defense, and online lobby.
- Player movement: four-direction movement with depth-lane limits, facing, jumping, attack/hurt/down states, invulnerability, and lives.
- Combat: melee strikes, charge attacks, weapons, ranged projectiles, blocking, knockback, and hit effects.
- Enemies: multiple archetypes with different health, speed, damage, weapons, bosses, and special behaviors.
- Progression: waves, drops, cash, weapon slots, shop, score, and upgrades.
- Rendering: layered city background, depth-sorted fighters, character/weapon art, HUD, menus, and effects.
- Input: keyboard, browser gamepads, and touch controls.

## Native port milestones
1. Replace the placeholder fighter demo with the original game's title/menu and a native Vita control layer.
2. Port one-player movement, depth positioning, jump, and character drawing.
3. Port enemy spawning/AI, health, melee combat, knockback, and defeat/revive behavior.
4. Port wave progression, drops, weapons, projectiles, and HUD.
5. Port shop, customization, versus/defense modes where feasible; assess browser-only online features separately.
6. Test on Vita hardware and package a clearly named release VPK.

## Porting rules
- Do not call a scaffold or test scene a finished port.
- Do not replace original mechanics with unrelated placeholder gameplay.
- Keep each milestone buildable; verify on hardware before calling it complete.
- The browser-only networking/audio/touch code needs native equivalents or explicit scope decisions; it cannot be copied directly into SDL2 C.
