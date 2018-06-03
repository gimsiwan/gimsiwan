package pizza;

public class OriginalPizza extends PizzaOrTopping{
	
	private double cost;
	private long cookingTime;
	
	public OriginalPizza(int orderNum){
		super(orderNum);
		description = "OriginalPizza\n";
		cost = 3.75;
		cookingTime=10000;
	}

	public long getCookingTime(){
		return cookingTime;
	}
	
	public double cost(){
		return cost;
	}
}
