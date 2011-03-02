#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
#include <mysql.h>
#include <time.h>
#include <stdio.h>
#include <cstring>
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#define back "one.jpg"
#define use "hand.jpg"
using namespace std;

SDL_Surface* screen;
SDL_Surface* fontSurface;
SDL_Color fColor;
SDL_Rect fontRect;

SDL_Event Event;

TTF_Font* font;

SDL_Rect background_position;
SDL_Surface* background = NULL;	// placed here so that any function can access them
SDL_Surface* user = NULL;
void printF(const char *c, SDL_Surface* screen, int x, int y);

void welcomemessage(SDL_Surface *Surface)
{
	 //Print to center of screen
    	printF("Welcome to BUILDS", screen, -1,10);
	printF("Please Swipe your ID", screen, -1, 80);
	printF("Next BUILDS Meeting:", screen, -1, 85);
	printF("6:30 pm Wednesday ", screen, -1, 90);
}


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

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	    int bpp = surface->format->BytesPerPixel;
	        /* Here p is the address to the pixel we want to retrieve */
	        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		    switch(bpp) {
			        case 1:
					        return *p;
						        break;

							    case 2:
							        return *(Uint16 *)p;
								        break;

									    case 3:
									        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
											            return p[0] << 16 | p[1] << 8 | p[2];
										        else
												            return p[0] | p[1] << 8 | p[2] << 16;
											        break;

												    case 4:
												        return *(Uint32 *)p;
													        break;

														    default:
														        return 0;       /* shouldn't happen, but avoids warnings */
															    }
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	    int bpp = surface->format->BytesPerPixel;
	        /* Here p is the address to the pixel we want to set */
	        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		    switch(bpp) {
			        case 1:
					        *p = pixel;
						        break;

							    case 2:
							        *(Uint16 *)p = pixel;
								        break;

									    case 3:
									        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
											            p[0] = (pixel >> 16) & 0xff;
												                p[1] = (pixel >> 8) & 0xff;
														            p[2] = pixel & 0xff;
															            } else {
																	                p[0] = pixel & 0xff;
																			            p[1] = (pixel >> 8) & 0xff;
																				                p[2] = (pixel >> 16) & 0xff;
																						        }
										        break;

											    case 4:
											        *(Uint32 *)p = pixel;
												        break;
													    }
}




SDL_Surface *ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
	    if(!Surface || !Width || !Height)
		            return 0;
	        
	        SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
				        Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
		    double  _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
			            _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

		        for(Sint32 y = 0; y < Surface->h; y++)
				        for(Sint32 x = 0; x < Surface->w; x++)
						            for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
								                    for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
											                        putpixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x, 
																                        static_cast<Sint32>(_stretch_factor_y * y) + o_y, getpixel(Surface, x, y));

			    return _ret;
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
void printF(const char *c, SDL_Surface* screen, int x, int y){
        fontSurface = TTF_RenderText_Solid(font, c, fColor);
        if(x==-1)
        {
        	fontRect.x = screen->w/2 - ((strlen(c)*18)/2);
        }
        else
        {
        	fontRect.x= x;
        }
        fontRect.y = ((double)(y)/100)*screen->h;
        SDL_BlitSurface(fontSurface, NULL, screen, &fontRect);
        SDL_Flip(screen);
}

int main(int argc, char** argv)
{
	background_position.x = 0; 			// initialize position rectangle
	background_position.y = 0;
   	int fd;
   	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd == -1)
	{
		fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NONBLOCK);
		if(fd == -1)
		{
			cout<<"Could not connect to Ardunio on USB port 0 or 1"<<endl;
			return 1;
		}
		cout<<"Connected to Arduino on port 1"<<endl;
	}
	cout<<"Connected to Arduino on port 0"<<endl;
	struct termios options;
	tcgetattr(fd, &options);
	
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);
							
    	// Initialize the SDL library with the Video subsystem
    	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

	// Get the current video hardware information
 	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();

    	//Create the screen
    	if(argc==2)
	{
		screen = SDL_SetVideoMode(640, 480, 16,SDL_SWSURFACE);
	}
	else
	{
		screen = SDL_SetVideoMode(myPointer->current_w, myPointer->current_h, 16,  SDL_DOUBLEBUF | SDL_FULLSCREEN);
	}
	if (screen == NULL)
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	//background
	user = ScaleSurface(Load_image(use),screen->w,screen->h); // load our cake
	background = ScaleSurface(Load_image(back), screen->w, screen->h);
	SDL_BlitSurface( background, NULL, screen, &background_position ); // put the background on
    	//Initialize fonts
    	fontInit();
	
	welcomemessage(screen);
	
	string id2;
    do {
        // Process the events
        
        while (SDL_WaitEvent(&Event)) 
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
						if(argc==2)
						{
						}
						else
						{
							id2.replace(0,10,"");
							id2.replace(0,1,"u");
						}
						MYSQL mysql; 
						MYSQL_RES *res; 
						MYSQL_ROW row; 
						string name;
						char query[90]; 
						mysql_init(&mysql); 
						if(argc==2)
						{
							if (!mysql_real_connect(&mysql,"thingone.furstlabs.com","root","","door",0,NULL,0))
							{
								fprintf(stderr, "Failed to connect to database: Error: %s\n",
								mysql_error(&mysql));
							}
						}
						else if (!mysql_real_connect(&mysql,"localhost","door","buildsdoor","door",0,NULL,0))
						{
							fprintf(stderr, "Failed to connect to database: Error: %s\n",
								mysql_error(&mysql));
						}

						sprintf(query,"SELECT id, first FROM users where swipe=\"%s\"",id2.c_str()); 
						
						if(mysql_real_query(&mysql,query,(unsigned int) strlen(query))!= 0 )
						{
							cout<<mysql_error(&mysql)<<endl;
						}
						res = mysql_store_result(&mysql); 

						while((row = mysql_fetch_row(res)))
						{ 
							name = "Hello ";
							name.append(row[1]);
							
							sprintf(query,"insert into log(user,time) VALUES(\"%s\",NOW())",row[0]); 
							
							if(mysql_real_query(&mysql,query,(unsigned int) strlen(query))!= 0 )
							{
								cout<<mysql_error(&mysql)<<endl;
							}
							
							SDL_FillRect(screen,NULL, 0x000000);
							SDL_Flip(screen); 
							SDL_BlitSurface(user, NULL, screen, &background_position ); // show the users background
							//Say hello
							printF((char*)name.c_str(), screen, -1, 10); 

							//unlock the door
							int ww = 0;
							ww= write(fd, "u\r", 3);
							if(ww != 3)
							{
								cout<<"Did not succesfully write to Arduino. Only wrote "<<ww<<" bytes"<<endl;
							}
							
							mysql_free_result(res); 
						}
						SDL_Delay(5000);  // I put this here so you will be able to see the animation change!	
						id2="";	
						SDL_BlitSurface( background, NULL, screen, &background_position ); // put the background on
						welcomemessage(screen);
					}
                break;
				default:
				break;
			}
		}
	}
	while (Event.type != SDL_QUIT);

    // Cleanup
    SDL_Quit();

    return 0;
}
