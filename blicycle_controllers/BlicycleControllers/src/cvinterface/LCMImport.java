package cvinterface;

import java.io.IOException;
import lcm.lcm.*;
import blicycle.blicycle_packet_t;

public class LCMImport{
	
	public LCM lcm;
	public LCMSubscriber lcmsub;
	
	public double delta=0;
	public double phi=Math.PI/2;
	public byte encodedLanes;
	public byte currLane;
	public int lock=0;
	

	public LCMImport(String LCMChannel){
		lcm = LCM.getSingleton();
		
		lcmsub = new BlikeSubscriber();
		
		lcm.subscribe(LCMChannel, lcmsub);
	}


public class BlikeSubscriber implements LCMSubscriber
{
    public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins)
    {
        System.out.println("Received message on channel " + channel);
        blicycle_packet_t packet;
        try {
            packet = new blicycle_packet_t(ins);
            
            delta = packet.delta;
            phi = packet.phi;
            encodedLanes = packet.totalLanes;
            currLane = packet.lane;
            lock = packet.lock;
            
            
        } catch (IOException ex) {
            System.out.println("Error decoding message: " + ex);
            return;
        }

        
    }
}


}

