package pizza;
public class Pineapple extends Topping{
	
	private PizzaOrTopping pizzaOrTopping;
	private double cost;
	private int cookingTime;
	
	public Pineapple(PizzaOrTopping pizzaOrTopping){
		super(pizzaOrTopping.getOrderNum());
		this.pizzaOrTopping = pizzaOrTopping;
		cost = 1.00;
		cookingTime = 3500;
	}
	
	public String toString(){
		return "\tPineapples\n";
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
