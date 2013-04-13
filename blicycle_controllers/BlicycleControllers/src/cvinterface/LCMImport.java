package cvinterface;

import java.io.IOException;
import java.util.ArrayList;

import lcm.lcm.*;
import blicycle.blicycle_packet_t;

public class LCMImport{
	
	public LCM lcm;
	public LCMSubscriber lcmsub;
	
	public double pixelToMeter = 100.;
	public double delta=0;
	public double phi=Math.PI/2;
	public byte encodedLanes;
	public byte currLane;
	public int lock=0;
	public long lastTime=0;
	
	public double alpha = 0.8;
	public ArrayList<Double> medianFilt;
	

	public LCMImport(String LCMChannel){
		lcm = LCM.getSingleton();
		
		lcmsub = new BlikeSubscriber();
		
		lcm.subscribe(LCMChannel, lcmsub);
		
		medianFilt = new ArrayList<Double>(4);
		medianFilt.add(1.);
		medianFilt.add(1.57);
		medianFilt.add(2.);
	}
	
	public double delta(){
		return delta;
	}

	public double phi(){
		return phi;
	}
	
	public double timeSmooth(double x, double oldx){
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
		double delta = (newx-oldx)*alpha;
		return delta+oldx;
	}

public class BlikeSubscriber implements LCMSubscriber
{
    public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins)
    {
        //System.out.println("Received message on channel " + channel);
        blicycle_packet_t packet;
        try {
            packet = new blicycle_packet_t(ins);
            
            delta = packet.delta/pixelToMeter;
            phi = timeSmooth(packet.phi, phi);
            System.out.println(packet.phi+" "+ phi+" "+ packet.timestamp);
            encodedLanes = packet.totalLanes;
            currLane = packet.lane;
            lock = packet.lock;
            lastTime = packet.timestamp;
            
            
        } catch (IOException ex) {
            System.out.println("Error decoding message: " + ex);
            return;
        }

        
    }
}


}

