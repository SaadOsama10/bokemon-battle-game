#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structs.h"
    
int main(){
    Type *Types = malloc(18 * sizeof(Type));
    Move *Moves = malloc(486 * sizeof(Move));
    Pokemon *Pokemons = malloc(1015 * sizeof(Pokemon));
    Player p1, p2;
    initialize(Types, Moves, Pokemons, &p1, &p2);
    game(&p1, &p2);
    free(Types);
    free(Moves);
    free(Pokemons);
    return 0;
}