
#include "game.h"

unsigned int score = 0;

void score_reset() {
  score = 0;
}

void score_add(unsigned int amount) {
  score += amount;
}