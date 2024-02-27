#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../assets/playerSprite.h"
#include "../assets/keySprite.h"
#include "../header/controls.h"

#define MAX_STR_LENGTH 100
#define NUM_STRINGS 100
bool buttonPressed = false;
bool paused = false;

static void handlePlayerMovement(Vector2 *pos, float deltaTime, Rectangle wall, Rectangle player) {
	if (!getControls().right &&
		!getControls().left &&
		!getControls().up &&
		!getControls().down
		) {
		buttonPressed = false;
	}

	if (paused) return;

	if (!buttonPressed) {
		if (getControls().right && pos->x < 52) {
			
			player.x += 13;
			if (!CheckCollisionRecs(player, wall)) {
				pos->x += 13;
			}
			buttonPressed = true;
		}

		if (getControls().left && pos->x > 13) {
			player.x -= 13;
			if (!CheckCollisionRecs(player, wall)) {
				pos->x -= 13;
			}
			
			buttonPressed = true;
		}

		if (getControls().up && pos->y > 3) {
			player.y -= 13;
			if (!CheckCollisionRecs(player, wall)) {
				pos->y -= 13;
			}
			
			buttonPressed = true;
		}

		if (getControls().down) {
			player.y += 13;
			if (!CheckCollisionRecs(player, wall)) {
				pos->y += 13;
			}
			
			buttonPressed = true;
		}
	}

	
}

int main() {
	int screenWidth = 800;
	int screenHeight = 600;
	int targetFPS = 60;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Prison Escape");
	SetTargetFPS(targetFPS);

	resetControls();

	Vector2 position = { 20, 20 };
	Camera2D camera = { 0 };
	camera.target = position;
	camera.rotation = 0.0f;
	camera.zoom = 5.0f;

	int room[] = {
		415,415,415,415,415,415,
		415,625,625,625,625,415,
		415,625,625,625,625,415,
		415,625,625,625,625,415,
		415,625,625,625,625,415,
		415,625,625,625,625,415,
		415,100,415,415,415,415
	};
	int totalTiles = sizeof(room) / sizeof(room[0]);
	Rectangle rectangles[42];
	Vector2 positions[42];
	Color colors[42];

	int count = 0;

	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 7; y++) {
			rectangles[count].x = 0;
			rectangles[count].y = 0;
			rectangles[count].width = 12;
			rectangles[count].height = 12;

			positions[count].x = 13 * x;
			positions[count].y = 13 * y;

			// floor
			if (room[x + y * 6] == 625) {
				colors[count] = (Color){ 0x35, 0x34, 0x36, 0xFF };
			}
			
			// wall
			if (room[x + y * 6] == 415) {
				colors[count] = (Color){ 0x5c, 0x5c, 0x5c, 0xFF };
			}

			count++;
		}
	}

	Image playerImage = {
	   .data = PLAYERSPRITE_DATA,
	   .width = PLAYERSPRITE_WIDTH,
	   .height = PLAYERSPRITE_HEIGHT,
	   .format = PLAYERSPRITE_FORMAT,
	   .mipmaps = 1
	};

	Texture2D playerSprite = LoadTextureFromImage(playerImage);
	Rectangle playerRec = { 0.0f, 0.0f, PLAYERSPRITE_WIDTH, PLAYERSPRITE_HEIGHT };
	Vector2 playerPosition = { positions[21].x,  positions[1].y - 10 };
	Rectangle playerCollider = { 0.0f, 0.0f, 8, 5 };
	
	float deltaTime = GetFrameTime();

	bool keysHeld = false;
	Image keyImage = {
		.data = KEYSPRITE_DATA,
		.width = KEYSPRITE_WIDTH,
		.height = KEYSPRITE_HEIGHT,
		.format = KEYSPRITE_FORMAT,
		.mipmaps = 1
	};

	Texture2D keySprite = LoadTextureFromImage(keyImage);
	Rectangle keyRec = { 0.f, 0.f, KEYSPRITE_WIDTH, KEYSPRITE_HEIGHT };
	Vector2 keyPosition = { positions[20].x, positions[10].y };
	Rectangle keyCollider = { keyPosition.x, keyPosition.y, 10, 10 };

	Rectangle doorCollider = { 13, 13 * 6, 12, 12  };
	bool isDoorCollide = false;
	bool err = false;
	bool collision = false;

	Rectangle bottomWallCollider = { 13 * 2, 13 * 6, 51, 12 };
	while (!WindowShouldClose()) {
		ClearBackground((Color) { 0x17, 0x17, 0x17, 0xFF });
		deltaTime = GetFrameTime();
		handlePlayerMovement(&playerPosition, deltaTime, bottomWallCollider, playerCollider);
 		camera.target.x = playerRec.x;
		camera.target.y = playerRec.y;
		camera.offset = (Vector2){ GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 200};

		BeginMode2D(camera);
		for (int i = 0; i < totalTiles; i++) {
			DrawRectangle(positions[i].x, positions[i].y, rectangles[i].width, rectangles[i].height, colors[i]);
		}
		DrawRectangleRec(doorCollider, WHITE);
		collision = CheckCollisionRecs(playerCollider, keyCollider);
		isDoorCollide = CheckCollisionRecs(playerCollider, doorCollider);

		if (collision) {
			keysHeld = true;
		}

		
		// DrawRectangleRec(bottomWallCollider, WHITE);
		if (!keysHeld) {
			// DrawRectangleRec(keyCollider, WHITE);
			DrawTextureRec(keySprite, keyRec, keyPosition, WHITE);
		}
		
		playerCollider.x = playerPosition.x + 2;
		playerCollider.y = playerPosition.y + 15;

		// DrawRectangleRec(playerCollider, WHITE);
		DrawTextureRec(playerSprite, playerRec, playerPosition, WHITE);
		
		EndMode2D();
		if (isDoorCollide) {
			if (!keysHeld) {
				playerPosition.y -= 13;
				err = true;
			}
			else {
				paused = true;
				DrawText("Winner", 0, 0, 20, WHITE);
;			}
		}

		if (err) {
			DrawText("Key is needed to proceed!", 0, 0, 20, WHITE);
		}

		if (keysHeld && !paused) {
			err = false;
			DrawText("Key aquired!", 0, 0, 20, WHITE);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}

