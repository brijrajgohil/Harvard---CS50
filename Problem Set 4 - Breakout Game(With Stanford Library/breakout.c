//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// height and width of paddle
#define PHEIGHT 15
#define PWIDTH 80

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// height and width of bricks
#define BHEIGHT 10
#define BWIDTH 37

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
GLabel initLives(GWindow window);
GObject detectCollision(GWindow window, GOval ball, int n);

int main(int argc, string argv[])
{
    // check for god mode
    int n = 0;
    if (argv[1] != NULL && strcmp(argv[1], "GOD") == 0) n = 1;
    
    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // instantiate lives
    GLabel lifeLabel = initLives(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // variable to pause game when life lost
    int paused = 0;
    
    // y and x velocity of ball
    double yVelocity = 1;
    double xVelocity;
    
    // text of score and life labels
    char scoreCount[12];
    char lifeCount[12];
    
    // x velocity above 0.5 under 1.0
    do
    {
        xVelocity = drand48();
    } while(xVelocity <= 0.5);
    // approximately 50 % chance for ball to move to either left or right
    double random = drand48();
    if (random < 0.5)
    {
        xVelocity = -xVelocity;
    }
    
    // Start game when mouse clicked
    if (n == 0) waitForClick();

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one, and GOD wasn't passed
        if (event != NULL && n == 0)
        {
            // if the event was movement set paddle at mouse coordinate
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - PWIDTH / 2;
                
                setLocation(paddle, x, HEIGHT - 50);
            }
        }
        // if the argument GOD was passed set paddle below ball
        else if(n == 1)
        {
            setLocation(paddle, getX(ball) + RADIUS - PWIDTH / 2, HEIGHT - 50);
        }
        // move ball
        move(ball, xVelocity, yVelocity);
        
        // if ball gets below paddle
        if (getY(ball) + RADIUS * 2 >= HEIGHT)
        {
            // Remove ball
            removeGWindow(window, ball);
            
            // one life less
            lives--;

            // new ball
            ball = initBall(window);

            // make sure ball doesn't move
            yVelocity = 0;
            xVelocity = 0;

            // update label to show new life count
            sprintf(lifeCount, "Lives: %d", lives);
            setLabel(lifeLabel, lifeCount);
            
            // pause the game
            paused = 1;
        }
        // bounces ball off top
        else if (getY(ball) <= 0)
        {
            yVelocity = -yVelocity;
        }
        // bounces ball off right side
        else if (getX(ball) + RADIUS * 2 >= WIDTH)
        {
            xVelocity = -xVelocity;
        }
        // bounces ball off left side
        else if (getX(ball) <= 0)
        {
            xVelocity = -xVelocity;
        }
        // object to detect collisions between ball and paddle/bricks
        GObject object = detectCollision(window, ball, 1);
        // if a collision has occured
        if (object != NULL)
        {
            // if the collision was with top of paddle, bounce off
            if (object == paddle && getY(ball) <= HEIGHT - PHEIGHT - 52)
            {
                yVelocity = -yVelocity;
            }
            // if collision with rectangle, not paddle (brick)
            else if (object != paddle && strcmp(getType(object), "GRect") == 0)
            {   
                // remove brick
                removeGWindow(window, object);
                
                // bounce off brick
                yVelocity = -yVelocity; 
                
                // if ball struck the side of the brick, bounce back
                if (getY(ball) < getY(object) + BHEIGHT && getY(ball) + 2 * RADIUS > getY(object)) xVelocity = -xVelocity; 
                
                // + 1 point
                points++;
                
                // update label to show new score
                sprintf(scoreCount, "Score: %d", points);
                setLabel(label, scoreCount);
                
                // one brick less
                bricks--;
                
            }
        }
        
        // if the game is paused and the mouse is clicked, resume game
        if (paused == 1 && event != NULL && getEventType(event) == MOUSE_CLICKED)
        {
            // set the velocity of ball to start game
            yVelocity = 1;
            do
            {
                xVelocity = drand48();
            } while (xVelocity <= 0.5);
            random = drand48();
            if (random < 0.5)
            {
                xVelocity = -xVelocity;
            }
            // game not paused
            paused = 0; 
        }
        
        // delay game, making it playable
        pause(2);
    }
    // update labels to either "YOU WiN!" or "YOU LOSE!", with corresponding colors when game over
    if (lives > 0)
    {
        setLabel(label, "YOU WIN!");
        setColor(lifeLabel, "GREEN");
        setLabel(lifeLabel, "YOU WIN!");
    }
    if (lives < 1)
    {
        setLabel(lifeLabel, "YOU LOSE!");
        setColor(label, "RED");
        setLabel(label, "YOU LOSE!");
    }
    
    // updates current location of labels, based on their new lengths
    setLocation(label, WIDTH / 4 - getWidth(label) / 2, 20);
    setLocation(lifeLabel, (WIDTH / 4) * 3 - getWidth(lifeLabel) / 2, 20);
    
    // wait for click before exiting
    waitForClick();
    
    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // initial starting block for bricks, x and y
    int x = 0;
    int y = 55;

    // color counter to change colors of bricks
    int color = 0;

    // nested for loops to create ROWS * COLS bricks.
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            // Create brick, and fill
            GRect brick = newGRect(x, y, BWIDTH, BHEIGHT);
            setFilled(brick, true);
            
            // set color based on color counter and color scheme
            if (color % 5 == 0) setColor(brick, "BLUE");
            if (color % 5 == 1) setColor(brick, "GREEN");
            if (color % 5 == 2) setColor(brick, "YELLOW");
            if (color % 5 == 3) setColor(brick, "ORANGE");
            if (color % 5 == 4) setColor(brick, "RED");
            
            // add brick to window
            add(window, brick);
            
            // update x coordinate for next brick
            x = x + BWIDTH + 3;
        }
        // reset x coordinate
        x = 0;
        
        // update y coordinate for next row
        y = y + BHEIGHT + 5;
        
        // update color counter
        color++;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // Insitantiates ball in middle of window, with radius of RADIUS
    GOval ball = newGOval(WIDTH / 2 - RADIUS / 2, HEIGHT / 2 - RADIUS / 2, RADIUS * 2, RADIUS * 2);
    
    // solid ball, with color black
    setFilled(ball, true);
    setColor(ball, "BLACK");
    
    // add ball to window, and return ball
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // instantiates paddle in middle and bottom of screen
    // with width PWIDTH and height PHEIGHT
    GRect paddle = newGRect((WIDTH / 2) - (PWIDTH / 2), HEIGHT - 50, PWIDTH, PHEIGHT);
    
    // solid paddle with color black
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    
    // add to window, and return paddle
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // instantiates label with initial text "Score: 0"
    GLabel label = newGLabel("Score: 0");
    
    // color: green, and add to window
    setColor(label, "GREEN");
    add(window, label);
    
    // set the location in the top, a fourth of WIDTH of window
    setLocation(label, WIDTH / 4 - getWidth(label) / 2, 20);
    
    // change size of label to 20 and return lable
    setFont(label, "Dialog-*-20");
    return label;
}

/** 
 * Instantiates, configures, and returns label for lives.
 */
GLabel initLives(GWindow window)
{
    // variable to contain initial lives, based on macro LIVES
    char l[12];
    sprintf(l, "Lives: %d", LIVES);
    
    // Instantiates label with initial text "Lives: %LIVES"
    GLabel label = newGLabel(l);
    
    // color: red, and add to window
    setColor(label, "RED");
    add(window, label);
    
    // set the location in the top, three fourths of WIDTH of window
    setLocation(label, (WIDTH / 4) * 3 - getWidth(label) / 2, 20);
    
    // change size of label to 20 and return label
    setFont(label, "Dialog-*-20");
    return label;
}
/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GObject ball, int n)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }
    // check for collision at ball's bottom-left corner
    if (n == 1)
    {
        object = getGObjectAt(window, x, y + 2 * RADIUS);
        if (object != NULL)
        {
            return object;
        }

        // check for collision at ball's bottom-right corner
       object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
      if (object != NULL)
        {
            return object;
        }
    }

    // no collision
    return NULL;
}
