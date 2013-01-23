package gui;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.image.BufferedImage;
import java.text.DecimalFormat;

import java.util.Timer;
import java.util.TimerTask;

import javax.swing.*;

import blicyclesim.SimulatorEngine;

/**
 * This class pops up a window that shows the currently measured handlebar data.
 * @author steve
 *
 */
public class RoadFollowerDisplay extends JPanel implements WindowListener {
		
	// State information 
	protected double x = 0;
	protected double y = 0;
	protected double theta = Math.PI / 2;
	protected double thetaSteer = 0.0;
	
	// Target information
	protected double xTarget;
	protected double yTarget;
	protected boolean drawTarget;
	
	protected static final int WIDTH = 600;
	protected static final int HEIGHT = 600;
	protected static final double PIXELS_PER_METER = 50;
	
	// Bicycle geometry (for drawing the bike!)
	protected double bicycleWheelbase = 1.01;
	protected double wheelDiameter = 0.700;	// 700c tires. Nice!
	protected double wingspan = 0.7;	// Handlebars
	
	public RoadFollowerDisplay() {
		super();
		
	}
	
	protected void createAndShowGUI() {
		JFrame frame = new JFrame("Road Follower - Birdseye View");
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		
		frame.add(this);
		frame.pack();
		frame.setBackground(Color.WHITE);
		frame.setSize(WIDTH, HEIGHT);
		frame.addWindowListener(this);
		frame.setLocation(750, 100);
		frame.setVisible(true);
		
		// Start a task to keep rendering.
		Timer timer = new Timer();
		timer.scheduleAtFixedRate(new TimerTask() {
			public void run() {
				render();
			}
		}, 0, 1000 / 20);
		
		System.out.println("Road Follower Display is available.");
		
	}
	
	
	public void setState(double x, double y, double theta, double thetaSteer) {
		synchronized(this) {
			this.x = x;
			this.y = y;
			this.theta = theta;
			this.thetaSteer = thetaSteer;
			this.drawTarget = false;
		}
	}
	
	public void setState(double x, double y, double theta, double thetaSteer, double xTarget, double yTarget) {
		synchronized(this) {
			this.x = x;
			this.y = y;
			this.theta = theta;
			this.thetaSteer = thetaSteer;
			this.xTarget = xTarget;
			this.yTarget = yTarget;
			this.drawTarget = true;
		}
	}
	
	public void showGUI() {
		//javax.swing.SwingUtilities.invokeLater(new Runnable() {
		//	public void run() {
				createAndShowGUI();
		//	}
		//});
	}
	
	/**
	 * Re-render this screen
	 */
	private void render() {
		
		// Create a backbuffer and render to it
		BufferedImage buffer = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_INT_RGB);
		Graphics2D g2D = (Graphics2D) buffer.getGraphics();
		g2D.setBackground(Color.LIGHT_GRAY);
		g2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		
		// Clear the background
		g2D.clearRect(0, 0, WIDTH, HEIGHT);
	
		double x;
		double y;
		double theta;
		
		double xTarget;
		double yTarget;
		boolean drawTarget;
		
		synchronized(this) {
			x = this.x;
			y = this.y;
			theta = this.theta;
		
			xTarget = this.xTarget;
			yTarget = this.yTarget;
			drawTarget = this.drawTarget;
			
		}
		
		double yFrame = y + 2.0;
		double xFrame = 0;
		
		// Draw yellow lane markings	
		int ymin = (int) (2 * Math.floor(worldY(HEIGHT, yFrame) / 2));
		int ymax = (int) (2 * Math.ceil(2 * worldY(0, yFrame) / 2));
		g2D.setColor(Color.YELLOW);
		for(int yi = ymin; yi <= ymax; yi+= 2) {
			g2D.fillRect((int) screenX(-0.05, xFrame), (int) screenY(yi, yFrame), (int) (0.1 * PIXELS_PER_METER), (int) (1.0 * PIXELS_PER_METER));
		}
		
		// Draw grass
		double xGrass = screenX(-2.0, xFrame);
		g2D.setColor(Color.GREEN);
		g2D.fillRect(0, 0, (int) xGrass, WIDTH);
		
		// Now draw the target, if desired.
		double targetX = 0.0;
		double targetY = 0.0;
		if (drawTarget) {
			targetX = screenX(xTarget, xFrame);
			targetY = screenY(yTarget, yFrame);
			g2D.setColor(Color.RED);
			g2D.drawOval( (int) (targetX - 10), (int) (targetY - 10), 20, 20);
			g2D.drawOval( (int) (targetX - 20), (int) (targetY - 20), 40, 40);
			g2D.drawOval( (int) (targetX - 30), (int) (targetY - 30), 60, 60);
		}
		
	
		// Now draw the bicycle on the screen
		double xBicycle = screenX(x, xFrame);
		double yBicycle = screenY(y, yFrame);
		if (drawTarget) {
			// Draw a dotted line to the bicycle
			g2D.setColor(Color.BLUE);
			g2D.setStroke(new BasicStroke((float) (0.05 * PIXELS_PER_METER), BasicStroke.CAP_ROUND, BasicStroke.JOIN_BEVEL, 1.0f, new float[] {9}, 0.0f));
			g2D.drawLine((int) xBicycle, (int) yBicycle, (int) targetX, (int) targetY);
		}
//		g2D.setColor(Color.BLACK);
//		g2D.fillOval((int) (xBicycle - 10), (int) (yBicycle - 10), 20, 20);
//		double xPoint = screenX(x + 0.75*Math.cos(theta), xFrame);
//		double yPoint = screenY(y + 0.75*Math.sin(theta), yFrame);
//		g2D.setStroke(new BasicStroke((float) (0.05 * PIXELS_PER_METER)));
//		g2D.drawLine((int) xBicycle, (int) yBicycle, (int) xPoint, (int) yPoint);
		g2D.setColor(Color.BLACK);
		g2D.setStroke(new BasicStroke(10.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		double xPoint1 = screenX(x + wheelDiameter/2.0*Math.cos(theta), xFrame);
		double yPoint1 = screenY(y + wheelDiameter/2.0*Math.sin(theta), yFrame);
		double xPoint2 = screenX(x - wheelDiameter/2.0*Math.cos(theta), xFrame);
		double yPoint2 = screenY(y - wheelDiameter/2.0*Math.sin(theta), yFrame);
		g2D.drawLine((int) xPoint1, (int) yPoint1, (int) xPoint2, (int) yPoint2);
		double xFront = x + bicycleWheelbase*Math.cos(theta);
		double yFront = y + bicycleWheelbase*Math.sin(theta);
		xPoint1 = screenX(xFront + wheelDiameter/2.0*Math.cos(theta + thetaSteer), xFrame);
		yPoint1 = screenY(yFront + wheelDiameter/2.0*Math.sin(theta + thetaSteer), yFrame);
		xPoint2 = screenX(xFront - wheelDiameter/2.0*Math.cos(theta + thetaSteer), xFrame);
		yPoint2 = screenY(yFront - wheelDiameter/2.0*Math.sin(theta + thetaSteer), yFrame);
		g2D.drawLine((int) xPoint1, (int) yPoint1, (int) xPoint2, (int) yPoint2);
		g2D.setColor(Color.GRAY);
		g2D.setStroke(new BasicStroke(6.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		xPoint1 = screenX(xFront, xFrame);
		yPoint1 = screenY(yFront, yFrame);
		g2D.drawLine((int) xBicycle, (int) yBicycle, (int) xPoint1, (int) yPoint1);
		xPoint1 = screenX(xFront + wingspan/2.0*Math.cos(theta + thetaSteer + Math.PI/2), xFrame);
		yPoint1 = screenY(yFront + wingspan/2.0*Math.sin(theta + thetaSteer + Math.PI/2), yFrame);
		xPoint2 = screenX(xFront - wingspan/2.0*Math.cos(theta + thetaSteer + Math.PI/2), xFrame);
		yPoint2 = screenY(yFront - wingspan/2.0*Math.sin(theta + thetaSteer + Math.PI/2), yFrame);
		g2D.setColor(Color.GRAY);
		g2D.drawLine((int) xPoint1, (int) yPoint1, (int) xPoint2, (int) yPoint2);
		
		// Now draw the buffer to the screen
		this.getGraphics().drawImage(buffer, 0, 0, null);
		
	}
	
	public static double screenX(double worldX, double xFrame) {
		return WIDTH/2 + (worldX - xFrame) * PIXELS_PER_METER;
	}
	
	public static double screenY(double worldY, double yFrame) {
		return HEIGHT/2 - (worldY - yFrame) * PIXELS_PER_METER;
	}
	
	public static double worldX(double screenX, double xFrame) {
		return (screenX - WIDTH/2) / PIXELS_PER_METER + xFrame;
	}
	
	public static double worldY(double screenY, double yFrame) {
		return -(screenY - HEIGHT/2) / PIXELS_PER_METER + yFrame;
	}
		
	

	@Override
	public void windowActivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosed(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosing(WindowEvent arg0) {
		// TODO Auto-generated method stub
		System.out.println("Caught it!");
		
		// Trigger a save before we go down.
		SimulatorEngine.saveNow();
		
		System.exit(0);
		
	}

	@Override
	public void windowDeactivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowDeiconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowIconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowOpened(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	
	
	
}
