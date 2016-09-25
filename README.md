# Graphics-Space-Invaders
This program implements the Space Invaders video game.

When the game starts, user can control the ship using either keyboard left and right key or the mouse cursor position. The aliens/enemies will start moving to the right and when it hits the boarder, moving down and going back to the opposite direction. Notice that the aliens/enemies will increase the speed gradually while moving down toward the ship. The alients/enemies will continue dropping down bombs and if the user's ship is hitten by the bomb, the ship will lose one life. The ship has a total of 3 lives and the goal is to shoot all the alients/enemies. The missles are unlimited, meaning that the player can use either the space key or the left mouse button to shoot as many missles as they want. The player can use the 'z' and 'c' keys to control the rotation of the gun and if the player repeated holding the space key down, the missles can be shooted repeatedly in a chunk. As soon as the missle hits an alien ship, the alien ship and the missle are both destroyed. When all aliens are destroyed, the player wins; else if the player loses all the lives of its ship or the aliens move down and reach the same level as their ship, the player died.

During the game, the player is allowed to use either Right button or the 'd' key to pause the game. While the 'd' key or Right button being pressed, the state of the game is advanced by one step. By repeatedly hitting the right button/'d' key, the program will perform consecutive single steps. When the 'r' key is pressed, the game resumes if it is paused. Finally, when the 'q'/'escape' key is hit, the program quits.

Extra Features Implemented beyond the Requirements:
1. Multiple lives for ship
2. Control with both keyboard and mouse to accomodate different player's preferences
3. Rotating gun and repeated shooting
4. Personalized object design.
