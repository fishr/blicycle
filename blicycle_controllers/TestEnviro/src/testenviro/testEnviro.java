package testenviro;

public class testEnviro {

	
	public static void main(String[] args) {
		
		// Log file information
		boolean save_this_trial = true;	// True => Will save and write to the following file. Otherwise will not save!
		String log_filename = "recording";
		
		
		//fire up LCMeng
		lcmman.LCMEng lcmeng = new lcmman.LCMEng();
		
		//fire up GUI, passing along the observable
		gui.GuiMake testGui = new gui.GuiMake(lcmeng.data);
		
		loggingUtil log = new loggingUtil(lcmeng.data, log_filename, save_this_trial);
		
		testGui.setLog(log);
	}

}
