#include "raylib.h"
#include <string>

using namespace std;

// Forward declarations
static void InitGame(void);         // Initialize game
static void UpdateGame(void);
static void DrawGame(void);
static void UpdateDrawFrame(void);  // Update game & draw

// Global variables

// Screen
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 500;
static const int FPS = 60;
static const int BORDER_OFFSET = 15; // 
static const int TOP_BOTTOM_RECT_HEIGHT = 10;
static const int WALL_OFFSET = 10;

// Racket
static const int SMALL_RACKET_HEIGHT = 20;
static const int MEDIUM_RACKET_HEIGHT = 40;
static const int LARGE_RACKET_HEIGHT = 60;

static const int RACKET_WIDTH = 10;
static const int RACKET_X_POSITION = SCREEN_WIDTH - BORDER_OFFSET - RACKET_WIDTH; 
static const int PIXEL_MOVEMENT_PER_CLICK = 5;

// Dashed line down the middle
static const int DASHED_LINE_WIDTH = 10;
static const int DASHED_LINE_HEIGHT = 15;
static const int DASHED_LINE_GAP = 10;
static const int DASHED_LINE_MIDDLE_X = SCREEN_WIDTH / 2;
static const int DASHED_LINE_START_Y = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;
static const int DASHED_LINE_END_Y = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT - DASHED_LINE_GAP - DASHED_LINE_HEIGHT;

// Score
static const int FONT_SIZE = 40;
static const int TENS_DIGIT_X_POSITION = (SCREEN_WIDTH / 4);
static const int TENS_DIGIT_Y_POSITION = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT + 10;
static const int ONES_DIGIT_X_POSITION = TENS_DIGIT_X_POSITION + 30;
static const int ONES_DIGIT_Y_POSITION = TENS_DIGIT_Y_POSITION;

// Structs
struct Player {
    int y_position {};
    int score {};
    int racket_size {};
};

struct Ball {
    int x_position {};
    int y_position {};
    int radius {};
    bool active {};
};

struct BoundaryRectangle {
    int x_position {};
    int y_position {};
    int width {};
    int height {};
};

// Local game variables
static bool game_over = false;
static Player player = { 0 };
static Ball ball = { 0 };
static BoundaryRectangle top = { 0 };
static BoundaryRectangle bottom = { 0 };
static BoundaryRectangle wall = { 0 };

int main() {
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    InitGame();
    SetTargetFPS(FPS);

    // Main game loop
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }

    // De-initialization
    CloseWindow();

    return 0;
}

void InitGame(void) {
    // rectangles on the top and bottom
    top.x_position = BORDER_OFFSET;
    top.y_position = BORDER_OFFSET;
    top.width = SCREEN_WIDTH - (2 * BORDER_OFFSET);
    top.height = TOP_BOTTOM_RECT_HEIGHT;

    bottom.x_position = BORDER_OFFSET;
    bottom.y_position = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT;
    bottom.width = SCREEN_WIDTH - (2 * BORDER_OFFSET);
    bottom.height = TOP_BOTTOM_RECT_HEIGHT;

    // wall on the left side
    wall.x_position = BORDER_OFFSET + WALL_OFFSET;
    wall.y_position = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;
    wall.width = TOP_BOTTOM_RECT_HEIGHT;
    wall.height = SCREEN_HEIGHT - (2 * (BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT)); // off by 5 pixels

    // player
    player.racket_size = MEDIUM_RACKET_HEIGHT;
    player.y_position = (SCREEN_HEIGHT / 2) - (player.racket_size / 2); // initialize racket position to the middle
    player.score = 0;
}

void UpdateGame(void) {
    // Update position of the racket based on user input (up & down arrows)
    int bottom_racket_boundary = SCREEN_HEIGHT - BORDER_OFFSET - TOP_BOTTOM_RECT_HEIGHT - player.racket_size;
    int top_racket_boundary = BORDER_OFFSET + TOP_BOTTOM_RECT_HEIGHT;

    if (IsKeyDown(KEY_DOWN)) {
        player.y_position += PIXEL_MOVEMENT_PER_CLICK;
    }

    if (player.y_position >= bottom_racket_boundary) {
        // Paddle can't go below the lower wall
        player.y_position = bottom_racket_boundary;
    }

    if (IsKeyDown(KEY_UP)) {
        player.y_position -= PIXEL_MOVEMENT_PER_CLICK;
    }

    if (player.y_position <= top_racket_boundary) {
        // Paddle can't go above the upper wall
        player.y_position = top_racket_boundary;
    }

    // TODO: update the position of the ball
    // TODO: check for collision
    // TODO: update the score based on collision
}

void DrawGame(void) {
    BeginDrawing();

    // black background
    ClearBackground(BLACK);

    // draw top and bottom rectangles in white
    DrawRectangle(top.x_position, top.y_position, top.width, top.height, WHITE);
    DrawRectangle(bottom.x_position, bottom.y_position, bottom.width, bottom.height, WHITE);

    // draw wall
    DrawRectangle(wall.x_position, wall.y_position, wall.width, wall.height, WHITE);

    // draw dashed line down the middle
    for (int cur_y = DASHED_LINE_START_Y; cur_y <= DASHED_LINE_END_Y; cur_y += (DASHED_LINE_GAP + DASHED_LINE_HEIGHT)) {
        DrawRectangle(DASHED_LINE_MIDDLE_X, DASHED_LINE_GAP + cur_y, DASHED_LINE_WIDTH, DASHED_LINE_HEIGHT, WHITE);
    }

    // draw racket position
    DrawRectangle(RACKET_X_POSITION, player.y_position, RACKET_WIDTH, player.racket_size, WHITE);

    // TODO: draw the new ball position

    // draw the score
    string ones_digit = to_string(player.score % 10);
    string tens_digit = to_string(player.score / 10);

    DrawText(ones_digit.c_str(), ONES_DIGIT_X_POSITION, ONES_DIGIT_Y_POSITION, FONT_SIZE, WHITE);

    if (player.score >= 10) {
        DrawText(tens_digit.c_str(), TENS_DIGIT_X_POSITION, TENS_DIGIT_Y_POSITION, FONT_SIZE, WHITE);
    }

    EndDrawing();
}

void UpdateDrawFrame(void) {
    UpdateGame();
    DrawGame();
}

// add to GitHub...
// add auto-formatting here...
// add the GameOver logic and the ability to play the game again...
// add the ability to change the size of the paddle...
// add a sound...
