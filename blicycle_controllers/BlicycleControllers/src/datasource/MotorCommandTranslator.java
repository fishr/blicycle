package datasource;

/**
 * A helper class to translate to motor commands.
 * @author Steve
 *
 */
public class MotorCommandTranslator {

	/**
	 * Round and limit
	 */
	public static int roundAndLimit(double a) {
		int c = (int) Math.round(a);
		if (c < -4) {
			c = -4;
		} else if (c > 4) {
			c = 4;
		}
		return c;
	}
	
	
	/**
	 * Map a center point to which motors to turn on.
	 */
	public static byte makeSingleMotorCenterpoint(int centerpoint) {
		
		if (centerpoint < -4) {
			return (byte) (1 << 7);
		} else if (centerpoint > 4) {
			return (1 << 0);
		}
		
		switch(centerpoint) {
		case -4:
			return (byte) (1 << 7);
			
		case -3:
			return (1 << 6);
			
		case -2:
			return (1 << 5);
			
		case -1:
			return (1 << 4);
			
		case 0:
			return 0;
			
		case 1:
			return (1 << 3);
			
		case 2:
			return (1 << 2);
			
		case 3:
			return (1 << 1);
			
		case 4:
			return (1 << 0);
			
		default:
			return 0;
			
		}
	}
	
	public static int decodeVibrationMotorMask(int mask) {
		switch(mask) {
		case 1 << 7:
			return -4;
			
		case (1 << 6):
			return -3;
			
		case (1 << 5):
			return -2;
			
		case (1 << 4):
			return -1;
			
		case 0:
			return 0;
			
		case (1 << 3):
			return 1;
			
		case (1 << 2):
			return 2;
			
		case (1 << 1):
			return 3;
			
		case (1 << 0):
			return 4;
			
		default:
			return 0;
			
		}
	}
}
