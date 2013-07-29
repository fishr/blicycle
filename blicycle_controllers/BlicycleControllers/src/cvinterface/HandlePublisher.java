package cvinterface;

import bot_param.entry_t;

public class HandlePublisher {
	
	private String channelName;
	private entry_t handleCmd;
	
	public HandlePublisher(String handleName){
		channelName = handleName;
		handleCmd.key = "1";
	}

	public void publish(int cmd) {
		handleCmd.value = String.valueOf(cmd);
		LCMImport.getLCM().publish(channelName, handleCmd);
	}

}
