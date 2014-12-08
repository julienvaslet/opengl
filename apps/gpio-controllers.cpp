#include <tools/logger/Stdout.h>
#include <tools/gpio/GPIO.h>
#include <data/parser/IniParser.h>

#include <cstdlib>
#include <unistd.h>
#include <signal.h>

#include <sstream>
#include <fstream>

#define GPIO_CONFIGURATION_FILE	"data/controllers/gpio.conf"

using namespace std;
using namespace tools::logger;
using namespace tools::gpio;
using namespace data;

bool running = true;

void signalHandler( int signo )
{
	if( signo == SIGTERM )
	{
		#ifdef DEBUG0
		Logger::get() << "Received SIGTERM." << Logger::endl;
		#endif
		
		running = false;
	}
	else if( signo == SIGINT )
	{
		#ifdef DEBUG0
		Logger::get() << "Received SIGINT." << Logger::endl;
		#endif
		
		running = false;
	}
}

int main( int argc, char ** argv )
{
	// Initialize standard-output logger
	new Stdout( "stdout", true );
	
	if( signal( SIGTERM, signalHandler ) == SIG_ERR )
	{
		Logger::get() << "Unable to set handle for SIGTERM signals." << Logger::endl;
		Logger::destroy();
		exit( 1 );
	}
	
	if( signal( SIGINT, signalHandler ) == SIG_ERR )
	{
		Logger::get() << "Unable to set handle for SIGKILL signals." << Logger::endl;
		Logger::destroy();
		exit( 1 );
	}
	
	ifstream iniFile( GPIO_CONFIGURATION_FILE );

	if( !iniFile.is_open() )
	{
		Logger::get() << "Unable to open file " GPIO_CONFIGURATION_FILE "." << Logger::endl;
		Logger::destroy();
		exit( 2 );
	}

	stringstream ss;
	ss << iniFile.rdbuf();
	iniFile.close();
			
	parser::IniParser * parser = new parser::IniParser( ss.str() );
	ini::Configuration * conf = parser->parse();
	delete parser;
	delete conf;
	
	GPIO::initialize();
	GPIO::get()->open( 21, GPIO::In );
	GPIO::get()->open( 20, GPIO::In );
	GPIO::get()->logStatus();

	bool pushed1 = false;
	bool pushed2 = false;

	while( running )
	{
		bool status1 = GPIO::get()->read( 21 );
		bool status2 = GPIO::get()->read( 20 );
		
		if( status1 != pushed1 )
		{
			pushed1 = status1;
			
			if( !pushed1 )
			{
				Logger::get() << "Button1 pushed." << Logger::endl;
			}
			else
			{
				Logger::get() << "Button1 released." << Logger::endl;
			}
		}
		
		if( status2 != pushed2 )
		{
			pushed2 = status2;
			
			if( !pushed2 )
			{
				Logger::get() << "Button2 pushed." << Logger::endl;
			}
			else
			{
				Logger::get() << "Button2 released." << Logger::endl;
			}
		}
		
		// Wait 100 ms
		usleep( 100000 );
	}
	
	GPIO::destroy();
	Logger::destroy();
	
	return 0;
}
