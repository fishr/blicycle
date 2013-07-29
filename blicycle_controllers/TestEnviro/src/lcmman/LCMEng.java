package lcmman;

import java.util.Observable;

import lcm.lcm.LCMSubscriber;


public class LCMEng {
	
	private lcm.lcm.LCM lcmEng;
	private LCMSubscriber ardsub;	
	private LCMSubscriber yeisub;
	
	
	public lcmData data;
	
	public LCMEng(){
		lcmEng = lcm.lcm.LCM.getSingleton();

		data = new lcmData();

		ardsub = new ArduinoSubscriber(data);
		yeisub = new YeiSubscriber(data);
		
		lcmEng.subscribe("arduinoUpdates", ardsub);  //TODO set this to whatever the arduino channels are
		lcmEng.subscribe("YEI_IMU", yeisub); //TODO set this to yei channel
	}
	
	public class lcmData extends Observable{
		private int speed = 0;
		private double steering = 0;
		private double[] q = {0,0,0,0};
		private boolean b = false;
		private long startTime;
		private long msgTime;
		private double[] linearAccel = {0,0,0};
		private boolean t = false;
		
		
		public synchronized double[] getQuat() {
			return q;
		}

		public synchronized void setQuat(double[] quat) {
			if(quat.length!=4){
				System.out.println("bad quat");
				return;
			}
			this.q = quat;
			//newMsg();
		}

		public synchronized int getSpeed() {
			return speed;
		}

		public synchronized void setSpeed(int speed) {
			this.speed = speed;
			newMsg();
		}

		public synchronized double getSteering() {
			return steering;
		}

		public synchronized void setSteering(double d) {
			this.steering = d;
			newMsg();
		}

		public synchronized double getRoll() {
			return Math.atan2(2*(q[0]*q[1]+q[2]*q[3]), 1-2*(q[1]*q[1]+q[2]*q[2]));
		}

		public synchronized double getPitch() {
			return Math.asin(2*(q[0]*q[2]-q[3]*q[1]));
		}

		public synchronized double getYaw() {
			return Math.atan2(2*(q[0]*q[3]+q[1]*q[2]), 1-2*(q[2]*q[2]+q[3]*q[3]));
		}

		public synchronized void newMsg() {
			this.setChanged();
			this.notifyObservers();
		}
		
		public synchronized void clearMsg(){
			this.clearChanged();
		}

		public synchronized void setSpeedButton() {
			b = !b;
			setStartTime();
			newMsg();
		}
		
		public synchronized boolean checkSpeedStarted() {
			return b;
		}

		public synchronized void setStartTime() {
			startTime = System.currentTimeMillis();
		}
		
		public synchronized long getStartTime(){
			return startTime;
		}

		public double[] getLogLine() {
			return new double[] {(double) getMsgTime(), (double) getSpeed(),(double) getSteering(), (double) getPitch(), linearAccel[0], linearAccel[1], linearAccel[2]};
		}

		public synchronized void setAccel(double[] linear_acceleration) {
			linearAccel = linear_acceleration;
		}

		public synchronized void setMsgTime(long utime) {
			msgTime = utime;
		}
		
		public synchronized long getMsgTime(){
			return msgTime;
		}

		public void setTurnButton() {
			t = !t;
			newMsg();			
		}

		public synchronized boolean checkTurnStarted() {
			return t ;
		}

	}

}
