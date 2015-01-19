#include <pong/PlayScene.h>
#include <pong/Constants.h>
#include <pong/PlaySceneEventHandler.h>

#include <opengl/OpenGL.h>
#include <opengl/Font.h>
#include <opengl/Screen.h>
#include <opengl/Point2D.h>
#include <opengl/Color.h>
#include <opengl/Vector.h>

#include <game/Player.h>

#include <controller/Controller.h>

#include <sstream>

using namespace opengl;
using namespace controller;
using namespace std;

#ifdef DEBUG1
#include <tools/logger/Logger.h>
using namespace tools::logger;
#endif

namespace pong
{
	PlayScene::PlayScene() : Scene(), lastDrawTicks(0), lastTickEvent(0), lastBallMove(0), background(NULL), ball(NULL), ballDirection(0.0f, 0.0f, 0.0f), racket1(NULL), racket2(NULL), scorePlayer1(0), scorePlayer2(0), playing(false), paused(false), player1Ready(false), player2Ready(false)
	{
		Player * player1 = Player::get( "Player1" );
		Player * player2 = Player::get( "Player2" );
		
		if( player1 != NULL )
			player1->setEventHandler( new PlaySceneEventHandler( this, 1 ) );

		if( player2 != NULL )
			player2->setEventHandler( new PlaySceneEventHandler( this, 2 ) );
		
		// One player mode for tests
		else if( player1 != NULL )
			player1->setEventHandler( new PlaySceneEventHandler( this, 0 ) );

		if( player1 == NULL && player2 == NULL )
		{
			#ifdef DEBUG1
			Logger::get() << "[PlayScene] No player found. Exiting." << Logger::endl;
			#endif
			
			this->running = false;
		}
		
		Color white( "ffffff" );
		this->background = new Background();
		this->ball = new ColoredRectangle( BALL_SIZE, BALL_SIZE, white );
		this->ball->getAnchor().moveTo( BALL_SIZE / 2.0f, BALL_SIZE / 2.0f );
		
		this->racket1 = new ColoredRectangle( RACKET_WIDTH, RACKET_HEIGHT, white );
		this->racket1->getOrigin().moveTo( WALL_SIZE * 3.0f, (SCREEN_HEIGHT - RACKET_HEIGHT) / 2.0f, 0.0f );
		
		this->racket2 = new ColoredRectangle( RACKET_WIDTH, RACKET_HEIGHT, white );
		this->racket2->getOrigin().moveTo( SCREEN_WIDTH - (WALL_SIZE * 3.0f) - RACKET_WIDTH, (SCREEN_HEIGHT - RACKET_HEIGHT) / 2.0f, 0.0f );
		
		this->updateScoreStrings();
		
		#ifdef DEBUG1
		this->framesPerSecond = 0;
		this->operationsPerSecond = 0;
		this->sFramesPerSecond = "0 FPS";
		this->sOperationsPerSecond = "0 OPS";
		#endif
	}
	
	PlayScene::~PlayScene()
	{
		if( this->background != NULL )
			delete this->background;
			
		if( this->ball != NULL )
			delete this->ball;
			
		if( this->racket1 != NULL )
			delete this->racket1;
			
		if( this->racket2 != NULL )
			delete this->racket2;
	}
	
	void PlayScene::updateScoreStrings()
	{
		stringstream ss;
		
		ss.str( "" );
		ss << this->scorePlayer1;
		this->sScorePlayer1 = ss.str();
		
		ss.str( "" );
		ss << this->scorePlayer2;
		this->sScorePlayer2 = ss.str();
	}
	
	void PlayScene::initializeNewGame()
	{
		this->scorePlayer1 = 0;
		this->scorePlayer2 = 0;
		this->updateScoreStrings();
		
		this->initializeBall();
		
		this->player1Ready = false;
		this->player2Ready = false;
		this->playing = true;
		this->paused = false;
	}
	
	void PlayScene::initializeBall( unsigned int player )
	{
		this->ball->getOrigin().moveTo( SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f );
		
		ColoredRectangle * racket = this->racket1;
		
		if( player == 2 )
			racket = this->racket2;
			
		// Initialize direction to player's racket center
		this->ballDirection.setX( (racket->getOrigin().getX() + (racket->getWidth() / 2.0f)) - this->ball->getOrigin().getX() );
		this->ballDirection.setY( (racket->getOrigin().getY() + (racket->getHeight() / 2.0f)) - this->ball->getOrigin().getY() );
		this->ballDirection.setZ( 0.0f );
		
		// This slow the ball for the beggining (dx < 1)
		this->ballDirection.normalize();
	}
	
	void PlayScene::handleEvent( SDL_Event * event )
	{
		switch( event->type )
		{
			case SDL_QUIT:
			{
				this->running = false;
				break;
			}
		
			case SDL_JOYAXISMOTION:
			case SDL_JOYBUTTONUP:
			case SDL_JOYBUTTONDOWN:
			{
				Controller::handleEvent( event );
				break;
			}
		}
	}
	
	void PlayScene::moveUp( unsigned int player )
	{
		if( !this->paused )
		{
			if( player == 1 || player == 0 )
			{
				if( !this->playing )
				{
					this->player1Ready = true;
			
					if( this->player2Ready )
						this->initializeNewGame();
				}
		
				this->racket1->getOrigin().moveBy( 0.0f, -1.0f * RACKET_SPEED, 0.0f );
	
				if( this->racket1->getOrigin().getY() < WALL_SIZE )
					this->racket1->getOrigin().setY( WALL_SIZE );
				else if( this->racket1->getOrigin().getY() > SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE )
					this->racket1->getOrigin().setY( SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE );
			}
		
			if( player == 2 || player == 0 )
			{
				if( !this->playing )
				{
					this->player2Ready = true;
				
					if( this->player1Ready )
						this->initializeNewGame();
				}
			
				this->racket2->getOrigin().moveBy( 0.0f, -1.0f * RACKET_SPEED, 0.0f );
		
				if( this->racket2->getOrigin().getY() < WALL_SIZE )
					this->racket2->getOrigin().setY( WALL_SIZE );
				else if( this->racket2->getOrigin().getY() > SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE )
					this->racket2->getOrigin().setY( SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE );
			}
		}
	}
	
	void PlayScene::moveDown( unsigned int player )
	{
		if( !this->paused )
		{
			if( player == 1 || player == 0 )
			{
				if( !this->playing )
				{
					this->player1Ready = true;
				
					if( this->player2Ready )
						this->initializeNewGame();
				}
			
				this->racket1->getOrigin().moveBy( 0.0f, RACKET_SPEED, 0.0f );
		
				if( this->racket1->getOrigin().getY() < WALL_SIZE )
					this->racket1->getOrigin().setY( WALL_SIZE );
				else if( this->racket1->getOrigin().getY() > SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE )
					this->racket1->getOrigin().setY( SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE );
			}
		
			if( player == 2 || player == 0 )
			{	
				if( !this->playing )
				{
					this->player2Ready = true;
				
					if( this->player1Ready )
						this->initializeNewGame();
				}
				
				this->racket2->getOrigin().moveBy( 0.0f, RACKET_SPEED, 0.0f );
		
				if( this->racket2->getOrigin().getY() < WALL_SIZE )
					this->racket2->getOrigin().setY( WALL_SIZE );
				else if( this->racket2->getOrigin().getY() > SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE )
					this->racket2->getOrigin().setY( SCREEN_HEIGHT - RACKET_HEIGHT - WALL_SIZE );
			}
		}
	}
	
	void PlayScene::buttonAction( unsigned int player )
	{
		if( !this->playing )
		{
			this->running = false;
		}
		else
		{
			this->paused = !this->paused;
			
			if( player == 1 || player == 0 )
			{
			}
		
			if( player == 2 || player == 0 )
			{
			}
		}
	}
	
	void PlayScene::live( unsigned int ticks )
	{
		if( ticks - this->lastTickEvent > 15 )
		{
			Controller::tickEvent( ticks );
			this->lastTickEvent = ticks;
		}
	
		if( this->playing && !this->paused )
		{
			if( ticks - this->lastBallMove > 15 )
			{
				#ifdef DEBUG1
				this->operationsPerSecond++;
				
				if( ticks - this->lastOPSRefresh >= 1000 )
				{
					stringstream ops;
					ops << this->operationsPerSecond << " OPS";
					this->sOperationsPerSecond = ops.str();
				
					this->operationsPerSecond = 0;
					this->lastOPSRefresh = ticks;
				}
				#endif
				
				this->ball->getOrigin().moveBy( BALL_SPEED * this->ballDirection.getX(), BALL_SPEED * this->ballDirection.getY(), BALL_SPEED * this->ballDirection.getZ() );
				
				// Check for goal for Player2
				if( this->ball->getOrigin().getX() <= WALL_SIZE )
				{
					this->scorePlayer2++;
					this->updateScoreStrings();
					this->initializeBall( 2 );
				}
				
				// Check for goal for Player1
				else if( this->ball->getOrigin().getX() >= SCREEN_WIDTH - WALL_SIZE - BALL_SIZE )
				{
					this->scorePlayer1++;
					this->updateScoreStrings();
					this->initializeBall( 1 );
				}
				
				// Check for collisions
				else
				{
					// Up & Down walls
					if( this->ball->getOrigin().getY() <= WALL_SIZE || this->ball->getOrigin().getY() >= SCREEN_HEIGHT - WALL_SIZE - BALL_SIZE )
					{
						this->ballDirection.setY( this->ballDirection.getY() * -1.0f );
					}
					
					// Rackets
					else
					{
						// Racket1
						if( this->ball->getOrigin().getX() >= this->racket1->getOrigin().getX() && this->ball->getOrigin().getX() <= this->racket1->getOrigin().getX() + RACKET_WIDTH )
						{
							if( this->ball->getOrigin().getY() + BALL_SIZE >= this->racket1->getOrigin().getY() && this->ball->getOrigin().getY() <= this->racket1->getOrigin().getY() + RACKET_HEIGHT )
							{
								this->ballDirection.setX( 1.0f );
							}
						}
						
						// Racket2
						else if( this->ball->getOrigin().getX() + BALL_SIZE >= this->racket2->getOrigin().getX() && this->ball->getOrigin().getX() + BALL_SIZE <= this->racket2->getOrigin().getX() + RACKET_WIDTH )
						{
							if( this->ball->getOrigin().getY() + BALL_SIZE >= this->racket2->getOrigin().getY() && this->ball->getOrigin().getY() <= this->racket2->getOrigin().getY() + RACKET_HEIGHT )
							{
								this->ballDirection.setX( -1.0f );
							}
						}
					}
				}
				
				// Is there a winner ?
				if( scorePlayer1 >= 10 || scorePlayer2 >= 10 )
					this->playing = false;
				
				this->lastBallMove = ticks;
			}
		}
	}
	
	void PlayScene::render( unsigned int ticks )
	{
		vector<Point3D> vertices;
		vector<Color> colors;
		vector<unsigned int short> indices;
		
		if( ticks - this->lastDrawTicks > 15 )
		{
			#ifdef DEBUG1
			this->framesPerSecond++;
			
			if( ticks - this->lastFPSRefresh >= 1000 )
			{
				stringstream fps;
				fps << this->framesPerSecond << " FPS";
				this->sFramesPerSecond = fps.str();
				
				this->framesPerSecond = 0;
				this->lastFPSRefresh = ticks;
			}
			#endif
			
			Screen::get()->clear();
			
			// Background walls
			this->background->render();
			
			// Score
			unsigned int width = Font::get("bitmap")->renderWidth( sScorePlayer1, 2.0f );
			Font::get("bitmap")->render( Point2D( 300.0f - (width / 2.0f), 25.0f ), sScorePlayer1, 2.0f );
			width = Font::get("bitmap")->renderWidth( sScorePlayer2, 2.0f );
			Font::get("bitmap")->render( Point2D( 450.0f + (width / 2.0f), 25.0f ), sScorePlayer2, 2.0f );
			
			// Rackets
			this->racket1->prepareRendering( vertices, colors, indices );
			this->racket2->prepareRendering( vertices, colors, indices );
			
			if( this->playing )
			{
				if( this->paused )
				{
					string pauseStr = "PAUSE";
					Point2D origin(0.0f, 0.0f);
					Font::get("bitmap")->renderSize( origin, pauseStr, 2.0f );
					Font::get("bitmap")->render( Point2D( (SCREEN_WIDTH - origin.getX()) / 2.0f, (SCREEN_HEIGHT - origin.getY()) / 2.0f ), pauseStr, 2.0f );
				}
				
				this->ball->prepareRendering( vertices, colors, indices );
			}
			else
			{
				string ready = "READY!";
				string areYouReady = "Move your racket when ready!";
				
				Point2D origin(0.0f, 0.0f);
				Font::get("bitmap")->renderSize( origin, areYouReady, 1.0f );
				Font::get("bitmap")->render( Point2D( (SCREEN_WIDTH - origin.getX()) / 2.0f, (SCREEN_HEIGHT - origin.getY()) / 2.0f ), areYouReady, 1.0f );
				
				origin.moveTo( 0.0f, 0.0f );
				Font::get("bitmap")->renderSize( origin, ready, 0.7f );
				
				if( this->player1Ready )
					Font::get("bitmap")->render( Point2D( ((SCREEN_WIDTH / 2.0f) - origin.getX()) / 2.0f, SCREEN_HEIGHT - WALL_SIZE * 4.0f - origin.getY() ), ready, 0.7f );
					
				if( this->player2Ready )
					Font::get("bitmap")->render( Point2D( (SCREEN_WIDTH / 2.0f) + (((SCREEN_WIDTH / 2.0f) - origin.getX()) / 2.0f), SCREEN_HEIGHT - WALL_SIZE * 4.0f - origin.getY() ), ready, 0.7f );
			}
			
			ColoredRectangle::render( vertices, colors, indices );
			
			#ifdef DEBUG1
			Font::get("bitmap")->render( Point2D( WALL_SIZE, WALL_SIZE ), this->sFramesPerSecond );
			Font::get("bitmap")->render( Point2D( WALL_SIZE, WALL_SIZE + 30 ), this->sOperationsPerSecond );
			#endif
			
			Screen::get()->render();
			
			this->lastDrawTicks = ticks;
		}
	}
	
	void PlayScene::endScene()
	{
		this->running = false;
		this->nextScene = NULL;
	}
}

