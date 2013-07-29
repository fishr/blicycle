package controller;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import datasource.MotorCommandTranslator;

public class DesiredAngleProportionalController implements Controller {

	// Fields
	
	protected double gain;
	protected double desiredAngle;
	
	// Logging information
	double correction;
	double discretizedCorrection;
	long t_start;
	double thetaSteer;
	double t;
	
	public DesiredAngleProportionalController(double gain, double desiredAngle) {
		this.gain = gain;
		this.desiredAngle = desiredAngle;
		
		t_start = System.nanoTime();
	}
	
	
	@Override
	public double[] getLogLine() {
		return new double[] {t, thetaSteer, desiredAngle, discretizedCorrection};
	}

	@Override
	public byte stepController(double steeringTheta) {
		correction = computeError(steeringTheta);
		discretizedCorrection = MotorCommandTranslator.roundAndLimit(correction);
		t = (System.nanoTime() - t_start) / 1000000000.0;
		thetaSteer = steeringTheta;
		
		return MotorCommandTranslator.makeSingleMotorCenterpoint((int)discretizedCorrection);
	}
	
	public double computeError(double steeringTheta) {
		double deltaTheta = centerAngleAtZero(desiredAngle - steeringTheta);
		return gain * deltaTheta;
	}
	
	
	// Takes in an angle, such as an angle in [0, 2*pi], and shifts it so that
	// it will be centered at 0 - i.e., in [-pi, pi].
	// Please note that it returns an angle that represents the same angle as before,
	// but now that it is centered at 0 it is safe to do proportional control on it.
	protected double centerAngleAtZero(double angle) {
		if (angle > Math.PI) {
			double i = Math.floor((angle + Math.PI) / (2*Math.PI));
			return angle - 2*i*Math.PI;
		} else if (angle < -Math.PI) {
			double i = Math.floor(-(angle - Math.PI)/(-2*Math.PI));
			return angle + 2*i*Math.PI;
		} else {
			return angle;
		}
	}
	
	// Set the desired angle
	protected void setAngle(double desiredAngle) {
		this.desiredAngle = desiredAngle;
	}


}
