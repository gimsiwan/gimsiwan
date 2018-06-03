
public class Region extends Thread{
	
	private String name;
	private int regionNum;
	private int population;
	private Candidate[] candidates;
	
	public Region(String name, int regionNum, int population, Candidate[] candidates){
		this.name=name;
		this.regionNum=regionNum;
		this.population=population;
		this.candidates=candidates;
	}
	
	public void generateVotes(){
		int random = (int)Math.floor(Math.random()*candidates.length);
		candidates[random].addVote(regionNum);
		population--;
	}
	
	public void run(){
		while(true){
			if(population<=0) break;
			generateVotes();
		}
	}
}
