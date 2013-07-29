package blicyclesoundmanager;

public class Main {

	/**
	 * @param args
	 */
	
	
	public static void main(String[] args) {
				
		AudioHandler audioEngine = new AudioHandler();
		//AudioHandlerLCM audioEngine = new AudioHandlerLCM();
		
		(new Thread(audioEngine)).start();
		while(true){
			
		while(!audioEngine.whyUpdate()){
			try {
				Thread.sleep(20);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		audioEngine.update();
	}
	}

}
