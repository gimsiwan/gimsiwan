package pizza;
public class Bacon extends Topping{
	
	private PizzaOrTopping pizzaOrTopping;
	private double cost;
	private int cookingTime;
	
	public Bacon(PizzaOrTopping pizzaOrTopping){
		super(pizzaOrTopping.getOrderNum());
		this.pizzaOrTopping = pizzaOrTopping;
		cost = 0.75;
		cookingTime = 2000;
	}
	
	public String toString(){
		return "\tBacon\n";
	}
	
	public String getDescription(){
		return pizzaOrTopping.getDescription() + toString();
	}
	
	public long getCookingTime(){
		return pizzaOrTopping.getCookingTime() + cookingTime;
	}

	public double cost(){
		return pizzaOrTopping.cost() + cost;
	}
}
