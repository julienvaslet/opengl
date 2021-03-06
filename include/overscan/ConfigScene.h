#ifndef __OVERSCAN_CONFIGSCENE_H
#define __OVERSCAN_CONFIGSCENE_H	1

#include <game/Scene.h>
#include <opengl/ColoredRectangle.h>
#include <string>
#include <vector>

using namespace game;
using namespace opengl;
using namespace std;

namespace overscan
{
	class ConfigScene : public Scene
	{	
		protected:
			unsigned int lastDrawTicks;
			int top;
			int left;
			int right;
			int bottom;
			int step;
			string sTop;
			string sLeft;
			string sRight;
			string sBottom;
			string instructions;
			vector<ColoredRectangle *> rectangles;
			
			void updateStrings();
			void updateOverscan();
			void initializeRectangles();
			void initializeStep( int step = 0 );
			
		public:
			ConfigScene();
			virtual ~ConfigScene();
			
			virtual void handleEvent( SDL_Event * event );
			virtual void live( unsigned int ticks );
			virtual void render( unsigned int ticks );
			
			void moveDown( unsigned int times = 1 );
			void moveUp( unsigned int times = 1 );
			void moveLeft( unsigned int times = 1 );
			void moveRight( unsigned int times = 1 );
			void nextStep();
			void previousStep();
	};
}

#endif

