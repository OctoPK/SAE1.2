#include <stdio.h>
#include <unistd.h>
#include "board.h"
#include "ANSI-color-codes.h"
#ifdef _WIN32
#include <conio.h>
#include <locale.h>
#endif
#define N_PLAYER GREENB BHWHITE "N" CRESET
#define S_PLAYER CYANB BHWHITE "S" CRESET
#define C_KILL WHITEB BBLACK "#" CRESET
#define CLEAR "\33[H\33[2J"

// fonction pour convertire en majuscule une lettre
char upper_case(char c){
  if(c <= 'z' && c >= 'a') return c + ('A' - 'a');
  return c;
}

// fonction pour l'animation du début
void anim(int time){
    fflush(stdout);
    usleep(time);
}


// Les 4 prochaine fonction sert à l'affichage de la grille hexagonale sauf que je n'ai pas eu le temp de tout finir

// sous fonction de la grille hexagonale
void chapeau(int n, int décalage, int round)
{    
    for (int i = 0; i < décalage; i++)
    {
        printf(" ");
    }
    for (int i = 0; i < n; i++)
    {
        printf("\033[38;5;124m / \\" CRESET);
        if(round == 0) anim(5000);
    }
    printf("\n");
}

void reverse_chapeau(int n, int décalage, int round)
{
    for (int i = 0; i < décalage; i++)
    {
        printf(" ");
    }
    for (int i = 0; i < n; i++)
    {
        printf("\033[38;5;124m \\ /" CRESET);
        if(round == 0) anim(5000);
    }
    printf("\n");
}

void corp(int décalage, int round, board game, int num_l, int col)
{
    int start;
    if(num_l-1 <= 4) start = col+HEX_SIDE+num_l-1;
    else start = HEX_SIDE+num_l-1-col;
    for (int i = 0; i < décalage; i++)
    {
        printf(" ");
    }
    printf("%d", num_l);
    printf("\033[38;5;202m| ");
    for (int i = col; i < start; i++)
    {
        if(get_content(game, num_l-1, i) == NORTH_KING) printf(N_PLAYER" ");
        else if(get_content(game, num_l-1, i) == SOUTH_KING) printf(S_PLAYER" ");
        else if(get_content(game, num_l-1, i) == KILLED) printf(C_KILL" ");
        else printf("  ");
        printf("\033[38;5;202m| " CRESET);
        if(round == 0) anim(5000);
    }
    printf("\b%d", num_l);
    printf("\n");
}

// affiche toute la grille hexagonale
void grille_hexagonale(int n, int round, board game)
{
    int size=n;
    int i;
    int j;
    for (int i = 0; i <= 2*n+2; i++) printf(" ");
    for(int i = 1; i <= 5; i++) printf("%d   ", i);
    printf("\n");
    for (i = 0; i < n; i++)
    {        
        printf(" ");
        chapeau(size, 2*(n-i-1), round);
        corp(2*(n-i-1), round, game, i+1, 0);
        if(i+1 == n) {
            printf(" ");
            reverse_chapeau(size, 2*(n-i-1), round);
        }
        size++;
    }
    size--;
    for (j = 1; j < n; j++)
    {
        i++;
        size--;
        corp(j*2, round, game, i, j);
        printf(" ");
        reverse_chapeau(size, j*2, round);
    }
    for (int i = 0; i < 2*n-1; i++) printf(" ");
    for(i = 5; i < 5+HEX_SIDE; i++) printf("%d   ", i);
    printf("\n");
}

// sous fonction servant à l'affichage de la grille classique
void line(int nb_cols, int nb, int round){
    printf(RED "+" CRESET);
    for(int j = 0; j < nb_cols; j++){
        for(int i = 0; i < nb; i++) {
            printf(BLUE "-" CRESET);
            if(round == 0) anim(2000);
        }
        printf(RED "+" CRESET);

    }
    printf("\n");
}

// affiche soit la grille classique soit l'hexagonale en fonction de si le paramètre est activé ou non
void afficher(board game, int round, int hex){
    printf(CLEAR);
    if(hex == 0){
        printf("  ");
        for(int j = 0; j < NB_COLS; j++){
            printf("  %d ", j+1);
            // pour affiché l'animation que au démarrage
            if(round == 0) anim(10000);
        }
        printf("\n");
        for(int i = 0; i < NB_LINES; i++){
            printf("  ");
            line(NB_COLS, 3, round);
            printf("%d ", i+1);
            for(int j = 0; j < NB_COLS; j++){
                printf(HGREEN "| " CRESET);
                // condition pour savoir cette qu'elle case affiché soit les cellules mortes soit les roi 
                if(get_content(game, i, j) == NORTH_KING) printf(N_PLAYER" ");
                else if(get_content(game, i, j) == SOUTH_KING) printf(S_PLAYER" ");
                else if(get_content(game, i, j) == KILLED) printf(C_KILL" ");
                else printf("  ");
                if(round == 0) anim(6000);
            }
            printf(HGREEN "|" CRESET " %d\n", i+1);
        }
        printf("  ");
        line(NB_COLS, 3, round);
        printf(" ");
        for(int j = 0; j < NB_COLS; j++){
            printf("  %d ", j+1);
            if(round == 0) anim(10000);
        }
        printf("\n");
    }
    else {
        grille_hexagonale(5, round, game);
    }
}


// fonction qui retourne la direction en fonction de la réponse de l'utilisateur 
direction player_move(int usr){
    direction dir;
        switch (usr)
        {
        case 9:
            dir = NE;
            break;
        case 8:
            dir = N;
            break;
        case 7:
            dir = NW;
            break;
        case 6:
            dir = E;
            break;
        case 4:
            dir = W;
            break;
        case 3:
            dir = SE;;
            break;
        case 2:
            dir = S;
            break;
        case 1:
            dir = SW;
            break;
        default:
            break;
        }
        return dir;
}

// regarde si le déplacement est correct et affiche pourquoi
bool is_move_correct(board game, int usr, int silent) {
    if (usr > 9 || usr < 1 || usr == 5) {
        if (!silent) printf(RED "ERREUR \nCoup invalide\n" CRESET "Retentez : ");
        return 0;
    }
    if (is_hex(game) && (usr == 8 || usr == 2)) {
        if (!silent) printf(RED "ERREUR !\n Ceci n'est pas une direction\n" CRESET "Retentez : ");
        return 0;
    }
    switch (move_toward(game, player_move(usr))) {
    case RULES:
        if (!silent) printf(RED "ERREUR !\n Ce n'est pas une direction ou tu n'es pas censé te déplacé\n" CRESET "Retentez : ");
        return 0;
    case OUT:
        if (!silent) printf(RED "ERREUR !\n Ce coup vous fait sortir du plateau\n" CRESET "Retentez : ");
        return 0;
    case BUSY:
        if (!silent) printf(RED "ERREUR !\n Cette case est déjà occupé par le roi adverse\n" CRESET "Retentez : ");
        return 0;
    case OK:
        if (!silent) {
            printf("Pièce déplacé !\n");
            usleep(10000);
        }
        return 1;
    default:
        if (!silent) printf("ERREUR !\n Contactez le développeur");
        return 0;
    }
}

// pareil que la fonction d'avant mais pour les cellules tuée
int is_kill_cell_correct(board game, int usr_lig, int usr_col){
    switch (kill_cell(game, usr_lig-1, usr_col-1))
    {
    case RULES:
        if(uses_range(game)) printf(RED "ERREUR !\n la cellule est trop loin\n" CRESET "Retentez !\n");
        else printf(RED "ERREUR !\n Vous devez d'abord bougé \n" CRESET "Retentez !\n");
        return 0;
    case OUT:
        printf(RED "ERREUR !\n La cellule est en dehors du plateau ou déjà tuée\n" CRESET "Retentez !\n");
        return 0;
    case BUSY:
        printf(RED "ERREUR !\n La cellule est déjà occupé par le roi adverse !\n" CRESET "Retentez !\n");
        return 0;
    case OK:
        printf("Cellule tuée !\n");
        usleep(100000);
        return 1;
    default:
        printf("ERREUR !\n");
        return 0;
    }
}

bool is_direction_possible(board game, int direction){
    board cp_game = copy_game(game);
    if(!is_move_correct(cp_game, direction, 1)) {
        destroy_game(cp_game);
        return false;
    } else { 
        destroy_game(cp_game);
        return true;
    }
}

// affiche la grille de déplacement uniquement valable pour la grille classique 
void afficher_direction(int round, bool hex, board game){
    if(!hex){
        printf("\n");
        printf(CYAN "Voici la grille des déplacement :\n\n");
        for(int i = 2; i >= 0; i--){
            line(3, 3, round);
            for(int j = 1; j <= 3; j++){
                if(3*i+j == 5) printf(HGREEN "|" CRESET " * ");
                else if(is_direction_possible(game, 3*i+j)) printf(HGREEN "| " GREEN "%d " CRESET, 3*i+j);
                else printf(HGREEN "| " RED "%d " CRESET, 3*i+j);
                if(round == 0) anim(15000);
            }
            printf(HGREEN "|\n" CRESET);
        }
        line(3, 3, round);
    } else {
        printf("\n");
        printf(CYAN "Voici la grille des déplacement :\n\n");
        printf("     \033[38;5;124m/ \\ / \\\n" CRESET);
        printf("    ");
        if(is_direction_possible(game, 7)) printf("\033[38;5;202m| " GREEN "7 " CRESET);
        else printf("\033[38;5;202m| " RED "7 " CRESET);
        if(is_direction_possible(game, 9)) printf("\033[38;5;202m| " GREEN "9 \033[38;5;202m|" CRESET);
        else printf("\033[38;5;202m| " RED "9 \033[38;5;202m|" CRESET);
        printf("\n   \033[38;5;124m/ \\ / \\ / \\\n" CRESET);
        printf("  ");
        if(is_direction_possible(game, 4)) printf("\033[38;5;202m| " GREEN "4 " CRESET);
        else printf("\033[38;5;202m| " RED "4 " CRESET);
        printf("\033[38;5;202m| " CRESET "* ");
        if(is_direction_possible(game, 6)) printf("\033[38;5;202m| " GREEN "6 \033[38;5;202m|" CRESET);
        else printf("\033[38;5;202m| " RED "6 \033[38;5;202m|" CRESET);
        printf("\n   \033[38;5;124m\\ / \\ / \\ /\n" CRESET);
        printf("    ");
        if(is_direction_possible(game, 1)) printf("\033[38;5;202m| " GREEN "1 " CRESET);
        else printf("\033[38;5;202m| " RED "1 " CRESET);
        if(is_direction_possible(game, 3)) printf("\033[38;5;202m| " GREEN "3 \033[38;5;202m|" CRESET);
        else printf("\033[38;5;202m| " RED "3 \033[38;5;202m|" CRESET);
        printf("\n     \033[38;5;124m\\ / \\ /\n" CRESET);
    }
}

// fonction qui gère le choix des règles de l'utilisateur 
void choose_rule(int *hex, int *rule){
    char usr;
    printf("Souhaitez vous une partie avec une grille hexagonale (Y/N) (pas encore fini) : ");
    scanf("%c", &usr);
    getchar();
    usr = upper_case(usr);
    if(usr == 'Y') *hex = 1;
    else *hex = 0;
    printf("Souhaitez vous la règles de portée ? (Y/N) : ");
    scanf("%c", &usr);
    getchar();
    usr = upper_case(usr);
    if(usr == 'Y') *rule = 1;
    else *rule = 0;
}

// fonction pour le choix des nom 
void choose_player(char *player1, char *player2){
    printf("Choisisez le nom du premier joueur (64 caractère maximale): ");
    scanf("%63[\n]", player1);
    scanf("%63[^\n]", player1);
    printf("Choisisez le nom du deuxième joueur (64 caractère maximale): ");
    scanf("%63[\n]", player2);
    scanf("%63[^\n]", player2);
}

void afficher_stat(board game, int round, char* player1, char* player2){
    afficher(game, round, is_hex(game));
    if(round%2 == 0) afficher_direction(round, is_hex(game), game);
    // pour savoir c'est a qui de jouer
    printf("\n");
    if(current_player(game) == NORTH) printf("C'est a " GREENB BHWHITE "%s" CRESET " de jouer !\n", player1);
    else printf("C'est a " CYANB BHWHITE "%s" CRESET " de jouer !\n", player2); 
}

int main(){
    #ifdef _WIN32
    system("chcp 65001");
    setlocale(LC_ALL, ".UTF-8");
    #endif
    int usr = 0;
    int usr1;
    int round = 0;
    int hex;
    int rule;
    char player1[64];
    char player2[64];
    choose_rule(&hex, &rule);
    choose_player(player1, player2);
    board game = new_special_game(hex, rule);
    while(get_winner(game) == NO_PLAYER){
        afficher_stat(game, round, player1, player2);
        round++;
        printf("Choisisez la direction souhaité : ");
        // permet à l'utilisateur la rentrée de mouvement incorrecte et de cellule a tuée
        do{
            scanf("%d", &usr);
            getchar();
        } while(!is_move_correct(game, usr, 0));
        afficher_stat(game, round, player1, player2);
        round++;
        printf("\nChoisisez la cellule a tuée.\n");
        do{
            printf("Numéro de la ligne : ");
            scanf("%d", &usr);
            getchar();
            printf("Numéro de la colonne : ");
            scanf("%d", &usr1);
            getchar();
        } while(!is_kill_cell_correct(game, usr, usr1));
    }
    sleep(1);
    printf(CLEAR);
    if(get_winner(game) == NORTH) printf("Bravo a " GREENB BHWHITE "%s" CRESET " qui a gagné !\n", player1);
    else printf("Bravo a " CYANB BHWHITE "%s" CRESET " qui a gagné !\n", player2); 
    printf("La partie a durée %d tour !\n", round);
    destroy_game(game);
    #ifdef _WIN32
    printf("(Appuyer sur n'importe qu'elle touche du clavier pour quittez !)");
    getch();
    #else
    printf("(Appuyez sur entrez pour quitter !)");
    getchar();
    #endif
    return 0;
}
