package pizza;
public class Mushroom extends Topping{

	private PizzaOrTopping pizzaOrTopping;
	private double cost;
	private int cookingTime;
	
	public Mushroom(PizzaOrTopping pizzaOrTopping){
		super(pizzaOrTopping.getOrderNum());
		this.pizzaOrTopping = pizzaOrTopping;
		cost = 1.25;
		cookingTime = 3000;
	}
	
	public String toString(){
		return "\tMushrooms\n";
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
