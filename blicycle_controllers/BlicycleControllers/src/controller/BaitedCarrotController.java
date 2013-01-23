package controller;

import gui.RoadFollowerDisplay;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import datasource.MotorCommandTranslator;


/**
 * The analogy here is a bunny chasing a carrot that is always ahead of it!
 * @author Steve
 *
 */
public class BaitedCarrotController implements Controller {

	// Controller properties
	protected double gain;
	
	// Information about the bicycle
	protected double velocityMax;
	protected double velocitySlowdownCoeff = 2.0;	// How much the bike slows down when turning
	protected double bicycleWheelbase = 1.01;

	// Information about the target
	protected double targetOffset = 4.0;	// About 10ft forward
	
	
	// State information
	protected double x;
	protected double y;
	protected double theta;
	
	// More state information - computed
	double dx_dt;
	double dy_dt;
	double dTheta_dt;
	double velocity;
	double dt;
	
	// Logging information
	protected long t_start;
	protected double thetaSteer;
	protected double t;
	protected double t_last;
	protected double error;
	protected double discretizedCorrection;
	protected double beta;
	
	// A sub-controller, used to control on angle.
	protected DesiredAngleProportionalController angleController;
	
	// Store a pretty display!
	protected RoadFollowerDisplay display;
	
	public BaitedCarrotController(double gain, double velocity, double initX, double initTheta) {
		// Store key parameters
		this.gain = gain;
		this.velocityMax = velocity;
		x = initX;
		y = 0;
		theta = initTheta;
		t_start = System.nanoTime();
		
		// Set up the angle controller
		angleController = new DesiredAngleProportionalController(gain, Math.PI / 2);
		
		// Start a display
		display = new RoadFollowerDisplay();
		display.showGUI();
	}
	
	
	@Override
	public double[] getLogLine() {
		return new double[] {t, thetaSteer, x, y, theta, error, beta, discretizedCorrection};
	}

	
	public void simulateDynamics() {
		// SIMULATE THE BICYCLE
		// Calculate derivatives
		velocity = Math.max(velocityMax - velocitySlowdownCoeff * Math.abs(thetaSteer), 0.0);
		dTheta_dt = velocity * Math.tan(thetaSteer) / bicycleWheelbase;
		dx_dt = velocity * Math.cos(theta);
		dy_dt = velocity * Math.sin(theta);
		
		t = (System.nanoTime() - t_start) / 1000000000.0;
		dt = t - t_last;
		t_last = t;
		
		// Integrate over time to approximate the new position of the bicycle
		x += dx_dt * dt;
		y += dy_dt * dt;
		theta += dTheta_dt * dt;
	
	}
	
	public void setState(double deltaX, double deltaTheta) {
		this.x = deltaX;
		this.theta = Math.PI/2 + deltaTheta;
	}
	
	public int generateControlOutput() {
		// Try forward predicting some amount in time and run the controller from this reference point.
		dt = 0.01;
		double xFwd = x;
		double yFwd = y;
		double thetaFwd = theta;
		
		// Compute some derivatives
		velocity = Math.max(velocityMax - velocitySlowdownCoeff * Math.abs(thetaSteer), 0.0);
		dTheta_dt = velocity * Math.tan(thetaSteer) / bicycleWheelbase;
		dx_dt = velocity * Math.cos(theta);
		dy_dt = velocity * Math.sin(theta);
		
		for(int i = 0; i < 100; i++) {
			dx_dt = velocity * Math.cos(theta);
			dy_dt = velocity * Math.sin(theta);
			
			xFwd += dx_dt * dt;
			yFwd += dy_dt * dt;
			thetaFwd += dTheta_dt * dt;
		}
		
		// RUN THE CARROT BAIT CONTROLLER
		// Given wherever we think are, find a goal position (the "carrot") to aim for.
		double xTarget = 0.0;
		double yTarget = yFwd + targetOffset;
		
		// Now, compute the angle from the bicycle to this target position.
		beta = Math.atan2(yTarget - yFwd, xTarget - xFwd);
		
		// Use the angle sub-controller to steer the bicycle towards this line!
		angleController.setAngle(beta);
		error = -angleController.computeError(thetaFwd);
		
		discretizedCorrection = MotorCommandTranslator.roundAndLimit(error);
		t = (System.nanoTime() - t_start) / 1000000000.0;
		
		// Update the state information on the display
		display.setState(x, y, theta, thetaSteer, xTarget, yTarget);
		
		// Return output command
		return MotorCommandTranslator.makeSingleMotorCenterpoint((int) discretizedCorrection);
	}
	
	@Override
	public int stepController(double steeringTheta) {
		
		synchronized(this) {
			thetaSteer = steeringTheta;
		
			// Simulate the dynamics of the bicycle
			 simulateDynamics();
			
			// Generate a control output
			return generateControlOutput();
		}
		
	}

	
	
}
