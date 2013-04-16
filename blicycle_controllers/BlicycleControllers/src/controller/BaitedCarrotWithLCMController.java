package controller;

import cvinterface.LCMImport;

public class BaitedCarrotWithLCMController extends BaitedCarrotController {


	// LCM information
	LCMImport lcmConnect;
	
	// Useful for mixing in data acquired from the CV engine
	protected double deltaX_last = 0.0;
	protected double deltaTheta_last = 0.0;
	protected double alpha = 0.0;	// Decays to 0 over time
	
	// The desired track position. Subtracted from the CV engine's input to 
	// compute deltaX.
	protected double desiredDeltaX = 0.0;
	
	@Override
	public int stepController(double steeringTheta) {
		
		// Now, compute a mapping to deltaX, deltaTheta
		if (lcmConnect.lock != 0) {
		
			double deltaX = lcmConnect.delta() - desiredDeltaX;
			double deltaTheta = Math.PI-lcmConnect.phi(); //in radians
						
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
	
	
	
	public BaitedCarrotWithLCMController(double gain, double velocity,
			double initX, double initTheta) {
		super(gain, velocity, initX, initTheta);
		
		System.out.println("Setting up LCM interface\n");
		lcmConnect = new LCMImport("bikedata");
						
		}
	/*
	
	public void updateState(double deltaX, double deltaTheta) {
		synchronized(this) {
			this.setState(deltaX, deltaTheta);
		}	
	}
*/
}
