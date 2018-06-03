package pizza;
public class Pepperoni extends Topping{
	
	private PizzaOrTopping pizzaOrTopping;
	private double cost;
	private int cookingTime;
	
	public Pepperoni(PizzaOrTopping pizzaOrTopping){
		super(pizzaOrTopping.getOrderNum());
		this.pizzaOrTopping = pizzaOrTopping;
		cost = 2.00;
		cookingTime = 2500;
	}
	
	public String toString(){
		return "\tPepperoni\n";
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
