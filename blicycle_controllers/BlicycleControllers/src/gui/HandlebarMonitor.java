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
public class HandlebarMonitor extends JPanel implements WindowListener {
		
	// Fields
	protected double angle;
	protected int motorBitmask;
	
	protected static final int WIDTH = 600;
	protected static final int HEIGHT = 600;
	
	public HandlebarMonitor() {
		super();
		
	}
	
	protected void createAndShowGUI() {
		JFrame frame = new JFrame("Handlebar Monitor");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		frame.add(this);
		frame.pack();
		frame.setBackground(Color.WHITE);
		frame.setSize(WIDTH, HEIGHT);
		frame.setLocation(100, 100);
		frame.addWindowListener(this);
		frame.setVisible(true);
		
		// Start a task to keep rendering.
		Timer timer = new Timer();
		timer.scheduleAtFixedRate(new TimerTask() {
			public void run() {
				render();
			}
		}, 0, 1000 / 20);
		
		System.out.println("Handlebar Monitor is available.");
		
	}
	
	
	public void setData(double a, int m) {
		this.angle = a;
		this.motorBitmask = m;
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
		g2D.setBackground(Color.WHITE);
		g2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		
		// Clear the background
		g2D.clearRect(0, 0, WIDTH, HEIGHT);
		
		// Draw the handlebars
		int LINE_WIDTH = WIDTH/3;
		int x0 = (int) Math.round(WIDTH/2 + LINE_WIDTH * Math.cos(angle));
		int x1 = (int) Math.round(WIDTH/2 - LINE_WIDTH * Math.cos(angle));
		int y0 = (int) Math.round(HEIGHT/3 - LINE_WIDTH* Math.sin(angle));
		int y1 = (int) Math.round(HEIGHT/3 + LINE_WIDTH * Math.sin(angle));
		g2D.setStroke(new BasicStroke(35.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_MITER));
		g2D.setColor(Color.BLACK);
		g2D.drawLine(x0, y0, x1, y1);
		
		// Draw each vibrator motor on the screen
		g2D.setStroke(new BasicStroke(20.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_MITER));
		for(int i = 0; i < 4; i++) {
			if ((motorBitmask & (1 << i)) != 0) {
				g2D.setColor(Color.RED);
			} else {
				g2D.setColor(Color.DARK_GRAY);
			}
			
			double L = LINE_WIDTH - 20*i - 20;
			int x = (int) Math.round(WIDTH/2 + L*Math.cos(angle));
			int y = (int) Math.round(HEIGHT/3 - L*Math.sin(angle));
			g2D.drawLine(x, y, x, y);
		}
		for(int i = 4; i < 8; i++) {
			if ((motorBitmask & (1 << i)) != 0) {
				g2D.setColor(Color.RED);
			} else {
				g2D.setColor(Color.DARK_GRAY);
			}
			
			double L = LINE_WIDTH - 20*(7-i) - 20;
			int x = (int) Math.round(WIDTH/2 - L*Math.cos(angle));
			int y = (int) Math.round(HEIGHT/3 + L*Math.sin(angle));
			g2D.drawLine(x, y, x, y);
		}
		
		
		// Paint the current angle (in degrees) on the screen
		g2D.setFont(new Font("Libertino", Font.PLAIN, 36));
		DecimalFormat df = new DecimalFormat("#.#");
		String angleString = df.format(180.0 / Math.PI * angle) + " degs";

		g2D.setColor(Color.BLACK);
		g2D.drawString(angleString, 250, HEIGHT - 70);
		
		// Now draw the buffer to the screen

		this.getGraphics().drawImage(buffer, 0, 0, null);
		
	}

	@Override
	public void windowActivated(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosed(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosing(WindowEvent e) {
		// TODO Auto-generated method stub
		System.out.println("Caught it!");
		
		// Trigger a save before we go down.
		SimulatorEngine.saveNow();
		
		System.exit(0);
		
	}

	@Override
	public void windowDeactivated(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowDeiconified(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowIconified(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowOpened(WindowEvent e) {
		// TODO Auto-generated method stub
		
	}

	
	
	
}
