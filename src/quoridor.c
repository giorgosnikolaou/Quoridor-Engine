#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>


#include "quoridor.h"
#include "agent.h"
#include "moves.h"
#include "evaluation.h"


#define LETTERS 26

#define HORIZONTAL_WALL "="
#define VERTICAL_WALL "H"


static Board quoridor_init(uint32_t dimension)
{
    assert(dimension % 2);

    Board board = malloc(sizeof(*board));
    
    board->walls = (int)((7 / 4.0) * dimension - 23 / 4.0);

    board->dimension = dimension;
    board->enemy_walls = board->walls;
    board->player_walls = board->walls;

    board->player.i = dimension - 1;
    board->player.j = dimension / 2;

    board->enemy.i = 0;
    board->enemy.j = dimension / 2;
    
    
    board->letter_numbering = malloc(sizeof(char*) * dimension);

    for (uint32_t i = 0; i < dimension; i++)
    {

        size_t length = i == 0;
        uint32_t temp = i;

        while (temp)
        {
            temp /= LETTERS;
            length++;
        }

        board->letter_numbering[i] = malloc(sizeof(char) * (length + 1));

        size_t value = 1;

        for (size_t j = 0; j < length; j++)
            value *= LETTERS;

        for (size_t j = 0; j < length; j++, value /= LETTERS)
        {
            size_t remainder = i % value;
            size_t digit = remainder / (value / LETTERS) - (j == 0 && length != 1);
            
            board->letter_numbering[i][j] = digit + 'A';//  letters[digit][0];
        }

        board->letter_numbering[i][length] = '\0';
        // printf("%s\n", board->letter_numbering[i]);

        if (i == dimension - 1)
            board->max_lettering = length;
    }

    board->nodes = malloc(sizeof(*board->nodes) * (dimension - 1));
    for (size_t i = 0; i < dimension - 1; i++)
        board->nodes[i] = calloc(dimension - 1, sizeof(*(board->nodes[i])));
    
    board->hor_walls = malloc(sizeof(*board->hor_walls) * (dimension - 1));
    for (size_t i = 0; i < dimension - 1; i++)
        board->hor_walls[i] = calloc(dimension, sizeof(*(board->nodes[i])));
    

    board->ver_walls = malloc(sizeof(*board->ver_walls) * dimension);
    for (size_t i = 0; i < dimension; i++)
        board->ver_walls[i] = calloc(dimension - 1, sizeof(*(board->nodes[i])));

    board->history = vector_create(10, free);

    return board;
}

static void quoridor_destroy(Board board)
{
    for (size_t i = 0; i < board->dimension - 1; i++)
        free(board->nodes[i]);
    
    free(board->nodes);

    for (size_t i = 0; i < board->dimension - 1; i++)
        free(board->hor_walls[i]);
    
    free(board->hor_walls);

    for (size_t i = 0; i < board->dimension; i++)
        free(board->ver_walls[i]);
    
    free(board->ver_walls);

    for (size_t i = 0; i < board->dimension; i++)
        free(board->letter_numbering[i]);
    
    free(board->letter_numbering);

    vector_destroy(board->history);

    free(board);

}

static void print_str(const char* str, uint8_t spaces)
{
    for (uint8_t i = 0; i < spaces; i++)
        printf("%s", str);
}

static void quoridor_print(Board board)
{
    uint32_t size = size = board->dimension;
    
    uint32_t white_x = board->player.j;
    uint32_t white_y = board->player.i;

    uint32_t black_x = board->enemy.j;
    uint32_t black_y = board->enemy.i;

    uint32_t temp = size;
    uint8_t digits = size == 0;

    while (temp)
    {
        temp /= 10;
        digits++;
    }

    print_str(" ", digits + 3);

    for (uint32_t i = 0; i < size; i++)
    {
        print_str(" ", board->max_lettering - strlen(board->letter_numbering[i]) + 1);
        printf("%s", board->letter_numbering[i]);
        print_str(" ", 2);
    }
    
    printf("\n");

    print_str(" ", digits + 2);
    printf("+");
    for (uint32_t j = 0; j < size; j++)
    {
        print_str("-", board->max_lettering + 2);
        printf("+");
    }
    

    printf("\n");

    for (uint32_t i = 0; i < size - 1; i++)
    {

        uint32_t temp = size - i;
        uint8_t curr_digits = (size - i) == 0;

        while (temp)
        {
            temp /= 10;
            curr_digits++;
        }

        print_str(" ", digits - curr_digits + 1);
        printf("%d", size - i);
        printf(" |");


        for (uint32_t j = 0; j < size - 1; j++)
        {
            print_str(" ", board->max_lettering);
            printf("%s", (i == white_y && j == white_x) ? "W" : (i == black_y && j == black_x) ? "B" : " ");
            printf(" ");
            
            printf("%s", board->ver_walls[i][j] ? VERTICAL_WALL : "|");
            
        }

        print_str(" ", board->max_lettering);
        printf("%s", (i == white_y && (size - 1) == white_x) ? "W" : (i == black_y && (size - 1) == black_x) ? "B" : " ");
        printf(" ");

        printf("|");
        print_str(" ", digits - curr_digits + 1);
        printf("%d", size - i);
        printf(" ");

        if (i == 0)
            printf("black walls: %d", board->enemy_walls);
        else if (i == 1)
            printf("white walls: %d", board->player_walls);

        printf("\n");

        print_str(" ", digits + 2);
        printf("+");
        for (uint32_t j = 0; j < size - 1; j++)
        {
            print_str(board->hor_walls[i][j] ? HORIZONTAL_WALL : "-", board->max_lettering + 2);
            printf("+");
        }

        print_str(board->hor_walls[i][size - 1] ? HORIZONTAL_WALL : "-", board->max_lettering + 2);
        printf("+");
        

        printf("\n");


    }

    temp = 1;
    uint8_t curr_digits = 1;

    print_str(" ", digits - curr_digits + 1);
    printf("%d", temp);
    printf(" |");


    for (uint32_t j = 0; j < size - 1; j++)
    {
        // Node curr = board->board[size - 2][j];
        print_str(" ", board->max_lettering);
        printf("%s", ((size - 1) == white_y && j == white_x) ? "W" : ((size - 1) == black_y && j == black_x) ? "B" : " ");
        printf(" ");
        
        // printf("%s", ((curr.walls >> 2) & 1) ? VERTICAL_WALL : "|");
        printf("%s", board->ver_walls[size - 1][j] ? VERTICAL_WALL : "|");
        
    }

    print_str(" ", board->max_lettering);
    printf("%s", ((size - 1) == white_y && (size - 1) == white_x) ? "W" : ((size - 1) == black_y && (size - 1) == black_x) ? "B" : " ");
    printf(" ");

    printf("|");
    print_str(" ", digits - curr_digits + 1);
    printf("%d", 1);
    printf(" ");

    printf("\n");



    print_str(" ", digits + 2);
    printf("+");

    for (uint32_t j = 0; j < size; j++)
    {
        print_str("-", board->max_lettering + 2);
        printf("+");
    }

    printf("\n");
    
    print_str(" ", digits + 3);

    for (uint32_t i = 0; i < size; i++)
    {
        print_str(" ", board->max_lettering - strlen(board->letter_numbering[i]) + 1);
        printf("%s", board->letter_numbering[i]);
        print_str(" ", 2);
    }
    
    printf("\n");

}

static void quoridor_reset(Board board)
{
    board->enemy_walls = board->walls;
    board->player_walls = board->walls;

    board->player.i = board->dimension - 1;
    board->player.j = board->dimension / 2;

    board->enemy.i = 0;
    board->enemy.j = board->dimension / 2;

    size_t dimm = board->dimension - 1;
    for (size_t i = 0; i < dimm; i++)
    {
        for (size_t j = 0; j < dimm; j++)
            board->nodes[i][j] = 0;
    }
    
    for (size_t i = 0; i < dimm; i++)
    {
        for (size_t j = 0; j < board->dimension; j++)
            board->hor_walls[i][j] = 0;
    }
    

    for (size_t i = 0; i < board->dimension; i++)
    {
        for (size_t j = 0; j < dimm; j++)
            board->ver_walls[i][j] = 0;
    }
    vector_destroy(board->history);
    board->history = vector_create(10, free);
}



extern size_t prunes;
extern size_t evaluations;

static char* create_string(const char* str)
{
    size_t len = strlen(str) + 1;
    char* p = malloc(sizeof(char) * len);
    memcpy(p, str, len);
    return p;
}

Engine engine_create(const char* name)
{
    Engine en = malloc(sizeof(*en));
    en->name = create_string(name);

    en->board = NULL;

    return en;
}


static const char* commands[] = {  "name", "known_command", "list_commands", "quit", "boardsize", "clear_board", "walls", 
                            "playmove", "playwall", "genmove", "undo", "winnner", "showboard" };

static const size_t commands_count = sizeof(commands) / sizeof(*commands);

static void engine_name(Engine engine)
{
    printf("= %s\n\n", engine->name);
}


static void engine_known_command(const char* command)
{
    for (size_t i = 0; i < commands_count; i++)
    {
        if (strcmp(command, commands[i]) == 0)
        {
            printf("= true\n\n");
            return ;
        }
    }

    printf("= false\n\n");
}

static void engine_list_commands()
{
    printf("=%s", (commands_count == 0) ? "\n\n" : " ");
    for (size_t i = 0; i < commands_count; i++)
        printf("\t%s\n", commands[i]);
}

static void engine_quit(Engine engine)
{
    free(engine->name);
    quoridor_destroy(engine->board);
    printf("=\n\n");
}


static void engine_boardsize(Engine engine, const uint32_t size)
{
    if (size % 2 == 0)
    {
        printf("? unacceptable size\n\n");
        return ;
    }

    if (engine->board != NULL)
        quoridor_destroy(engine->board);
    
    engine->board = quoridor_init(size);

    printf("=\n\n");
}

static void engine_clear_board(Engine engine)
{
    quoridor_reset(engine->board);
    printf("=\n\n");
}


static void engine_walls(Engine engine, const uint32_t walls)
{
    engine->board->walls = walls;
    engine->board->enemy_walls = walls;
    engine->board->player_walls = walls;
    printf("=\n\n");
}

static bool valid_move(Board board, char player, uint32_t i, uint32_t j)
{
    Position* p = (player == 'W') ? &board->player : &board->enemy;
    Position* e = (player == 'B') ? &board->player : &board->enemy;

    uint32_t dimm = board->dimension - 1;

    // Up
    if (p->i > 0)
    {
        if (board->hor_walls[p->i - 1][p->j] == 0)
        {
            if (e->i + 1 != p->i || e->j != p->j)
            {    // U
                if (p->i == i + 1 && p->j == j)
                    return true;
            }
            else
            {
                // UU
                if ((p->i > 1) && board->hor_walls[p->i - 2][p->j] == 0)
                {
                    if (p->i == i + 2 && p->j == j)
                        return true;
                }
                else
                {
                    // UL
                    if (p->j > 0 && board->ver_walls[p->i - 1][p->j - 1] == 0)
                    {
                        if (p->i == i + 1 && p->j == j + 1)
                            return true;
                    }

                    // UR
                    if (p->j < dimm && board->ver_walls[p->i - 1][p->j] == 0)
                    {
                        if (p->i == i + 1 && p->j + 1 == j)
                            return true;
                    }
                }                
            }
            
        }
    }


    // Down
    if (p->i < dimm)
    {
        if (board->hor_walls[p->i][p->j] == 0)
        {
            // No enemy on the right
            if (e->i != p->i + 1 || e->j != p->j)
            {
                if (p->i + 1 == i && p->j == j)
                    return true;
            }
            else
            {
                // DD
                if (p->i < dimm - 1 && board->hor_walls[p->i + 1][p->j] == 0)
                {
                    if (p->i + 2 == i && p->j == j)
                        return true;
                }
                else
                {
                    // DL
                    if (p->j > 0 && board->ver_walls[p->i + 1][p->j - 1] == 0)
                    {
                        if (p->i + 1 == i && p->j == j + 1)
                            return true;
                    }

                    // DR
                    if (p->j < dimm && board->ver_walls[p->i + 1][p->j] == 0)
                    {
                        if (p->i + 1 == i && p->j + 1 == j)
                            return true;
                    }
                }
            }
            
        }
    }

    // Right
    if (p->j < dimm)
    {
        // fprintf(stderr, "\t\t\t\t\t\t%d %d\n", p->i, p->j);
        if (board->ver_walls[p->i][p->j] == 0)
        {
            // No enemy on the right
            if (e->i != p->i || e->j != p->j + 1)
            {
                // R
                if (p->i == i && p->j + 1 == j)
                    return true;
            }
            else
            {
                // RR
                if (p->j < dimm - 1 && board->ver_walls[p->i][p->j + 1] == 0)
                {
                    if (p->i == i && p->j + 2 == j)
                        return true;
                }
                else
                {
                    // RU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j + 1] == 0)
                    {
                        if (p->i == i + 1 && p->j + 1 == j)
                            return true;
                    }

                    // RD
                    if (p->i < dimm && board->hor_walls[p->i][p->j + 1] == 0)
                    {
                        if (p->i + 1 == i && p->j + 1 == j)
                            return true;
                    }
                }
            }
            
        }
    }

    // Left
    if (p->j > 0)
    {
        if (board->ver_walls[p->i][p->j - 1] == 0)
        {
            // No enemy on the left
            if (e->i != p->i || e->j + 1 != p->j)
            {
                // L
                if (p->i == i && p->j == j + 1)
                    return true;
            }
            else
            {
                // LL
                if (p->j > 1 && board->ver_walls[p->i][p->j - 2] == 0)
                {
                    if (p->i == i && p->j == j + 2)
                        return true;
                }
                else
                {
                    // LU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j - 1] == 0)
                    {
                        if (p->i == i + 1 && p->j == j + 1)
                            return true;
                    }

                    // LD
                    if (p->i < dimm && board->hor_walls[p->i][p->j - 1] == 0)
                    {
                        if (p->i + 1 == i && p->j == j + 1)
                            return true;
                    }
                }
                
            }
            
        }
    }

    return false;

}

static void engine_playmove(Engine engine, char player, uint32_t i, uint32_t j)
{
    if (player != 'B' && player != 'W')
    {
        printf("? Agent must be one of b/black or w/white (no concern for capitalization)\n\n");
        return ;
    }
    

    if (i > engine->board->dimension - 1 || j > engine->board->dimension - 1 || !valid_move(engine->board, player, i, j))
    {
        printf("? illegal move\n\n");
        return ;
    }

    Action* action = malloc(sizeof(*action));
    action->action = MOVE;
    action->agent = player;
    Position agent_pos = player == 'B' ? engine->board->enemy : engine->board->player;

    action->info.move.old_i = agent_pos.i;
    action->info.move.old_j = agent_pos.j;

    action->info.move.new_i = i;
    action->info.move.new_j = j;

    // add move to history
    vector_insert(engine->board->history, action);

    play_action(engine->board, *action);

    printf("=\n\n");

}

static void engine_playwall(Engine engine, char player, uint32_t i, uint32_t j, char ori)
{
    if (player != 'B' && player != 'W')
    {
        printf("? Agent must be one of b/black or w/white (no concern for capitalization)\n\n");
        return ;
    }

    if (ori != 'V' && ori != 'H')
    {
        printf("? Orientation must be one of h/horizontal or v/vertical (no concern for capitalization)\n\n");
        return ;
    }
    
    uint8_t vertical = ori == 'V';

    if (i > engine->board->dimension - 1 || j > engine->board->dimension - 1 || !audit_place_wall(engine->board, i, j, vertical, player))
    {
        printf("? illegal move\n\n");
        return ;
    }

    Action* action = malloc(sizeof(*action));
    action->action = PLACE;
    action->agent = player;

    action->info.wall.x = i;
    action->info.wall.y = j;
    action->info.wall.orientation = vertical;

    vector_insert(engine->board->history, action);
    play_action(engine->board, *action);

    printf("=\n\n");
    

    
}

struct timespec requestStart;

char maximizing;
char minimizing;
double time_left;
static void engine_genmove(Engine engine, char player)
{
    if (player != 'B' && player != 'W')
    {
        printf("? Agent must be one of b/black or w/white (no concern for capitalization)\n\n");
        return ;
    }

    maximizing = player;
    minimizing = (player == 'B') ? 'W' : 'B';

    time_left = 4.5;
    uint32_t depth = 1;
    uint32_t max_depth = 5;
    // uint32_t max_depth = engine->board->dimension << 2;

    Vector moves = vector_create(10, free);

    while (time_left >= 0 && depth <= max_depth)
    {
        if ((vector_size(engine->board->history) << 1) <= engine->board->dimension  && depth >= 4)
            break;

        prunes = 0;
        evaluations = 0;  

        // Calculate time taken by a request
        clock_gettime(CLOCK_REALTIME, &requestStart);

        ActionPair value = alpha_beta(engine->board, depth, 1, -INFINITY, INFINITY, evaluate);

        if (value == NULL)
            break;

        vector_insert(moves, value);

        if (value->evaluation == INFINITY || value->evaluation == -INFINITY)
            break;
        

        // Calculate time it took
        struct timespec requestEnd;
        clock_gettime(CLOCK_REALTIME, &requestEnd);
        double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
        time_left -= accum;

        depth++;

        if (accum >= time_left)
            break;
    }


    ActionPair to_play = vector_get_at(moves, vector_size(moves) - 1);
    play_action(engine->board, *to_play->action);

    vector_insert(engine->board->history, to_play->action);

    if (to_play->action->action == MOVE)
    {
        uint32_t i = engine->board->dimension - to_play->action->info.move.new_i;
        char j = to_play->action->info.move.new_j + 'A';
        printf("= %c%d\n\n", j, i);
    }
    else
    {
        uint32_t i = engine->board->dimension - to_play->action->info.wall.x;
        char j = to_play->action->info.wall.y + 'A';
        char ori = to_play->action->info.wall.orientation ? 'V' : 'H';
        printf("= %c%d %c\n\n", j, i, ori);
    }


    // FILE* f = fopen("depths.txt", "a");
    // fprintf(f, "%ld\n", vector_size(moves));
    // fclose(f);
    

    vector_destroy(moves);   

}


static void engine_undo(Engine engine, int times)
{
    size_t size = vector_size(engine->board->history);

    
    // times is 0 means there was no argument for times
    if (times < 0)
        times = 1;
    else if (times > (int) size || times == 0)
    {
        printf("? Cannot undo\n\n");
        return ;
    }


    for (uint32_t i = 0; i < times; i++)
    {
        Action* ac = vector_get_at(engine->board->history, --size);
        undo_action(engine->board, *ac);
        vector_remove(engine->board->history);
    }

    printf("=\n\n");
}

static void engine_winner(Engine engine)
{
    if (engine->board->player.i == 0)
    {
        printf("= true white\n\n");
        return ;
    }

    if (engine->board->enemy.i == engine->board->dimension - 1)
    {
        printf("= true black\n\n");
        return ;
    }

    printf("= false\n\n");
}

static void engine_showboard(Engine engine)
{
    quoridor_print(engine->board);
    printf("=\n\n");
}

void read_command(Engine engine)
{
    char buffer[1000];

    int res = scanf("%s", buffer);

    if (strcmp(buffer, "name") == 0)
        engine_name(engine);
    else if (strcmp(buffer, "known_command") == 0)
    {
        res = scanf("%s", buffer);
        engine_known_command(buffer);
    }
    else if (strcmp(buffer, "list_commands") == 0)
        engine_list_commands();
    else if (strcmp(buffer, "quit") == 0)
        engine_quit(engine);
    else if (strcmp(buffer, "boardsize") == 0)
    {
        uint32_t size;
        res = scanf("%d", &size);   
        engine_boardsize(engine, size);
        
    }
    else if (strcmp(buffer, "clear_board") == 0)
        engine_clear_board(engine);
    else if (strcmp(buffer, "walls") == 0)
    {
        uint32_t walls;
        res = scanf("%d", &walls);  
        engine_walls(engine, walls);
    }
    else if (strcmp(buffer, "playmove") == 0)
    {
        // check if first letter is W or w for white player and b or B for black player
        res = scanf("%s", buffer);
        
        char player =   (buffer[0] == 'w' || buffer[0] == 'W') ? 'W' : 
                        (buffer[0] == 'b' || buffer[0] == 'B') ? 'B' : '\0'; 

        // j is a letter A to Z or a to z
        char ch;
        res = scanf(" %c", &ch);

        uint32_t j = (ch >= 'A') ? ch - 'A' : ch; 

        // i is a number 1 to board->dimension
        uint32_t i;
        res = scanf("%d", &i);
        i = engine->board->dimension - i;

        engine_playmove(engine, player, i, j);
        
    }
    else if (strcmp(buffer, "playwall") == 0)
    {
        // check if first letter is W or w for white player and b or B for black player
        res = scanf("%s", buffer);
        
        char player =   (buffer[0] == 'w' || buffer[0] == 'W') ? 'W' : 
                        (buffer[0] == 'b' || buffer[0] == 'B') ? 'B' : '\0'; 

        // j is a letter A to Z or a to z
        char ch;
        res = scanf(" %c", &ch);

        uint32_t j = (ch >= 'A') ? ch - 'A' : ch; 

        // i is a number 1 to board->dimension
        uint32_t i;
        res = scanf("%d", &i);
        i = engine->board->dimension - i;

        res = scanf("%s", buffer);
        
        char ori =  (buffer[0] == 'h' || buffer[0] == 'H') ? 'H' : 
                    (buffer[0] == 'v' || buffer[0] == 'V') ? 'V' : '\0'; 


        engine_playwall(engine, player, i, j, ori);
        
    }
    else if (strcmp(buffer, "genmove") == 0)
    {
        res = scanf("%s", buffer);
        
        char player =   (buffer[0] == 'w' || buffer[0] == 'W') ? 'W' : 
                        (buffer[0] == 'b' || buffer[0] == 'B') ? 'B' : '\0'; 
            
        engine_genmove(engine, player);
        
    }
    else if (strcmp(buffer, "undo") == 0)
    {
        fgets(buffer, 2, stdin);
        if (buffer[0] == ' ')
        {
            uint32_t num;
            scanf("%d", &num);
            engine_undo(engine, num);
            return ;
        }
        
        engine_undo(engine, -1);
    }
    else if (strcmp(buffer, "winner") == 0)
        engine_winner(engine);
    else if (strcmp(buffer, "showboard") == 0)
        engine_showboard(engine);
    
    else 
    {
        printf("? Unknown command\n\n");
    }

    fflush(stdout);
}


