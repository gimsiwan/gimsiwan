package pizza;
public class Cheese extends Topping{

	private PizzaOrTopping pizzaOrTopping;
	private double cost;
	private int cookingTime;
	
	public Cheese(PizzaOrTopping pizzaOrTopping){
		super(pizzaOrTopping.getOrderNum());
		this.pizzaOrTopping = pizzaOrTopping;
		cost = 1.00;
		cookingTime = 3000;
	}
	
	public String toString(){
		return "\tExtra Cheese\n";
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
