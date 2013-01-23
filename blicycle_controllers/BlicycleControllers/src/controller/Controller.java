package controller;

import java.awt.Graphics2D;

/**
 * A controller maps steering angle inputs into vibration commands.
 * It can also visualize stuff.
 * @author steve
 *
 */
public interface Controller {

	// Main controller method - map a steering angle into a motor control bitmask.
	public int stepController(double steeringTheta);
	
	// Retrieve debugging log data be saved to a file
	public double[] getLogLine();
	
	
}
