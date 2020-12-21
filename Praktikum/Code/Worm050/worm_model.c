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

//Getters
struct pos getWormHeadPos(struct worm* aworm){
    return aworm->wormpos[aworm->headindex];
}

// Initialize the worm
extern enum ResCodes initializeWorm(struct worm* aworm, int len_max, struct pos headpos, enum WormHeading dir, enum ColorPairs color) {
    //Local variables for loops etc.
    int array_index_iterator;
    
    //Initialize last usable index to len_max -1
    aworm->maxindex = len_max -1;
    
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
extern void showWorm(struct worm* aworm) {
    // Due to our encoding we just need to show the head element
    // All other elements are already displayed
    placeItem(
            aworm->wormpos[aworm->headindex].y,
            aworm->wormpos[aworm->headindex].x,
            SYMBOL_WORM_INNER_ELEMENT, aworm->wcolor);
}

// Remove the worm's elements from the display
extern void cleanWormTail(struct worm* aworm){
  int tailindex;

  //Compute tailindex
  tailindex = (aworm->headindex + 1) % (aworm->maxindex + 1);

  //Check if array element of tailindex is in use
  if (aworm->wormpos[tailindex].y != UNUSED_POS_ELEM) {
    placeItem(aworm->wormpos[tailindex].y, aworm->wormpos[tailindex].x , SYMBOL_FREE_CELL, COLP_FREE_CELL);
  }
}

extern void moveWorm(struct worm* aworm, enum GameStates* agame_state) {
    struct pos headpos;
    //int headpos_y;
    //int headpos_x;
    // Compute und store new head position according to current heading.
    headpos.y = aworm->wormpos[aworm->headindex].y + aworm->dy;
    headpos.x = aworm->wormpos[aworm->headindex].x + aworm->dx;

    // Check if we would leave the display if we move the worm's head according
    // to worm's last direction.
    // We are not allowed to leave the display's window.
    if (headpos.x < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //links aus dem Bild
    } else if (headpos.x > getLastCol() ) { 
        *agame_state = WORM_OUT_OF_BOUNDS;                 //rechts aus dem Bild
    } else if (headpos.y < 0) {  
        *agame_state = WORM_OUT_OF_BOUNDS;                 //oben aus dem Bild    
	} else if (headpos.y > getLastRow() ) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //unten aus dem Bild
    } else {
        // We will stay within bounds.
        // Check if the worm's head will collide with itself at the new position
        if (isInUseByWorm(aworm, headpos)) {
          //Stop game
          *agame_state = WORM_CROSSING;
        }
    }

    //Check the status of *agame_state
    //Go on if nothing happened
    if (*agame_state == WORM_GAME_ONGOING) {
      aworm->headindex = (aworm->headindex + 1) % (aworm->maxindex +1);
      //Store new coordinates of head element in worm structure
      aworm->wormpos[aworm->headindex].y = headpos.y;
      aworm->wormpos[aworm->headindex].x = headpos.x;
    }
}

// A simple collision detection
extern bool isInUseByWorm(struct worm* aworm, struct pos new_headpos) {
    int i;
    bool collision = false;
    i = aworm->headindex;
    do {
        //Compare the position of the current worm element with the new_headpos
        if (aworm->wormpos[i].y == new_headpos.y && aworm->wormpos[i].x ==  new_headpos.x) {
            collision = true;
            break;
        }    
        
        else {
          i = i-1;
          if (i<0) {
            i = aworm->maxindex;
          }
          //i = (i+1) % (theworm_maxindex + 1), for reversing direction of iteration;
        }
    } while (i != aworm->headindex && aworm->wormpos[i].x != UNUSED_POS_ELEM);
    //Return what we found out
    return collision;
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
