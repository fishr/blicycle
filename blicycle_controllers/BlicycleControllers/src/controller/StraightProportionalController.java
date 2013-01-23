package controller;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import datasource.MotorCommandTranslator;

public class StraightProportionalController extends DesiredAngleProportionalController {
	
	public StraightProportionalController(double gain) {
		super(gain, 0.0);
	}


}
