package cvinterface;

import java.io.IOException;
import java.util.ArrayList;

import lcm.lcm.LCM;
import lcm.lcm.LCMDataInputStream;
import lcm.lcm.LCMSubscriber;
import blicycle.blicycle_packet_t;


public class VisionSubscriber implements LCMSubscriber
{
	
	private static final double pixelToMeter = 100.;
	private double delta=0;
	private double phi=Math.PI/2;
	private byte encodedLanes;
	private byte currLane;
	private int lock=0;
	
	private static final double alpha = 0.8;
	private ArrayList<Double> medianFilt;
	
	public VisionSubscriber(String visionInChannel){
		LCMImport.getLCM().subscribe(visionInChannel, this);

		medianFilt = new ArrayList<Double>(4);
		medianFilt.add(1.);
		medianFilt.add(1.57);
		medianFilt.add(2.);
	}
	
    public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins)
    {
        //System.out.println("Received message on channel " + channel);
        blicycle_packet_t packet;
        try {
            packet = new blicycle_packet_t(ins);
            
            delta = packet.delta/pixelToMeter;
            phi = timeSmooth(packet.phi, phi);
            //System.out.println(packet.phi+" "+ phi+" "+ packet.timestamp);
            encodedLanes = packet.totalLanes;
            currLane = packet.lane;
            lock = packet.lock;            
            
        } catch (IOException ex) {
            System.out.println("Error decoding message: " + ex);
            return;
        }        
    }
    
	private double timeSmooth(double x, double oldx){  //gets the median of the last three measurements to avoid random spikes
		medianFilt.add(0,x);
		medianFilt.remove(3);
		double newx;
		if(medianFilt.get(0)>=medianFilt.get(1)){
				if(medianFilt.get(1)>=medianFilt.get(2)){
					newx = medianFilt.get(1);
				}else{
					if(medianFilt.get(0)>medianFilt.get(2)){
						newx = medianFilt.get(2);
					}else{
						newx = medianFilt.get(0);
					}
				}
		}else{
			if(medianFilt.get(1)<=medianFilt.get(2)){
				newx = medianFilt.get(1);
			}else{
				if(medianFilt.get(0)>medianFilt.get(2)){
					newx = medianFilt.get(0);
				}else{
					newx = medianFilt.get(2);
				}
			}
		}
		//System.out.println("median is "+newx);
		double delta = (newx-oldx)*alpha; //smoothes the data a bit as well, again to help filter craziness
		return delta+oldx;
	}

	public double getDelta() {
		return delta;
	}
	
	public double getPhi(){
		return phi;
	}

	public int getLock() {
		return lock;
	}
}