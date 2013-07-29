package datasource;

/**
 * Returns a sinusoidally-varying sine wave source.
 * Please note that this class is intended as a test-interface so that the rest
 * of the code can be more easily tested!
 * @author steve
 *
 */
public class SineDataSource implements AngleDataStream {

	// Fields
	protected long t0;
	protected double omega;
	protected double amplitude;
	
	public SineDataSource(double omega, double amplitude) {
		this.omega = omega;
		this.amplitude = amplitude;
		
		// Also record the current time, which is the starting time.
		this.t0 = System.currentTimeMillis();
	}
	
	@Override
	public double getAngle() {
		// Compute the current angle.
		double t = 0.001 * (System.currentTimeMillis() - t0);
		return amplitude * Math.sin(omega * t);
	}

	@Override
	public void setVal(byte val) {
		// Don't care
	}

}
