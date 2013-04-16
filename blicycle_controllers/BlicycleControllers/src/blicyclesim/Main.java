package blicyclesim;

import controller.BaitedCarrotController;
import controller.BaitedCarrotWithLCMController;
import controller.BaitedCarrotWithCVController;
import controller.DesiredAngleProportionalController;
import controller.RoadFollowerController;
import controller.StraightProportionalController;
import datasource.AngleDataStream;
import datasource.ArduinoDataSource;
import datasource.HumanSimulator;
import datasource.SineDataSource;
import gui.HandlebarMonitor;


public class Main {
	
	public static void main(String[] args) {
		System.out.println("Blicycle Simulator v 1.5!");
		
		// Select which angular steering data source to use
		//SineDataSource steering = new SineDataSource(-1.0, Math.PI/12);

		// Simulate a rider on the bicycle
		HumanSimulator steering = new HumanSimulator(0.005, 25);
		
		//ArduinoDataSource steering = new ArduinoDataSource("/dev/tty.usbmodemfa131", 115200);
		//ArduinoDataSource steering = new ArduinoDataSource("/dev/ttyACM0", 115200);
		/*
		try {
			steering.start();
		} catch (Exception e) {
			System.out.println("ERROR! Could not connect to arduino!");
			e.printStackTrace();
		}*/
		

		
		
		// Log file information
		boolean save_this_trial = false;	// True => Will save and write to the following file. Otherwise will not save!
		String log_filename = "recording.csv";	// Filename where to save. PLEASE CHANGE OTHERWISE WILL OVERWRITE!!! Relative
												// to the BlicycleSimulator/ directory.
		
		
		// Select a controller! Uncomment exactly one line.
		// There are currently two types - DesiredAngleProportionalController takes in a GAIN_THETA, and DESIRED_THETA.
		// 			It does proportional control to tell you to reach the desired theta.
		// 
		// 	The RoadFollowerController simulates bicycle dynamics. It takes in paramters: GAIN_X, GAIN_THETA, VELOCITY, INIT_X (0 is center),
		// 		and initial theta (pi/2 is facing forward). This controller also makes another display that shows a simulation.
		// 		Please note that error = gain_x * deltaX + gain_theta * deltaTheta
		
		//DesiredAngleProportionalController controller = new DesiredAngleProportionalController(-8.0, 0.0);
		// DesiredAngleProportionalController controller = new DesiredAngleProportionalController(-16.0, Math.PI/6);
		
		
		// BaitedCarrotController controller = new BaitedCarrotController(4.0, 4.0, 3.0 * (Math.random() - 0.5), Math.PI/2 + (Math.random() - 0.5) * Math.PI);
		// BaitedCarrotController controller = new BaitedCarrotController(4.0, 4.0, 3.0, Math.PI / 4);
		// RoadFollowerController controller = new RoadFollowerController(-3.0, 0.0, 4.0, 0.0, Math.PI/2);		// Just position
		// RoadFollowerController controller = new RoadFollowerController(0.0, -3.0, 4.0, 0.0, Math.PI/2);		// Just angle (bad)
		// RoadFollowerController controller = new RoadFollowerController(-1.0, -1.0, 4.0, 0.0, Math.PI/2);		// Angle and position
		
		//BaitedCarrotWithCVController controller = new BaitedCarrotWithCVController(3.0, 4.0, 2.0, Math.PI / 4);
		BaitedCarrotWithLCMController controller = new BaitedCarrotWithLCMController(3.0, 4.0, 2.0, Math.PI/4);
		
		/*******************************************************************************************
		 * *****************************************************************************************
		 * *****************************************************************************************
		 * 
		 * 		End editing!
		 * 
		 * *****************************************************************************************
		 * *****************************************************************************************
		 * *****************************************************************************************
		 */
		
		
		// Set up and start the simulation
		SimulatorEngine engine = new SimulatorEngine(steering, controller, log_filename, save_this_trial);
		engine.startSimulating();
		
	}

}
