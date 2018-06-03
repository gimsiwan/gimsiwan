
public class Subject {

	private String name, tutor, room;
	
	public Subject(String name, String tutor, String room){
		this.name =name;
		this.tutor = tutor;
		this.room = room;
	}
	
	public Subject(String name){
		this.name = name;
	}
	
	public Subject(Subject sub){
		this.name = sub.name;
		this.tutor = sub.tutor;
		this.room = sub.room;
	}
	
	public String getName(){
		return this.name;
	}
	public String getTutor(){
		return this.tutor;
	}
	public String getRoom(){
		return this.room;
	}
	
	public void setTutor(String tutor){
		this.tutor = tutor;
	}
	public void setRoom(String room){
		this.room = room;
	}
	
	public boolean equals(Subject sub){
		if(this.name.equals(sub.name) && this.tutor.equals(sub.tutor) && this.room.equals(sub.room)) return true;
		else return false;
	}
	
	public String toString(){
		return this.name;
	}
	
	public String getDetails(){
		return "Name: " + this.name + "\nTutor: " + this.tutor + "\nRoom: " + this.room + "\n";
	}
}
