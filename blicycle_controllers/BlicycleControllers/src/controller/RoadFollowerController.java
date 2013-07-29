package controller;

import gui.RoadFollowerDisplay;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import datasource.MotorCommandTranslator;

public class RoadFollowerController implements Controller {

	// Controller properties
	protected double gainX;
	protected double gainTheta;
	
	// Information about the bicycle
	protected double velocity;
	protected double bicycleWheelbase = 1.01;

	// State information
	protected double x;
	protected double y;
	protected double theta;
	
	
	// Logging information
	protected long t_start;
	protected double thetaSteer;
	protected double t;
	protected double t_last;
	protected double error;
	protected double discretizedCorrection;
	
	// Store a pretty display!
	protected RoadFollowerDisplay display;
	
	public RoadFollowerController(double gainX, double gainTheta, double velocity, double initX, double initTheta) {
		// Store key parameters
		this.gainX = gainX;
		this.gainTheta = gainTheta;
		this.velocity = velocity;
		x = initX;
		y = 0;
		theta = initTheta;
		t_start = System.nanoTime();
		
		// Start a display
		display = new RoadFollowerDisplay();
		display.showGUI();
	}
	
	
	@Override
	public double[] getLogLine() {
		return new double[] {t, thetaSteer, x, y, theta, error, discretizedCorrection};
	}

	@Override
	public byte stepController(double steeringTheta) {
		
		// Calculate derivatives
		double dTheta_dt = velocity * Math.tan(steeringTheta) / bicycleWheelbase;
		double dx_dt = velocity * Math.cos(theta);
		double dy_dt = velocity * Math.sin(theta);
		t = (System.nanoTime() - t_start) / 1000000000.0;
		double dt = t - t_last;
		t_last = t;
		thetaSteer = steeringTheta;
		
		// Integrate over time to approximate the new position of the bicycle
		x += dx_dt * dt;
		y += dy_dt * dt;
		theta += dTheta_dt * dt;
		
		// Update the state information on the display
		error = gainX * x + gainTheta*(Math.PI/2 - theta);
		
		discretizedCorrection = MotorCommandTranslator.roundAndLimit(error);
		t = (System.nanoTime() - t_start) / 1000000000.0;
		
		// Update the state information on the display
		display.setState(x, y, theta, thetaSteer);
		
		// Return output command
		return MotorCommandTranslator.makeSingleMotorCenterpoint((int) discretizedCorrection);
	}


}
