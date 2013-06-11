package blicyclesoundmanager;

public class AudioHandlerLCM extends AudioHandler {

	LCMImport lcmEngine = null;
	
	public AudioHandlerLCM(){
		lcmEngine = new LCMImport("soundCmd");

	}
	
	@Override
	public void update(){
		this.soundRequestOverride = lcmEngine.solo;
		this.newSoundRequest = lcmEngine.newSound;
		this.soundRequestId = lcmEngine.id;
		this.balanceRequest = lcmEngine.fade;
		System.out.println("got a packet");
	}
	
}
