package pizza;

public abstract class Topping extends PizzaOrTopping{
	
	public Topping(int orderNum){
		super(orderNum);
	}
	
	public abstract String getDescription();

}
