package controller;

import cvinterface.LCMImport;
import cvinterface.VisionSubscriber;

public class BaitedCarrotWithLCMController extends BaitedCarrotController {


	// LCM information
	private VisionSubscriber visionIn;
	
	// Useful for mixing in data acquired from the CV engine
	protected double deltaX_last = 0.0;
	protected double deltaTheta_last = 0.0;
	protected double alpha = 0.0;	// Decays to 0 over time
	
	// The desired track position. Subtracted from the CV engine's input to 
	// compute deltaX.
	protected double desiredDeltaX = 0.0;
	
	
	public BaitedCarrotWithLCMController(double gain, double velocity,
			double initX, double initTheta, LCMImport lcm, String visionDataChannel) {
		super(gain, velocity, initX, initTheta);

		visionIn = lcm.startVisionIn(visionDataChannel);
		}
	
	@Override
	public byte stepController(double steeringTheta) {
		
		// Now, compute a mapping to deltaX, deltaTheta
		if (visionIn.getLock() != 0) {
		
			double deltaX = visionIn.getDelta() - desiredDeltaX;
			double deltaTheta = Math.PI-visionIn.getPhi(); //in radians
						
				//System.out.println("deltaX = " + deltaX + ", deltaTheta = " + 180.0 / Math.PI * deltaTheta);
				
				deltaX_last = deltaX;
				x = deltaX;
				deltaTheta_last = deltaTheta;// + Math.PI / 2;
				theta = deltaTheta;
				alpha = 0.1;
			
		} else {
			System.out.println("No lock (but heartbeat!)");
		}
		
		
		// Also update the state!
		x += alpha * (deltaX_last - x);
		theta += alpha * (deltaTheta_last - theta);
		
		alpha *= 0.8;
		
		return super.stepController(steeringTheta);
	}
	
	

	/*
	
	public void updateState(double deltaX, double deltaTheta) {
		synchronized(this) {
			this.setState(deltaX, deltaTheta);
		}	
	}
*/
}
