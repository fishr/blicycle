package controller;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

import cvinterface.CVMapper;

/**
 * Implements the BaitedCarrotCotnroller, but operates on data received
 * from socket from the Computer Vision module instead of simulating
 * all of the bicycle's dynamics.
 *
 * @author Steve
 *
 */
public class BaitedCarrotWithCVController extends BaitedCarrotController {

	// Socket fields for listening to the CV engine
	protected final int socketPort = 20000;
	protected ServerSocket serverSocket;
	
	// CV mapping information
	CVMapper cvMapper;
	
	// Useful for mixing in data acquired from the CV engine
	protected double deltaX_last = 0.0;
	protected double deltaTheta_last = 0.0;
	protected double alpha = 0.0;	// Decays to 0 over time
	
	// The desired track position. Subtracted from the CV engine's input to 
	// compute deltaX.
	protected double desiredDeltaX = 2.0;
	
	@Override
	public int stepController(double steeringTheta) {
		
		// Also update the state!
		x += alpha * (deltaX_last - x);
		theta += alpha * (deltaTheta_last - theta);
		
		alpha *= 0.8;
		
		return super.stepController(steeringTheta);
	}
	
	public BaitedCarrotWithCVController(double gain, double velocity, double initX, double initTheta) {
		super(gain, velocity, initX, initTheta);
		
		// Also, create a socket server for things to connect to
		try {
			serverSocket = new ServerSocket(socketPort);
			System.out.println("Socket server listening on port " + socketPort);
		} catch (IOException e) {
			System.out.println("Could not open socket on port " + socketPort + "!");
		}
		
		// Load the CVMapper
		System.out.println("Loading (rho, phi) -> (deltaX, deltaTheta) mappings for CV...");
		cvMapper = new CVMapper("rhoi.txt","phii.txt", "xi.txt", "thetai.txt");
		
		
		(new Thread(new Runnable() {
			@Override
			public void run() {
				Socket s = null;
				System.out.println("Waiting for client request...");
				while(true) {
					try {
						s = serverSocket.accept();
						
						System.out.println("Client connected!");
						BufferedReader input = new BufferedReader(new InputStreamReader(s.getInputStream()));
						
						String inputLine;
						while((inputLine = input.readLine()) != null) {
							// Process the input line
							//System.out.println("Received: " + inputLine);
							
							// Now parse it
							String[] split = inputLine.split(",");
							if (split.length != 3) {
								System.out.println("Warning: Received corrupt data from CV module!");
								continue;
							}
							int locked = Integer.parseInt(split[0]);
							double rho = Double.parseDouble(split[1]);
							double phi = Double.parseDouble(split[2]);
							
							// Now, compute a mapping to deltaX, deltaTheta
							if (locked != 0) {
								double[] result = cvMapper.lookupXAndTheta(rho, phi);
							
								double deltaX = result[0] - desiredDeltaX;
								double deltaTheta = Math.PI / 180.0 * result[1];
								
								if (!(deltaX == -desiredDeltaX && deltaTheta == 0.0)) {
								
									System.out.println("deltaX = " + deltaX + ", deltaTheta = " + 180.0 / Math.PI * deltaTheta);
									
									// Set it!
									//setState(deltaX, deltaTheta);
									deltaX_last = deltaX;
									deltaTheta_last = deltaTheta + Math.PI / 2;
									alpha = 0.1;
									
									
								} else {
									System.out.println("Point out of good boundary!");
								}
								
							} else {
								System.out.println("No lock (but heartbeat!)");
							}
							
						}
						
					} catch (Exception e) {
						e.printStackTrace();
					}
					
					// Wait a bit before continuing and trying to accept a new socket.
					try {
						this.wait(100);
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
					
				
				
			}
		})).start();
	}
	
	
	public void updateState(double deltaX, double deltaTheta) {
		synchronized(this) {
			this.setState(deltaX, deltaTheta);
		}	
	}
	
	
}
