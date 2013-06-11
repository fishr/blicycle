package blicyclesoundmanager;

import java.io.File;

public class Utils {

    public final static String ogg = "ogg";
    public final static String wav = "wav";

    /*
     * Get the extension of a file.
     */  
    public static String getExtension(File f) {
        String ext = null;
        String s = f.getName();
        int i = s.lastIndexOf('.');

        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    }
    
    public static String getExtension(String s) {
        String ext = null;
        int i = s.lastIndexOf('.');

        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    }
    
    public static float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
    }
  
}