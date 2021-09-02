#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <vector>

using namespace std;

class Game{
private:
    int** game_win_map;
    int shape[4][4];
    int shape_h, shape_w;
    
    WINDOW *game_win, *hint_win, *score_win;
    int game_win_height, game_win_width, hint_win_height, hint_win_width, score_win_height, score_win_width;

    int ishape, head_x, head_y;
    int next_ishape, score;

    int getrand(int n){
        return rand() % n;
    }


public:
    bool gameover;

    void go_windows_program();
    void create_three_windows();
    void update_three_windows();
    void goLeft();
    void goRight();
    void goDown();
    void rotate();
    void setShape(int ishape);
    bool isaggin();
    bool theLineCanOffset(int line);
    void del_all_win();
};

int main(){
    timeval timeout;

    srand((unsigned int)time(0));
    Game game;
    game.go_windows_program();
    game.create_three_windows();
    while(!game.gameover){
        fd_set set;
        FD_ZERO(&set);
        FD_SET(0,&set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        if(select(1,&set,NULL,NULL,&timeout) == 0){
            game.goDown();
        }else{
            int key;
            while((key = getch()) == 's' || key == 'S');
            if(key == KEY_LEFT)game.goLeft();
            if(key == KEY_RIGHT)game.goRight();
            if(key == KEY_DOWN)game.goDown();
            if(key == KEY_UP)game.rotate();
        } 
    }
    game.del_all_win();
    return 0;
}

void Game::go_windows_program(){
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, true);
    refresh();
}

void Game::create_three_windows(){
    game_win_height = 30;
    game_win_width = 40;
    hint_win_height = 10;
    hint_win_width = 30;
    score_win_height = 10;
    score_win_width = 30;

    game_win_map = new int *[game_win_height];
    for(int i = 0;i < game_win_height;i++)game_win_map[i] = new int[game_win_width];

    score = 0;
    gameover = false;
    ishape = getrand(7);
    next_ishape = getrand(7);

    game_win = newwin(game_win_height, game_win_width, 0, 0);
    wborder(game_win,'*','*','*','*','*','*','*','*');
    setShape(ishape);
    head_y = 1;
    head_x = (game_win_width - shape_w) / 2;
    for(int i = 0;i < shape_h;i++){
        for(int j = 0;j < shape_w;j++){
            if(shape[i][j] == 1)mvwaddch(game_win,head_y + i, head_x + j, '#');
        }
    }

    hint_win = newwin(hint_win_height, hint_win_width, 0, game_win_width + 10);
    box(hint_win, 0, 0);
    mvwaddstr(hint_win, 0, 2, "hint");
    setShape(next_ishape);
    for(int i = 0;i < shape_h;i++){
        for(int j = 0;j < shape_w;j++){
            if(shape[i][j] == 1)mvwaddch(hint_win, (hint_win_height - shape_h) / 2 + i, (hint_win_width - shape_w) / 2 + j, '#');
        }
    }

    score_win = newwin(score_win_height, score_win_width, hint_win_height + 10, game_win_width + 10);
    box(score_win, 0, 0);
    mvwaddstr(score_win, 0, 2, "score");
    mvwprintw(score_win, score_win_height / 2, score_win_width / 2 - 2, "%d", score);
    

    wrefresh(game_win);
    wrefresh(hint_win);
    wrefresh(score_win);
    setShape(ishape);
}

void Game::setShape(int ishape){
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
            shape[i][j] = 0;
        }
    }
    switch(ishape){
        case 0:
            shape_h = 1;
            shape_w = 4;
            for(int j = 0;j < 4;j++)shape[0][j] = 1;
            break;
        case 1:
            shape_h = 2;
            shape_w = 3;
            shape[0][0] = 1;
            for(int j = 0;j < 3;j++)shape[1][j] = 1;
            break;
        case 2:
            shape_h = 2;
            shape_w = 3;
            shape[0][2] = 1;
            for(int j = 0;j < 3;j++)shape[1][j] = 1;
            break;
        case 3:
            shape_h = 2;
            shape_w = 3;
            shape[0][1] = 1; shape[0][2] = 1;
            shape[1][0] = 1; shape[1][1] = 1;
            break;
        case 4:
            shape_h = 2;
            shape_w = 3;
            shape[0][0] = 1; shape[0][1] = 1;
            shape[1][1] = 1; shape[1][2] = 1;
            break;
        case 5:
            shape_h = 2;
            shape_w = 2;
            shape[0][0] = 1; shape[0][1] = 1;
            shape[1][0] = 1; shape[1][1] = 1;
            break;
        case 6:
            shape_h = 2;
            shape_w = 3;
            shape[0][1] = 1;
            for(int j = 0;j < 3;j++)shape[1][j] = 1;
            break;
    }
}

bool Game::isaggin(){
    if(head_y <= 0 || head_y + shape_h - 1 >= game_win_height - 1 || head_x <= 0 || head_x + shape_w - 1 >= game_win_width - 1)return true;
    for(int i = 0;i < shape_h;i++){
        for(int j = 0;j < shape_w;j++){ 
            if(shape[i][j] == 1 && game_win_map[head_y + i][head_x + j] == 1)return true;
        }
    }
    return false;
}

void Game::goDown(){
    head_y++;
    if(isaggin()){
        head_y--;
        update_three_windows();
    }else{
        for(int i = shape_h - 1;i >= 0;i--){
            for(int j = 0;j < shape_w;j++){
                if(shape[i][j] == 1){
                    mvwaddch(game_win, head_y - 1 + i, head_x + j, ' ');
                    mvwaddch(game_win, head_y + i, head_x + j, '#');
                }
            }
            wrefresh(game_win);
        }
    }
}

void Game::goLeft(){
    head_x--;
    if(isaggin()){
        head_x++;
    }else{
        for(int j = 0;j < shape_w;j++){
            for(int i = 0;i < shape_h;i++){
                if(shape[i][j] == 1){
                    mvwaddch(game_win, head_y + i, head_x + 1 + j, ' ');
                    mvwaddch(game_win, head_y + i, head_x + j, '#');
                }
            }
            wrefresh(game_win);
        }
    }
}

void Game::goRight(){
    head_x++;
    if(isaggin()){
        head_x--;
    }else{
        for(int j = shape_w - 1;j >= 0;j--){
            for(int i = 0;i < shape_h;i++){
                if(shape[i][j] == 1){
                    mvwaddch(game_win, head_y + i, head_x - 1 + j, ' ');
                    mvwaddch(game_win, head_y + i, head_x + j, '#');
                }
            }
            wrefresh(game_win);
        }
    }
}

void Game::rotate(){
    int tmp[4][4] = {0};
    int tmp_h = shape_w, tmp_w = shape_h;
    for(int i = 0;i < tmp_h;i++){
        for(int j = 0;j < tmp_w;j++){
            tmp[i][j] = shape[shape_h - 1 - j][i];
        }
    }
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
            swap(shape[i][j], tmp[i][j]);  
        }
    }
    swap(shape_h, tmp_h);
    swap(shape_w, tmp_w);
    if(isaggin()){
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                swap(shape[i][j], tmp[i][j]);
            }
        }
        swap(shape_h, tmp_h);
        swap(shape_w, tmp_w);
    }else{
        for(int i = 0;i < tmp_h;i++){
            for(int j = 0;j < tmp_w;j++){
                if(tmp[i][j] == 1)mvwaddch(game_win, head_y + i, head_x + j, ' ');
            }
        }
        for(int i = 0;i < shape_h;i++){
            for(int j = 0;j < shape_w;j++){
                if(shape[i][j] == 1)mvwaddch(game_win, head_y + i, head_x + j, '#');
            }
        }
    }
    wrefresh(game_win);
}

void::Game::update_three_windows(){
    score += 10;
    for(int i = 0;i < shape_h;i++){
        for(int j = 0;j < shape_w;j++){
            if(shape[i][j] == 1)game_win_map[head_y + i][head_x + j] = 1;
        }
    }
    for(int i = game_win_height - 2; i >= 1;i--){
        while(theLineCanOffset(i)){
            score += 50;
            for(int j = 1;j <= game_win_width - 2;j++){
                game_win_map[i][j] = 0;
                mvwaddch(game_win, i, j, ' ');
            }
            for(int ii = i - 1;ii >= 1;ii--){
                for(int jj = 1;jj <= game_win_width - 2;jj++){
                    game_win_map[ii + 1][jj] = game_win_map[ii][jj];
                    game_win_map[ii][jj] = 0;
                    mvwaddch(game_win, ii, jj, ' ');
                    if(game_win_map[ii + 1][jj] == 0)mvwaddch(game_win, ii + 1, jj, ' ');
                    else mvwaddch(game_win, ii + 1, jj, '#');
                }
            }
        }
        wrefresh(game_win);
    }
    ishape = next_ishape;
    next_ishape = getrand(7);
    setShape(ishape);
    head_y = 1;
    head_x = (game_win_width - shape_w) / 2;
    if(isaggin())gameover = true;
    else{
        for(int i = 0;i < shape_h;i++){
            for(int j = 0;j < shape_w;j++){
                if(shape[i][j] == 1)mvwaddch(game_win, head_y + i, head_x + j, '#');
            }
        }
    }
    wrefresh(game_win);
    setShape(next_ishape);
    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
            mvwaddch(hint_win, (hint_win_height - 4) / 2 + i, (hint_win_width - 4) / 2 + j, ' ');
        }
    }
    for(int i = 0;i < shape_h;i++){
        for(int j = 0;j < shape_w;j++){
            if(shape[i][j] == 1)mvwaddch(hint_win, (hint_win_height - shape_h) / 2 + i, (hint_win_width - shape_w) / 2 + j, '#');
        }
    }
    wrefresh(hint_win);
    mvwprintw(score_win, score_win_height / 2, score_win_width / 2 - 2, "%d", score);
    wrefresh(score_win);
    setShape(ishape);
}

bool Game::theLineCanOffset(int line){
    for(int j = 1;j <= game_win_width - 2;j++){
        if(game_win_map[line][j] == 0)return false;
    }
    return true;
}

void Game::del_all_win(){
    delwin(game_win);
    delwin(hint_win);
    delwin(score_win);
    system("clear");
    int y,x;
    getmaxyx(stdscr, y, x);
    mvprintw(y / 2 - 1, x / 2 - 12, "GameOver, your score is %d", score);
    mvprintw(y / 2 + 1, x / 2 - 24, "Game will exit automatically after five seconds");
    refresh();
    sleep(5);
    endwin();
}