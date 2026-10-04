#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*example run for effectiveness function 
effectiveness(&FireType, "Grass")
Loop checks:
i=0: compare "Normal" with "Grass" → no

i=1: "Fire" vs "Grass" → no

i=2: "Water" vs "Grass" → no

i=3: "Grass" vs "Grass" → yes → return 2.0*/
double effectiveness(Type *atkType, char *defTypeName) {
    for (int i = 0; i < 19; i++) {
        if (strcmp(atkType->effects[i].defName, defTypeName) == 0) {
            return atkType->effects[i].multiplier;
        }
    }
    return 1.0;
}
double STAB(Pokemon *attacker, Move *move) {
    if (strcmp(attacker->types[0].name, move->type.name) == 0)
        return 1.5;
    if (strcmp(attacker->types[1].name, move->type.name) == 0)
        return 1.5;
    return 1.0;
}
void attack(Pokemon *attacker, Pokemon *defender, int moveIndex) {
    Move *mv = &attacker->moves[moveIndex];
    
    if (mv->power <= 0) return; // power 0 = no damage (safe)

    double atkStat, defStat;

    if (mv->category == PHYSICAL) {
        atkStat = (double)attacker->attack;
        defStat = (double)defender->defense;
    } else {
        atkStat = (double)attacker->spAtk;
        defStat = (double)defender->spDef;
    }

    if (defStat <= 0.0) defStat = 1.0; // safety

    double type1 = effectiveness(&mv->type, defender->types[0].name);
    double type2 = effectiveness(&mv->type, defender->types[1].name);
    double stab  = STAB(attacker, mv);

    double damageDouble = (double)mv->power * (atkStat / defStat) * type1 * type2 * stab;

    int damage = (int)damageDouble;   // you can also round if you want, but int cast is fine

    defender->currentHP -= damage;
    printf("%s used %s! Damage: %d\n", attacker->name, mv->name, damage);
    if (defender->currentHP < 0) 
    defender->currentHP = 0;
    if (defender->currentHP == 0) {
        printf("%s fainted!\n", defender->name);
    }
}
//getting the index of the next alive pokemon 
int nextAliveIndex(Player *p) {
    int startingIndex = p->currentIndex;
    for (int step = 1; step <= 6; step++) {
        int i = (startingIndex + step) % 6;
        if (p->Pokemons[i].currentHP > 0)
            return i;
    }
    //return p->currentIndex;
    return -1;
}

void applyDamage(Player *p1, Player *p2, int move1, int move2){
    Pokemon *a = &p1->Pokemons[p1->currentIndex];
    Pokemon *b = &p2->Pokemons[p2->currentIndex];

    /* Decide who attacks first by speed 
        even if the speeds are equal player 1 will attack. */
    int p1First = (a->speed >= b->speed);

    /* If a player switched, they do not attack this round */
    int p1CanAttack = (move1 >= 0);
    int p2CanAttack = (move2 >= 0);

    if (p1First) {
        if (p1CanAttack && a->currentHP > 0 && b->currentHP > 0) {
            attack(a, b, move1);
        }
        /* If defender fainted, they can't attack back */
        if (b->currentHP > 0) {
            if (p2CanAttack && b->currentHP > 0 && a->currentHP > 0) {
                attack(b, a, move2);
            }
        }
    } else {
        if (p2CanAttack && b->currentHP > 0 && a->currentHP > 0) {
            attack(b, a, move2);
        }
        if (a->currentHP > 0) {
            if (p1CanAttack && a->currentHP > 0 && b->currentHP > 0) {
                attack(a, b, move1);
            }
        }
    }

    // /* Auto-switch if someone fainted (after the round) */
    // if (p1->Pokemons[p1->currentIndex].currentHP == 0) {
    //     //p1->currentIndex = nextAliveIndex(p1);
    //     int idx = nextAliveIndex(p1);
    //     if (idx != -1) {
    //         p1->currentIndex = idx;
    //     }
    // }
    // if (p2->Pokemons[p2->currentIndex].currentHP == 0) {
    //     //p2->currentIndex = nextAliveIndex(p2);
    //     int idx = nextAliveIndex(p2);
    //     if (idx != -1) {
    //         p2->currentIndex = idx;
    //     }
    // }

}
///printing thre current active pokemon
void printActive(Player *p) {
    Pokemon *pokemon = &p->Pokemons[p->currentIndex];
    printf("%s active: %s HP=%d/%d\n",
           p->name, pokemon->name, pokemon->currentHP, pokemon->maxHP);
}
// printging the moves of the pokemon
void printMoves(Pokemon *pokemon) {
    for (int i = 0; i < 4; i++) {
        printf("%d - %s  ", i + 1, pokemon->moves[i].name);
        if (i == 1) printf("\n");
    }
    printf("\n");
}
// this function prints the player's pokemons and shows if there are fainted ones
void printTeamChoices(Player *p) {
    for (int i = 0; i < 6; i++) {
        printf("%d - %s", i + 1, p->Pokemons[i].name);
        if (p->Pokemons[i].currentHP <= 0) 
        printf(" (fainted)");
        if (i == p->currentIndex) 
        printf(" (active)");

        printf("  ");

        if (i == 1 || i == 3) 
        printf("\n");
    }
    printf("\n");
}
// this function checks if the player has any other pokemons alive other than the current one 
int hasSwitchOption(Player *p) {
    for (int i = 0; i < 6; i++) {
        if (i != p->currentIndex && p->Pokemons[i].currentHP > 0) {
            return 1; // there is a valid switch target
        }
    }
    return 0; // no one else alive
}
//this function runs 1 round of the game 
void playRound(Player *p1, Player *p2){
    int p1Choice = 0, p2Choice = 0;
    int p1MoveIndex = -1; /* -1 = no attack (switched) */
    int p2MoveIndex = -1;

    printf("\n================ NEW ROUND ================\n");
    printActive(p1);
    printActive(p2);

    int p1CanSwitch = hasSwitchOption(p1);
    int p2CanSwitch = hasSwitchOption(p2);
    /* --- First choices: Attack or Change --- */
    /* ---------- Player 1 main choice ---------- */
    while (1) {
        printf("\nPlayer 1: 1-Attack  2-Change Pokemon\n> ");

        if (scanf("%d", &p1Choice) != 1) {//If user types abc, it returns 0, not 1 → invalid input.
            printf("Invalid input.\n");
            while (getchar() != '\n') {}//Clears the input buffer until newline
            continue;//restart the loop
        }
            // if (p1Choice == 1) 
            // break;
            // if (p1Choice == 2 && p1CanSwitch) 
            // break;
        if (p1Choice < 1 || p1Choice > 2) {
            printf("Please enter 1 or 2.\n");
            continue;
        }
        if (p1Choice == 2 && !p1CanSwitch) {
            printf("No other Pokemon to switch to.\n");
            continue;
        }
        break; // valid choice
        // printf("No other pokemon to switch to.\n");
    }
    /* ---------- Player 2 main choice ---------- */
    while (1) {
        printf("\nPlayer 1: 1-Attack  2-Change Pokemon\n> ");
        
        if (scanf("%d", &p2Choice) != 1) {//If user types abc, it returns 0, not 1 → invalid input.s
            printf("Invalid input.\n");
            while (getchar() != '\n') {}//Clears the input buffer until newline
            continue;//restart the loop
        }
        if (p2Choice < 1 || p2Choice > 2) {
            printf("Please enter 1 or 2.\n");
            continue;
        }
        if (p2Choice == 2 && !p2CanSwitch) {
            printf("No other Pokemon to switch to.\n");
            continue;
        }
        break;
    }
    /* --- Second choices depending on first choice --- */
    /* Player 1 */
    /* ---------- Player 1 follow-up ---------- */
    if (p1Choice == 1) {
        while (1) {
            printf("\n%s choose a move:\n", p1->name);
            printMoves(&p1->Pokemons[p1->currentIndex]);
            printf("Please select a move (1-4): ");

            if (scanf("%d", &p1MoveIndex) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n') {}
                continue;
            }
            if (p1MoveIndex >= 1 && p1MoveIndex <= 4) {
                p1MoveIndex--; /* convert to 0-based */
                break;
            }
            printf("Please select between 1 and 4.\n");
        }
    } else {
        while (1) {
            int pick;
            printf("\n%s choose a Pokemon to switch:\n", p1->name);
            printTeamChoices(p1);
            printf("Please select a Pokemon (1-6): ");

            if (scanf("%d", &pick) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n') {}
                continue;
            }
            pick--; /* convert to 0-based */

            if (pick < 0 || pick > 5) {
                printf("Please select between 1 and 6.\n");
                continue;
            }
            if (p1->Pokemons[pick].currentHP <= 0) {
                printf("That Pokemon is fainted.\n");
                continue;
            }
            if (pick == p1->currentIndex) {
                printf("That Pokemon is already active.\n");
                continue;
            }

            p1->currentIndex = pick;//it switches the pokemon by changing it's current index to other pokemon's index
            break;
        }
        p1MoveIndex = -1;//P1 switched so they cant attack this round
    }
    /* Player 2 */
    /* ---------- Player 2 follow-up ---------- */
    if (p2Choice == 1) {
        while (1) {
            printf("\n%s choose a move:\n", p2->name);
            printMoves(&p2->Pokemons[p2->currentIndex]);
            printf("Please select a move (1-4): ");

            if (scanf("%d", &p2MoveIndex) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n') {}
                continue;
            }
            if (p2MoveIndex >= 1 && p2MoveIndex <= 4) {
                p2MoveIndex--;
                break;
            }
            printf("Please select between 1 and 4.\n");
        }
    } else {
        while (1) {
            int pick;
            printf("\n%s choose a Pokemon to switch:\n", p2->name);
            printTeamChoices(p2);
            printf("Please select a Pokemon (1-6): ");

            if (scanf("%d", &pick) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n') {}
                continue;
            }
            pick--;

            if (pick < 0 || pick >= 6) {
                printf("Please select between 1 and 6.\n");
                continue;
            }
            if (p2->Pokemons[pick].currentHP <= 0) {
                printf("That Pokemon is fainted.\n");
                continue;
            }
            if (pick == p2->currentIndex) {
                printf("That Pokemon is already active.\n");
                continue;
            }

            p2->currentIndex = pick;
            break;
        }
        p2MoveIndex = -1;
    }
    //apply damage
    printf("\n--- Applying damage ---\n");
    applyDamage(p1, p2, p1MoveIndex, p2MoveIndex);
    //end of round
    printf("\nEnd of round:\n");
    // print end-of-round status without switching to next pokemon yet 
    Pokemon *p1pokemon = &p1->Pokemons[p1->currentIndex];
    Pokemon *p2pokemon = &p2->Pokemons[p2->currentIndex];

    if (p1pokemon->currentHP > 0)
        printf("%s active: %s HP=%d/%d\n", p1->name, p1pokemon->name, p1pokemon->currentHP, p1pokemon->maxHP);
    else
        printf("%s fainted: %s HP=%d/%d\n", p1->name, p1pokemon->name, p1pokemon->currentHP, p1pokemon->maxHP);

    if (p2pokemon->currentHP > 0)
        printf("%s active: %s HP=%d/%d\n", p2->name, p2pokemon->name, p2pokemon->currentHP, p2pokemon->maxHP);
    else
        printf("%s fainted: %s HP=%d/%d\n", p2->name, p2pokemon->name, p2pokemon->currentHP, p2pokemon->maxHP);

        // /* Auto-switch if someone fainted (after the round) */
    if (p1->Pokemons[p1->currentIndex].currentHP == 0) {
        //p1->currentIndex = nextAliveIndex(p1);
        int idx = nextAliveIndex(p1);
        if (idx != -1) {
            p1->currentIndex = idx;
        }
    }
    if (p2->Pokemons[p2->currentIndex].currentHP == 0) {
        //p2->currentIndex = nextAliveIndex(p2);
        int idx = nextAliveIndex(p2);
        if (idx != -1) {
            p2->currentIndex = idx;
        }
    }
}
/* helper: check if player still has any alive pokemon */
int hasAlivePokemon(Player *p) {
    for (int i = 0; i < 6; i++) {
        if (p->Pokemons[i].currentHP > 0) 
        return 1;
    }
    return 0;
}
void game(Player *p1, Player *p2){
    printf("\n===== GAME START =====\n");
    //printf("%s vs %s\n", p1->name, p2->name);
    
    while (hasAlivePokemon(p1) && hasAlivePokemon(p2)) {
        playRound(p1, p2);
        /* optional: if one player just lost their last pokemon, stop immediately */
        // if (!hasAlivePokemon(p1) || !hasAlivePokemon(p2)) break;
    }
    printf("\n===== GAME OVER =====\n");
    if (hasAlivePokemon(p1) && !hasAlivePokemon(p2)) {
        printf("Winner: %s\n", p1->name);
    } else if (hasAlivePokemon(p2) && !hasAlivePokemon(p1)) {
        printf("Winner: %s\n", p2->name);
    } else {
        printf("Draw.\n");
    }
}