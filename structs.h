//structs
#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct{
char atkName[50];
char defName[50];
double multiplier;
}TypeEffect;

// in the type.txt we have 18 types + 1 None type
typedef struct{
char name[50];
TypeEffect effects[19];
}Type;
extern Type NoneType;

typedef enum{
PHYSICAL, SPECIAL
}Category;

// move count = 486
typedef struct{
char name[50];
Type type;
Category category; 
int power;
}Move;

// pokemon count = 1015
typedef struct{
char name[50];
Type types[2];
int maxHP;
int currentHP;
int attack;
int defense;
int spAtk;
int spDef;
int speed;
Move moves[4];
}Pokemon;

typedef struct{
char name[50];
Pokemon Pokemons[6];
int currentIndex; // currentIndex: Current index in Pokemons array
}Player;

void initializeTypes(Type Types[]);
void initializeMoves(Move Moves[], Type Types[]);
void initializePokemons(Pokemon Pokemons[],Type Types[], Move Moves[]);
double effectiveness(Type *atkType, char *defTypeName);
double STAB(Pokemon *attacker, Move *move);
void attack(Pokemon *attacker, Pokemon *defender, int moveIndex);
int nextAliveIndex(Player *p);
void applyDamage(Player *p1, Player *p2, int move1, int move2);
void printActive(Player *p);
void printMoves(Pokemon *pokemon);
void printTeamChoices(Player *p);
int hasSwitchOption(Player *p);
void playRound(Player *p1, Player *p2);
int hasAlivePokemon(Player *p);
void initialize(Type Types[], Move Moves[], Pokemon Pokemons[], Player *p1, Player *p2);
void game(Player *p1, Player *p2);

#endif