#ifndef __OPENGL_ELEMENTARRAYBUFFEROBJECT_H
#define __OPENGL_ELEMENTARRAYBUFFEROBJECT_H	1

#include <SDL2/SDL_opengl.h>
#include <vector>

#include <opengl/BufferObject.h>

using namespace std;

namespace opengl
{
	class ElementArrayBufferObject : public BufferObject
	{
		protected:
			unsigned int size;
		
			void bind();
			
		public:
			ElementArrayBufferObject();
			virtual ~ElementArrayBufferObject();
			
			void setData( const vector<unsigned int>& data, Frequency frequency = Static, Access access = Draw );

			void draw( unsigned int start = 0, unsigned int end = 0 );
			void draw( OpenGL::DrawMode mode, unsigned int start = 0, unsigned int end = 0 );
	};
}

#endif
