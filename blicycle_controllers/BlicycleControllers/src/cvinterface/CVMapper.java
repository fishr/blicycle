package cvinterface;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.LinkedList;
import java.util.List;

/**
 * This class maps frmo the (rho, theta) pixel space representation from the computer vision module to the 
 * (deltaX, deltaTheta) format used by the controllers.
 * @author Steve
 *
 */
public class CVMapper{

	
	protected double[][] rhoData;
	protected double[][] phiData;
	protected double[][] xData;
	protected double[][] thetaData;
	
	public CVMapper(String rhoFile, String phiFile, String xFile, String thetaFile) {
		rhoData = parseCSVFile(rhoFile);
		phiData = parseCSVFile(phiFile);
		xData = parseCSVFile(xFile);
		thetaData = parseCSVFile(thetaFile);
	}
	
	
	protected double[][] parseCSVFile(String filename) {
		try {
			BufferedReader input = new BufferedReader(new FileReader(filename));
			String inputLine;
			List<double[]> temp = new LinkedList<double[]>();
			int numRows = 0;
			int numCols = 0;
			while((inputLine = input.readLine()) != null) {
				String fields[] = inputLine.split(",");
				if (numCols == 0) {
					numCols = fields.length;
				} else if (numCols != fields.length) {
					System.out.println("Error! Not a rectangular grid file: " + filename);
					return null;
				}
				
				double[] line = new double[numCols];
				for(int i = 0; i < numCols; i++) {
					line[i] = Double.parseDouble(fields[i]);
				}
				temp.add(line);
			}
			
			// We've read every line. Now convert to a final array.
			double output[][] = new double[temp.size()][numCols];
			int rowIndex = 0;
			for(double[] row : temp) {
				for(int columnIndex = 0; columnIndex < numCols; columnIndex++) {
					output[rowIndex][columnIndex] = row[columnIndex];
				}
				rowIndex++;
			}
			return output;
		} catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	/**
	 * Compute the reverse lookup, and return a 2-element array with x and theta.
	 * @return
	 */
	public double[] lookupXAndTheta(double rho, double phi) {
		int index_rho = lookupIndex(rho, rhoData);
		int index_phi = lookupIndex(phi, phiData);
		
		double[] output = new double[2];
		
		// Is this point valid?
		if (isValidPoint(index_phi, index_rho)) {
			
			output[0] = xData[index_phi][index_rho];
			output[1] = thetaData[index_phi][index_rho];
			
		} else {
			// We might be on the fringe - if so, choose a close value arbitrarily and return that.
		
			output[0] = 0.0;
			output[1] = 0.0;
			
//			for(int i = (int) Math.max(index_rho - 1, 0); i <= Math.min(index_rho + 1, xData[0].length - 1); i++) {
//				for(int j = (int) Math.max(index_phi - 1, 0); j <= Math.min(index_phi + 1, xData.length - 1); j++) {
//					if (isValidPoint(j, i)) {
//						// Return it!
//						output[0] = xData[j][i];
//						output[1] = thetaData[j][i];
//						
//						System.out.println(" --> Returning fringe point!");
//						return output;
//					}
//				}
//			}
			
			
			
		}
		
		
		
		
		return output;
	}
	
	public boolean isValidPoint(int i_phi, int i_rho) {
		return !(xData[i_phi][i_rho] == 0.0 && thetaData[i_phi][i_rho] == 0.0);
	}
	
	protected int lookupIndex(double val, double[][] values) {
		int closestIndex = 0;
		double closestDistance = 1000000000.0;
		for(int i = 0; i < values[0].length; i++) {
			double d = Math.abs(val - values[0][i]);
			if (d < closestDistance) {
				closestDistance = d;
				closestIndex = i;
			}
		}
		
		return closestIndex;
	}
	
}
