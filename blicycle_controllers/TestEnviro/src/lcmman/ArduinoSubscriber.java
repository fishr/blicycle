package lcmman;

import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;
import lcmman.LCMEng.lcmData;

public class ArduinoSubscriber implements LCMSubscriber {

	private final double MEASUREMENT_LEFT_45_DEGREES = 655;
	private final double MEASUREMENT_RIGHT_45_DEGREES = 320;
	
	private final double SCALE_MEASURE;
	private final double SCALE_OFFSET;
	
	private lcmData lcmdata;
	
	public ArduinoSubscriber(lcmData data) {
		lcmdata = data;
		
		// Set the scale
		SCALE_MEASURE = (Math.PI / 2.0) / (MEASUREMENT_LEFT_45_DEGREES - MEASUREMENT_RIGHT_45_DEGREES);
		SCALE_OFFSET = (MEASUREMENT_LEFT_45_DEGREES + MEASUREMENT_RIGHT_45_DEGREES) / 2;
	}

	@Override
	public void messageReceived(LCM arg0, String arg1, LCMDataInputStream arg2) {
		bot_param.entry_t newMsg;
		try{
			newMsg = new bot_param.entry_t(arg2);
		}catch(Exception ie){
			System.out.println("bad arduino packet");
			return;
		}
		if(newMsg.key.contains("s")){
			lcmdata.setSteering(convertSteering(Integer.parseInt(newMsg.value)));
		}else if(newMsg.key.contains("v")){
			lcmdata.setSpeed(Integer.parseInt(newMsg.value));
		}else if(newMsg.key.contains("r")){
			lcmdata.setSpeedButton();
		}else if(newMsg.key.contains("l")){
			lcmdata.setTurnButton();
		}
		
	}
	
	public double convertSteering(int val){
		return Math.toDegrees(SCALE_MEASURE * (val - SCALE_OFFSET));
		//return (double) val;
	}
}
