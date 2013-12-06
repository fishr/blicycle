package datasource;

import java.util.Observable;
import java.util.Observer;

import cvinterface.HandlePublisher;
import cvinterface.HandleSubscriber;
import cvinterface.LCMImport;

public class LCMArduinoSource implements AngleDataStream, Observer {
		/**
		 * Calibration data:
		 * Straight ahead: 494
		 * Horizontal left 90 degrees: 832
		 * Horizontal right 90 degrees: 171
		 * 
		 */
		
		// For simulation potentiometer:
		// private final double MEASUREMENT_LEFT_45_DEGREES = 768;
		// private final double MEASUREMENT_RIGHT_45_DEGREES = 470;
		
		// For bicycle potentiometer:
		//private final double MEASUREMENT_LEFT_90_DEGREES = 832;
		//private final double MEASUREMENT_RIGHT_90_DEGREES = 145;
		private static final double MEASUREMENT_LEFT_45_DEGREES = 655;
		private static final double MEASUREMENT_RIGHT_45_DEGREES = 320;
		
		private final double SCALE_MEASURE;
		private final double SCALE_OFFSET;
		
		private static final double log2 = 0.30102999566;
		
		// The last-received angle
		private double angle;
		
		//LCM handleBar getters
		private final HandlePublisher handlesOut;
		private final HandleSubscriber handlesIn;
		private byte oldCmd;
		
				
		public LCMArduinoSource(LCMImport lcm, String handleInChannel, String handleOutChannel){
			// Set the scale
			SCALE_MEASURE = (Math.PI / 2.0) / (MEASUREMENT_LEFT_45_DEGREES - MEASUREMENT_RIGHT_45_DEGREES);
			SCALE_OFFSET = (MEASUREMENT_LEFT_45_DEGREES + MEASUREMENT_RIGHT_45_DEGREES) / 2;
						
			handlesOut = lcm.startHandlebarsOut(handleOutChannel);
			handlesIn = lcm.startHandlebarsIn(handleInChannel);
			handlesIn.makeObserver(this);
		}
		
		
	@Override
	public double getAngle() {
		return angle;
	}

	@Override
	public void setVal(byte val) {
		//incoming data is a bitmask of which motors should be set
		//means I can take the base2 log and add one to get the motorCmd
		handlesOut.publish(debitshift(oldCmd)+8); //end old cmd
		handlesOut.publish(debitshift(val));      //send new value
		oldCmd = val;
	}
	
	public int debitshift(byte val){
		return (int)Math.round(Math.log(val)/log2)+1;
	}

	@Override
	public void update(Observable arg0, Object arg1) {
		angle = SCALE_MEASURE * (((HandleSubscriber.HandleData)arg0).getSteering() - SCALE_OFFSET);
		System.out.print(angle);
	}

}
