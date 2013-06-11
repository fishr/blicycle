package blicyclesoundmanager;

import java.io.IOException;
import java.util.ArrayList;

import blicycle.sound_packet_t;

import lcm.lcm.*;


public class LCMImport {

	public LCM lcm;
	public LCMSubscriber lcmsub;
	
	public byte fade = 0;
	public int id;
	public boolean newSound = false;
	public boolean solo = false;
	
	public LCMImport(String LCMChannel){
		lcm = LCM.getSingleton();
		
		lcmsub = new SoundSubscriber();
		
		lcm.subscribe(LCMChannel, lcmsub);
	}
	
	
	
	
	public class SoundSubscriber implements LCMSubscriber
	{
	    public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins)
	    {
	        System.out.println("Received message on channel " + channel);
	        sound_packet_t packet;
	        try {
	            packet = new sound_packet_t(ins);
	            
	            fade = packet.fade;
	            id = packet.sound_id;
	            newSound = packet.newSound;
	            solo = packet.solo;
	           
	            
	            
	        } catch (IOException ex) {
	            System.out.println("Error decoding message: " + ex);
	            return;
	        }

	        
	    }
	}

}
