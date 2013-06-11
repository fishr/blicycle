package blicyclesoundmanager;

import java.io.File;
import java.io.IOException;
import javax.sound.sampled.*;

public class AudioHandler implements Runnable {
	private int soundId =0;
	protected int soundRequestId = 0;
	public int numSounds=0;
	private int lastSound = -1;
	protected boolean newSoundRequest = false; 
	protected boolean soundRequestOverride = false;
	private boolean newSound = false;
	private boolean soundOverride = false;
	protected byte balanceRequest = (byte) 0;
	private byte balance = (byte) 0;

	
	//setup our line in and test file
	Clip lineIn = null;
	Mixer soundMixer = null;
	Mixer.Info[] mixerInfo;
	AudioInputStream fileFormat;
	
	File[] sounds = null;

	@Override
	public void run() {

		loadMixer();
		//loadOGGSounds();  //only one sound loader plz
		loadWAVSounds();
		
		while(true){
			processSounds();
		}
	}

	private void loadMixer(){
		mixerInfo = AudioSystem.getMixerInfo();


		System.out.println("Available audio devices...");

		for(int i=0;i<mixerInfo.length;i++){
			System.out.println(mixerInfo[i].getName());
		}
		System.out.println(" ");
		System.out.print("Using:   ");
		System.out.println(mixerInfo[1].getName());

		soundMixer = AudioSystem.getMixer(mixerInfo[1]);
	}
	


	private void loadOGGSounds(){
		File soundDir = new File("/usr/share/sounds/ubuntu/stereo");
		sounds = soundDir.listFiles(new OggFilter());
		numSounds = sounds.length;
			
		
	}

	private void loadWAVSounds(){
		File soundDir = new File("Sounds");
		//File soundDir = new File("/home/blicycle/Music/Windows Classic/Robotz");
		sounds = soundDir.listFiles(new WavFilter());
		numSounds = sounds.length;
			
		
	}
	
	
	private void processSounds(){
		loadUpdates();
		if(!newSound){
			//System.out.println("old request");
			Thread.yield();
			return;
		}
		if(soundId==0){
			System.out.println("soundId is 0");
			Thread.yield();
			return;
		}else if((soundId>numSounds)||(soundId<0)){
			System.out.println("soundId out of bounds");
		}else{
			int i = soundId-1;
			boolean isActive = false;
			if(lineIn!=null){
				if(lineIn.isRunning())
					isActive = true;
			}
			if(isActive&&soundOverride){
				lineIn.stop();
				lineIn.flush();
				lineIn.close();
				isActive = false;
			}
			if((i!=lastSound)&&!isActive){
				try {
					fileFormat = AudioSystem.getAudioInputStream(sounds[i]);
					//fileFormat = AudioSystem.getAudioInputStream(new File("/home/blicycle/Music/Windows Classic/Windows Me/CHIMES.WAV"));
					System.out.println(sounds[i]);
					DataLine.Info lineInInfo = new DataLine.Info(Clip.class, fileFormat.getFormat()); // format is an AudioFormat object
					if (!soundMixer.isLineSupported(lineInInfo)) {  //used with getMixer
					//if (!AudioSystem.isLineSupported(lineInInfo)) {  //used with no mixer
						System.out.print("Unsupported line type?");
					}
					// Obtain and open the line.
					try {
						if(lineIn!=null){
							if(lineIn.isOpen())
								lineIn.close();
						}
						
						lineIn= (Clip) soundMixer.getLine(lineInInfo);  //used with getMixer
						//lineIn= (Clip) AudioSystem.getLine(lineInInfo);  //used with no mixer
						lineIn.open(fileFormat);
					} catch (Exception ex) {
						System.out.print("oops1");
						ex.printStackTrace();
					}
					
					
					if(!lineIn.isOpen()){
						System.out.println("line not opened");
					}
					
					

				} catch (Exception e) {
					System.out.print("oops ");
					e.printStackTrace();
				}
				finally{
					try {
						fileFormat.close();
					} catch (IOException e) {
						System.out.println("could not close stream");
						e.printStackTrace();
					}
				}
			lineIn.setFramePosition(0);
			if(lineIn.isControlSupported(FloatControl.Type.PAN)){
				System.out.println("pan");
				FloatControl panCtl = (FloatControl) lineIn.getControl(FloatControl.Type.PAN);
				panCtl.setValue(Utils.map((float)balance, (float) -128, (float) 127, panCtl.getMinimum(), panCtl.getMaximum()));
			}else if(lineIn.isControlSupported(FloatControl.Type.BALANCE)){
				System.out.println("bal");
				FloatControl panCtl = (FloatControl) lineIn.getControl(FloatControl.Type.BALANCE);
				panCtl.setValue(Utils.map((float)balance, (float) -128, (float) 127, panCtl.getMinimum(), panCtl.getMaximum()));
			}
			lineIn.start();			
			System.out.println("sound");
			newSoundRequest = false;
			}
			Thread.yield();

		}	
	}

	public synchronized void update(){
		soundRequestId++;
		newSoundRequest=true;
		//if(soundRequestId ==6){
			soundRequestOverride = true;
		/*}else{
			soundRequestOverride  = false;
		}
		if(soundRequestId>8){
			newSoundRequest = false;
		}*/
		balanceRequest = (byte) 0;
	}
	
	private synchronized void loadUpdates(){
		newSound = newSoundRequest;
		soundId = soundRequestId;
		soundOverride = soundRequestOverride;
		balance = balanceRequest;
	}
}
