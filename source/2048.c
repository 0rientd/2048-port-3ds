/*
 ============================================================================
 Name        : 2048 3DS Port
 Author      : 0rientd
 Description : 3DS version of the "2048" game for GNU/Linux
 ============================================================================
 */

#define VERSION "1.0.0"

#include <3ds.h>          // 3DS specific functions and definitions
#include <stdio.h>		    // defines: printf, puts, getchar
#include <stdlib.h>		    // defines: EXIT_SUCCESS
#include <string.h>		    // defines: strcmp
#include <unistd.h>		    // defines: STDIN_FILENO, usleep
#include <stdbool.h>	    // defines: true, false
#include <stdint.h>		    // defines: uint8_t, uint32_t
#include <time.h>		      // defines: time
#include <signal.h>		    // defines: signal, SIGINT

#define SIZE 4

// this function receives 2 pointers (indicated by *) so it can set their values
void getColors(uint8_t value, uint8_t scheme, uint8_t *foreground, uint8_t *background) {
	uint8_t original[] = {8, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5, 255, 6, 255, 7, 255, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 255, 0, 255, 0};
	
	uint8_t *schemes[] = { original };

	// modify the 'pointed to' variables (using a * on the left hand of the assignment)
	*foreground = *(schemes[scheme] + (1 + value * 2) % sizeof(original));
	*background = *(schemes[scheme] + (0 + value * 2) % sizeof(original));
	// alternatively we could have returned a struct with two variables
}

uint8_t getDigitCount(uint32_t number) {
	uint8_t count = 0;
	
  do {
		number /= 10;
		count += 1;
	} while (number);

	return count;
}

void drawBoard(uint8_t board[SIZE][SIZE], uint8_t scheme, uint32_t score) {
	uint8_t x, y, fg, bg;

	printf("\x1b[6;15H%17lu points\n\n", score); // move cursor to 6th line
	
  for (y = 0; y < SIZE; y++) {
		printf("           "); // left padding for centering
		for (x = 0; x < SIZE; x++) {
			// send the addresses of the foreground and background variables,
			// so that they can be modified by the getColors function
			getColors(board[x][y], scheme, &fg, &bg);
			
      printf("\x1b[38;5;%u;48;5;%um", fg, bg); // set color
			printf("       ");
			printf("\x1b[m"); // reset all modes
		}

		printf("\n");

		printf("           "); // left padding for centering
		for (x = 0; x < SIZE; x++) {
			getColors(board[x][y], scheme, &fg, &bg);
			printf("\x1b[38;5;%u;48;5;%um", fg, bg); // set color
      
			if (board[x][y] != 0) {
				uint32_t number = 1 << board[x][y];
				uint8_t t = 7 - getDigitCount(number);

				printf("%*s%lu%*s", t - t / 2, "", number, t / 2, "");
			} else {
				printf("   .   ");
			}

			printf("\x1b[m"); // reset all modes
		}

		printf("\n");
		
		printf("           "); // left padding for centering
    for (x = 0; x < SIZE; x++) {
			getColors(board[x][y], scheme, &fg, &bg);
		
      printf("\x1b[38;5;%u;48;5;%um", fg, bg); // set color
			printf("       ");
			printf("\x1b[m"); // reset all modes
		}

		printf("\n");
	}

	printf("\n");
	printf("\x1b[A"); // one line up
}


uint8_t findTarget(uint8_t array[SIZE], uint8_t x, uint8_t stop) {
	uint8_t t;
	
  // if the position is already on the first, don't evaluate
	if (x == 0) {
		return x;
	}

	for (t = x - 1;; t--) {
		if (array[t] != 0) {
			if (array[t] != array[x]) {
				// merge is not possible, take next position
				return t + 1;
			}

			return t;
		} else {
			// we should not slide further, return this one
			if (t == stop) {
				return t;
			}
		}
	}

	// we did not find a target
	return x;
}

bool slideArray(uint8_t array[SIZE], uint32_t *score) {
	bool success = false;
	uint8_t x, t, stop = 0;

	for (x = 0; x < SIZE; x++) {
		if (array[x] != 0) {
			t = findTarget(array, x, stop);

			// if target is not original position, then move or merge
			if (t != x) {
				// if target is zero, this is a move
				if (array[t] == 0) {
					array[t] = array[x];
				} else if (array[t] == array[x]) {
					// merge (increase power of two)
					array[t]++;
					// increase score
					*score += 1 << array[t];
					// set stop to avoid double merge
					stop = t + 1;
				}

				array[x] = 0;
				success = true;
			}
		}
	}

	return success;
}

void rotateBoard(uint8_t board[SIZE][SIZE]) {
	uint8_t i, j, n = SIZE;
	uint8_t tmp;

	for (i = 0; i < n / 2; i++) {
		for (j = i; j < n - i - 1; j++) {
			tmp = board[i][j];
			board[i][j] = board[j][n - i - 1];
			board[j][n - i - 1] = board[n - i - 1][n - j - 1];
			board[n - i - 1][n - j - 1] = board[n - j - 1][i];
			board[n - j - 1][i] = tmp;
		}
	}
}

bool moveUp(uint8_t board[SIZE][SIZE], uint32_t *score) {
	bool success = false;
	uint8_t x;

	for (x = 0; x < SIZE; x++) {
		success |= slideArray(board[x], score);
	}
	
  return success;
}

bool moveLeft(uint8_t board[SIZE][SIZE], uint32_t *score) {
	bool success;

	rotateBoard(board);
	success = moveUp(board, score);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	
  return success;
}

bool moveDown(uint8_t board[SIZE][SIZE], uint32_t *score) {
	bool success;
	
  rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board, score);
	rotateBoard(board);
	rotateBoard(board);
	
  return success;
}

bool moveRight(uint8_t board[SIZE][SIZE], uint32_t *score) {
	bool success;
	
  rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board, score);
	rotateBoard(board);
	
  return success;
}

bool findPairDown(uint8_t board[SIZE][SIZE]) {
	bool success = false;
	uint8_t x, y;
	
  for (x = 0; x < SIZE; x++) {
		for (y = 0; y < SIZE - 1; y++) {
			if (board[x][y] == board[x][y + 1])
				return true;
		}
	}

	return success;
}

uint8_t countEmpty(uint8_t board[SIZE][SIZE]) {
	uint8_t x, y;
	uint8_t count = 0;
	
  for (x = 0; x < SIZE; x++) {
		for (y = 0; y < SIZE; y++) {
			if (board[x][y] == 0) {
				count++;
			}
		}
	}

	return count;
}

bool gameEnded(uint8_t board[SIZE][SIZE]) {
	bool ended = true;
	
  if (countEmpty(board) > 0)
		return false;
	
  if (findPairDown(board))
		return false;
	
  rotateBoard(board);
	
  if (findPairDown(board))
		ended = false;
	
  rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	
  return ended;
}

void addRandom(uint8_t board[SIZE][SIZE]) {
	static bool initialized = false;
	uint8_t x, y;
	uint8_t r, len = 0;
	uint8_t n, list[SIZE * SIZE][2];

	if (!initialized) {
		srand(time(NULL));
		initialized = true;
	}

	for (x = 0; x < SIZE; x++) {
		for (y = 0; y < SIZE; y++) {
			if (board[x][y] == 0) {
				list[len][0] = x;
				list[len][1] = y;
				len++;
			}
		}
	}

	if (len > 0) {
		r = rand() % len;
		x = list[r][0];
		y = list[r][1];
		n = (rand() % 10) / 9 + 1;
		
    board[x][y] = n;
	}
}

void initBoard(uint8_t board[SIZE][SIZE]) {
	uint8_t x, y;

	for (x = 0; x < SIZE; x++) {
		for (y = 0; y < SIZE; y++) {
			board[x][y] = 0;
		}
	}

	addRandom(board);
	addRandom(board);
}

void updateTimeOnScreen() {
  u32 hours, minutes, seconds;

  time_t unixTime = time(NULL);
  struct tm* timeStruct = gmtime((const time_t *)&unixTime);

  hours = timeStruct->tm_hour;
  minutes = timeStruct->tm_min;
  seconds = timeStruct->tm_sec;

  printf("\x1b[2;2H%02lu:%02lu:%02lu", hours, minutes, seconds);
}

int main(int argc, char **argv) {
	uint8_t board[SIZE][SIZE];
	uint8_t scheme = 0;
	uint32_t score = 0;
	bool success;

  // Initialize the graphics system (for 3DS)
  gfxInitDefault();

  consoleInit(GFX_BOTTOM, NULL);
  printf("\x1b[2;14H2048 for 3DS\n");

	printf("\x1b[29;2H\x1b[32;40mSELECT\x1b[0m to restart");
	printf("\x1b[29;27H\x1b[31;40mSTART\x1b[0m to exit");

  consoleInit(GFX_TOP, NULL);
  
  printf("\x1b[30;33HPorted by \x1b[35;40m0rientd\x1b[0m");
	
  initBoard(board);
	drawBoard(board, scheme, score);

	while (aptMainLoop()) {
    hidScanInput();
    u32 kDown = hidKeysDown();

    if (kDown & KEY_LEFT) {
      success = moveLeft(board, &score);
    } else if (kDown & KEY_RIGHT) {
      success = moveRight(board, &score);
    } else if (kDown & KEY_UP) {
      success = moveUp(board, &score);
    } else if (kDown & KEY_DOWN) {
      success = moveDown(board, &score);
    } else {
      success = false;
    }

		if (success) {
			drawBoard(board, scheme, score);
			usleep(150 * 1000); // 150 ms
			addRandom(board);
			drawBoard(board, scheme, score);
			
      if (gameEnded(board)) {
        printf("\n\x1b[22;22H\x1b[31;40mGAME OVER\x1b[0m\n\n");
        printf("\x1b[24;14HPress SELECT to restart\n");
			}
		}

		if (kDown & KEY_START) break;

		if (kDown & KEY_SELECT) {
      initBoard(board);
      score = 0;

      printf("\x1b[2J\x1b[0;0H");
      printf("\x1b[30;33HPorted by \x1b[35;40m0rientd\x1b[0m");
			drawBoard(board, scheme, score);
		}

    updateTimeOnScreen();

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxSwapBuffers();

    //Wait for VBlank
    gspWaitForVBlank();
	}

  gfxExit();
	return 0;
}
