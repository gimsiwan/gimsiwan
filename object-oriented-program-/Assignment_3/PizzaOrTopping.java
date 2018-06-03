package pizza;

public abstract class PizzaOrTopping {

	protected String description;
	private boolean isFinished;
	private int orderNum;
	
	public PizzaOrTopping(int orderNum){
		this.orderNum =orderNum;
		isFinished = false;
		description = "Unknown Pizza";
	}
	
	public String getDescription(){
		return description;
	}
	
	public int getOrderNum(){
		return orderNum;
	}
	
	public boolean getIsFinished(){
		return isFinished;
	}
	
	public void finish(){
		isFinished = true;
	}
	
	public String toString(){
		return description;
	}
	
	public abstract long getCookingTime();
	
	public abstract double cost();
}
