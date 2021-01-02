// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The worm model

#include <curses.h>
#include "worm.h"
#include "board_model.h"
#include "worm_model.h"

//*****************************************************************
// Data defining the worm
//*****************************************************************
enum ColorPairs theworm_wcolor; //  Code of color pair used for the worm 

// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************

// Growing of the Worm
void growWorm(struct worm* aworm, enum Boni growth){
  if (aworm->cur_lastindex + growth <= aworm->maxindex){
    aworm->cur_lastindex += growth;
  } 
  else {
    aworm->cur_lastindex = aworm->maxindex;  
  }
}

//Getters
int getWormLength(struct worm* aworm){
  return aworm->cur_lastindex + 1;
}

struct pos getWormHeadPos(struct worm* aworm){
    return aworm->wormpos[aworm->headindex];
}

// Initialize the worm
extern enum ResCodes initializeWorm(struct worm* aworm, int len_max, int len_cur, struct pos headpos, enum WormHeading dir, enum ColorPairs color) {
    //Local variables for loops etc.
    int array_index_iterator;
    
    //Initialize last usable index to len_max -1
    aworm->maxindex = len_max -1;
    // Current Last usable index (may grow up to maxindex)
    aworm->cur_lastindex = len_cur -1;
    
    //Initialize headindex
    aworm->headindex = 0;
    
    // Marking of unused positions in the array with code UNUSED_POS_ELEM
    for (array_index_iterator = 1; array_index_iterator <= aworm->maxindex; array_index_iterator++){
      aworm->wormpos[array_index_iterator].y = UNUSED_POS_ELEM;
      aworm->wormpos[array_index_iterator].x = UNUSED_POS_ELEM;
    }

    // Initialize the positions of the head of the worm
    aworm->wormpos[aworm->headindex] = headpos;
    
    // Initialize the heading of the worm
    setWormHeading(aworm, dir);

    // Initialze color of the worm
    aworm->wcolor = color;

    return RES_OK;
}

// Show the worms's elements on the display
// Simple version
extern void showWorm(struct board* aboard, struct worm* aworm) {
    // Due to our encoding we just need to show the head element
    // All other elements are already displayed
    placeItem(aboard,
            aworm->wormpos[aworm->headindex].y,
            aworm->wormpos[aworm->headindex].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_HEAD_ELEMENT,
            aworm->wcolor);

    int tailindex = (aworm->headindex + 1) % (aworm->cur_lastindex + 1);
    placeItem(aboard,
            aworm->wormpos[tailindex].y,
            aworm->wormpos[tailindex].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_TAIL_ELEMENT,
            aworm->wcolor);
    for (int i = aworm->headindex -1; i != tailindex; i--){
      if (i<0){
        i = aworm->cur_lastindex+1;
        continue;
      }
      placeItem(aboard,
            aworm->wormpos[i].y,
            aworm->wormpos[i].x,
            BC_USED_BY_WORM,
            SYMBOL_WORM_INNER_ELEMENT,
            aworm->wcolor);

    } 

}

// Remove the worm's elements from the display
extern void cleanWormTail(struct board* aboard, struct worm* aworm){
  int tailindex;

  //Compute tailindex
  tailindex = (aworm->headindex + 1) % (aworm->cur_lastindex + 1);

  //Check if array element of tailindex is in use
  if (aworm->wormpos[tailindex].y != UNUSED_POS_ELEM) {
    placeItem(aboard, aworm->wormpos[tailindex].y, aworm->wormpos[tailindex].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
  }
}

extern void moveWorm(struct board* aboard, struct worm* aworm, enum GameStates* agame_state) {
    struct pos headpos;
    headpos = aworm->wormpos[aworm->headindex];
    //int headpos_y;
    //int headpos_x;
    // Compute und store new head position according to current heading.
    headpos.x += aworm->dx;
    headpos.y += aworm->dy;

    // Check if we would leave the display if we move the worm's head according
    // to worm's last direction.
    // We are not allowed to leave the display's window.
    if (headpos.x < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //links aus dem Bild
    } else if (headpos.x > getLastColOnBoard(aboard)) { 
        *agame_state = WORM_OUT_OF_BOUNDS;                 //rechts aus dem Bild
    } else if (headpos.y < 0) {  
        *agame_state = WORM_OUT_OF_BOUNDS;                 //oben aus dem Bild    
	} else if (headpos.y > getLastRowOnBoard(aboard)) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //unten aus dem Bild
    } else {
        // We will stay within bounds.
        // Check if the worm's head hits something
        switch(getContentAt(aboard,headpos)){  
          case BC_FOOD_1:
            *agame_state = WORM_GAME_ONGOING;
            // Grow accordingly
            growWorm(aworm, BONUS_1);
            decrementNumberOfFoodItems(aboard);
            break;
          case BC_FOOD_2:
            *agame_state = WORM_GAME_ONGOING;
            // Grow accordingly
            growWorm(aworm, BONUS_2);
            decrementNumberOfFoodItems(aboard);
            break;
          case BC_FOOD_3:
            *agame_state = WORM_GAME_ONGOING;
            // Grow accordingly
            growWorm(aworm, BONUS_3);
            decrementNumberOfFoodItems(aboard);
            break; 
          case BC_BARRIER:
            //Stop game            
            *agame_state = WORM_CRASH;
            break;
          case BC_USED_BY_WORM:
            //Stop game            
            *agame_state = WORM_CROSSING;
            break;
          default:
            {;} // Do nothing
        }
    }

    //Check the status of *agame_state
    //Go on if nothing happened
    if (*agame_state == WORM_GAME_ONGOING) {
      // Ring Buffer to increase Headindex
      aworm->headindex++;
      if (aworm->headindex > aworm->cur_lastindex) {
        aworm->headindex=0;
      }
      //Store new coordinates of head element in worm structure
      aworm->wormpos[aworm->headindex] = headpos;
    }
}



// Setters
extern void setWormHeading(struct worm* aworm, enum WormHeading dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            aworm->dx=0;
            aworm->dy=-1;
            break;
        case WORM_DOWN :// User wants down
            aworm->dx=0;
            aworm->dy=1;
            break;
        case WORM_LEFT :// User wants left
            aworm->dx=-1;
            aworm->dy=0;
            break;
        case WORM_RIGHT      :// User wants right
            aworm->dx=1;
            aworm->dy=0;
            break;
    }
}
