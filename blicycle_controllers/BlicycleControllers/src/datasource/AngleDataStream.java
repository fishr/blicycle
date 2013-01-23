package datasource;

/**
 * Any class that implements this interface is capable of providing angular data (in radians) when it is polled!
 * @author steve
 *
 */
public interface AngleDataStream {
	
	// Return the latest angle data, in radians
	public double getAngle();
	
	// Allow setting a value
	public void setVal(int val);
	
}
