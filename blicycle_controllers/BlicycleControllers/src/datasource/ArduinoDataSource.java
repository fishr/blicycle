package datasource;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

// A class that communicates over serial (in our case, USB)
// to an Arduino running appropriate code.
public class ArduinoDataSource implements AngleDataStream {

	// Serial port stuff
	protected String serialPortName;
	protected int speed;
	protected boolean isConnected;
	protected OutputStream outStream;
	protected InputStream inStream;
	
	// Scaling information
	private final int MAX_VALUE = 1023;
	private final double MAX_ANGLE = 3.0;
	private final double MIN_ANGLE = -3.0;
	
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
	private final double MEASUREMENT_LEFT_45_DEGREES = 693;
	private final double MEASUREMENT_RIGHT_45_DEGREES = 358;
	
	private final double SCALE_MEASURE;
	private final double SCALE_OFFSET;
	
	// The last-received angle
	protected double angle;
	protected int send_val;
	
	public ArduinoDataSource(String serialPortName, int speed) {
		this.serialPortName = serialPortName;
		this.speed = speed;
		isConnected = false;
		outStream = null;
		
		// Set the scale
		SCALE_MEASURE = (Math.PI / 2.0) / (MEASUREMENT_LEFT_45_DEGREES - MEASUREMENT_RIGHT_45_DEGREES);
		SCALE_OFFSET = (MEASUREMENT_LEFT_45_DEGREES + MEASUREMENT_RIGHT_45_DEGREES) / 2;
		
	}
	
	
	@Override
	public double getAngle() {
		// TODO Auto-generated method stub
		return angle;
	}
	
	
	/**
	 * Starts a separate thread to continually read values from the serial
	 * port on the arduino.
	 */
	protected void startReadingValues() {
		(new Thread(new Runnable() {
			public void run() {
				BufferedReader reader = new BufferedReader(new InputStreamReader(inStream));

				while(true) {
					try {
						String line = reader.readLine();
						double val = Integer.valueOf(line);
						//angle = val / MAX_VALUE * (MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
						angle = SCALE_MEASURE * (val - SCALE_OFFSET);
						
						//System.out.println(line);
						
//						if (send_val == 0) {
//							send_val = 0;
//						} else if (send_val <= 8) {
//							send_val = 15;
//						} else if (send_val >= 16) {
//							send_val = 240;
//						}
//						if (send_val == 1) {
//							send_val = 2;
//						} else if (send_val == 16) {
//							send_val = 32;
//						}
						
//						if (send_val == 0) {
//							send_val = 0;
//						} else if (send_val >= 16) {
//							send_val = 32;
//						} else {
//							send_val = 8;
//						}
//						
						// Also write the send char while we're at it
						outStream.write(send_val);
						// outStream.write(MotorCommandTranslator.makeSingleMotorCenterpoint(4));
						
					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
				
			}
		})).start();
	}
	
	
	// Attempts to connect the port, returning true on success.
	// Throws an error if we can't connect to it.
	public void start() throws Exception {
		CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(serialPortName);
		if (portIdentifier.isCurrentlyOwned()) {
			throw new RuntimeException("Error: The serial port " + serialPortName + " is already owned!");
		} else {
			CommPort commPort = portIdentifier.open(this.getClass().getName(), 2000);
			if (commPort instanceof SerialPort) {
				SerialPort serialPort = (SerialPort) commPort;
				serialPort.setSerialPortParams(speed, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
				
				outStream = serialPort.getOutputStream();
				inStream = serialPort.getInputStream();
				isConnected = true;
			} else {
				throw new RuntimeException("Got a non-serial port, but only serial ports supported!");
			}
		}
		
		
		// If the connection was succesful so far, start reading in values!
		startReadingValues();
		
	}
	
	// Writes data to the port
	public void write(byte[] data) throws IOException {
		outStream.write(data);
	}


	@Override
	public void setVal(int val) {
		send_val = val;
	}

	
	
	
}