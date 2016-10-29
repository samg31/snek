#include "SDL/SDL.h"
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <cmath>

enum DIRECTION {
    NORTH,
    WEST,
    SOUTH,
    EAST
};

struct Food
{
    Food()
	:texture( SDL_LoadBMP( "../img/snek.bmp" ) )
    {
	int randx = (rand() % 640);
	int randy = (rand() % 480);
	x = randx - floor(log2(randx));
	y = randy - floor(log2(randy));
    }
    ~Food()
    {
	SDL_FreeSurface( texture );
    }
    void Render( SDL_Surface* screen )
    {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( texture, NULL, screen, &offset ); 
    }
    int x;
    int y;
    SDL_Surface* texture;
};

struct SnekNode
{
    SnekNode()
	:next(), prev(), x(), y(), texture( SDL_LoadBMP( "../img/snek.bmp" ) )
    {
    }
    ~SnekNode()
    {
	SDL_FreeSurface( texture );
    }
    void Render( SDL_Surface* screen )
    {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( texture, NULL, screen, &offset ); 
    }
    SnekNode* next;
    SnekNode* prev;
    SDL_Surface* texture;
    int x;
    int y;
};

class SnekList
{
public:
    SnekNode* head;
    SnekNode* tail;
    SnekList()
	:head( new SnekNode ), tail( head ), mDir( EAST )
    {
	head->x = tail->x = 640/2 - floor(log2(640));
	head->y = tail->y = 480/2 - floor(log2(640));
    }
    ~SnekList()
    {
	while( tail )
	{
	    SnekNode* newtail = tail->prev;
	    pop();
	    tail = newtail;
	}
    }
    void push()
    {
	SnekNode* temptail = new SnekNode;
	temptail->prev = tail;
	tail->next = temptail;
	tail = temptail;

	tail->x = tail->prev->x + 16;
	tail->y = tail->prev->y;
    }
    void move()
    {
	SnekNode* visitor = head;
	std::vector<std::pair<int,int>> positions;
	while( visitor )
	{
	    positions.push_back( std::make_pair( visitor->x, visitor->y ) );
	    visitor = visitor->next;
	}
	visitor = head->next;
	switch( mDir )
	{
	case NORTH:
	    head->y -= 16;
	    break;
	case SOUTH:
	    head->y += 16;
	    break;
	case EAST:
	    head->x += 16;
	    break;
	case WEST:
	    head->x -= 16;
	    break;
	}

	for( int i = 0; visitor; ++i )
	{
	    visitor->x = positions[i].first;
	    visitor->y = positions[i].second;
	    visitor = visitor->next;
	}

    }
    void pop()
    {
	SnekNode* tempnext = nullptr;
	if( head != tail )
	{
	    tempnext = tail->prev;
	}
	delete tail;
	tail = tempnext;
	
    }
    void renderlist( SDL_Surface* screen )
    {
	SnekNode* visitor = head;
	while( visitor )
	{
	    visitor->Render( screen );
	    visitor = visitor->next;
	}
    }
    void updatepositions()
    {
	SnekNode* visitor = tail;
	while( visitor )
	{
	    visitor->y += 16;
	    visitor = visitor->prev;
	}
    }

    DIRECTION mDir;
};

bool Collision_With_Food( SnekList& snek, Food* food )
{
    return ( ( snek.head->x == food->x ) && ( snek.head->y == food->y ) );
}

int main( int argc, char** argv )
{
    srand( time( NULL ) );
    SDL_Init( SDL_INIT_EVERYTHING );

    SDL_Surface* screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
    SDL_Surface* background = SDL_LoadBMP( "../img/background.bmp" );
    SDL_BlitSurface( background, NULL, screen, NULL );
    SDL_Flip( screen );
    SDL_Event e;
    bool quit = false;
    SnekList list;

    SDL_Surface* vert = SDL_LoadBMP( "../img/vert.bmp" );
    SDL_Surface* horiz = SDL_LoadBMP( "../img/horiz.bmp" );

    std::vector<SDL_Rect> vert_offsets;
    for( int i = 0; i < 480; i+=16 )
    {
	SDL_Rect offset;
	offset.y = i;
    }

    bool is_food = true;
    auto food = new Food;

    while( !quit )
    {
	SDL_BlitSurface( background, NULL, screen, NULL );
	for( int i = 0; i < 480; i+=16 )
	{
	    SDL_Rect offset;
	    offset.y = i;
	    SDL_BlitSurface( vert, NULL, screen, &offset );
	}
	for( int i = 0; i < 640; i += 16 )
	{
	    SDL_Rect offset;
	    offset.x = i;
	    SDL_BlitSurface( horiz, NULL, screen, NULL );
	}
	list.renderlist( screen );
	food->Render( screen );
	SDL_Flip( screen );
	list.move();

	if( !is_food )
	{
	    food = new Food;
	    is_food = true;
	}
	while( SDL_PollEvent( &e ) )
	{
	    if( e.type == SDL_QUIT )
		quit = true;
	    if( e.type == SDL_KEYDOWN )
	    {
		if( e.key.keysym.sym == SDLK_p )
		    list.push();
		else if( e.key.keysym.sym == SDLK_UP )
		    list.mDir = NORTH;
		else if( e.key.keysym.sym == SDLK_DOWN )
		    list.mDir = SOUTH;
		else if( e.key.keysym.sym == SDLK_RIGHT )
		    list.mDir = EAST;
		else if( e.key.keysym.sym == SDLK_LEFT )
		    list.mDir = WEST;
		else if( e.key.keysym.sym == SDLK_ESCAPE )
		    quit = true;
	    }
	    
	}
	if( Collision_With_Food( list, food ) )
	{
	    SDL_Delay( 1 );
	    delete food;
	    list.push();
	    is_food = false;
	}
	SDL_Delay( 100 );
    }

    SDL_Quit();
    return 0;
}
