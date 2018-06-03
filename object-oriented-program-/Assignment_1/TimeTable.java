
public class TimeTable {
	
	public enum DAYS{MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY};
	
	Subject[][] timeTable = new Subject[10][5];
	
	private void initialize(){
		
		for(int i = 0; i < 10; i++){
			for(int j = 0; j < 5; j++){
				timeTable[i][j] = new Subject("----");
			}
		}
		for(int i =0;i<5;i++){
		timeTable[3][i] = new Subject("BREAK");
		timeTable[7][i] = new Subject("LUNCH");
		}
	}

	public TimeTable(){
		initialize();
	}
	
	public Subject getSchedule(String day, int period){
		String dayUpp ="";
		for(int i = 0;i<day.length();i++){
			char c = day.charAt(i);
			if(c>=97 && c<=122)
				dayUpp += String.valueOf(c).toUpperCase();
			else dayUpp += c;
		}
		Subject sub = new Subject(timeTable[period-1][DAYS.valueOf(dayUpp).ordinal()]);
		
		return sub;
	}
	
	public boolean setSchedule(String day, int period, String name, String tutor, String room){
		String dayUpp ="";
		for(int i = 0;i<day.length();i++){
			char c = day.charAt(i);
			if(c>=97 && c<=122)
				dayUpp += String.valueOf(c).toUpperCase();
			else dayUpp += c;
		}
		DAYS classday = DAYS.valueOf(dayUpp);
		int dayNum = classday.ordinal();
		
		if(period!=4 && period!=8){
		timeTable[period-1][dayNum] = new Subject(name, tutor, room);
		return true;
		}
		else return false;
		
	}
	
	public String toString(){
		String outcome = "  ";
		String wordLimit;
		for(DAYS e : DAYS.values()){
			outcome += String.format("%13s", e.name());
		}
		
		for(int i=0;i<10;i++){
			outcome += "\n";
			outcome += String.format("%2d", i+1);
			for(int j=0;j<5;j++){
				wordLimit = timeTable[i][j].toString();
				if(wordLimit.length()>9)
				wordLimit = wordLimit.substring(0, 9);
				outcome += String.format("%13s", wordLimit);
			}
			if(i==9) outcome += "\n";
		}	
		
		return outcome;
	}
}