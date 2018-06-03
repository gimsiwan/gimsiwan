
import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.io.PrintWriter;
import java.io.FileOutputStream;


public class ElectionSim {
	
	private String outputFile;
	private int population;
	private Candidate[] candidate;
	private Region[] region;
	

	public ElectionSim(String inputFile,String outputFile){
		
		this.outputFile = outputFile;
		Scanner inputStream=null;
		
		try{
			inputStream = new Scanner(new FileInputStream(inputFile));
		}catch(FileNotFoundException e){
			System.out.println(e.getMessage());
		}
		
		inputStream.next();
		this.population=inputStream.nextInt();
		
		inputStream.next();
		
		candidate = new Candidate[inputStream.nextInt()];
		
		inputStream.nextLine();
		
		for(int i=0;i<candidate.length;i++)
			candidate[i]= new Candidate(inputStream.nextLine(),population);
		
		inputStream.next();
		this.region = new Region[inputStream.nextInt()];
		
		for(int i=0;i<region.length;i++)
			region[i]= new Region(inputStream.next(),inputStream.nextInt(),inputStream.nextInt(),candidate);
		
		inputStream.close();
	}
	
	public void saveData(){
		
		Arrays.sort(candidate);
		PrintWriter outputStream = null;
		
		try{
			outputStream = new PrintWriter(new FileOutputStream(outputFile));
		}catch(FileNotFoundException e){
			System.out.println(e.getMessage());
		}
		
		for(int i=0;i<candidate.length;i++)
			outputStream.println(candidate[i]);
		
		outputStream.close();
	}

	public void runSimulation(){
		
		for(int i=0;i<region.length;i++)
			region[i].start();
		
		try{
			for(int i=0;i<region.length;i++)
				region[i].join();
		}catch(InterruptedException e){
			System.out.println(e.getMessage());
		}
		
		this.saveData();
	}
}
