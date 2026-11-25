#include "raylib.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// Forward declarations
static void InitGame(void);         // Initialize game
static void UpdateGame(void);
static void DrawGame(void);

// Global variables

// Screen
static const int SCREEN_WIDTH = 600;
static const int SCREEN_HEIGHT = 450;
static const int FPS = 60;
static const int BORDER_OFFSET = 15; // 
static const int TOP_BOTTOM_RECT_HEIGHT = 10;
static const int WALL_OFFSET = 10;

// Racket
static const int SMALL_RACKET_HEIGHT = 36;
static const int MEDIUM_RACKET_HEIGHT = 72;
static const int LARGE_RACKET_HEIGHT = 108;

static const int RACKET_WIDTH = 10;
static const int RACKET_X_POSITION = SCREEN_WIDTH - BORDER_OFFSET - RACKET_WIDTH; 
static const int PIXEL_MOVEMENT_PER_CLICK = 5;

// Dashed line down the middle
static const int DASHED_LINE_WIDTH = 10;
static const int DASHED_LINE_HEIGHT = 15;
static const int DASHED_LINE_GAP = 10;
static const int MIDDLE_X = SCREEN_WIDTH / 2;
static const int DASHED_LINE_START_Y = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;
static const int DASHED_LINE_END_Y = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT - DASHED_LINE_GAP - DASHED_LINE_HEIGHT;

// Score
static const int FONT_SIZE = 40;
static const int TENS_DIGIT_X_POSITION = (SCREEN_WIDTH / 4);
static const int TENS_DIGIT_Y_POSITION = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT + 10;
static const int ONES_DIGIT_X_POSITION = TENS_DIGIT_X_POSITION + 30;
static const int ONES_DIGIT_Y_POSITION = TENS_DIGIT_Y_POSITION;

// Ball
static const int BALL_RADIUS = 7;
static const int BALL_SPEED = 10;

static const int BALL_START_POS_X1 = MIDDLE_X + 20;
static const int BALL_START_POS_X2 = MIDDLE_X - 20;
static const int BALL_START_POS_Y = DASHED_LINE_START_Y + 5;

// Structs
struct Player {
    int y_position {};
    int score {};
    int racket_size {};
};

struct Ball {
    Vector2 position {};
    Vector2 speed {};
    int radius {};
    bool active {};
};

// Local game variables
static bool game_over = false;
static bool first_attempt = true;
static Player player = { 0 };
static Ball ball = { 0 };
static Rectangle top = { 0 };
static Rectangle bottom = { 0 };
static Rectangle wall = { 0 };
static Sound SOUND_PADDLE_HIT = {};

int main() {
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    InitGame();
    player.racket_size = MEDIUM_RACKET_HEIGHT;

    InitAudioDevice();
    SOUND_PADDLE_HIT = LoadSound("sounds/retro-pong-hit-sfx.wav");

    SetTargetFPS(FPS);


    // Main game loop
    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    // De-initialization
    CloseWindow();
    UnloadSound(SOUND_PADDLE_HIT);
    CloseAudioDevice();

    return 0;
}

// Start & reset the game
void InitGame(void) {
    // Boundary rectangles on the top and bottom
    top.x = BORDER_OFFSET;
    top.y = BORDER_OFFSET;
    top.width = SCREEN_WIDTH - (2 * BORDER_OFFSET);
    top.height = TOP_BOTTOM_RECT_HEIGHT;

    bottom.x = BORDER_OFFSET;
    bottom.y = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT;
    bottom.width = SCREEN_WIDTH - (2 * BORDER_OFFSET);
    bottom.height = TOP_BOTTOM_RECT_HEIGHT;

    // Wall on the left side
    wall.x = BORDER_OFFSET + WALL_OFFSET;
    wall.y = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;
    wall.width = TOP_BOTTOM_RECT_HEIGHT;
    wall.height = SCREEN_HEIGHT - (2 * (BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT));

    // Initialize player
    // player.racket_size = LARGE_RACKET_HEIGHT;
    player.y_position = (SCREEN_HEIGHT / 2) - (player.racket_size / 2); // Initialize the racket to be at the center of the screen so any adjusted racket size works
    player.score = 0;

    // Set the ball's starting position randomly
    ball.radius = BALL_RADIUS;
    ball.active = false;
    if (GetRandomValue(0, 1)) {
        ball.position.x = BALL_START_POS_X1;
    } else {
        ball.position.x = BALL_START_POS_X2;
    }
    ball.position.y = BALL_START_POS_Y;
}

void UpdateGame(void) {

    if (!game_over) {
        int bottom_racket_boundary = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT - player.racket_size;
        int top_racket_boundary = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;

        // Update position of the racket based on user input (up & down arrows)
        if (IsKeyDown(KEY_DOWN)) {
            player.y_position += PIXEL_MOVEMENT_PER_CLICK;
        }

        if (player.y_position >= bottom_racket_boundary) {
            player.y_position = bottom_racket_boundary; // Paddle can't go below the lower wall
        }

        if (IsKeyDown(KEY_UP)) {
            player.y_position -= PIXEL_MOVEMENT_PER_CLICK;
        }

        if (player.y_position <= top_racket_boundary) {
            player.y_position = top_racket_boundary; // Paddle can't go above the upper wall
        }

        // Ball launching logic
        if (!ball.active) {
            if (IsKeyPressed(KEY_SPACE) || !first_attempt) 
            {
                ball.active = true;
                first_attempt = false;
                
                // Set initial ball speed randomly
                if (GetRandomValue(0, 1)) {
                    ball.speed.x = 1;
                    ball.speed.y = 1;
                } else {
                    ball.speed.x = -1;
                    ball.speed.y = 1;
                }

                // Set initial ball speed to move at a 45 degree angle
                float rescale_factor = BALL_SPEED / (sqrt(pow(ball.speed.x, 2) + pow(ball.speed.y, 2))); // Scale so total speed is BALL_SPEED
                ball.speed.x *= rescale_factor;
                ball.speed.y *= rescale_factor;
            }
        }

        // Ball movement logic
        if (ball.active) {
            ball.position.x += ball.speed.x;
            ball.position.y += ball.speed.y;

            // Collision: ball vs. top or botttom
            if ((CheckCollisionCircleRec(ball.position, ball.radius, top) && ball.speed.y < 0) || (CheckCollisionCircleRec(ball.position, ball.radius, bottom) && ball.speed.y > 0)) {
                ball.speed.y *= -1;
                SetSoundPitch(SOUND_PADDLE_HIT, 1.0f);
                PlaySound(SOUND_PADDLE_HIT);
            }

            // Collision: ball vs. wall
            if (CheckCollisionCircleRec(ball.position, ball.radius, wall)) {
                ball.speed.x *= -1;
                SetSoundPitch(SOUND_PADDLE_HIT, 1.0f);
                PlaySound(SOUND_PADDLE_HIT);
            }

            // Collision: ball hits the racket
            Rectangle player_racket = {RACKET_X_POSITION, static_cast<float>(player.y_position), RACKET_WIDTH, static_cast<float>(player.racket_size)};
            if (CheckCollisionCircleRec(ball.position, ball.radius, player_racket) && ball.speed.x > 0) {
                ball.speed.x *= -1;
                ball.speed.y = 6 * (((ball.position.y - player.y_position) / (player.racket_size / 2)) - 1);

                float rescale_factor = BALL_SPEED / (sqrt(pow(ball.speed.x, 2) + pow(ball.speed.y, 2))); // scale so total speed is BALL_SPEED
                ball.speed.x *= rescale_factor;
                ball.speed.y *= rescale_factor;

                player.score++; // Update score
                SetSoundPitch(SOUND_PADDLE_HIT, 1.5f);
                PlaySound(SOUND_PADDLE_HIT);

                if (player.score == 99) {
                    game_over = true;
                }
            }

            // Miss: ball goes past paddle
            if ((ball.position.x - ball.radius) > SCREEN_WIDTH || (ball.position.y + ball.radius) < 0 || (ball.position.y - ball.radius) > SCREEN_HEIGHT) {
                game_over = true;

                SetSoundPitch(SOUND_PADDLE_HIT, 0.5f);
                PlaySound(SOUND_PADDLE_HIT);
            }
        }

    } else {
        if (IsKeyPressed(KEY_S)) {
            InitGame();
            player.racket_size = SMALL_RACKET_HEIGHT;
            game_over = false;
        }
        else if (IsKeyPressed(KEY_M)) {
            InitGame();
            player.racket_size = MEDIUM_RACKET_HEIGHT;
            game_over = false;
        }
        else if (IsKeyPressed(KEY_L)) {
            InitGame();
            player.racket_size = LARGE_RACKET_HEIGHT;
            game_over = false;
        }
    }
}

void DrawGame(void) {
    BeginDrawing();

    // Black background
    ClearBackground(BLACK);

    // Draw top and bottom rectangles in white
    DrawRectangle(top.x, top.y, top.width, top.height, WHITE);
    DrawRectangle(bottom.x, bottom.y, bottom.width, bottom.height, WHITE);

    // Draw wall
    DrawRectangle(wall.x, wall.y, wall.width, wall.height, WHITE);

    // Draw dashed line down the middle
    for (int cur_y = DASHED_LINE_START_Y; cur_y <= DASHED_LINE_END_Y; cur_y += (DASHED_LINE_GAP + DASHED_LINE_HEIGHT)) {
        DrawRectangle(MIDDLE_X, DASHED_LINE_GAP + cur_y, DASHED_LINE_WIDTH, DASHED_LINE_HEIGHT, WHITE);
    }

    // Draw racket position
    DrawRectangle(RACKET_X_POSITION, player.y_position, RACKET_WIDTH, player.racket_size, WHITE);

    // Draw the ball position
    if (ball.active) {
        DrawCircle(ball.position.x, ball.position.y, ball.radius, WHITE);
    }

    // Draw the score
    string ones_digit = to_string(player.score % 10);
    string tens_digit = to_string(player.score / 10);

    DrawText(ones_digit.c_str(), ONES_DIGIT_X_POSITION, ONES_DIGIT_Y_POSITION, FONT_SIZE, WHITE);

    if (player.score >= 10) {
        DrawText(tens_digit.c_str(), TENS_DIGIT_X_POSITION, TENS_DIGIT_Y_POSITION, FONT_SIZE, WHITE);
    }

    if (first_attempt) {
        DrawText("PRESS SPACE TO START", GetScreenWidth() / 2 - MeasureText("PRESS SPACE TO START", 20) / 2, GetScreenHeight() / 2 - 30, 20, GRAY);
    }

    // Draw game over screen
    if (game_over) {
        DrawText("PRESS S, M, or L TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS S, M, or L TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 30, 20, GRAY);
    }

    EndDrawing();
}
