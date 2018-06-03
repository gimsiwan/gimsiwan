package pizza;

public class Order implements Observer{
	
	private int orderNum;
	private boolean collected;
	private Subject pizzaOven;
	
	public Order(int orderNum, Oven pizzaOven){
		this.orderNum = orderNum;
		this.pizzaOven = pizzaOven;
		collected = false;
	}
	
	public void update(PizzaOrTopping pizza){
		if(this.orderNum == pizza.getOrderNum()){
			collected = true;
			pizzaOven.removeObserver(this);
		}
	}

}
