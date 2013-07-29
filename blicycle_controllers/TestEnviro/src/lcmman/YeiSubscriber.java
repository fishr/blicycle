package lcmman;

import java.util.Observable;

import yei.imu_t;

import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;
import lcmman.LCMEng.lcmData;

public class YeiSubscriber implements LCMSubscriber {

	private lcmData lcmdata;
		
	public YeiSubscriber(lcmData data) {
		lcmdata = data;
	}

	@Override
	public void messageReceived(LCM arg0, String arg1, LCMDataInputStream arg2) {
		imu_t yeidata;
		try{
			yeidata = new imu_t(arg2);
			lcmdata.setQuat(yeidata.orientation);	
			lcmdata.setAccel(yeidata.linear_acceleration);
			lcmdata.setMsgTime(yeidata.utime);
		}catch(Exception ie){
			System.out.println("bad yei packet");
			return;
		}
	}
}
