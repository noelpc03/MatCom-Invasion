#include <stdio.h>
#include <string.h>
#include <time.h>
#include "score.h"
#include "config.h"

void add_new_score(Score new_score) {
    // Get current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(new_score.date, sizeof(new_score.date), "%02d-%02d-%04d_%02d:%02d:%02d",
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    // Find correct position for the new score
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (new_score.score > high_scores[i].score) {
            // Shift lower scores down
            for (int j = MAX_HIGH_SCORES - 1; j > i; j--) {
                high_scores[j].score = high_scores[j - 1].score;
                strcpy(high_scores[j].name, high_scores[j - 1].name);
                strcpy(high_scores[j].date, high_scores[j - 1].date);
            }
            // Insert new score at correct position
            high_scores[i].score = new_score.score;
            strcpy(high_scores[i].name, new_score.name);
            strcpy(high_scores[i].date, new_score.date);
            return;
        }
    }
}

void save_high_scores(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file to save scores");
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        fprintf(file, "%d\n", high_scores[i].score);
        fprintf(file, "%s\n", high_scores[i].name);
        fprintf(file, "%s\n", high_scores[i].date);
    }
    fclose(file);
}

void load_high_scores(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file to load scores");
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (fscanf(file, "%d", &high_scores[i].score) != 1) {
            high_scores[i].score = 0;
            snprintf(high_scores[i].name, sizeof(high_scores[i].name), "PlayerX");
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        } else if (fscanf(file, "%s", high_scores[i].name) != 1) {
            snprintf(high_scores[i].name, sizeof(high_scores[i].name), "PlayerX");
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        } else if (fscanf(file, "%s", high_scores[i].date) != 1) {
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        }
    }
    fclose(file);
}

void save_game(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error opening file to save game.\n");
        return;
    }
    
    // Save bullets
    fwrite(bullets, sizeof(Bullet), NUMBER_BULLETS, file);
    
    // Save aliens
    fwrite(aliens, sizeof(Alien), NUMBER_ALIENS, file);
    
    // Save player
    fwrite(&player, sizeof(Player), 1, file);
    
    // Save score
    fwrite(&score, sizeof(Score), 1, file);
    
    fclose(file);
    printf("Game saved successfully.\n");
}

void load_game(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file to load game.\n");
        return;
    }
    
    // Load bullets
    fread(bullets, sizeof(Bullet), NUMBER_BULLETS, file);
    
    // Load aliens
    fread(aliens, sizeof(Alien), NUMBER_ALIENS, file);
    
    // Load player
    fread(&player, sizeof(Player), 1, file);
    
    // Load score
    fread(&score, sizeof(Score), 1, file);
    
    fclose(file);
    printf("Game loaded successfully.\n");
}
