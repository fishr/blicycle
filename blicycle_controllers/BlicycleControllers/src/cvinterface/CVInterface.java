package cvinterface;

public interface CVInterface {
	
	/**
	 * Compute the reverse lookup, and return a 2-element array with x and theta.
	 * @return
	 */
	public double[] lookupXAndTheta(double rho, double phi);
			
}
