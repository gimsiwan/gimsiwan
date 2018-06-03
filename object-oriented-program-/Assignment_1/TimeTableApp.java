import java.util.Scanner;

public class TimeTableApp {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		Scanner sc = new Scanner(System.in);
		TimeTable table1 = new TimeTable();
		System.out.println(table1);
		
		int i = 1;
		do{
			
		System.out.println("===================Main Menu===================");
		System.out.println("(1) Add a class to my time table:");
		System.out.println("(2) View the class at a specific time:");
		System.out.println("(3) Print the time table");
		System.out.println("(4) Exit the program");
		System.out.println("===================Main Menu===================");
			
		int command = sc.nextInt();
		String trush = sc.nextLine();
		
		switch(command){
		
		case 1:
			System.out.println("Please enter the day to add the class:");
			String daySet = sc.nextLine();
			System.out.println("Please enter the period to add the class:");
			int periodSet = sc.nextInt();
			String trush2 = sc.nextLine();
			System.out.println("Please enter the name of class");
			String nameSet = sc.nextLine();
			System.out.println("Please enter the name of tutor");
			String tutorSet = sc.nextLine();
			System.out.println("Please enter the name of Room");
			String roomSet = sc.nextLine();
			
			boolean isSuccess = table1.setSchedule(daySet, periodSet, nameSet, tutorSet, roomSet);
			
			if(isSuccess)
				System.out.println("Class successfully added");
			else
				System.out.println("Class was NOT successfully added");
			break;	
		
		case 2:
			System.out.println("Please enter the day to add the class:");
			String dayGet = sc.nextLine();
			System.out.println("Please enter the period to add the class:");
			int periodGet = sc.nextInt();
			String trush3 = sc.nextLine();
			System.out.println("At that time you have:");
			
			System.out.println(table1.getSchedule(dayGet, periodGet).getDetails());
			break;
			
		case 3:
			System.out.println(table1.toString());
			break;
		
		case 4:
			i = 0;
			break;
		}
	}while(i==1);
		
		
	}

}
