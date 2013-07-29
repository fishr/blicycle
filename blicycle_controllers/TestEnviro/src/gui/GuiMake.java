package gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.LayoutStyle;

import testenviro.loggingUtil;

import lcmman.LCMEng.lcmData;

public class GuiMake implements Observer, ActionListener{
	
	private lcmData lcmdata;
	
	private JLabel speedVal;
	private JLabel rollVal;
	private JLabel steeringVal;
	private JLabel pitchVal;
	private JLabel yawVal;
	
	JButton turnTestButton;
	JButton speedTestButton;
	
	private DefaultListModel speeds = new DefaultListModel();
	boolean speedTestStarted = false;
	JList speedList;

	private boolean turnTestActive;

	private JTextField turnDiaField;

	private loggingUtil logHandle;

	private JLabel turnFieldLabel;

	private boolean turnTestStarted;

	private int turnTestNum = 0;

	private JLabel turnTestDetail;

	public GuiMake(lcmData data) {
		Dimension panelSize = new Dimension(300, 50);
		
		JFrame guiFrame = new JFrame();
		
		lcmdata = data;
		
		lcmdata.addObserver(this);
		
		//make sure program exits on window close
		guiFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		guiFrame.setTitle("Test Status");
		guiFrame.setSize(800, 600);
		
		//center frame
		guiFrame.setLocationRelativeTo(null);
		
		final JPanel speedPanel = new JPanel();
		final JPanel steeringPanel = new JPanel();
		final JPanel rollPanel = new JPanel();
		final JPanel pitchPanel = new JPanel();
		final JPanel yawPanel = new JPanel();

		JLabel speed = new JLabel("Speed: ");
		Font labelFont = speed.getFont().deriveFont((float)25);
		speedVal = new JLabel("NAN");
		speedPanel.add(speed);
		speedPanel.add(speedVal);
		
		JLabel steering = new JLabel("Steering: ");
		steeringVal = new JLabel("NAN");
		steeringPanel.add(steering);
		steeringPanel.add(steeringVal);
		
		JLabel roll = new JLabel("Roll: ");
		rollVal = new JLabel("NAN");
		rollPanel.add(roll);
		rollPanel.add(rollVal);

		JLabel pitch = new JLabel("Pitch: ");
		pitchVal = new JLabel("NAN");
		pitchPanel.add(pitch);
		pitchPanel.add(pitchVal);

		JLabel yaw = new JLabel("Yaw: ");
		yawVal = new JLabel("NAN");
		yawPanel.add(yaw);
		yawPanel.add(yawVal);

		JLabel[] labels = {speed, speedVal, steering, steeringVal, roll, rollVal, pitch, pitchVal, yaw, yawVal};
		for(JLabel sett : labels){
			sett.setPreferredSize(panelSize);
			sett.setFont(labelFont);
		}

		speedList = new JList(speeds);
		speedList.setLayoutOrientation(JList.VERTICAL);
		speedList.setVisible(false);
		speedList.setFont(labelFont.deriveFont((float)30));
		
		JPanel buttonPane = new JPanel();
		
		speedTestButton = new JButton("Speed Test");
		speedTestButton.addActionListener(this);
		speedTestButton.setActionCommand("button");
		buttonPane.add(speedTestButton);
		
		turnTestButton = new JButton("Turn Test");
		turnTestButton.addActionListener(this);
		turnTestButton.setActionCommand("turn");
		buttonPane.add(turnTestButton);
		
		JPanel turnFieldPanel = new JPanel();
		turnFieldPanel.setLayout(new BoxLayout(turnFieldPanel, BoxLayout.PAGE_AXIS));
		
		turnFieldLabel = new JLabel("Filename: Angle");
		turnFieldLabel.setFont(labelFont.deriveFont((float)10));
		turnFieldPanel.add(turnFieldLabel);
		
		turnTestDetail = new JLabel("No Test");
		turnTestDetail.setFont(labelFont.deriveFont((float)30));
		turnFieldPanel.add(turnTestDetail);
		
		turnDiaField = new JTextField();
		turnDiaField.setFont(labelFont.deriveFont((float)10));
		turnFieldPanel.add(turnDiaField);
		
		//JPanel[] panels = {speedPanel, steeringPanel, yawPanel, pitchPanel, rollPanel};
		
		JPanel listPane = new JPanel();
		listPane.setLayout(new BoxLayout(listPane, BoxLayout.PAGE_AXIS));

		listPane.add(speedPanel);
		listPane.add(steeringPanel);
		listPane.add(rollPanel);
		listPane.add(pitchPanel);
		listPane.add(yawPanel);
		
		guiFrame.add(listPane, BorderLayout.CENTER);
		guiFrame.add(speedList, BorderLayout.EAST);
		guiFrame.add(buttonPane, BorderLayout.SOUTH);
		guiFrame.add(turnFieldPanel,BorderLayout.WEST);

		guiFrame.setVisible(true);
	}
	
	private void refreshInfo(){
		synchronized(lcmdata){
			try{
			rollVal.setText(String.valueOf(lcmdata.getRoll()));
			speedVal.setText(String.valueOf(lcmdata.getSpeed()));
			steeringVal.setText(String.valueOf(lcmdata.getSteering()));
			yawVal.setText(String.valueOf(lcmdata.getYaw()));
			pitchVal.setText(String.valueOf(lcmdata.getPitch()));
			}
			catch(NullPointerException ie){
				System.out.println("no info");
			}
		}
	}
	
	private void setSpeedInfo(){
		if(speedTestStarted!=lcmdata.checkSpeedStarted()){
			if(lcmdata.checkSpeedStarted()){
				setupSpeedTest();
			}else{
				removeSpeedTest();
			}
			speedTestStarted = !speedTestStarted;
		}
		if(((System.currentTimeMillis()-lcmdata.getStartTime())>5000)&&lcmdata.checkSpeedStarted()){
			speeds.addElement(String.valueOf(lcmdata.getSpeed()));
			lcmdata.setStartTime();
		}
	}
	
	private void setTurnInfo(){
		if(turnTestStarted!=lcmdata.checkTurnStarted()){
			if(lcmdata.checkTurnStarted()){
				setupTurnTest();
			}else{
				removeTurnTest();
			}
			turnTestStarted = !turnTestStarted;
		}
	}

	private void removeSpeedTest() {
		speedList.setVisible(false);
		speeds.clear();
		turnTestButton.setEnabled(true);
	}

	private void setupSpeedTest() {
		speeds.addElement(String.valueOf(lcmdata.getSpeed()));
		speedList.setVisible(true);
		turnTestButton.setEnabled(false);
	}

	@Override
	public void update(Observable arg0, Object arg1) {
		refreshInfo();
		setSpeedInfo();
		setTurnInfo();
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		if(arg0.getActionCommand().equals("button")){
			lcmdata.setSpeedButton();
		}else if(arg0.getActionCommand().equals("turn")){
			lcmdata.setTurnButton();
		}
	}

	private void setupTurnTest() {
		turnTestNum++;
		speedTestButton.setEnabled(false);
		String testName = turnDiaField.getText().trim();
		if(testName.length()==0){
			switch (turnTestNum){
				case 1: testName = "5";
					break;
				case 2: testName = "10";
					break;
				case 3: testName = "15";
					break;
				case 4: testName = "20";
					break;
				case 5: testName = "30";
					break;
				case 6: testName = "40";
					break;
				case 7: testName = "50";
					break;
				default: testName = "";
					break;
			}
		}
		logHandle.begin(testName);
		turnTestButton.setText("End");
		turnTestDetail.setText(testName);
		turnFieldLabel.setText("Diameter: dm");
		turnDiaField.setText("");
	}

	private void removeTurnTest() {
		logHandle.end(turnDiaField.getText());
		speedTestButton.setEnabled(true);
		turnTestButton.setText("Turn Test");
		loggingUtil.saveNow();
		turnFieldLabel.setText("Filename: angle");
		turnTestDetail.setText("No Test");
		turnDiaField.setText("");
	}

	public void setLog(loggingUtil log) {
		logHandle = log;
	}
}
