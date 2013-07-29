package datasource;

/**
 * Implements an expontentially-weighted moving average filter to simulate
 * the delayed response of a human operator riding the bike.
 * @author steve
 *
 */
public class HumanSimulator implements AngleDataStream {

	// The expontential weighting parameter that determines the 
	// response speed.
	protected double alpha;
	protected double theta;
	protected int setVals[];
	protected int setIndex;
	
	public HumanSimulator(double alpha, int delaySamples) {
		this.alpha = alpha;
		setVals = new int[delaySamples];
		setIndex = 0;
	}
	
	
	@Override
	public double getAngle() {
		return theta;
	}

	@Override
	public void setVal(byte val) {
		double desired = -0.2 * MotorCommandTranslator.decodeVibrationMotorMask(setVals[setIndex]);
		setVals[setIndex] = val;
		setIndex = (setIndex + 1) % setVals.length;
		//System.out.println(desired - theta);
		//theta += alpha * (desired - theta);
		theta += alpha * desired;
	}

}
