#include <stdio.h>
#include <stdlib.h>
#include "board.h"

/**
 * \file board.c
 *
 * \brief Source code associated with \ref board.h
 *
 * \author Biguey Armand
 */

/**
 * @brief The board of the game.
 */
struct board_s {
	cell grid[MAX_DIMENSION][MAX_DIMENSION];
	player current;
	int co_NK[2];
	int co_SK[2];
	bool hex;
	bool range;
	bool player_to_move;
};

 /**
  * A faire :
  * 
  * new_special_game
  * move_toward
  * kill_cell
  */

board new_game(){
	board new_board = malloc(sizeof(struct board_s));
	new_board->current = NORTH;
	new_board->hex = false;
	new_board->range = false;
	new_board->co_NK[0] = 0;
	new_board->co_NK[1] = 3;
	new_board->co_SK[0] = NB_LINES-1;
	new_board->co_SK[1] = 3;
	new_board->player_to_move = true;
	for(int l = 0; l < MAX_DIMENSION; l++) {
			for (int c = 0; c < MAX_DIMENSION; c++) {
				new_board->grid[l][c] = KILLED;
			}
		}
	for (int l = 0; l < NB_LINES; l++)
	{
		for (int c = 0; c < NB_COLS; c++)
		{
			if(l == 0 && c == 3) new_board->grid[l][c] = NORTH_KING;
			else if (l == NB_LINES-1 && c == 3) new_board->grid[l][c] = SOUTH_KING;
			else new_board->grid[l][c] = EMPTY;
		}
	}
	return new_board;
}

board copy_game(board original_game){
	board new_board = malloc(sizeof(struct board_s));
	new_board->current = original_game->current;
	new_board->co_NK[0] = original_game->co_NK[0];
	new_board->co_NK[1] = original_game->co_NK[1];
	new_board->co_SK[0] = original_game->co_SK[0];
	new_board->co_SK[1] = original_game->co_SK[1];
	new_board->hex = original_game->hex;
	new_board->range = original_game->range;
	new_board->player_to_move = original_game->player_to_move;
	for (int l = 0; l < MAX_DIMENSION; l++)
	{
		for (int c = 0; c < MAX_DIMENSION; c++)
		{
			new_board->grid[l][c] = original_game->grid[l][c];
		}
	}
	return new_board;
}

void destroy_game(board game){
	free(game);
};

// TODO ajouter les entêtes des autres fonctions et completer.

board new_special_game(bool is_hex, bool use_range){ // une game avec les variantes 
	board new_board = malloc(sizeof(struct board_s));
	new_board->current = NORTH;
	new_board->player_to_move = true;
	if(!is_hex){
		new_board = new_game();
		new_board->range = use_range;
	} else {
		new_board->hex = is_hex;
		new_board->range = use_range;
		new_board->co_NK[0] = 0;
		new_board->co_NK[1] = HEX_SIDE/2;
		new_board->co_SK[0] = 2*HEX_SIDE-2;
		new_board->co_SK[1] = (HEX_SIDE+ HEX_SIDE/2)-1;
		for(int l = 0; l < MAX_DIMENSION; l++) {
			for (int c = 0; c < MAX_DIMENSION; c++) {
				new_board->grid[l][c] = KILLED;
			}
		}
		for(int l = 0; l <= 2*HEX_SIDE-2; l++){
			for(int c = 0; c <= 2*HEX_SIDE-2; c++){
				if(c > 4+l || c < l-4) new_board->grid[l][c] = KILLED;
				else if (l == new_board->co_NK[0] && c == new_board->co_NK[1]) new_board->grid[l][c] = NORTH_KING;
				else if(l == new_board->co_SK[0] && c == new_board->co_SK[1]) new_board->grid[l][c] = SOUTH_KING;
				else new_board->grid[l][c] = EMPTY;
			}
		}
	}
	return new_board;
}

bool is_hex(board game){ // pour savoir si on joue la variante de l'hexagonale 
	return game->hex;
}

bool uses_range(board game){ // pour savoir si on joue la variante de la portée 
	return game->range;
}

player current_player(board game){ // retourne le joueur qui doit jouer et qui est en train de jouer 
	return game->current;
}

cell get_content(board game, int line, int column){ // retourne ce que contient la cellule 
	if (line >= MAX_DIMENSION || column >= MAX_DIMENSION || line < 0 || column < 0) return KILLED;
	else return game->grid[line][column];
}

int * get_current_co_player2(board game){
	if(game->current == NORTH) return game->co_NK;
	else return game->co_SK;
}

bool can_move2(board game, int co_player[2]){
	int nb_cell_empty = 0;
	for(int l = co_player[0]-1; l <= co_player[0]+1; l++){
		for(int c = co_player[1]-1; c <= co_player[1]+1; c++){
			if(get_content(game, l, c) == EMPTY && !(game->hex && ((l == co_player[0]-1 && c == co_player[1] + 1) || (l == co_player[0] + 1 && c == co_player[1] - 1)))) nb_cell_empty++;
		}
	}
	if(nb_cell_empty <= 0) return false;
	else return true;
}

player get_winner(board game){ // retourne si un joueur a gagner ou non et lequel 
	if(current_player(game) == NORTH && !can_move2(game, game->co_NK)) return SOUTH;
	else if(current_player(game) == SOUTH && !can_move2(game, game->co_SK)) return NORTH;
	else return NO_PLAYER;
}

int * co_cell_direction2(direction direction, int *co_player, board game){
	int * co = malloc(2*sizeof(int));
	switch (direction)
	{
	case NW:
		co[0] = co_player[0]-1;
		co[1] = co_player[1]-1;
		break;
		case NE:
		if(game->hex) {
			co[0] = co_player[0]-1;
			co[1] = co_player[1];
		} else {
			co[0] = co_player[0]-1;
			co[1] = co_player[1]+1;
		}
		break;
	case N:
		co[0] = co_player[0]-1;
		co[1] = co_player[1];
		break;
	
	case W:
		co[0] = co_player[0];
		co[1] = co_player[1]-1;
		break;
	case E:
		co[0] = co_player[0];
		co[1] = co_player[1]+1;
		break;
	case SW:
		if(game->hex){
			co[0] = co_player[0]+1;
			co[1] = co_player[1]; 
		} else {
			co[0] = co_player[0]+1;
			co[1] = co_player[1]-1;
		}
		break;
	case S:
		co[0] = co_player[0]+1;
		co[1] = co_player[1];
		break;
	case SE:
		co[0] = co_player[0]+1;
		co[1] = co_player[1]+1;
		break;
	default:
		co[0] = 404;
		break;
	}
	return co;
}

enum return_code move_toward(board game, direction direction){ // fait bouger le joueur ou retourne un message d'erreur s'il y a une erreur 
	int *co = co_cell_direction2(direction, get_current_co_player2(game), game);
	if(!game->player_to_move || co[0] == 404 || (game->hex && (direction == N || direction == S))) { 
		free(co);
		return RULES; 
	} else if (get_content(game, co[0], co[1]) == KILLED) {
		free(co);
		return OUT;
	} else if (get_content(game, co[0], co[1]) == NORTH_KING || get_content(game, co[0], co[1]) == SOUTH_KING) {
		free(co);
		return BUSY;
	} else {
		if(current_player(game) == SOUTH) {
			game->grid[co[0]][co[1]] = SOUTH_KING;
			game->grid[game->co_SK[0]][game->co_SK[1]] = EMPTY;
			game->co_SK[0] = co[0];
			game->co_SK[1] = co[1];
			game->player_to_move = false;
		} else {
			game->grid[co[0]][co[1]] = NORTH_KING;
			game->grid[game->co_NK[0]][game->co_NK[1]] = EMPTY;
			game->co_NK[0] = co[0];
			game->co_NK[1] = co[1];
			game->player_to_move = false; 
		}
		free(co);
		return OK;
	}
}

bool is_range_valid2(board game, int line, int column){
	if(!game->range) return true;
	else {
		int grid[MAX_DIMENSION][MAX_DIMENSION];
		int co[90][2];
		for (int i = 0; i < 90; i++) {
			co[i][0] = -1;
			co[i][1] = -1;
		}

		int vl = 0;
		for(int l = 0; l < MAX_DIMENSION; l++){
			for(int c = 0; c < MAX_DIMENSION; c++){
				if(get_content(game, l, c) == KILLED) grid[l][c] = -1;
				else if(get_content(game, l, c) == NORTH_KING || get_content(game, l, c) == SOUTH_KING) grid[l][c] = -2;
				else grid[l][c] = 1000;
			}
		}

		int *co_cur_p = get_current_co_player2(game);
		grid[co_cur_p[0]][co_cur_p[1]] = 0;
		co[0][0] = co_cur_p[0];
		co[0][1] = co_cur_p[1];
		while(vl <= 4){
			vl++;
			for (int k = 0; k < 90; k++) {
				
				if (co[k][0] != -1 && co[k][1] != -1) {
						
					for(int i = -1; i < 2; i++){
						for(int l = -1; l < 2; l++){
							if((co[k][0] + i) < 0 || (co[k][1] + l) < 0);
							else if(grid[co[k][0] + i][co[k][1] + l] >= vl && (grid[co[k][0] + i][co[k][1] + l] != -1 || grid[co[k][0] + i][co[k][1] + l] != -2) && !(game->hex && ((i == -1 && l == 1) || (i == 1 && l == -1)))) grid[co[k][0] + i][co[k][1] + l] = vl;
						}
					}
					co[k][0] = -1;
					co[k][1] = -1;
				}

			}
			int count = 0;
			for(int l = 0; l < MAX_DIMENSION; l++){
				for(int c = 0; c < MAX_DIMENSION; c++){
					if (grid[l][c] == vl) {
						co[count][0] = l;
						co[count][1] = c;
						count++;
					}
				}
			}
			
		}
		if(grid[line][column] <= 3) return true;
		else return false; 
	}
}

enum return_code kill_cell(board game, int line, int column){ // tue la cellule ciblé ou retourne un message d'erreur s'il y a une erreur 
	if(game->player_to_move) return RULES;
	else if(get_content(game, line, column) == KILLED || line >= MAX_DIMENSION || column >= MAX_DIMENSION) return OUT;
	else if(get_content(game, line, column) == NORTH_KING || get_content(game, line, column) == SOUTH_KING) return BUSY;
	else if(!is_range_valid2(game, line, column)) return RULES;
	else {
		game->grid[line][column] = KILLED;
		game->player_to_move = true;
		if(game->current == SOUTH) game->current = NORTH;
		else game->current = SOUTH;
		return OK;
	}
}
