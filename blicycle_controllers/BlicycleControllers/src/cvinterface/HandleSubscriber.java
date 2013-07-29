package cvinterface;

import java.util.Observable;
import java.util.Observer;

import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;
import bot_param.entry_t;

public class HandleSubscriber implements LCMSubscriber {

	private final HandleData data;
	
	public HandleSubscriber(String handleInChannel) {
		LCMImport.getLCM().subscribe(handleInChannel, this);
		data = new HandleData();
	}

	@Override
	public synchronized void messageReceived(LCM arg0, String arg1, LCMDataInputStream arg2) {
		entry_t steerMsg;
		try{
			steerMsg = new entry_t(arg2);
			data.setSteering(Integer.parseInt(steerMsg.value));
			data.hasChanged();
			data.notifyObservers();
		}
		catch(Exception e){
			System.out.println("bad steering packet");
		}
			
	}
	
	public void makeObserver(Observer hi){
		data.addObserver(hi);
	}

	public class HandleData extends Observable{
		private int steering;
		
		public int getSteering(){
			return steering;
		}
		
		public void setSteering(int newAngle){
			steering = newAngle;
		}
	}


}
