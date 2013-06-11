package blicyclesoundmanager;

import java.io.File;
import java.io.FilenameFilter;


	public class WavFilter implements FilenameFilter{

		@Override
		public boolean accept(File arg0, String arg1) {

	 
	        String extension = Utils.getExtension(arg1);
	        if (extension != null) {
	            if (extension.equals(Utils.wav)) {
	                    return true;
	            } else {
	                return false;
	            }
	        }
			return false;
		}

		
		
	}