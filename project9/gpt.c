#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Define constants
#define MAX_ROUNDS 10
#define MIN_VALUE 1
#define MAX_VALUE 100

// Shared data structure
typedef struct {
    int min, max;
    int guess1, guess2;
    int target;
    int rounds;
    int wins1, wins2;  // Track wins for both players
    pthread_mutex_t mutex;
    pthread_cond_t cond_ref, cond_players;
    int ref_turn, player1_turn, player2_turn;
    int correct_guess;  // Flag to indicate if someone guessed the target
    int game_over;      // Flag to signal game over
} game_data_t;

// Function declarations
void* player1_thread(void* arg);
void* player2_thread(void* arg);
void* referee_thread(void* arg);

int main() {
    pthread_t t_player1, t_player2, t_referee;
    game_data_t game_data;

    // Initialize game data
    game_data.min = MIN_VALUE;
    game_data.max = MAX_VALUE;
    game_data.rounds = 0;
    game_data.wins1 = 0;
    game_data.wins2 = 0;
    game_data.ref_turn = 1;
    game_data.player1_turn = 0;
    game_data.player2_turn = 0;
    game_data.correct_guess = 0;
    game_data.game_over = 0;

    pthread_mutex_init(&game_data.mutex, NULL);
    pthread_cond_init(&game_data.cond_ref, NULL);
    pthread_cond_init(&game_data.cond_players, NULL);

    srand(time(NULL));

    // Create threads
    pthread_create(&t_player1, NULL, player1_thread, &game_data);
    pthread_create(&t_player2, NULL, player2_thread, &game_data);
    pthread_create(&t_referee, NULL, referee_thread, &game_data);

    // Wait for threads to finish
    pthread_join(t_player1, NULL);
    pthread_join(t_player2, NULL);
    pthread_join(t_referee, NULL);

    // Clean up
    pthread_mutex_destroy(&game_data.mutex);
    pthread_cond_destroy(&game_data.cond_ref);
    pthread_cond_destroy(&game_data.cond_players);

    // Final score summary
    printf("\nFinal Score:\n");
    printf("Player 1 Wins: %d\n", game_data.wins1);
    printf("Player 2 Wins: %d\n", game_data.wins2);

    return 0;
}

// Player 1 guesses the average of the range
void* player1_thread(void* arg) {
    game_data_t* game = (game_data_t*)arg;
    while (1) {
        pthread_mutex_lock(&game->mutex);

        // Wait for referee to signal player's turn
        while (!game->player1_turn && !game->game_over)
            pthread_cond_wait(&game->cond_players, &game->mutex);

        // Exit if the game is over
        if (game->game_over) {
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        // Calculate the average guess
        game->guess1 = (game->min + game->max) / 2;
        printf("Player 1 guesses: %d\n", game->guess1);

        // Hand turn back to the referee
        game->player1_turn = 0;
        game->player2_turn = 1;
        pthread_cond_signal(&game->cond_players);
        pthread_mutex_unlock(&game->mutex);
    }
    return NULL;
}

// Player 2 guesses a random number between min and max
void* player2_thread(void* arg) {
    game_data_t* game = (game_data_t*)arg;
    while (1) {
        pthread_mutex_lock(&game->mutex);

        // Wait for player 1 to finish before guessing
        while (!game->player2_turn && !game->game_over)
            pthread_cond_wait(&game->cond_players, &game->mutex);

        // Exit if the game is over
        if (game->game_over) {
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        // Generate a random guess
        game->guess2 = rand() % (game->max - game->min + 1) + game->min;
        printf("Player 2 guesses: %d\n", game->guess2);

        // Hand turn back to the referee
        game->player2_turn = 0;
        game->ref_turn = 1;
        pthread_cond_signal(&game->cond_ref);
        pthread_mutex_unlock(&game->mutex);
    }
    return NULL;
}

// Referee manages the game logic
void* referee_thread(void* arg) {
    game_data_t* game = (game_data_t*)arg;

    while (1) {
        pthread_mutex_lock(&game->mutex);

        // Wait for both players' guesses
        while (!game->ref_turn)
            pthread_cond_wait(&game->cond_ref, &game->mutex);

        if (game->rounds >= MAX_ROUNDS) {
            printf("\nGame over!\n");
            game->game_over = 1;
            pthread_cond_broadcast(&game->cond_players);  // Signal all players to exit
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        // Wait for 1 second before starting each round
        sleep(1);

        // Set new target for the current round
        game->target = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
        printf("\nReferee sets the target for round %d: %d\n", game->rounds + 1, game->target);

        game->correct_guess = 0;  // Reset correct guess flag for new round

        // Loop until a player guesses correctly
        while (!game->correct_guess) {
            // Check Player 1's guess
            if (game->guess1 < game->target) {
                printf("Player 1's guess is too low.\n");
                game->min = game->guess1 + 1;
            } else if (game->guess1 > game->target) {
                printf("Player 1's guess is too high.\n");
                game->max = game->guess1 - 1;
            } else {
                printf("Player 1 guessed correctly!\n");
                game->wins1++;  // Player 1 wins the round
                game->correct_guess = 1;  // Correct guess found, end round
            }

            // If Player 1 hasn't guessed correctly, check Player 2's guess
            if (!game->correct_guess) {
                if (game->guess2 < game->target) {
                    printf("Player 2's guess is too low.\n");
                    game->min = game->guess2 + 1;
                } else if (game->guess2 > game->target) {
                    printf("Player 2's guess is too high.\n");
                    game->max = game->guess2 - 1;
                } else {
                    printf("Player 2 guessed correctly!\n");
                    game->wins2++;  // Player 2 wins the round
                    game->correct_guess = 1;  // Correct guess found, end round
                }
            }

            // Signal players for the next guess if the round hasn't ended
            if (!game->correct_guess) {
                game->player1_turn = 1;
                pthread_cond_signal(&game->cond_players);
                pthread_cond_wait(&game->cond_ref, &game->mutex);  // Referee waits for both guesses again
            }
        }

        // Increment round counter
        game->rounds++;

        // Display round summary
        printf("\nRound %d Summary:\n", game->rounds);
        printf("Player 1 Wins So Far: %d\n", game->wins1);
        printf("Player 2 Wins So Far: %d\n", game->wins2);

        if (game->rounds >= MAX_ROUNDS) {
            game->game_over = 1;
            pthread_cond_broadcast(&game->cond_players);  // Signal players to exit
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        // Reset the min and max range for the next round
        game->min = MIN_VALUE;
        game->max = MAX_VALUE;

        // Signal players for the next round
        game->ref_turn = 0;
        game->player1_turn = 1;
        pthread_cond_signal(&game->cond_players);
        pthread_mutex_unlock(&game->mutex);
    }
    return NULL;
}
