package pizza;

public class DeepPanPizza extends PizzaOrTopping{

	private double cost;
	private long cookingTime;
	
	public DeepPanPizza(int orderNum){
		super(orderNum);
		description = "Deep Pan\n";
		cost = 4.50;
		cookingTime = 15000;
	}
	
	public long getCookingTime(){
		return cookingTime;
	}
	
	public double cost(){
		return cost;
	}
}
