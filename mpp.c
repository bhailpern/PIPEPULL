// Mario Pipe Pulls (mpp.c)
// 23-24 November 2021
//
    /* Copyright (C) 2021 Brent Hailpern bhailpern@hotmail.com */

    /* This program is free software: you can redistribute it and/or modify */
    /* it under the terms of the GNU General Public License as published by */
    /* the Free Software Foundation, either version 3 of the License, or */
    /* (at your option) any later version. */

    /* This program is distributed in the hope that it will be useful, */
    /* but WITHOUT ANY WARRANTY; without even the implied warranty of */
    /* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
    /* GNU General Public License for more details. */

    /* You should have received a copy of the GNU General Public License */
    /* along with this program.  If not, see <https://www.gnu.org/licenses/>. */
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// size of pipe
#define PIPE_SIZE 30

// number of high ends per pipe
#define NUMBER_HIGH_ENDS 3

// starting number of rubies
#define RUBY_START 150

// cost of pulling a pipe
#define RUBIES_PER_PULL 5
#define RUBIES_PER_10_PULL 45

// codes for pipe elements
// we will only use high end and regular at this point, no spotlights
#define HIGH_END_ITEM 3
#define SUPER_ITEM 2
#define NORMAL_ITEM 1
#define EMPTY_ITEM 0

// number of experiments to run
#define MAX_EXPERIMENTS 10000

// bounds for random number genrator
#define RAND_RANGE PIPE_SIZE

// statistics to be collected
#define EXP_COL 0         // column representing the experiment number
#define RUBY_COL 1        // column representing the number of rubies spent
#define HE_COL 2          // column representing the number of high ends found
#define PULLS_COL 3       // column representing the number of pipe pulls
#define STAT_COLS 4               // total number of columns in table
#define STAT_ROWS MAX_EXPERIMENTS // total number of rows in table

#define DEBUG_STATUS false

// ========================== THE PIPE ==========================

typedef struct p_t {
  int contents[PIPE_SIZE];
  int next;
  int distribution[PIPE_SIZE]; // debug check for RNG distribution
} pipe_t;


// return random location in pipe
int randPipeLocation() {
  return ((int)((rand() % RAND_RANGE)));
}


// initialize pipe - in particular the distribution summarizer
void init_pipe(pipe_t* p) {
  int i;

  for (i=0; i<PIPE_SIZE; i++) {
    p->distribution[i] = 0;
  }
}


// reset the pipe back to full
void reset_pipe(pipe_t* p) {
  int i;
  int h;
  int loc;

  // set all items to "normal"
  for (i=0; i<PIPE_SIZE; i++) {
    p->contents[i] = NORMAL_ITEM;
  }

  // assign high ends to N random locations
  for (h=0; h<NUMBER_HIGH_ENDS; h++) {
    // find a random location that doesnt already have a high end item
    do {
      loc = randPipeLocation();
    } while (p->contents[loc] == HIGH_END_ITEM);

    // make that location into a high end
    p->contents[loc] = HIGH_END_ITEM;

    // keep track of high ends in debug table
    (p->distribution[loc])++;
  }

  // set next item to the first item in the pipe
  p->next = 0;
  
}


int cur_items_in_pipe(pipe_t* p) {
  return PIPE_SIZE - p->next;  
}


// initial high-end percentate in pipe
float init_percentage(pipe_t* p) {
  return ((float)NUMBER_HIGH_ENDS) / ((float)PIPE_SIZE);
}


// current high-end percentage in pipe p
float cur_percentage(pipe_t* p) {
  int i;
  int h=0; // number of high ends
  int a=0; // number of non-empty spaces

  for (i=0; i<PIPE_SIZE; i++) {
    if (p->contents[i] == HIGH_END_ITEM) {h++;}
  }

  a = cur_items_in_pipe(p);

  if (a==0) {
    return 0;
  } else {
    return ((float)h)/((float)a);
  }
}


// return true if pipe empty
bool pipe_empty(pipe_t* p) {
  if (p->next >= PIPE_SIZE) {
    return(true);
  } else {
    return(false);
  }
}


// pull from pipe - return item pulled
// -1 is error code if pipe is empty
int pull_pipe_helper(pipe_t* p) {
  int temp;
  if (pipe_empty(p)) {
    return -1;
  } else {
    temp = p->contents[p->next];
    p->contents[p->next] = EMPTY_ITEM;
    (p->next)++;
    return temp;
  }
}


// pull from pipe - return item pulled
// -1 is error code if pipe was empty
// or insufficient number of rubies
int pull_pipe(pipe_t* p, int* rubies) {
  if (*rubies < 5) {
    return -1;
  } else {
    *rubies -= 5;
    return pull_pipe_helper(p);
  }
}


// pull 10 items from pipe
// returns -1 if fewer than 10 items in pipe to begin with
// or if insufficient number of rubies
// otherwise returns the number of high-end items pulled
int pull_10_pipe(pipe_t* p, int* rubies){
  int he_items = 0; // number of high end items found it pipe
  int i;
  int item;

  if ((cur_items_in_pipe(p) < 10) || (*rubies < 45)) {
    return -1;
  } else {
    *rubies -= 45;
    for (i = 0; i<10; i++) {
      item = pull_pipe_helper(p);
      if (item == HIGH_END_ITEM) {
	he_items ++;
      }
    }
    return he_items;
  }
}


// print item from or in pipe
void print_item(int item) {
  switch (item) {
    case EMPTY_ITEM:
      printf("-");
      break;

    case NORMAL_ITEM:
      printf("n");
      break;

    case SUPER_ITEM:
      printf("s");
      break;

    case HIGH_END_ITEM:
      printf("HIGH END");
      break;

    default:
      printf("*** ERROR ***");
      break;
    }
};


// print contents of pipe
void print_pipe(pipe_t* p) {
  int i;

  printf("\n");
  for (i = 0; i<PIPE_SIZE; i++) {
    printf("%d: ", i);
    print_item(p->contents[i]);
    printf("\n");    
  }

  return;
}


// print contents of distribution debug array
void print_pipe_distribution(pipe_t* p) {
  int i;

  printf("\nDISTRIBUTION OF HIGH ENDS\n");
  for (i = 0; i<PIPE_SIZE; i++) {
    printf("%d: %d\n", i, p->distribution[i]);
  }

  return;
}


// print current pipe/ruby status
void print_state(pipe_t* p, int rubies) {
  printf("odds of %f with %d rubies and %d items in pipe\n",
	 cur_percentage(p), rubies, cur_items_in_pipe(p));
}


// *********** MAIN PROGRAM ****************

int main () {
  pipe_t pipe;
  int i,j;
  int item;
  int myrubies;
  int exp;
  int stats[STAT_ROWS][STAT_COLS];       // array to keep results of experiments
  int sums[STAT_COLS];                   // summary statistics
  int he_min;                            // minimum number of high ends in experiment
  int he_max;                            // maximum number of high ends in experiment
  
  bool debug_print = DEBUG_STATUS;

  //seed random numbers
  srand((unsigned)time(NULL));

  //initialize pipe debug facility
  init_pipe(&pipe);


  printf("\nmpp  Copyright (C) 2021 Brent Hailpern\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions\n\n");
  

  // **** STRATEGY: BEAT THE PIPE ****


  
  //initialize stats structures
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      stats[i][j] = 0;
    }
  }
  
  for (j=0; j<STAT_COLS; j++){
    sums[j] = 0;
  }
  


  // main experiment loop - BEAT THE PIPE STRATEGY
  for (exp=0; exp<MAX_EXPERIMENTS; exp++) {
    if (debug_print) {
      printf("\n====================\nSTART EXPERIMENT - BEAT THE PIPE - %d\n====================\n\n", exp);
    }
    
    myrubies = RUBY_START;

    reset_pipe(&pipe);

    // record experiment number in statistics table
    stats[exp][EXP_COL] = exp;

    if (debug_print) {
      printf("INTITIALLY: ");
      print_state(&pipe, myrubies);
      printf("\n");
    }

    while (myrubies >= 5) {
      if (debug_print) { 
	print_state(&pipe, myrubies);
      }

      if (cur_percentage(&pipe) < init_percentage(&pipe)) {
	reset_pipe(&pipe);
	
	if (debug_print) {
	  printf("RESET pipe\n");
	  printf("POST RESET: ");
	  print_state(&pipe, myrubies);
	}
      }
    
      if ((cur_items_in_pipe(&pipe) > 10) && (myrubies >= 45)) {
	if (debug_print) {
	  printf("10 PULL with %d rubies: ", myrubies);
	}
	
	item = pull_10_pipe(&pipe, &myrubies);

	if (item < 0) {
	  printf("\n***ERROR in 10 pull***\n");
	  return -1;
	}

	if (debug_print) {
	  printf("%d high-end(s)", item);
	}
	    
	// add 45 rubies to the number spent
	stats[exp][RUBY_COL] += 45;

	// add the number of high ends found
	stats[exp][HE_COL] += item;

	// update the number of pulls by 10
	stats[exp][PULLS_COL] += 10;

      } else if (cur_items_in_pipe(&pipe) > 0) {
	if (debug_print) {
	  printf("1 PULL with %d rubies: ", myrubies);
	}
	    
	item = pull_pipe(&pipe, &myrubies);

	if (debug_print) {
	  print_item(item);
	}
	
	// add 5 rubies to the number spent
	stats[exp][RUBY_COL] += 5;

	// add 1 if HE found
	if (item == HIGH_END_ITEM) {
	  (stats[exp][HE_COL])++;
	}

	// update the number of pulls by 1
	(stats[exp][PULLS_COL])++;
	
      } else {
	// somehow the pipe got empty - reset it
	reset_pipe(&pipe);

	if (debug_print) {
	  printf("Pipe is empty!\n");
	  printf("RESET pipe\n");
	  printf("POST RESET: ");
	  print_state(&pipe, myrubies);
	}
      }
      
      if (debug_print) {
	printf("\n\n");
      }
       
    }

    if (debug_print) {
      printf("Remaining rubies: %d\n", myrubies);
    }
  }

  // print out stats table

  printf("\nSTATISTICS: BEAT THE PIPE\n");
  if (debug_print) {
    printf("EXP   RUBY  HEs   PULLS\n");
  }

  he_min = stats[0][HE_COL];
  he_max = stats[0][HE_COL];
  
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      sums[j] += stats[i][j];     //gather summary sattistics

      if (debug_print) {
	printf("%-5d ",stats[i][j]);
      }
    }

    if (stats[i][HE_COL] < he_min) {
      he_min = stats[i][HE_COL];
    }

    if (stats[i][HE_COL] > he_max) {
      he_max = stats[i][HE_COL];
    }

    if (debug_print) {
      printf("\n");
    }
  }

  printf("Number of experiments run: %d\n", MAX_EXPERIMENTS);
  printf("Pipe size: %d\n", PIPE_SIZE);
  printf("Number of High Ends per Pipe: %d\n", NUMBER_HIGH_ENDS);
  printf("Rubies used per experiment: %d\n", RUBY_START);
  printf("Average number of High Ends: %f\n", ((float)(sums[HE_COL])/MAX_EXPERIMENTS));
  printf("Average number of Pulls: %f\n", ((float)(sums[PULLS_COL])/MAX_EXPERIMENTS));
  printf("Number of Rubies/Number of High End: %f\n", ((float)(sums[RUBY_COL])/(float)(sums[HE_COL])));
  printf("Min Number of High-Ends in a Single Experiment: %d\n", he_min);
  printf("Max Number of High-Ends in a Single Experiment: %d\n", he_max);
  




  // **** STRATEGY: RESET AFTER HIGH END ****



  //initialize stats structures
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      stats[i][j] = 0;
    }
  }
  
  for (j=0; j<STAT_COLS; j++){
    sums[j] = 0;
  }



  // main experiment loop - SINGLE PULL and RESET AFTER HIGH END
  for (exp=0; exp<MAX_EXPERIMENTS; exp++) {
    if (debug_print) {
      printf("\n====================\nSTART EXPERIMENT - SINGLE PULLS AND RESET AFTER HIGH END - %d\n====================\n\n",
	     exp);
    }
    
    myrubies = RUBY_START;

    reset_pipe(&pipe);

    // record experiment number in statistics table
    stats[exp][EXP_COL] = exp;

    if (debug_print) {
      printf("INTITIALLY: ");
      print_state(&pipe, myrubies);
      printf("\n");
    }

    while (myrubies >= 5) {
      if (debug_print) { 
	print_state(&pipe, myrubies);
      }
    
      if (cur_items_in_pipe(&pipe) > 0) {
	if (debug_print) {
	  printf("1 PULL with %d rubies: ", myrubies);
	}
	    
	item = pull_pipe(&pipe, &myrubies);

	if (debug_print) {
	  print_item(item);
	}
	
	// add 5 rubies to the number spent
	stats[exp][RUBY_COL] += 5;

	// add 1 if HE found and reset the pipe
	if (item == HIGH_END_ITEM) {
	  (stats[exp][HE_COL])++;
	  reset_pipe(&pipe);

	  if (debug_print) {
	    printf("\nPIPE RESET\n");
	  }
	}

	// update the number of pulls by 1
	(stats[exp][PULLS_COL])++;
	
      } else {
	// somehow the pipe got empty - reset it
	reset_pipe(&pipe);

	if (debug_print) {
	  printf("Pipe is empty!\n");
	  printf("RESET pipe\n");
	  printf("POST RESET: ");
	  print_state(&pipe, myrubies);
	}
      }
      
      if (debug_print) {
	printf("\n\n");
      }
    }

    if (debug_print) {
      printf("Remaining rubies: %d\n", myrubies);
    }
  }

  // print out stats table

  printf("\nSTATISTICS: SINGLE PULL and RESET AFTER HIGH END\n");
  if (debug_print) {
    printf("EXP   RUBY  HEs   PULLS\n");
  }
  
  he_min = stats[0][HE_COL];
  he_max = stats[0][HE_COL];
  
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      sums[j] += stats[i][j];     //gather summary sattistics

      if (debug_print) {
	printf("%-5d ",stats[i][j]);
      }
    }

    if (stats[i][HE_COL] < he_min) {
      he_min = stats[i][HE_COL];
    }

    if (stats[i][HE_COL] > he_max) {
      he_max = stats[i][HE_COL];
    }

    if (debug_print) {
      printf("\n");
    }
  }

  printf("Number of experiments run: %d\n", MAX_EXPERIMENTS);
  printf("Pipe size: %d\n", PIPE_SIZE);
  printf("Number of High Ends per Pipe: %d\n", NUMBER_HIGH_ENDS);
  printf("Rubies used per experiment: %d\n", RUBY_START);
  printf("Average number of High Ends: %f\n", ((float)(sums[HE_COL])/MAX_EXPERIMENTS));
  printf("Average number of Pulls: %f\n", ((float)(sums[PULLS_COL])/MAX_EXPERIMENTS));
  printf("Number of Rubies/Number of High End: %f\n", ((float)(sums[RUBY_COL])/(float)(sums[HE_COL])));
  printf("Min Number of High-Ends in a Single Experiment: %d\n", he_min);
  printf("Max Number of High-Ends in a Single Experiment: %d\n", he_max);



  // **** STRATEGY: BEAT THE PIPE USING SINGLE PULLS****


  
  //initialize stats structures
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      stats[i][j] = 0;
    }
  }
  
  for (j=0; j<STAT_COLS; j++){
    sums[j] = 0;
  }

  // main experiment loop - BEAT THE PIPE STRATEGY USING SINGLE PULLS
  for (exp=0; exp<MAX_EXPERIMENTS; exp++) {
    if (debug_print) {
      printf("\n====================\nSTART EXPERIMENT - BEAT THE PIPE USING SINGLE PULLS  - %d\n====================\n\n", exp);
    }
    
    myrubies = RUBY_START;

    reset_pipe(&pipe);

    // record experiment number in statistics table
    stats[exp][EXP_COL] = exp;

    if (debug_print) {
      printf("INTITIALLY: ");
      print_state(&pipe, myrubies);
      printf("\n");
    }

    while (myrubies >= 5) {
      if (debug_print) { 
	print_state(&pipe, myrubies);
      }

      if (cur_percentage(&pipe) < init_percentage(&pipe)) {
	reset_pipe(&pipe);
	
	if (debug_print) {
	  printf("RESET pipe\n");
	  printf("POST RESET: ");
	  print_state(&pipe, myrubies);
	}
      }
    
      if (cur_items_in_pipe(&pipe) > 0) {
	if (debug_print) {
	  printf("1 PULL with %d rubies: ", myrubies);
	}
	    
	item = pull_pipe(&pipe, &myrubies);

	if (debug_print) {
	  print_item(item);
	}
	
	// add 5 rubies to the number spent
	stats[exp][RUBY_COL] += 5;

	// add 1 if HE found
	if (item == HIGH_END_ITEM) {
	  (stats[exp][HE_COL])++;
	}

	// update the number of pulls by 1
	(stats[exp][PULLS_COL])++;
	
      } else {
	// somehow the pipe got empty - reset it
	reset_pipe(&pipe);

	if (debug_print) {
	  printf("Pipe is empty!\n");
	  printf("RESET pipe\n");
	  printf("POST RESET: ");
	  print_state(&pipe, myrubies);
	}
      }
      
      if (debug_print) {
	printf("\n\n");
      }
       
    }

    if (debug_print) {
      printf("Remaining rubies: %d\n", myrubies);
    }
  }

  // print out stats table

  printf("\nSTATISTICS: BEAT THE PIPE USING SINGLE PULLS\n");
  if (debug_print) {
    printf("EXP   RUBY  HEs   PULLS\n");
  }

  he_min = stats[0][HE_COL];
  he_max = stats[0][HE_COL];
  
  for (i=0;i<STAT_ROWS; i++) {
    for (j=0; j<STAT_COLS; j++) {
      sums[j] += stats[i][j];     //gather summary sattistics

      if (debug_print) {
	printf("%-5d ",stats[i][j]);
      }
    }

    if (stats[i][HE_COL] < he_min) {
      he_min = stats[i][HE_COL];
    }

    if (stats[i][HE_COL] > he_max) {
      he_max = stats[i][HE_COL];
    }

    if (debug_print) {
      printf("\n");
    }
  }

  printf("Number of experiments run: %d\n", MAX_EXPERIMENTS);
  printf("Pipe size: %d\n", PIPE_SIZE);
  printf("Number of High Ends per Pipe: %d\n", NUMBER_HIGH_ENDS);
  printf("Rubies used per experiment: %d\n", RUBY_START);
  printf("Average number of High Ends: %f\n", ((float)(sums[HE_COL])/MAX_EXPERIMENTS));
  printf("Average number of Pulls: %f\n", ((float)(sums[PULLS_COL])/MAX_EXPERIMENTS));
  printf("Number of Rubies/Number of High End: %f\n", ((float)(sums[RUBY_COL])/(float)(sums[HE_COL])));
  printf("Min Number of High-Ends in a Single Experiment: %d\n", he_min);
  printf("Max Number of High-Ends in a Single Experiment: %d\n", he_max);
  
	
  printf("\n");
  return 0;
}





