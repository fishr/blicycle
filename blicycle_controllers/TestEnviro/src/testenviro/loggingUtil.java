package testenviro;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.LinkedList;

import lcmman.LCMEng.lcmData;

public class loggingUtil {
	
	
	// Logging stuff
	protected static LinkedList<double[]> log;
	protected static String logFilenameBase;
	private static String logFilename = "blank.csv";
	protected static boolean shouldSave;
	
	protected lcmData dataEngine;
	private int granularity;
	private Thread logger; 
	
	public loggingUtil(lcmData input, String logFilename, boolean shouldSave) {
		this(input, logFilename, shouldSave, 1000);
	}

	
	public loggingUtil(lcmData input, String logFilename, boolean shouldSave, final int time) {
		this.dataEngine = input;
		loggingUtil.logFilenameBase = logFilename;
		loggingUtil.shouldSave = shouldSave;

		log = new LinkedList<double[]>();
		granularity = time;
	}
	
	
	void logData(){
		double[] logLine = dataEngine.getLogLine();
		if (shouldSave) {
			synchronized(log) {
				log.add(logLine);
			}
		}
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
				System.out.print("write successful to: ");
				System.out.println(logFilename);
				writer.close();
				
			} catch (Exception e) {
				System.out.println("Unable to save log file!");
			}
		}
	}


	public void begin(String name) {
		logFilename = logFilenameBase +"_" +name+ ".csv";
		logger = new Thread(){
			public void run(){
				while(true){
					try {
						Thread.sleep(granularity);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					logData();
				}
			}
		};
		
		logger.start();
	}


	public void end(String decimeters) {
		logger.stop();
		double[] diameter;
		try{
			diameter = new double[] {(double) Integer.parseInt(decimeters)};
		}
		catch(NumberFormatException ie){
			diameter = new double[] {(double) 0};
		}
		log.add(diameter);
		
	}
}
