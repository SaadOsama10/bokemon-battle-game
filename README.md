# ⚔️ Bokemon — Turn-Based Battle Game

**A two-player, turn-based terminal battle game in C. Teams of six fight using type effectiveness, physical/special stats and STAB, with data loaded from 1,000+ creatures and 486 moves.**

<p>
  <img src="https://img.shields.io/badge/C-1a1b27?style=flat-square&logo=c&logoColor=7aa2f7" alt="C" />
  <img src="https://img.shields.io/badge/CLI-1a1b27?style=flat-square" alt="CLI" />
</p>

Author: Saed O S Radi

> A **Pokémon-inspired fan project for a university course (non-commercial).** Pokémon names and stats in the data files belong to their respective owners.

---

## Overview

Two players share one terminal. At startup the game reads three data files:

| File | Contents |
|---|---|
| `types.txt` | The 18 types and the 18×18 type-effectiveness chart |
| `moves.txt` | 486 moves: name, type, category (Physical / Special) and power |
| `pokemon.txt` | 1,015 creatures: name, one or two types (`-` = none), HP, Attack, Defense, Sp. Atk, Sp. Def, Speed |

Each player gets a random team of **six**, and every creature gets **four random, unique moves**. Players then fight round by round until one side has no creatures left.

## How to Play

Each round shows both active creatures and their HP, then:

1. **Each player chooses:** `1` Attack or `2` Change Pokémon. Changing is only allowed if another team member is still standing.
2. **Follow-up:**
   - **Attack:** pick one of the active creature's four moves (`1`–`4`).
   - **Change:** pick a team member (`1`–`6`). Fainted and already-active members are rejected. **Switching uses up your turn:** you don't attack that round, and the incoming creature can still be hit.
3. **Resolution:** the faster creature (higher **Speed**) attacks first; Player 1 goes first on a tie. A creature knocked out before its turn doesn't attack.
4. **Fainting:** at the end of the round, a fainted creature is automatically replaced by the next healthy team member.
5. The game ends when one player has no healthy creatures left: **Winner: Player N**, or **Draw** if both sides run out at once.

Invalid input (letters, out-of-range numbers) is rejected and the prompt is repeated.

### Damage formula

```
damage = power × (Attack / Defense) × type1 × type2 × STAB
```

- **Physical** moves use the attacker's *Attack* vs the defender's *Defense*; **Special** moves use *Sp. Atk* vs *Sp. Def*.
- `type1` and `type2` are the move type's effectiveness against each of the defender's types, from `types.txt`.
- **STAB** (same-type attack bonus) is ×1.5 when the move's type matches one of the attacker's types.
- The result is truncated to an integer, and HP never drops below 0.

## Program Design

This is a **procedural C** program; it uses no object-oriented features. The code is split by responsibility:

| File | Responsibility |
|---|---|
| `structs.h` | All data types and function prototypes |
| `init.c` | Loading `types.txt`, `moves.txt` and `pokemon.txt`; building the two random teams |
| `battle.c` | Type effectiveness, STAB, damage, input handling, round logic, fainting/auto-switch, win check |
| `main.c` | Allocates the data tables, then calls `initialize()` and `game()` |

**Data model** (`structs.h`):

- `TypeEffect`: an attacking type, a defending type and a multiplier.
- `Type`: a type name plus its row of the effectiveness chart.
- `Move`: name, `Type`, `Category` (an `enum`: `PHYSICAL` / `SPECIAL`) and power.
- `Pokemon`: name, two `Type`s, HP/current HP, the five battle stats and four `Move`s.
- `Player`: name, a team of six `Pokemon` and the index of the active one.

The type, move and creature tables (about 14 MB, since every move and creature stores full copies of its types) are allocated on the heap with `malloc` in `main.c`. Players hold **copies** of the creatures they draw, so HP changes in battle never touch the master table.

## Build & Run

Requires a C17 compiler (`clang` or `gcc`). Run the game from the project folder, because the data files are opened by relative path.

```bash
cc -std=c17 -Wall -Wextra -O2 -o bokemon main.c battle.c init.c
./bokemon
```

It compiles without warnings under `-Wall -Wextra` with Apple clang 21.

## Sample Output

An excerpt from a scripted 10-round battle (player inputs follow each `>` and `:` prompt). The full transcript is in [`docs/sample-battle.txt`](docs/sample-battle.txt).

```text
Initialization done.
Player 1 starts with Vivillon
Player 2 starts with Raticate

===== GAME START =====

================ NEW ROUND ================
Player 1 active: Vivillon HP=80/80
Player 2 active: Raticate HP=55/55

Player 1: 1-Attack  2-Change Pokemon
> 1

Player 1: 1-Attack  2-Change Pokemon
> 1

Player 1 choose a move:
1 - Pluck  2 - DragonAscent  
3 - SplishySplash  4 - FleurCannon  
Please select a move (1-4): 1

Player 2 choose a move:
1 - MudShot  2 - Eternabeam  
3 - GravApple  4 - ChargeBeam  
Please select a move (1-4): 3

--- Applying damage ---
Raticate used GravApple! Damage: 32
Vivillon used Pluck! Damage: 78
Raticate fainted!

End of round:
Player 1 active: Vivillon HP=48/80
Player 2 fainted: Raticate HP=0/55

================ NEW ROUND ================
Player 1 active: Vivillon HP=48/80
Player 2 active: Vivillon HP=80/80

Player 1: 1-Attack  2-Change Pokemon
> 2

Player 1: 1-Attack  2-Change Pokemon
> 1

Player 1 choose a Pokemon to switch:
1 - Vivillon (active)  2 - Flareon  
3 - WalkingWake  4 - Azelf  
5 - Lairon  6 - Klefki  
Please select a Pokemon (1-6): 2

Player 2 choose a move:
1 - Pluck  2 - DragonAscent  
3 - SplishySplash  4 - FleurCannon  
Please select a move (1-4): 4

--- Applying damage ---
Vivillon used FleurCannon! Damage: 53

End of round:
Player 1 active: Flareon HP=12/65
Player 2 active: Vivillon HP=80/80

[... 7 more rounds ...]

--- Applying damage ---
WalkingWake used GrassPledge! Damage: 125
Staravia fainted!

End of round:
Player 1 active: WalkingWake HP=55/99
Player 2 fainted: Staravia HP=0/55

===== GAME OVER =====
Winner: Player 1
```

In round 1, Raticate is faster and attacks first. In round 2, Player 1 switches to Flareon instead of attacking, and the incoming Flareon takes the hit.

## Known Limitations

- **Second player's prompt is mislabelled.** Both "Attack / Change" prompts say "Player 1"; the second one is actually Player 2's (visible in the sample above).
- **Teams can repeat creatures.** Teams are drawn at random without a uniqueness check, so a creature can appear twice in one team or on both teams. Moves are assigned per creature at startup, so a repeated creature also has the same four moves (both players had a Vivillon in the sample).
- **No team selection.** Teams are always random; players can't choose their creatures.
