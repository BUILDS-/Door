#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <cstring>
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

using namespace std;

SDL_Surface* screen;
SDL_Surface* fontSurface;
SDL_Color fColor;
SDL_Rect fontRect;

SDL_Event Event;

TTF_Font* font;

SDL_Rect background_position;
SDL_Surface* loaded_image = NULL;	// placed here so that any function can access them
SDL_Surface* cake = NULL;
 void PrintKeyInfo( SDL_KeyboardEvent *key );
    void PrintModifiers( SDLMod mod );

    void PrintKeyInfo( SDL_KeyboardEvent *key ){

        /* Print the name of the key */
        printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
        /* We want to print the unicode info, but we need to make */
        /* sure its a press event first (remember, release events */
        /* don't have unicode info                                */
        if( key->type == SDL_KEYDOWN ){
            /* If the Unicode value is less than 0x80 then the    */
            /* unicode value can be used to get a printable       */
            /* representation of the key, using (char)unicode.    */
            printf(", Unicode: " );
            if( key->keysym.unicode < 0x80 && key->keysym.unicode > 0 ){
                printf( "%c (0x%04X)", (char)key->keysym.unicode,
                        key->keysym.unicode );
            }
            else{
                printf( "? (0x%04X)", key->keysym.unicode );
            }
        }
        printf( "\n" );
        /* Print modifier info */
        PrintModifiers( key->keysym.mod );
    }

    /* Print modifier info */
    void PrintModifiers( SDLMod mod ){
        printf( "Modifers: " );

        /* If there are none then say so and return */
        if( mod == KMOD_NONE ){
            printf( "None\n" );
            return;
        }

        /* Check for the presence of each SDLMod value */
        /* This looks messy, but there really isn't    */
        /* a clearer way.                              */
        if( mod & KMOD_NUM ) printf( "NUMLOCK " );
        if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) printf( "LCTRL " );
        if( mod & KMOD_RCTRL ) printf( "RCTRL " );
        if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
        if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
        if( mod & KMOD_RALT ) printf( "RALT " );
        if( mod & KMOD_LALT ) printf( "LALT " );
        if( mod & KMOD_CTRL ) printf( "CTRL " );
        if( mod & KMOD_SHIFT ) printf( "SHIFT " );
        if( mod & KMOD_ALT ) printf( "ALT " );
        printf( "\n" );
    }

SDL_Surface *Load_image( std::string filename )
{
	SDL_Surface* loaded_image = NULL;
	SDL_Surface* compatible_image = NULL;

	if(filename.c_str() == NULL) { // check to see if a filename was provided
		// if not exit the function
		return NULL;
	}

	// load the image using our new IMG_Load function from sdl-Image1.2
	loaded_image = IMG_Load( filename.c_str() );

	if( loaded_image == NULL ){ // check to see if it loaded properly
		// if not exit the function
		return NULL;
	}	

	// the image loaded fine so we can now convert it to the current display depth
	compatible_image = SDL_DisplayFormat( loaded_image );

	if( compatible_image != NULL ) {
		// specify a colour that will be used to signify 'transparent' pixels
		Uint32 colorkey = SDL_MapRGB( compatible_image->format, 0, 0, 0); // choose black
		// now tell SDL to remeber our choice
		SDL_SetColorKey( compatible_image, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);
		// SDL_RLEACCEL is run lenght encoding acceleration to speed up the colorkeying
		// SDL_SRCCOLORKEY tells SDL that this color key applies to the source image
	}

	// Destroy the old copy
	SDL_FreeSurface( loaded_image );

	// return a pointer to the newly created display compatible image
	return compatible_image;
}

//Initialize the font, set to white
void fontInit(){
        TTF_Init();
        font = TTF_OpenFont("/usr/share/fonts/truetype/unfonts/UnDotum.ttf", 36	);
        fColor.r = 255;
        fColor.g = 255;
        fColor.b = 255;
}

//Print the designated string at the specified coordinates
void printF(char *c, int x, int y){
        fontSurface = TTF_RenderText_Solid(font, c, fColor);
        fontRect.x = x;
        fontRect.y = y;
        SDL_BlitSurface(fontSurface, NULL, screen, &fontRect);
        SDL_Flip(screen);
}

int main(int argc, char** argv)
{
	background_position.x = 0; 			// initialize position rectangle
	background_position.y = 0;
   
    	// Initialize the SDL library with the Video subsystem
    	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	SDL_Surface* loaded_image = NULL;

	// Get the current video hardware information
 	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();

    	//Create the screen
    	screen = SDL_SetVideoMode(myPointer->current_w, myPointer->current_h, 16,  SDL_DOUBLEBUF | SDL_FULLSCREEN);
	if (screen == NULL)
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	//background
	loaded_image = Load_image("one.jpg"); // load our background Surface
	cake = Load_image("cake.bmp"); // load our cake

	SDL_BlitSurface( loaded_image, NULL, screen, &background_position ); // put the background on
    //Initialize fonts
    fontInit();

    //Print to center of screen
    printF("Welcome to BUILDS", screen->w/2 - 38*3, 100);
	printF("Please Swipe your ID", screen->w/2 - 38*3, 700);
	printF("Next BUILDS Meeting:", screen->w/2 - 38*3, 900);
	printF("6:30 pm Wednesday ", screen->w/2 - 38*3, 950);
	string id2;
    do {
        // Process the events
        
        while (SDL_PollEvent(&Event)) 
        {
            switch (Event.type)
            {
                case SDL_KEYUP:
                    if(Event.key.keysym.scancode != 0x24)
                    {
						id2.append(SDL_GetKeyName( Event.key.keysym.sym ));
					}
					else
					{
						id2.replace(0,10,"");
						id2.replace(0,1,"u");
						MYSQL mysql; 
						MYSQL_RES *res; 
						MYSQL_ROW row; 
						string name;
						char query[90]; 
						mysql_init(&mysql); 
						if (!mysql_real_connect(&mysql,"localhost","door","buildsdoor","door",0,NULL,0))
						{
							fprintf(stderr, "Failed to connect to database: Error: %s\n",
								mysql_error(&mysql));
						}

						sprintf(query,"SELECT first FROM users where swipe=\"%s\"",id2.c_str()); 
						
						cout<<query<<endl;
						
						if(mysql_real_query(&mysql,query,(unsigned int) strlen(query))!= 0 )
						{
							cout<<mysql_error(&mysql)<<endl;
						}
						res = mysql_use_result(&mysql); 

						while((row = mysql_fetch_row(res)))
						{ 
							name = "Hello ";
							name.append(row[0]);
							
							int fd;
							struct termios options;
							//SDL_BlitSurface(loaded_image, NULL, screen, &background_position ); // put the background on
							SDL_FillRect(screen,NULL, 0x000000);
							SDL_Flip(screen); 
							SDL_BlitSurface(cake, NULL, screen, &background_position ); // show the cakesd
							printF((char*)name.c_str(), screen->w/2 - 38*3, 100); 
							
										// without it all you would see is a blur!
							fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
							tcgetattr(fd, &options);
							
							cfsetispeed(&options, B9600);
							cfsetospeed(&options, B9600);

							write(fd, "u\r", 3);
							int nbytes;
							#define BUFSIZE		30
							char bufptr[BUFSIZE];
							while ((nbytes = read(fd, bufptr, BUFSIZE)) > 0)
							{
								if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
								{
											break;
								}
							}
							mysql_free_result(res); 
						}
						SDL_Delay( 5000);  // I put this here so you will be able to see the animation change!	
						id2="";	
						SDL_BlitSurface( loaded_image, NULL, screen, &background_position ); // put the background on
						//Print to center of screen
						printF("Welcome to BUILDS", screen->w/2 - 38*3, 100);
						printF("Please Swipe your ID", screen->w/2 - 38*3, 700);
						printF("Next BUILDS Meeting:", screen->w/2 - 38*3, 900);
						printF("6:30 pm Wednesday ", screen->w/2 - 38*3, 950);
					}
                break;
				default:
				break;
			}
		}
    //SDL_Delay(100);
	}
	while (Event.type != SDL_QUIT);

    // Cleanup
    SDL_Quit();

    return 0;
}
