package blicyclesim;

import gui.HandlebarMonitor;

import java.io.BufferedWriter;
import java.io.FileWriter;

import java.util.LinkedList;
import java.util.Timer;
import java.util.TimerTask;

import controller.Controller;

import datasource.AngleDataStream;

/**
 * This class runs the simulation!
 * @author Steve
 *
 */
public class SimulatorEngine  {

	// Fields
	protected AngleDataStream steeringDataSource;
	protected HandlebarMonitor handlebarMonitor;
	protected Controller controller;
	
	// Logging stuff
	protected static LinkedList<double[]> log;
	protected static String logFilename;
	protected static boolean shouldSave;
	
	// Impose angular limits
	protected static final double MAX_ANGLE = Math.PI/4;
	protected static final double MIN_ANGLE = -Math.PI/4;
	
	public SimulatorEngine(AngleDataStream src, Controller controller, String logFilename, boolean shouldSave) {
		
		// Store the source
		this.steeringDataSource = src;
		this.controller = controller;
		
		// Show a screen to show the status of this data source
		handlebarMonitor = new HandlebarMonitor();
		handlebarMonitor.showGUI();
		
		// Set up the logger
		this.logFilename = logFilename;
		this.shouldSave = shouldSave;
		log = new LinkedList<double[]>();
		
	}
	
	public double getAngle() {
		return steeringDataSource.getAngle();
	}
	
	
	public void startSimulating() {
		// Start simulating!
		// Set up a timer thread to run the simulation.
	
		Timer timer = new Timer();
		timer.scheduleAtFixedRate(new TimerTask() {
			public void run() {
				simulationStep();
			}
		}, 0, 1000 / 100);
		
		
	}
	
	public void simulationStep() {
		
		// Get a new angle, and impose limits.
		double steeringTheta = steeringDataSource.getAngle();
		if (steeringTheta > MAX_ANGLE) {
			steeringTheta = MAX_ANGLE;
		} else if (steeringTheta < MIN_ANGLE) {
			steeringTheta = MIN_ANGLE;
		}
		
		// Step the controller and retrieve its logging information
		byte motorBitmask = controller.stepController(steeringTheta);
		double[] logLine = controller.getLogLine();
		if (shouldSave) {
			synchronized(log) {
				log.add(logLine);
			}
		}
		
		// Send the bitmask to the motors
		steeringDataSource.setVal(motorBitmask);
		
		
		// Update the handlebar monitor
		handlebarMonitor.setData(steeringTheta, motorBitmask);
		
	}
	
	
	/**
	 * Call when we need to save - about to go down!
	 */
	public static void saveNow() {
		if (shouldSave) {
			try {
				// Try to save the file
				BufferedWriter writer = new BufferedWriter(new FileWriter(logFilename));
				
				synchronized(log) {
					for(double[] line : log) {
						// Write this line in CSV format
						if (line.length != 0) {
							for(int i = 0; i < line.length - 1; i++) {
								writer.write(String.valueOf(line[i]) + ", ");
							}
							writer.write(String.valueOf(line[line.length - 1]));
							writer.newLine();
						}
					}
				}
				
				writer.close();
				
			} catch (Exception e) {
				System.out.println("Unable to save log file!");
			}
		}
	}

}
