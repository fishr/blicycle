package cvinterface;

import java.io.IOException;
import java.util.Observable;
import java.util.Observer;

import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;
import bot_param.entry_t;

public class HandleSubscriber implements LCMSubscriber {

	private HandleData data;
	
	public HandleSubscriber(String handleInChannel) {
		LCMImport.getLCM().subscribe(handleInChannel, this);
		data = new HandleData();
	}

	@Override
	public synchronized void messageReceived(LCM arg0, String arg1, LCMDataInputStream arg2) {
		bot_param.entry_t steerMsg;
		try{
			bot_param.entry_t newMsg = new entry_t(arg2);
			if(newMsg.key.equals("s")){
				steerMsg=newMsg;
				data.setSteering(Integer.parseInt(steerMsg.value));
				//System.out.println(data.getSteering());
			}
		}
		catch(IOException e){
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
			data.setChanged();
			data.notifyObservers();
		}
	}


}
