
public class Candidate implements Comparable<Candidate>{
	
	private String name;
	private Vote[] votes;
	private int numVotes=0;
	
	public Candidate(String name, int maxVotes){
		this.name=name;
		votes= new Vote[maxVotes];
	};
	
	public String toString(){
		return "------------Candidate-----------\r\n"+"Name: "+ this.name + "\r\nVotes: " 
					+ numVotes + "\r\n\r\n============================";
	}
	
	public synchronized void addVote(int regionNum){
		votes[numVotes++] = new Vote(regionNum);
	}
	
	public int compareTo(Candidate candi){
		if(this.numVotes>candi.numVotes) return -1;
		else if(this.numVotes==candi.numVotes) return 0;
		else return 1;
	}
	
	private class Vote{
	
		private int regionNum;	
	
		public Vote(int regionNum){
			this.regionNum=regionNum;
		}
	}
}
