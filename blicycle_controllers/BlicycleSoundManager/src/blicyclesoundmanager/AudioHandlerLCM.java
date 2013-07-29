package blicyclesoundmanager;

import java.io.IOException;

import blicycle.sound_packet_t;
import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;

public class AudioHandlerLCM extends AudioHandler {

	sound_packet_t soundMsg;
	
	protected boolean newMsg = false;
	
	public AudioHandlerLCM(){
		LCM myLCM = LCM.getSingleton();
		myLCM.subscribe("soundCmd", new SoundSubscriber());
	}
	
	public class SoundSubscriber implements LCMSubscriber{

		@Override
		public void messageReceived(LCM arg0, String arg1,
				LCMDataInputStream arg2) {
			try {
				soundMsg = new sound_packet_t(arg2);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			newMsg = true;
		}
		
	}
	
	@Override
	public synchronized void update(){
		this.soundRequestOverride = soundMsg.solo;
		this.newSoundRequest = soundMsg.newSound;
		this.soundRequestId = soundMsg.sound_id;
		this.balanceRequest = soundMsg.fade;
	}
	
	@Override
	public boolean whyUpdate(){
		boolean doIUpdate = newMsg;
		newMsg = false;
		return doIUpdate;
	}
	
}
