#include "lcm/lcm-cpp.hpp"
#include "blicycle/sound_packet_t.hpp"
#include <lcmtypes/bot_param/entry_t.hpp>
#include <sstream>

using namespace blicycle;


// Global variables
sound_packet_t blikeSound;
bot_param::entry_t arduinoCmd;
lcm::LCM publish_lcm;


// Print help information
void print_help() {
    printf("** Invalid syntax!\n"
    		"       lcmChannel: name of channel\n");
}

std::string inttostring(int num){
	std::stringstream ss;
	ss<<num;
	return ss.str();
}


int main( int argc, char** argv ) {

	// Print a welcome message
	printf("LCM Post\n");

	// Based on the command line arguments to this executable, capture either from
	// a data file or from a live video stream
	if (argc != 2) {
		print_help();
		exit(1);
	}

	char* lcmChannel1 = argv[1];
	printf("   Channel: %s\n", lcmChannel1);


	//Set Up LCM

	  publish_lcm=lcm::LCM(lcm_create(NULL));



	//printf("   Capturing at:\n      Width: %d\n      Height:%d\n", width, height);
	  int i = 0;
	while(1) {
		blikeSound.fade=0;
		blikeSound.newSound=true;
		blikeSound.solo = true;
		//blikeSound.sound_id = i;
		blikeSound.sound_id = 23;
		if(i==0){
			i=1;
		}else if (i==16){
			i=0;
		}else if(i>8){
			i-=7;
		}else{
			i+=8;
		}

		// Send over the data!
		publish_lcm.publish<sound_packet_t>(lcmChannel1, &blikeSound);

		arduinoCmd.key = "hewo";
		arduinoCmd.value = inttostring(i);

		publish_lcm.publish<bot_param::entry_t>("channel", &arduinoCmd);

		sleep(1);
	}


}
