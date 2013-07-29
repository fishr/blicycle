package cvinterface;

import lcm.lcm.LCM;

public class LCMImport{
	
	static private LCM lcm;	

	public LCMImport(){
		lcm = LCM.getSingleton();
	}
	
	public VisionSubscriber startVisionIn(String visionInChannel){
		return new VisionSubscriber(visionInChannel);
	}
	
	public HandlePublisher startHandlebarsOut(String handleOutChannel){
		return new HandlePublisher(handleOutChannel);
	}
	
	public HandleSubscriber startHandlebarsIn(String handleInChannel){
		return new HandleSubscriber(handleInChannel);
	}

	public static LCM getLCM() {
		return lcm;
	}
}