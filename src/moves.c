#include "moves.h"
#include "quoridor.h"
#include "list.h"

static void unplace_wall(Board board, uint32_t i, uint32_t j, uint32_t vertical, char agent)
{
    uint32_t* walls = (agent == 'B') ? &board->enemy_walls : &board->player_walls;
    *walls += 1;

    board->nodes[i][j] = 0;

    if (vertical)
    {
        board->ver_walls[i][j] = 0;
        board->ver_walls[i + 1][j] = 0;

        return ;
    }

    board->hor_walls[i][j] = 0;
    board->hor_walls[i][j + 1] = 0;
}

static void place_wall(Board board, uint32_t i, uint32_t j, uint32_t vertical, char agent)
{
    uint32_t* walls = (agent == 'B') ? &board->enemy_walls : &board->player_walls;
    *walls -= 1;

    board->nodes[i][j] = 1;

    if (vertical)
    {
        board->ver_walls[i][j] = 1;
        board->ver_walls[i + 1][j] = 1;
        return ;
    }

    board->hor_walls[i][j] = 1;
    board->hor_walls[i][j + 1] = 1;
    
}

static void move_player(Board board, char player, uint32_t i, uint32_t j)
{
    Position* agent = (player == 'B') ? &board->enemy : &board->player;
    agent->i = i;
    agent->j = j;
}


typedef struct 
{
    uint32_t i;
    uint32_t j;
    uint32_t dist;
} * Tuple;

static Tuple create_tuple(uint32_t i, uint32_t j, uint32_t dist)
{
    Tuple p = malloc(sizeof(*p));
    p->i = i;
    p->j = j;
    p->dist = dist;
    return p;
}

static Cells* get_dist(Board b)
{
    size_t size = b->dimension;

    Cells* c = malloc(sizeof(*c) * 2);

    c[0] = malloc(sizeof(**c) * size);
    c[1] = malloc(sizeof(**c) * size);

    for (size_t i = 0; i < size; i++)
    {
        c[0][i] = malloc(sizeof(***c) * size);
        c[1][i] = malloc(sizeof(***c) * size);

        for (size_t j = 0; j < size; j++)
        {
            c[0][i][j] = UINT8_MAX;
            c[1][i][j] = UINT8_MAX;
        }
    }

    List f = list_create(NULL, free);
    List f1 = list_create(NULL, free);
    Cells temp = malloc(sizeof(*temp) * size);

    for (size_t j = 0; j < size; j++)
    {
        temp[j] = calloc(sizeof(**temp), size);
        temp[0][j] = 1;
        list_insert_next(f, LIST_FIRST, create_tuple(0, j, 0));
    }

    size_t dim = size - 1;

    for (ListNode node = list_first(f); node != NULL; node = list_next(node))
    {
        // printf("hehehe%d\n", list_size(f));
        Tuple t = list_node_value(node);
        uint32_t curr_dist = t->dist;

        size_t i = t->i;
        size_t j = t->j;

        c[0][i][j] = curr_dist;

        // U
        if (i > 0 && b->hor_walls[i - 1][j] == 0 && temp[i - 1][j] == 0)
        {
            // printf("Inserting %d %d\n", );
            temp[i - 1][j] = 1;
            list_insert_next(f, LIST_LAST, create_tuple(i - 1, j, curr_dist + 1));
        }
        
        // D
        if (i < dim && b->hor_walls[i][j] == 0 && temp[i + 1][j] == 0)
        {
            // printf("Inserting %d %d\n", );
            temp[i + 1][j] = 1;
            list_insert_next(f, LIST_LAST, create_tuple(i + 1, j, curr_dist + 1));
        }
        

        // L
        if (j > 0 && b->ver_walls[i][j - 1] == 0 && temp[i][j - 1] == 0)
        {
            // printf("Inserting %d %d\n", );
            temp[i][j - 1] = 1;
            list_insert_next(f, LIST_LAST, create_tuple(i, j - 1, curr_dist + 1));
        }

        // R
        if (j < dim && b->ver_walls[i][j] == 0 && temp[i][j + 1] == 0)
        {
            // printf("Inserting %d %d\n", i, j + 1);
            temp[i][j + 1] = 1;
            list_insert_next(f, LIST_LAST, create_tuple(i, j + 1, curr_dist + 1));
        }
    }

    list_destroy(f);

    for (size_t j = 0; j < size; j++)
    {
        for (size_t i = 0; i < dim; i++)
            temp[i][j] = 0;
        
        temp[dim][j] = 1;
        list_insert_next(f1, LIST_FIRST, create_tuple(dim, j, 0));
    }

    for (ListNode node = list_first(f1); node != NULL; node = list_next(node))
    {
        Tuple t = list_node_value(node);
        uint32_t curr_dist = t->dist;

        size_t i = t->i;
        size_t j = t->j;

        c[1][i][j] = curr_dist;

        // U
        if (i > 0 && b->hor_walls[i - 1][j] == 0 && temp[i - 1][j] == 0)
        {
            temp[i - 1][j] = 1;
            list_insert_next(f1, LIST_LAST, create_tuple(i - 1, j, curr_dist + 1));
        }
        
        // D
        if (i < dim && b->hor_walls[i][j] == 0 && temp[i + 1][j] == 0)
        {
            temp[i + 1][j] = 1;
            list_insert_next(f1, LIST_LAST, create_tuple(i + 1, j, curr_dist + 1));
        }
        

        // L
        if (j > 0 && b->ver_walls[i][j - 1] == 0 && temp[i][j - 1] == 0)
        {
            temp[i][j - 1] = 1;
            list_insert_next(f1, LIST_LAST, create_tuple(i, j - 1, curr_dist + 1));
        }

        // R
        if (j < dim && b->ver_walls[i][j] == 0 && temp[i][j + 1] == 0)
        {
            temp[i][j + 1] = 1;
            list_insert_next(f1, LIST_LAST, create_tuple(i, j + 1, curr_dist + 1));
        }
    }

    list_destroy(f1);


    for (size_t i = 0; i < size; i++)
        free(temp[i]);
    
    free(temp);

    return c;
}

static void destroy_cellsp(Cells* c, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(c[0][i]);
        free(c[1][i]);
    }

    free(c[0]);
    free(c[1]);
    free(c);
}

// Simple recursive dfs
// Returns false if no cycle was detected -> Agent has at list one path to its goal
// Otherwise returns false
static bool walls_cycle_rec(Board board, Cells temp_board, uint32_t i, uint32_t j, uint32_t goal)
{
    if (i == goal)
        return false;

    temp_board[i][j] = 1;

    uint32_t dim = board->dimension - 1;

    // U
    if (i > 0 && board->hor_walls[i - 1][j] == 0)
    {
        if (temp_board[i - 1][j] == 0 && !walls_cycle_rec(board, temp_board, i - 1, j, goal))
            return false;        
    }
    
    // D
    if (i < dim && board->hor_walls[i][j] == 0)
    {
        if (temp_board[i + 1][j] == 0 && !walls_cycle_rec(board, temp_board, i + 1, j, goal))
            return false;
    }

    // L
    if (j > 0 && board->ver_walls[i][j - 1] == 0)
    {
        if (temp_board[i][j - 1] == 0 && !walls_cycle_rec(board, temp_board, i, j - 1, goal))
            return false;
    }

    // R
    if (j < dim && board->ver_walls[i][j] == 0)
    {
        if (temp_board[i][j + 1] == 0 && !walls_cycle_rec(board, temp_board, i, j + 1, goal))
            return false;
    }

    return true;

}

static bool walls_cycle(Board board, char agent)
{
    Position* pos = agent == 'B' ? &board->enemy : &board->player;

    uint32_t goal = agent == 'B' ? board->dimension - 1 : 0;

    // Visited board
    Cells temp = malloc(sizeof(uint8_t*) * board->dimension);

    for (size_t i = 0; i < board->dimension; i++)
        temp[i] = calloc(board->dimension, sizeof(uint8_t));
    

    bool res = walls_cycle_rec(board, temp, pos->i, pos->j, goal);
    
    for (size_t i = 0; i < board->dimension; i++)
        free(temp[i]);

    free(temp);

    return res;   

}

bool audit_place_wall(Board board, uint32_t i, uint32_t j, uint8_t vertical, char agent)
{
    // Out of walls
    if (agent == 'B')
    {
        if (board->enemy_walls == 0)
            return false;
    }
    else 
    {
        if (board->player_walls == 0)
            return false;
    }

    // Out of bounds
    if (i < 0 || i >= board->dimension - 1 || j < 0 || j >= board->dimension - 1)
        return false;

    if (board->nodes[i][j])
        return false;


    if ((vertical && (board->ver_walls[i][j] || board->ver_walls[i + 1][j])) || 
        (!vertical && (board->hor_walls[i][j] || board->hor_walls[i][j + 1])) )
        return false;

    place_wall(board, i, j, vertical, agent);

    bool res = walls_cycle(board, 'W');
    bool res1 = walls_cycle(board, 'B'); 

    // Maybe use that and store it for future use (?)
    // Should be somewhat time consuming but could possibly help with a-b pruning and therefore become overall faster

    // Cells* c = get_dist(board);
    // bool res = c[0][i][j] == UINT8_MAX;
    // bool res1 = c[1][enemy->i][enemy->j] == UINT8_MAX;
    // destroy_cellsp(c, board->dimension);
    
    unplace_wall(board, i, j, vertical, agent);

    return res == false && res1 == false;
}


void get_distance(Board board, uint8_t* b, uint8_t* w)
{
    Cells* c = get_dist(board);
    *w = c[0][board->player.i][board->player.j];
    *b = c[1][board->enemy.i][board->enemy.j];
    destroy_cellsp(c, board->dimension);
}


void play_action(Board board, Action action)
{
    if (action.action == MOVE)
        move_player(board, action.agent, action.info.move.new_i, action.info.move.new_j);
    else
        place_wall(board, action.info.wall.x, action.info.wall.y, action.info.wall.orientation, action.agent);
}

void undo_action(Board board, Action action)
{
    if (action.action == MOVE)
        move_player(board, action.agent, action.info.move.old_i, action.info.move.old_j);
    else
        unplace_wall(board, action.info.wall.x, action.info.wall.y, action.info.wall.orientation, action.agent);
}



static void insert_move(Board board, char agent, uint32_t new_i, uint32_t new_j, Vector actions)
{
    
    Action* action = malloc(sizeof(*action));
    action->action = MOVE;
    action->agent = agent;
    Position agent_pos = agent == 'B' ? board->enemy : board->player;

    action->info.move.old_i = agent_pos.i;
    action->info.move.old_j = agent_pos.j;

    action->info.move.new_i = new_i;
    action->info.move.new_j = new_j;
    
    vector_insert(actions, action);
}

static uint32_t abs_value(int value)
{
    if (value < 0)
        return -value;
    
    return value;
}

static void insert_place(Board board, char agent, uint32_t x, uint32_t y, uint8_t orientation, Vector actions)
{
    if (audit_place_wall(board, x, y, orientation, agent))
    {
        Action* action = malloc(sizeof(*action));
        action->action = PLACE;
        action->agent = agent;

        action->info.wall.x = x;
        action->info.wall.y = y;
        action->info.wall.orientation = orientation;

        Position* ep = (agent == 'B') ? &board->player : &board->enemy;

        // Manhattan distance to enemy
        action->info.wall.dist_from_enemy = abs_value(x - ep->i) + abs_value(y - ep->j);

        vector_insert(actions, action);
    }
}

// With move sorting the following two can become one
// This has the most basic of sorting imaginable 
// Goal is down -> down first, then up
// Goal is up -> up first, then down
static Vector get_legal_moves_black(Board board)
{
    Vector actions = vector_create(10, free);

    Position* p = &board->enemy;
    Position* e = &board->player;

    uint32_t dimm = board->dimension - 1;

    // Down
    if (p->i < dimm)
    {
        if (board->hor_walls[p->i][p->j] == 0)
        {
            // No enemy on the right
            if (e->i != p->i + 1 || e->j != p->j)
                // D
                insert_move(board, 'B', p->i + 1, p->j, actions);
            else
            {
                // DD
                if (p->i < dimm - 1 && board->hor_walls[p->i + 1][p->j] == 0)
                    insert_move(board, 'B', p->i + 2, p->j, actions);
                else
                {
                    // DL
                    if (p->j > 0 && board->ver_walls[p->i + 1][p->j - 1] == 0)
                        insert_move(board, 'B', p->i + 1, p->j - 1, actions);

                    // DR
                    if (p->j < dimm && board->ver_walls[p->i + 1][p->j] == 0)
                        insert_move(board, 'B', p->i + 1, p->j + 1, actions);
                }
                
            }
            
        }
    }

    // Up
    if (p->i > 0)
    {
        if (board->hor_walls[p->i - 1][p->j] == 0)
        {
            // No enemy on the right
            if (e->i + 1 != p->i || e->j != p->j)
                // U
                insert_move(board, 'B', p->i - 1, p->j, actions);
            else
            {
                // UU
                if ((p->i > 1) && board->hor_walls[p->i - 2][p->j] == 0)
                    insert_move(board, 'B', p->i - 2, p->j, actions);
                else
                {
                    // UL
                    if (p->j > 0 && board->ver_walls[p->i - 1][p->j - 1] == 0)
                        insert_move(board, 'B', p->i - 1, p->j - 1, actions);

                    // UR
                    if (p->j < dimm && board->ver_walls[p->i - 1][p->j] == 0)
                        insert_move(board, 'B', p->i - 1, p->j + 1, actions);
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
                // R
                insert_move(board, 'B', p->i, p->j + 1, actions);
            else
            {
                // RR
                if (p->j < dimm - 1 && board->ver_walls[p->i][p->j + 1] == 0)
                    insert_move(board, 'B', p->i, p->j + 2, actions);
                else
                {
                    // RU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j + 1] == 0)
                        insert_move(board, 'B', p->i - 1, p->j + 1, actions);

                    // RD
                    if (p->i < dimm && board->hor_walls[p->i][p->j + 1] == 0)
                        insert_move(board, 'B', p->i + 1, p->j + 1, actions);
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
                // L
                insert_move(board, 'B', p->i, p->j - 1, actions);
            else
            {
                // LL
                if (p->j > 1 && board->ver_walls[p->i][p->j - 2] == 0)
                    insert_move(board, 'B', p->i, p->j - 2, actions);
                else
                {
                    // LU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j - 1] == 0)
                        insert_move(board, 'B', p->i - 1, p->j - 1, actions);

                    // LD
                    if (p->i < dimm && board->hor_walls[p->i][p->j - 1] == 0)
                        insert_move(board, 'B', p->i + 1, p->j - 1, actions);
                }
                
            }
            
        }
    }

    return actions;
}

static Vector get_legal_moves_white(Board board)
{
    Vector actions = vector_create(10, free);

    Position* p = &board->player;
    Position* e = &board->enemy;

    uint32_t dimm = board->dimension - 1;

    // Up
    if (p->i > 0)
    {
        if (board->hor_walls[p->i - 1][p->j] == 0)
        {
            // No enemy on the right
            if (e->i + 1 != p->i || e->j != p->j)
                // U
                insert_move(board, 'W', p->i - 1, p->j, actions);
            else
            {
                // UU
                if ((p->i > 1) && board->hor_walls[p->i - 2][p->j] == 0)
                    insert_move(board, 'W', p->i - 2, p->j, actions);
                else
                {
                    // UL
                    if (p->j > 0 && board->ver_walls[p->i - 1][p->j - 1] == 0)
                        insert_move(board, 'W', p->i - 1, p->j - 1, actions);

                    // UR
                    if (p->j < dimm && board->ver_walls[p->i - 1][p->j] == 0)
                        insert_move(board, 'W', p->i - 1, p->j + 1, actions);
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
                // D
                insert_move(board, 'W', p->i + 1, p->j, actions);
            else
            {
                // DD
                if (p->i < dimm - 1 && board->hor_walls[p->i + 1][p->j] == 0)
                    insert_move(board, 'W', p->i + 2, p->j, actions);
                else
                {
                    // DL
                    if (p->j > 0 && board->ver_walls[p->i + 1][p->j - 1] == 0)
                        insert_move(board, 'W', p->i + 1, p->j - 1, actions);

                    // DR
                    if (p->j < dimm && board->ver_walls[p->i + 1][p->j] == 0)
                        insert_move(board, 'W', p->i + 1, p->j + 1, actions);
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
                // R
                insert_move(board, 'W', p->i, p->j + 1, actions);
            else
            {
                // RR
                if (p->j < dimm - 1 && board->ver_walls[p->i][p->j + 1] == 0)
                    insert_move(board, 'W', p->i, p->j + 2, actions);
                else
                {
                    // RU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j + 1] == 0)
                        insert_move(board, 'W', p->i - 1, p->j + 1, actions);

                    // RD
                    if (p->i < dimm && board->hor_walls[p->i][p->j + 1] == 0)
                        insert_move(board, 'W', p->i + 1, p->j + 1, actions);
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
                // L
                insert_move(board, 'W', p->i, p->j - 1, actions);
            else
            {
                // LL
                if (p->j > 1 && board->ver_walls[p->i][p->j - 2] == 0)
                    insert_move(board, 'W', p->i, p->j - 2, actions);
                else
                {
                    // LU
                    if (p->i > 0 && board->hor_walls[p->i - 1][p->j - 1] == 0)
                        insert_move(board, 'W', p->i - 1, p->j - 1, actions);

                    // LD
                    if (p->i < dimm && board->hor_walls[p->i][p->j - 1] == 0)
                        insert_move(board, 'W', p->i + 1, p->j - 1, actions);
                }
                
            }
            
        }
    }

    return actions;
}

 
static int compare_walls(const void* a, const void* b)
{
    return ((Action*)a)->info.wall.dist_from_enemy - ((Action*)b)->info.wall.dist_from_enemy;
}

Vector get_legal_actions(Board board, char agent)
{
    
    // If we calculate and store the distance of each cell ot the goal during the cycle check (as describedf there)
    // we can sort the moves simply by choosing first the move with the least distance to the goal, which is a good criteria
    Vector actions = (agent == 'B') ? get_legal_moves_black(board) : get_legal_moves_white(board);

    size_t len = vector_size(actions);

    for (uint32_t i = 0, size = board->dimension - 1; i < size; i++)
        for (uint32_t j = 0; j < size; j++)
        {
            insert_place(board, agent, i, j, 0, actions);
            insert_place(board, agent, i, j, 1, actions);
        }
    
    // Could also sort by distance to player 
    // Something like sort by value of wall where value of wall = min{dist from enemy, dist from player}
    // Supporting walls for player and walls that make it hard for the enemy to go to the goal state
    vector_qsort(actions, compare_walls, len);


    return actions;
}

