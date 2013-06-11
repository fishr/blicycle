package blicyclesoundmanager;

public class Main {

	/**
	 * @param args
	 */
	
	
	public static void main(String[] args) {
				
		AudioHandler audioEngine = new AudioHandler();
		
		(new Thread(audioEngine)).start();

		while(true){
			try {
		
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		audioEngine.update();
	}
	}

}
