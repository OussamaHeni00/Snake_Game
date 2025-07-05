
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
const int WIDTH  = 800 , HEIGHT = 600;
const int TILE_SIZE =20;
const int MAX_LENGTH =1000;

typedef enum{ UP, DOWN,LEFT,RIGHT}Direction;
typedef enum {PLAYING, GAME_OVER} GameState;

void  restGame(SDL_Rect snake[], int* snake_length, SDL_Rect * food, Direction * dir, GameState * state, int *score,Uint32 *startTime){
    *snake_length = 1;
    snake[0].x = WIDTH /2;
    snake[0].y = HEIGHT /2;
    snake[0].w = TILE_SIZE;
    snake[0].h = TILE_SIZE;
    food ->x = (rand() % (WIDTH / TILE_SIZE)) * TILE_SIZE;
    food ->y = (rand() % (HEIGHT / TILE_SIZE)) * TILE_SIZE;
    food->w =  TILE_SIZE;
    food->h = TILE_SIZE;
    * dir = RIGHT;
    * state = PLAYING;
    * score = 0;
    * startTime = SDL_GetTicks();
}
void renderText(SDL_Renderer * renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color){
    SDL_Surface * surface = TTF_RenderText_Solid(font,text,color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x,y,surface ->w,surface ->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer,texture,NULL,&dest);
    SDL_DestroyTexture(texture);
}
int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }
    if(TTF_Init() == -1){
        printf("TTF_Init failed: %s\n",TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf",24);
    if(!font){
        printf("Failed to load font : %s\n", TTF_GetError());
        return 1 ;
    }
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    srand(time(NULL));

    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    if(!window){
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }
    Mix_Chunk *eatSound = Mix_LoadWAV("eat.mp3");
    Mix_Chunk *hitSound = Mix_LoadWAV("hit.mp3");
    Mix_Music *bgMusic = Mix_LoadMUS("music.mp3");
    if (!eatSound || !hitSound || !bgMusic){
        printf("Failed to load audio: %s\n",Mix_GetError());
        return 1;
    }
    Mix_PlayMusic(bgMusic,-1);
    SDL_Rect snake[MAX_LENGTH];
    int snake_length = 1;
    SDL_Rect food ;
    Direction dir = RIGHT;
    GameState state = PLAYING;
    int score = 0;
    bool running = true;
    Uint32 startTime = SDL_GetTicks();
    restGame(snake,&snake_length,&food,&dir,&state,&score,&startTime);
    Uint32 last_move = SDL_GetTicks();
    SDL_Event event;
    char score_text[20];
    char timer_Text[50];
    SDL_Color white ={255,255,255,255};
    bool gameOverSoundPlayed = false;
// Game Looop
    
    while(running){
        
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = false;

        if(event.type == SDL_KEYDOWN){
        switch (event.key.keysym.sym){
        
        case SDLK_UP: if ( dir != DOWN) dir = UP;break;
        case SDLK_DOWN: if ( dir != UP) dir = DOWN;break;
        case SDLK_LEFT: if ( dir != RIGHT) dir = LEFT;break;
        case SDLK_RIGHT: if ( dir != LEFT) dir = RIGHT;break;
        case SDLK_n: restGame(snake,&snake_length,&food,&dir,&state,&score,&startTime);gameOverSoundPlayed = false;break;
        case SDLK_q: running =false; break;
        }
            }
                 }
        if (state ==PLAYING && SDL_GetTicks() - last_move > 100){
            last_move = SDL_GetTicks();
            for( int i = snake_length -1;i >0; i--){
                snake[i] =snake[i-1];
            }
            switch(dir){
                case UP : snake[0].y -= TILE_SIZE; break;
                case DOWN : snake[0].y += TILE_SIZE; break;
                case LEFT : snake[0].x -= TILE_SIZE; break;
                case RIGHT : snake[0].x += TILE_SIZE; break;
            }
            if (snake[0].x <0 || snake[0].x >=WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT)
            state = GAME_OVER;
            
            for (int i=1; i<snake_length ; i++ ){
                    if(snake[0].x == snake[i].x && snake[0].y ==snake[i].y)
                    state = GAME_OVER;
                
        }
        if (snake[0].x == food.x && snake[0].y == food.y) {
            if(snake_length < MAX_LENGTH) snake_length++;
            food.x = (rand() % (WIDTH / TILE_SIZE)) * TILE_SIZE;
            food.y = (rand() % (HEIGHT / TILE_SIZE)) * TILE_SIZE;
            score++;
            Mix_PlayChannel(-1,eatSound,0);
        }
    }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // last number 0 transparent 255 non transparent
        SDL_RenderClear(renderer); // use Setrenderdrawcolor clean the screen and use the color
        Uint32 elapsed =(SDL_GetTicks()- startTime) / 1000;
        char timerText[50];
        sprintf(timerText," %d s",elapsed);
        SDL_Color white = {255,255,255,255};
        if (state == PLAYING){
             SDL_SetRenderDrawColor(renderer,255,0,0,255);
             SDL_RenderFillRect(renderer,&food);
        
             sprintf(score_text,"Score : %d",score);
             renderText(renderer,font,score_text,WIDTH/2-60,HEIGHT -50,white);
             renderText(renderer,font,timerText,WIDTH/2-60,10,white);
             SDL_SetRenderDrawColor(renderer,0,255,0,255);
        for(int i=0;i< snake_length;i++){
            SDL_RenderFillRect(renderer,&snake[i]);
        }
    }else if (state ==GAME_OVER){
        Mix_PlayChannel(-1,hitSound,0);
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        SDL_RenderClear(renderer);
        SDL_Rect panel = {WIDTH/2 - 150, HEIGHT/2 -60,350,160};
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderDrawRect(renderer,&panel);
        
        
        
        sprintf(score_text,"Score : %d",score);
        renderText(renderer,font,"Game Over", WIDTH/2 -80,HEIGHT/2 - 60,white);
        renderText(renderer,font,score_text,WIDTH/2-60,HEIGHT/2-20,white);
        sprintf(timer_Text,"time : %s",timerText);
        renderText(renderer,font,timer_Text,350,10,white);
        renderText(renderer,font,"Press N = New Game | Q = Quit", WIDTH/2 -150,HEIGHT/2 + 20,white);

    }

        SDL_RenderPresent(renderer); // double buffering first draw in back buffer then transport it all to screen
        SDL_Delay(16);
    }
    
    Mix_FreeChunk(eatSound);
    Mix_FreeChunk(hitSound);
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    Mix_Quit();

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;

}