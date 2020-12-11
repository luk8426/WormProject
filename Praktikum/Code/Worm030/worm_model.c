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
int theworm_wormpos_y[WORM_LENGTH];  // y-coordinate of the worm's head
int theworm_wormpos_x[WORM_LENGTH];  // x-coordinate of the worm's head
// Last usable indox into the arrays theworm_wormpos_x & ..._y
int theworm_maxindex;
// Index to the array for the worm's head position
int theworm_headindex;

// The current heading of the worm
// These are offsets from the set {-1,0,+1}
int theworm_dx;
int theworm_dy;

enum ColorPairs theworm_wcolor; //  Code of color pair used for the worm 

// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************


// Initialize the worm
extern enum ResCodes initializeWorm(int len_max, int headpos_y, int headpos_x, enum WormHeading dir, enum ColorPairs color) {
    //Local variables for loops etc.
    int array_index_iterator;
    
    //Initialize last usable index to len_max -1
    theworm_maxindex = len_max -1;
    
    //Initialize headindex
    theworm_headindex = 0;
    
    // Marking of unused positions in the array with code UNUSED_POS_ELEM
    for (array_index_iterator = 1; array_index_iterator <= theworm_maxindex; array_index_iterator++){
      theworm_wormpos_x[array_index_iterator] = UNUSED_POS_ELEM;
      theworm_wormpos_y[array_index_iterator] = UNUSED_POS_ELEM;
    }

    // Initialize the positions of the head of the worm
    theworm_wormpos_x[theworm_headindex] = headpos_x;
    theworm_wormpos_y[theworm_headindex] = headpos_y;
    
    // Initialize the heading of the worm
    setWormHeading(dir);

    // Initialze color of the worm
    theworm_wcolor = color;

    return RES_OK;
}

// Show the worms's elements on the display
// Simple version
extern void showWorm() {
    // Due to our encoding we just need to show the head element
    // All other elements are already displayed
    placeItem(
            theworm_wormpos_y[theworm_headindex],
            theworm_wormpos_x[theworm_headindex],
            SYMBOL_WORM_INNER_ELEMENT,theworm_wcolor);
}

// Remove the worm's elements from the display
extern void cleanWormTail(){
  int tailindex;

  //Compute tailindex
  tailindex = (theworm_headindex+1) % (theworm_maxindex+1);

  //Check if array element of tailindex is in use
  if (theworm_wormpos_y[tailindex] != UNUSED_POS_ELEM) {
    placeItem(theworm_wormpos_y[tailindex],theworm_wormpos_x[tailindex], SYMBOL_FREE_CELL, COLP_FREE_CELL);
  }
}

extern void moveWorm(enum GameStates* agame_state) {
    int headpos_y;
    int headpos_x;
    // Compute und store new head position according to current heading.
    headpos_y = theworm_wormpos_y[theworm_headindex] + theworm_dy;
    headpos_x = theworm_wormpos_x[theworm_headindex] + theworm_dx;

    // Check if we would leave the display if we move the worm's head according
    // to worm's last direction.
    // We are not allowed to leave the display's window.
    if (headpos_x < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //links aus dem Bild
    } else if (headpos_x > getLastCol() ) { 
        *agame_state = WORM_OUT_OF_BOUNDS;                 //rechts aus dem Bild
    } else if (headpos_y < 0) {  
        *agame_state = WORM_OUT_OF_BOUNDS;                 //oben aus dem Bild    
	} else if (headpos_y > getLastRow() ) {
        *agame_state = WORM_OUT_OF_BOUNDS;                 //unten aus dem Bild
    } else {
        // We will stay within bounds.
        // Check if the worm's head will collide with itself at the new position
        if (isInUseByWorm(headpos_y, headpos_x)) {
          //Stop game
          *agame_state = WORM_CROSSING;
        }
    }

    //Check the status of *agame_state
    //Go on if nothing happened
    if (*agame_state == WORM_GAME_ONGOING) {
      theworm_headindex = (theworm_headindex + 1) % (theworm_maxindex +1);
      //Store new coordinates of head element in worm structure
      theworm_wormpos_y[theworm_headindex] = headpos_y;
      theworm_wormpos_x[theworm_headindex] = headpos_x;
    }
}

// A simple collision detection
extern bool isInUseByWorm(int new_headpos_y, int new_headpos_x) {
    int i;
    bool collision = false;
    i = theworm_headindex;
    do {
        //Compare the position of the current worm element with the new_headpos
        if (theworm_wormpos_y[i] == new_headpos_y && theworm_wormpos_x[i] ==  new_headpos_x) {
            collision = true;
            break;
        }    
        
        else {
          i = i-1;
          if (i<0) {
            i = theworm_maxindex;
          }
          //i = (i+1) % (theworm_maxindex + 1), for reversing direction of iteration;
        }
    } while (i != theworm_headindex && theworm_wormpos_x[i] != UNUSED_POS_ELEM);
    //Return what we found out
    return collision;
}


// Setters
extern void setWormHeading(enum WormHeading dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            theworm_dx=0;
            theworm_dy=-1;
            break;
        case WORM_DOWN :// User wants down
            theworm_dx=0;
            theworm_dy=1;
            break;
        case WORM_LEFT :// User wants left
            theworm_dx=-1;
            theworm_dy=0;
            break;
        case WORM_RIGHT      :// User wants right
            theworm_dx=1;
            theworm_dy=0;
            break;
    }
}
